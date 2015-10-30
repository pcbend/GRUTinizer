#include "TPipeline.h"

#include <iostream>

#include "TChain.h"

#include "TDataLoop.h"
#include "THistogramLoop.h"
#include "TRawEventSource.h"
#include "TMultiRawFile.h"
#include "TOrderedRawFile.h"
#include "TRootInputLoop.h"
#include "TSequentialRawFile.h"
#include "TTerminalLoop.h"
#include "TUnpackLoop.h"
#include "TWriteLoop.h"

TPipeline::TPipeline()
  : is_online(false), time_order(false), time_order_depth(1000) { }

TPipeline::~TPipeline() {
  Stop();
  Join();
}

bool TPipeline::CanStart(bool print_reason) {
  if(!input_raw_files.size() && !input_root_files.size()){
    if(print_reason){
      std::cerr << "TPipeline: No input file given" << std::endl;
    }
    return false;
  }

  if(input_raw_files.size() && input_root_files.size()){
    if(print_reason){
      std::cerr << "TPipeline: Can't specify input root file and input raw file" << std::endl;
    }
    return false;
  }

  if(input_raw_files.size() > 1 && is_online){
    if(print_reason){
      std::cerr << "TPipeline: Can't open more than one raw file in online mode" << std::endl;
    }
    return false;
  }

  return true;
}

void TPipeline::Initialize() {
  if(!CanStart()){
    return;
  }

  if(input_raw_files.size()) {
    SetupRawReadLoop();
  } else if (input_root_files.size()) {
    SetupRootReadLoop();
  }

  SetupHistogramLoop();
  SetupOutputLoop();
}

void TPipeline::SetupRawReadLoop() {
  // I am sorting raw data
  TRawEventSource* event_source;
  if(input_raw_files.size() == 1){
    // Load a single data file
    event_source = OpenSingleFile(input_raw_files[0]);
  } else {
    // Load multiple sequential data files
    TSequentialRawFile* sequential = new TSequentialRawFile;
    for(auto& filename : input_raw_files){
      sequential->Add(OpenSingleFile(filename));
    }
    event_source = sequential;
  }

  TDataLoop* data_loop = new TDataLoop(event_source, raw_event_queue);
  pipeline.push_back(data_loop);
  TUnpackLoop* unpack_loop = new TUnpackLoop(raw_event_queue, unpacked_event_queue);
  pipeline.push_back(unpack_loop);
}

void TPipeline::SetupRootReadLoop() {
  TChain* event_tree = new TChain("EventTree");
  for(auto& filename : input_root_files) {
    event_tree->Add(filename.c_str());
  }

  TRootInputLoop* loop = new TRootInputLoop(event_tree);
  pipeline.push_back(loop);
}

void TPipeline::SetupHistogramLoop() {
  THistogramLoop* histogram_loop;
  if (histogram_library.length()){
    histogram_loop = new THistogramLoop(unpacked_event_queue,
                                        post_histogram_queue,
                                        histogram_library);
  } else {
    histogram_loop = new THistogramLoop(unpacked_event_queue,
                                        post_histogram_queue);
  }
  pipeline.push_back(histogram_loop);
}

void TPipeline::SetupOutputLoop() {
  if(output_root_file.length() && !input_root_files.size()){
    TWriteLoop* write_loop = new TWriteLoop(post_histogram_queue, output_root_file);
    pipeline.push_back(write_loop);
  } else {
    TTerminalLoop* terminal_loop = new TTerminalLoop(post_histogram_queue);
    pipeline.push_back(terminal_loop);
  }
}

TRawEventSource* TPipeline::OpenSingleFile(const std::string& filename) {
  TRawEventSource* output = TRawEventSource::EventSource(filename.c_str(), is_online);
  if(time_order){
    TOrderedRawFile* ordered = new TOrderedRawFile(output);
    ordered->SetDepth(time_order_depth);
    output = ordered;
  }
  return output;
}

void TPipeline::Start() { }

void TPipeline::Stop() { }

void TPipeline::Join() { }

void TPipeline::ProgressBar() { }

void TPipeline::AddRawDataFile(std::string filename) {
  input_raw_files.push_back(filename);
}

void TPipeline::SetOutputRootFile(std::string filename) {
  output_root_file = filename;
}

void TPipeline::SetHistogramLibrary(std::string filename) {
  histogram_library = filename;
}

void TPipeline::SetInputRing(std::string ringname) {
  input_ring = ringname;
}

void TPipeline::SetIsOnline(bool is_online) {
  this->is_online = is_online;
}

void TPipeline::SetTimeOrdering(bool time_order) {
  this->time_order = time_order;
}

void TPipeline::AddInputRootFile(std::string filename) {
  input_root_files.push_back(filename);
}
