#ifndef TBANK29_H
#define TBANK29_H

#include <TObject.h>
#include <TMath.h>

#include <TClonesArray.h>

#include "TDetector.h"
#include "TMode3.h"

class TBank29 : public TDetector {

public:
  TBank29();
  ~TBank29();

  virtual void Copy(TObject& obj) const;
  virtual void Clear(Option_t *opt = "");
  //virtual void Compare(const TObject&) const;
  virtual void Print(Option_t *opt = "") const;

  virtual void          InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(int i)        { return *(TDetectorHit*)channels->At(i); }
  virtual int           Size()  const        { return channels->GetEntries(); }

  //void PrintHit(int i){ gretina_hits->At(i)->Print(); }

private:
  virtual int BuildHits();

  TClonesArray* channels;//("TMode3Hit");

  ClassDef(TBank29,1);
};





#endif
