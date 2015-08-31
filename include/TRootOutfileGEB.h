#ifndef _TROOTOUTFILEGEB_H_
#define _TROOTOUTFILEGEB_H_

#include <map>

#include "TFile.h"
#include "TObject.h"
#include "TTree.h"

#include "TRootOutfile.h"
#include "TGRUTTypes.h"
#include "TDetector.h"
#include "TRawEvent.h"

class TGretina;
//class TPhosWall;
//class TS800;

class TRootOutfileGEB : public TRootOutfile {
public:
  TRootOutfileGEB();
  ~TRootOutfileGEB();

  virtual void Init(const char* output_filename);
  virtual void InitHists();
  virtual void FillHists();

  void Clear(Option_t* option = "");
  void Print(Option_t* option = "") const;

private:
  TGretina*  gretina;
  // TS800*     s800;
  // TPhoswall* phoswall;

  TFile* outfile;

  ClassDef(TRootOutfileGEB, 0);
};

#endif /* _TROOTOUTFILE_H_ */
