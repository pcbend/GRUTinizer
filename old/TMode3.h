#ifndef TMODE3_H
#define TMODE3_H

#include <TObject.h>
#include <TMath.h>

#include <TClonesArray.h>

#include "TDetector.h"
#include "TMode3Hit.h"

class TMode3 : public TDetector {

public:
  TMode3();
  ~TMode3();

  virtual void Copy(TObject& obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");

  virtual void          InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(const int &i)            { return *(TMode3Hit*)mode3_hits->At(i); }

  const TMode3Hit& GetMode3Hit(int i) { return *(TMode3Hit*)mode3_hits->At(i); }
  void PrintHit(int i){ mode3_hits->At(i)->Print(); }

private:
  virtual int BuildHits();

  TClonesArray* mode3_hits;//("TMode3Hit");

  ClassDef(TMode3,3);
};





#endif
