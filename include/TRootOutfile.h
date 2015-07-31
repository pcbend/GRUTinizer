#ifndef _TROOTOUTFILE_H_
#define _TROOTOUTFILE_H_

#include <map>

#include "TFile.h"
#include "TObject.h"
#include "TTree.h"

#include "TGRUTTypes.h"
#include "TDetector.h"
#include "TRawEvent.h"

class TJanus;
class TSega;

class TRootOutfile : public TObject {
public:
  TRootOutfile();
  ~TRootOutfile();

  void Init(const char* output_filename);

  void AddRawData(const TRawEvent& event, kDetectorSystems det_type);
  void FillTree();

  void FinalizeFile();
  void CloseFile();

  void Print(Option_t* option = "");

private:
  std::map<kDetectorSystems, TDetector*> det_list;

  TJanus*    janus;
  TSega*     sega;
  // TS800*     s800;
  // TGretina*  gretina;
  // TCaesar*   caesar;
  // TPhoswall* phoswall;

  TTree* event_tree;
  TTree* scaler_tree;

  TFile* outfile;


  ClassDef(TRootOutfile, 0);
};

#endif /* _TROOTOUTFILE_H_ */
