#include "TOnlineTree.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include "TH1.h"
#include "TH2.h"
#include "TObject.h"
#include "TObjString.h"
#include "TRandom.h"
#include "TRegexp.h"
#include "TTreeFormula.h"

#include "TGRUTOptions.h"
#include "TPreserveGDirectory.h"
#include "TRuntimeObjects.h"


TOnlineTree* online_events = NULL;
TOnlineTree* online_scalers = NULL;

TOnlineTree::TOnlineTree(const char* name, const char* title, int circular_size)
  : TTree(name,title), directory(Form("%s_dir",name),Form("%s_dir",name)),
    event_num(0), actual_event_num(0), last_fill(0),
    circular_size(circular_size), saved_dir(NULL) {
  SetDirectory(0);
  SetCircular(circular_size);

  Branch("event_num", &event_num, "event_num/L");

  if(!strcmp(name,"EventTree")){
    online_events = this;
  } else if(!strcmp(name,"ScalerTree")){
    online_scalers = this;
  }

  detector_list.SetOwner(false);

  if(!strcmp(name,"EventTree") &&
     TGRUTOptions::Get()->CompiledHistogramFile().length()) {
    compiled_histograms.Load(TGRUTOptions::Get()->CompiledHistogramFile());
  }
}

TOnlineTree::~TOnlineTree() { }

void TOnlineTree::RegisterDetectorBranch(TDetector* det){
  detector_list.Add(det);
}

TObject* TOnlineTree::GetObjectStringLeaves(){
  std::stringstream ss;
  for(auto& elem : GetStringLeaves()){
    ss << elem << "\n";
  }
  return new TObjString(ss.str().c_str());
}


TObject* TOnlineTree::GetObjectStringLeaves(TTree *tree){
  std::stringstream ss;
  for(auto& elem : GetStringLeaves(tree)){
    ss << elem << "\n";
  }
  return new TObjString(ss.str().c_str());
}

std::vector<std::string> TOnlineTree::GetStringLeaves() {
  std::vector<std::string> output;

  TIter leaves(TTree::GetListOfBranches());
  TObject* obj = NULL;
  while((obj = leaves.Next())){
    recurse_down(output, obj->GetName(), (TBranch*)obj);
  }

  return output;
}

std::vector<std::string> TOnlineTree::GetStringLeaves(TTree *tree) {
  std::vector<std::string> output;

  TIter leaves(tree->GetListOfBranches());
  TObject* obj = NULL;
  while((obj = leaves.Next())){
    recurse_down(output, obj->GetName(), (TBranch*)obj);
  }

  return output;
}

void TOnlineTree::recurse_down(std::vector<std::string>& terminal_leaves, std::string current_branch, TBranch* branch){
  if(branch->GetListOfBranches()->GetEntries() == 0){
    terminal_leaves.push_back(current_branch);
  } else {

    TIter leaves(branch->GetListOfBranches());
    TObject* obj = NULL;
    while((obj = leaves.Next())){
      recurse_down(terminal_leaves, current_branch + "." + obj->GetName(), (TBranch*)obj);
    }
  }
}

TList* TOnlineTree::GetHistograms() {
  std::lock_guard<std::mutex> lock(fill_mutex);

  TList* output = new TList;
  output->SetOwner(false);

  TIter iter(directory.GetList());
  TObject* obj = NULL;
  while((obj = iter.Next())){
    if(obj->InheritsFrom(TH1::Class())){
      output->Add(obj);
    }
  }

  return output;
}

Int_t TOnlineTree::Fill(){
  std::lock_guard<std::mutex> lock(fill_mutex);

  event_num = actual_event_num++;
  Int_t output = TTree::Fill();

  FillCompiledHistograms();

  if(actual_event_num - last_fill > circular_size * 0.7){
    FillParsedHistograms_MutexTaken();
  }

  return output;
}

void TOnlineTree::FillCompiledHistograms() {
  TRuntimeObjects obj(&detector_list, directory.GetList());
  compiled_histograms.Call(obj);
}

void TOnlineTree::AddHistogram(const char* name,
                               int bins, double low, double high, const char* varexp,
                               const char* gate){
  std::lock_guard<std::mutex> lock(fill_mutex);

  TPreserveGDirectory preserve;
  directory.cd();

  HistPattern1D pat;

  pat.name = name;
  pat.varexp = varexp;
  pat.bins = bins;
  pat.low = low;
  pat.high = high;
  pat.gate = gate;

  // The histogram will be grabbed by the gDirectory, no memory leak
  new TH1I(name,name,
           bins, low, high);
  hist_patterns_1d.push_back(pat);
}

void TOnlineTree::AddHistogram(const char* name,
                               int binsX, double lowX, double highX, const char* varexpX,
                               int binsY, double lowY, double highY, const char* varexpY,
                               const char* gate){
  std::lock_guard<std::mutex> lock(fill_mutex);

  TPreserveGDirectory preserve;
  directory.cd();

  HistPattern2D pat;

  pat.name = name;
  pat.gate = gate;

  pat.varexp = Form("%s:%s",varexpY,varexpX);

  pat.varexpX = varexpX;
  pat.binsX = binsX;
  pat.lowX = lowX;
  pat.highX = highX;

  pat.varexpY = varexpY;
  pat.binsY = binsY;
  pat.lowY = lowY;
  pat.highY = highY;

  // The histogram will be grabbed by the gDirectory, no memory leak
  new TH2I(name,name,
           binsX, lowX, highX,
           binsY, lowY, highY);

  hist_patterns_2d.push_back(pat);
}

void TOnlineTree::FillParsedHistograms_MutexTaken() {
  TPreserveGDirectory preserve;
  directory.cd();

  for(auto& pattern : hist_patterns_1d) {
    TTree::Project(("+"+pattern.name).c_str(), pattern.varexp.c_str(),
                   pattern.gate, "", circular_size,
                   GetEntries() - (actual_event_num - last_fill));
  }

  for(auto& pattern : hist_patterns_2d) {
    TTree::Project(("+"+pattern.name).c_str(), pattern.varexp.c_str(),
                   pattern.gate, "", circular_size,
                   GetEntries() - (actual_event_num - last_fill));
  }
  last_fill = actual_event_num;
}

bool TOnlineTree::HasHistogram(std::string name) {
  return directory.GetList()->FindObject(name.c_str());
}

void TOnlineTree::FillParsedHistograms() {
  std::lock_guard<std::mutex> lock(fill_mutex);
  FillParsedHistograms_MutexTaken();
}

std::string TOnlineTree::GetHistPattern(std::string name) {
  //TODO: Change this to a std::map lookup
  for(auto& pat : hist_patterns_1d) {
    if(pat.name == name){
      std::stringstream ss;
      ss << GetName() << "\n";
      ss << pat.name << "\n";
      ss << pat.gate << "\n";
      ss << pat.varexp << "\n";
      ss << pat.bins << "\n";
      ss << pat.low << "\n";
      ss << pat.high;
      return ss.str();
    }
  }

  for(auto& pat : hist_patterns_2d) {
    if(pat.name == name){
      std::stringstream ss;
      ss << GetName() << "\n";
      ss << pat.name << "\n";
      ss << pat.gate << "\n";
      ss << pat.varexpX << "\n";
      ss << pat.binsX << "\n";
      ss << pat.lowX << "\n";
      ss << pat.highX << "\n";
      ss << pat.varexpY << "\n";
      ss << pat.binsY << "\n";
      ss << pat.lowY << "\n";
      ss << pat.highY;
      return ss.str();
    }
  }

  return "";
}
