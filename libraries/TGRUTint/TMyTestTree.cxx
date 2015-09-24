#include "TMyTestTree.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "TObject.h"
#include "TRandom.h"
#include "TRegexp.h"

TMyTestTree::TMyTestTree(int circular_size)
  : TTree("t","t"), circular_size(circular_size), saved_dir(NULL) {
  SetDirectory(0);
  SetCircular(circular_size);
}

TMyTestTree::~TMyTestTree(){}

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
  Int_t output = TTree::Fill();

  if(GetEntries() > circular_size * 0.9){
    RefillHistograms();
  }

  return output;
}

Long64_t TMyTestTree::Draw(const char* varexp, const char* selection,
                           Option_t* option, Long64_t nentries, Long64_t firstentry){
  HistPattern new_pat;
  new_pat.varexp = varexp;

  TRegexp re(">>[-+]?");
  new_pat.varexp(re) = ">>+";

  new_pat.selection = selection;
  hist_patterns.push_back(new_pat);
}

void TMyTestTree::RefillHistograms() {
  TDirectory* bak = gDirectory;
  directory.cd();
  for(auto& pattern : hist_patterns) {
    gDirectory->ls();
    //TTree::Project(pattern.varexp.Data(), pattern.selection.Data());
    TTree::Draw(pattern.varexp.Data(), pattern.selection.Data(), "goff");
    gDirectory->ls();
  }
  SetEntries(0);
  bak->cd();
}
