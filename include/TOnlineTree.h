#ifndef _TONLINETREE_H_
#define _TONLINETREE_H_

#ifndef __CINT__
#include <mutex>
#endif

#include <map>
#include <utility>

#include "TCut.h"
#include "TDirectory.h"
#include "TTree.h"

#include "TCompiledHistograms.h"
#include "TDetector.h"

class TOnlineTree : public TTree {
public:
  TOnlineTree(const char* name = "circular", const char* title = "circular",
              int circular_size = 32768);
  virtual ~TOnlineTree();

  void RegisterDetectorBranch(TDetector* det);

  void AddHistogram(const char* name,
                    int bins, double low, double high, const char* varexp,
                    const char* gate = "");

  void AddHistogram(const char* name,
                    int binsX, double lowX, double highX, const char* varexpX,
                    int binsY, double lowY, double highY, const char* varexpY,
                    const char* gate = "");

  bool HasHistogram(std::string name);
  void FillParsedHistograms();

  virtual Int_t Fill();

  std::vector<std::string> GetStringLeaves();
  TObject* GetObjectStringLeaves();

  static std::vector<std::string> GetStringLeaves(TTree*);
  static TObject* GetObjectStringLeaves(TTree*);

  TList* GetHistograms();

  TList* GetVariables();
  void SetVariable(const char* name, double value);
  void RemoveVariable(const char* name);

  void cd(){ saved_dir = gDirectory; directory.cd(); }
  void popd() {
    if(saved_dir) {
      saved_dir->cd();
    }
    saved_dir = NULL;
  }

  TDirectory& GetDirectory() { return directory; }

  std::string GetHistPattern(std::string name);

  std::string GetCompiledHistogramLibrary() const;
  void LoadCompiledHistogramLibrary(const std::string& filename);

  void ClearHistograms();

private:
  static void recurse_down(std::vector<std::string>& terminal_leaves, std::string current_branch, TBranch* branch);

  void FillCompiledHistograms();
  void FillParsedHistograms_MutexTaken();

#ifndef __CINT__
  std::mutex fill_mutex;
#endif

  TList detector_list;
  TList variable_list;
  TCompiledHistograms compiled_histograms;

  TDirectory  directory;
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
