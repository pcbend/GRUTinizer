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

#include "TPreserveGDirectory.h"

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
}

TOnlineTree::~TOnlineTree() { }

void TOnlineTree::AddDetectorBranch(TDetector** det, const char* name){
  Branch(name, name, det);
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

  if(actual_event_num - last_fill > circular_size * 0.7){
    RefillHistograms_MutexTaken();
  }

  return output;
}

void TOnlineTree::AddHistogram(const char* name,
                               int bins, double low, double high, const char* varexp,
                               const char* gate){
  TPreserveGDirectory preserve;
  directory.cd();

  HistPattern1D pat;

  pat.name = Form("+%s",name);
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
  TPreserveGDirectory preserve;
  directory.cd();

  HistPattern2D pat;

  pat.name = Form("+%s",name);
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

void TOnlineTree::RefillHistograms_MutexTaken() {
  TPreserveGDirectory preserve;
  directory.cd();

  for(auto& pattern : hist_patterns_1d) {
    TTree::Project(pattern.name.c_str(), pattern.varexp.c_str(),
                   pattern.gate, "", circular_size,
                   GetEntries() - (actual_event_num - last_fill));
  }

  for(auto& pattern : hist_patterns_2d) {
    TTree::Project(pattern.name.c_str(), pattern.varexp.c_str(),
                   pattern.gate, "", circular_size,
                   GetEntries() - (actual_event_num - last_fill));
  }

  last_fill = actual_event_num;
}

void TOnlineTree::RefillHistograms() {
  std::lock_guard<std::mutex> lock(fill_mutex);
  RefillHistograms_MutexTaken();
}
