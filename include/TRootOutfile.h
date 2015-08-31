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
    virtual void FillHits() = 0;

    // Common to all. 
    void AddRawData(const TRawEvent& event, kDetectorSystems det_type);
    virtual void FillTree();   //takes from det_list;
    virtual void FinalizeFile();
    virtual void CloseFile();

    virtual void Clear(Option_t* option = "");  // Clears det_list.
    virtual void Print(Option_t* option = "") const;

  protected:
    TTree *AddTree(const char *tname);
    TTree *FindTree(const char *tname); 
    std::map<kDetectorSystems,TDetector*> det_list;
    std::map<std::string,TList> hist_list;
    void   SetOutfile(const char *fname) { outfile = new TFile(fname,"recreate"); }
    TFile *GetOutfile()  { return outfile; }

  private:
    TList tree_list;
   
    TFile* outfile;
   
  ClassDef(TRootOutfile, 0);
};

#endif /* _TROOTOUTFILE_H_ */
