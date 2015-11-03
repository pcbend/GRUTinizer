#include "TRootInputLoop.h"

#include <sstream>

#include "TBranchElement.h"

TRootInputLoop::TRootInputLoop(TTree* tree)
  : tree(tree), ientry(0), nentries(tree->GetEntries()) {
  SetupBranches();
}

TRootInputLoop::~TRootInputLoop() {
  delete tree;
}

bool TRootInputLoop::Iteration() {
  if(ientry >= nentries){
    return false;
  }

  // Construct each object, read from tree.
  for(auto& elem : det_map) {
    *elem.second = (TDetector*)elem.first->New();
  }
  tree->GetEntry(ientry);

  // Load into unpacked event, push into queue
  TUnpackedEvent* event = new TUnpackedEvent;
  for(auto& elem : det_map) {
    event->AddDetector(*elem.second);
  }
  output_queue.Push(event);

  ientry++;
  return true;
}

void TRootInputLoop::SetupBranches() {
  TIter branch_iter(tree->GetListOfBranches());
  TObject* obj;
  while((obj = branch_iter())) {
    TBranchElement* branch = (TBranchElement*)obj;
    TClass* cls = branch->GetClass();
    TDetector** det = new TDetector*;
    *det = NULL;
    det_map[cls] = det;
    tree->SetBranchAddress(branch->GetName(), det);
  }
}

std::string TRootInputLoop::Status() {
  std::stringstream ss;
  ss << tree->GetName() << ": "
     << std::setw(8) << ientry << " / " << nentries;
  return ss.str();
}
