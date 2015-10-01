#ifndef _TONLINETREE_H_
#define _TONLINETREE_H_

#include "TCut.h"
#include "TDirectory.h"
#include "TTree.h"

#include "TDetector.h"

class TOnlineTree : public TTree {
public:
  TOnlineTree(const char* name = "circular", const char* title = "circular",
              int circular_size = 32768);
  virtual ~TOnlineTree();

  void AddDetectorBranch(TDetector** det, const char* name);

  void AddHistogram(const char* name,
                    int bins, double low, double high, const char* varexp,
                    const char* gate = "");

  void AddHistogram(const char* name,
                    int binsX, double lowX, double highX, const char* varexpX,
                    int binsY, double lowY, double highY, const char* varexpY,
                    const char* gate = "");

  void RefillHistograms();

  virtual Int_t Fill();

  std::vector<std::string> GetStringLeaves();
  TObject* GetObjectStringLeaves();

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

  long event_num;
  long actual_event_num; // Because ROOT is obnoxious, see the src file
  long last_fill;
  int circular_size;

  struct HistPattern1D {
    std::string name;
    TCut gate;
    std::string varexp;
    int bins;
    double low, high;
  };

  struct HistPattern2D {
    std::string name;
    TCut gate;
    std::string varexp;
    std::string varexpX;
    int binsX;
    double lowX, highX;
    std::string varexpY;
    int binsY;
    double lowY, highY;
  };

  std::vector<HistPattern1D> hist_patterns_1d;
  std::vector<HistPattern2D> hist_patterns_2d;


  ClassDef(TOnlineTree,0);
};

extern TOnlineTree* online_events;
extern TOnlineTree* online_scalers;

#endif /* _TONLINETREE_H_ */
