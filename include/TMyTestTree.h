#ifndef _TMYTESTTREE_H_
#define _TMYTESTTREE_H_

#include "TDirectory.h"
#include "TTree.h"

#include "TDetector.h"

class TMyTestTree : public TTree {
public:
  TMyTestTree(int circular_size = 32768);
  virtual ~TMyTestTree();

  void AddDetectorBranch(TDetector** det, const char* name);

  void PrintStatus() const;

  using TTree::Draw;

  virtual void Draw(Option_t* opt = ""){
    Draw(opt, "");
  }

  virtual Long64_t Draw(const char* varexp,
                        const char* selection,
                        Option_t* option = "",
                        Long64_t nentries = 1000000000,
                        Long64_t firstentry = 0);

  void RefillHistograms();

  virtual Int_t Fill();

  std::vector<std::string> GetStringLeaves();

  void cd(){ saved_dir = gDirectory; directory.cd(); }
  void popd() {
    if(saved_dir) {
      saved_dir->cd();
    }
    saved_dir = NULL;
  }

private:
  void recurse_down(std::vector<std::string>& terminal_leaves, std::string current_branch, TBranch* branch);

  TDirectory directory;
  TDirectory* saved_dir;

  int circular_size;

  struct HistPattern {
    TString varexp;
    TString selection;
  };

  //std::map<TObject*, HistPattern> hist_patterns;
  std::vector<HistPattern> hist_patterns;


  ClassDef(TMyTestTree,0);
};

#endif /* _TMYTESTTREE_H_ */
