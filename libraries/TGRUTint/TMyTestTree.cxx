#include "TMyTestTree.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include "TH1.h"
#include "TH2.h"
#include "TObject.h"
#include "TRandom.h"
#include "TRegexp.h"

#include "TPreserveGDirectory.h"

TMyTestTree::TMyTestTree(const char* name, int circular_size)
  : TTree(name,name), directory(Form("%s_dir",name),Form("%s_dir",name)),
    event_num(0), actual_event_num(0), last_fill(0),
    circular_size(circular_size), saved_dir(NULL) {
  SetDirectory(0);
  SetCircular(circular_size);

  Branch("event_num", &event_num, "event_num/I");
}

TMyTestTree::~TMyTestTree() { }

void TMyTestTree::AddDetectorBranch(TDetector** det, const char* name){
  Branch(name, name, det);
}

std::vector<std::string> TMyTestTree::GetStringLeaves() {
  std::vector<std::string> output;

  TIter leaves(TTree::GetListOfBranches());
  TObject* obj = NULL;
  while((obj = leaves.Next())){
    recurse_down(output, obj->GetName(), (TBranch*)obj);
  }

  for(auto& name : output){
    std::cout << name << std::endl;
  }

  return output;
}

void TMyTestTree::recurse_down(std::vector<std::string>& terminal_leaves, std::string current_branch, TBranch* branch){
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


Int_t TMyTestTree::Fill(){
  event_num = actual_event_num++;
  std::cout << "Filling at " << event_num << std::endl;
  Int_t output = TTree::Fill();

  if(actual_event_num - last_fill > circular_size * 0.7){
    RefillHistograms();
  }

  return output;
}

void TMyTestTree::AddHistogram(const char* name,
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

void TMyTestTree::AddHistogram(const char* name,
                               int binsX, double lowX, double highX, const char* varexpX,
                               int binsY, double lowY, double highY, const char* varexpY,
                               const char* gate){
  TPreserveGDirectory preserve;
  directory.cd();

  HistPattern2D pat;

  pat.name = Form("+%s",name);
  pat.gate = gate;

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

void TMyTestTree::RefillHistograms() {
  TPreserveGDirectory preserve;
  directory.cd();

  std::cout << "Last Fill: " << last_fill
            << "\tActual Event Num: " << actual_event_num
            << std::endl;

  for(auto& pattern : hist_patterns_1d) {
    TTree::Project(pattern.name.c_str(), pattern.varexp.c_str(),
                   pattern.gate && Form("event_num>=%d",last_fill));
  }

  for(auto& pattern : hist_patterns_2d) {
    TTree::Project(pattern.name.c_str(), (pattern.varexpY + ":" + pattern.varexpX).c_str(), pattern.gate);
  }

  // std::cout << "Last Fill: " << last_fill
  //           << "\tEvent Num: " << event_num
  //           << "\tActual Event Num: " << actual_event_num
  //           << std::endl;
  last_fill = actual_event_num;
}
