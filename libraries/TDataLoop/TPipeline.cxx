#include "TPipeline.h"

#include <iostream>
#include <sstream>

#include "TChain.h"
#include "TFile.h"

#include "TDataLoop.h"
#include "THistogramLoop.h"
#include "TRawEventSource.h"
#include "TMultiRawFile.h"
#include "TOrderedRawFile.h"
#include "TPreserveGDirectory.h"
#include "TRootInputLoop.h"
#include "TSequentialRawFile.h"
#include "TTerminalLoop.h"
#include "TUnpackLoop.h"
#include "TWriteLoop.h"

TPipeline::TPipeline()
  : output_file(NULL), output_directory(NULL),
    data_loop(NULL), unpack_loop(NULL), root_input_loop(NULL),
    histogram_loop(NULL), write_loop(NULL), terminal_loop(NULL),
    is_online(false), time_order(false), time_order_depth(1000) { }

TPipeline::~TPipeline() {
  Stop();
  Join();
  Write();

  if(output_file){
    output_file->Close();
    delete output_file;
  } else if(output_directory) {
    delete output_directory;
  }

  for(auto thread : pipeline){
    delete thread;
  }
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

int TPipeline::Initialize() {
  if(!CanStart()){
    return 1;
  }

  if(input_raw_files.size()) {
    SetupRawReadLoop();
  } else if (input_root_files.size()) {
    SetupRootReadLoop();
  }

  TPreserveGDirectory preserve;
  SetupOutputFile();
  GetDirectory()->cd();

  SetupHistogramLoop();
  SetupOutputLoop();

  return 0;
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

  data_loop = new TDataLoop(event_source, raw_event_queue);
  pipeline.push_back(data_loop);
  unpack_loop = new TUnpackLoop(raw_event_queue, unpacked_event_queue);
  pipeline.push_back(unpack_loop);
}

void TPipeline::SetupRootReadLoop() {
  TChain* event_tree = new TChain("EventTree");
  for(auto& filename : input_root_files) {
    TFile file(filename.c_str());
    TObject* obj =file.Get("EventTree");
    file.Close();

    if(obj){
      event_tree->Add(filename.c_str());
    }
  }

  root_input_loop = new TRootInputLoop(event_tree, unpacked_event_queue);
  pipeline.push_back(root_input_loop);
}

void TPipeline::SetupOutputFile() {
  if(output_root_file.length()) {
    output_file = new TFile(output_root_file.c_str(), "RECREATE");
    output_directory = output_file;
  } else {
    output_directory = new TDirectory("_", "_");
  }
}

TDirectory* TPipeline::GetDirectory() {
  return output_directory;
}

void TPipeline::SetupHistogramLoop() {
  histogram_loop = new THistogramLoop(unpacked_event_queue,
                                      post_histogram_queue,
                                      GetDirectory());
  if (histogram_library.length()){
    histogram_loop->LoadLibrary(histogram_library);
  }
  pipeline.push_back(histogram_loop);
}

void TPipeline::SetupOutputLoop() {
  if(output_root_file.length() && !input_root_files.size()){
    write_loop = new TWriteLoop(post_histogram_queue, *GetDirectory());
    pipeline.push_back(write_loop);
  } else {
    terminal_loop = new TTerminalLoop(post_histogram_queue);
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

bool TPipeline::IsFinished() {
  if(pipeline.size()){
    // Finished if the data loop has stopped running
    //   and all queues are empty.
    return (!pipeline[0]->IsRunning() &&
            AllQueuesEmpty());
  } else {
    return true;
  }
}

bool TPipeline::AllQueuesEmpty() {
  return !(raw_event_queue.Size() ||
           unpacked_event_queue.Size() ||
           post_histogram_queue.Size());
}

void TPipeline::Start() {
  int error = Initialize();
  if(error){
    return;
  }

  Resume();

  // TODO: Use mutexes to prevent this sleep from being needed
  //       The problem is that TTree::Branch is not threadsafe.
  //       I can't find everywhere that needs a mutex to prevent this.
  while(InLearningPhase()){
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void TPipeline::Stop() {
  for(auto thread : pipeline){
    thread->Stop();
  }
}

void TPipeline::Pause() {
  for(auto thread : pipeline){
    thread->Pause();
  }
}

void TPipeline::Resume() {
  for(auto thread : pipeline){
    thread->Resume();
  }
}

void TPipeline::Join() {
  for(auto thread : pipeline){
    thread->Join();
  }
}

std::string TPipeline::Status() {
  // TODO: Give clear summary of each queue, not just the first element of the loop.
  // std::stringstream ss;
  // ss << raw_event_queue.ItemsPushed() << "/" << raw_event_queue.ItemsPopped() << "\t"
  //    << unpacked_event_queue.ItemsPushed() << "/" << unpacked_event_queue.ItemsPopped() << "\t"
  //    << post_histogram_queue.ItemsPushed() << "/" << post_histogram_queue.ItemsPopped();
  // return ss.str();

  if(pipeline.size()){
    return pipeline[0]->Status();
  } else {
    return "";
  }
}

void TPipeline::Write() {
  TPreserveGDirectory(preserve);
  GetDirectory()->cd();
  if(histogram_loop) {
    histogram_loop->Write();
  }
  if(write_loop) {
    write_loop->Write();
  }
}

void TPipeline::AddRawDataFile(std::string filename) {
  input_raw_files.push_back(filename);
}

void TPipeline::SetOutputRootFile(std::string filename) {
  output_root_file = filename;
}

void TPipeline::SetHistogramLibrary(std::string filename) {
  histogram_library = filename;
  if(histogram_loop) {
    histogram_loop->LoadLibrary(filename);
  }
}

std::string TPipeline::GetHistogramLibrary() const {
  return histogram_library;
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

void TPipeline::ClearHistograms() {
  if(histogram_loop) {
    histogram_loop->ClearHistograms();
  }
}

void TPipeline::ReplaceRawDataFile(std::string filename) {
  TRawEventSource* event_source = OpenSingleFile(filename);
  delete data_loop;
  data_loop = new TDataLoop(event_source, raw_event_queue);
}


void TPipeline::SetReplaceVariable(const char* name, double value){
  if(histogram_loop) {
    histogram_loop->SetReplaceVariable(name, value);
  }
}

void TPipeline::RemoveVariable(const char* name) {
  if(histogram_loop) {
    histogram_loop->RemoveVariable(name);
  }
}

TList* TPipeline::GetVariables() {
  if(histogram_loop) {
    return histogram_loop->GetVariables();
  } else {
    return NULL;
  }
}

bool TPipeline::InLearningPhase() {
  if(write_loop){
    return write_loop->InLearningPhase();
  } else {
    return false;
  }
}
