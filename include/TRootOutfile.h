#ifndef _TROOTOUTFILE_H_
#define _TROOTOUTFILE_H_

#include <map>

#include "TFile.h"
#include "TObject.h"
#include "TTree.h"
#include "TList.h"

#include "TGRUTTypes.h"
#include "TDetector.h"
#include "TRawEvent.h"

class TRootOutfile : public TObject {
public:
  TRootOutfile();
  ~TRootOutfile();

  // Needs to be made for each.
  virtual void Init(const char* output_filename) = 0;
  virtual void FillHists() = 0;

  // Common to all.
  void AddRawData(const TRawEvent& event, kDetectorSystems det_type);
  virtual void FillTree(const char* tname, long next_timestamp=-1);   //takes from det_list;
  virtual void FillAllTrees();
  virtual void FinalizeFile();
  virtual void CloseFile();

  virtual void Clear(Option_t* option = "");  // Clears det_list.
  virtual void Print(Option_t* option = "") const;

protected:
  bool BuildCondition(TRawEvent& new_event);

  void AddTree(const char *tname,const char *ttitle=0,bool build=false,int build_window=-1,
               bool is_online = false, int circular_size = 10000);
  void AddBranch(const char* treename, const char* branchname, const char* classname,
                 TDetector** obj, kDetectorSystems det_system);

  TTree *FindTree(const char *tname);

  std::map<std::string,TList*> hist_list;

  void   SetOutfile(const char *fname);
  TFile *GetOutfile()  { return outfile; }

private:
  void UpdateDetList(kDetectorSystems det_system, TDetector* detector, const char* tree_name);

  struct tree_element{
    TTree* tree;
    // Build size, in timestamp units
    // If build_window is -1, then each event will be filled as they are read
    int build_window;
    long event_build_window_close;
    bool build_det;
    bool has_data;
  };

  struct det_list_element{
    TDetector* det;
    tree_element* tree_elem;
  };

  tree_element* FindTreeElement(const char* tname);

  std::map<kDetectorSystems,det_list_element> det_list;
  std::map<std::string, tree_element> trees;

  TFile* outfile;

  ClassDef(TRootOutfile, 0);
};

#endif /* _TROOTOUTFILE_H_ */
