#ifndef TGRANDRAIDEN_H
#define TGRANDRAIDEN_H

#include <TObject.h>
#include <TMath.h>

#include <TClonesArray.h>

#include "TDetector.h"
#include "TGrandRaidenHit.h"

class TGrandRaiden : public TDetector {

public:
  TGrandRaiden();
  ~TGrandRaiden();

  virtual void Copy(TObject& obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");

  virtual void          InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(int i)            { return GRHits.at(i); }

  const TGrandRaidenHit& GetGrandRaidenHit(int i) { return GRHits.at(i); }
  void PrintHit(int i){ GRHits.at(i).Print(); }

private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  std::vector<TGrandRaidenHit> GRHits;

  ClassDef(TGrandRaiden,1);
};





#endif
