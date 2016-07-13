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
  // Allows for looping over all hits with for(auto& hit : gr) { }
  std::vector<TGrandRaidenHit>::iterator begin() { return GRHits.begin(); }
  std::vector<TGrandRaidenHit>::iterator end() { return GRHits.end(); }



  bool CheckHit(UInt_t i) { return (i < GRHits.size()) ? true : false; }
  RCNPEvent* GetGRHit(UInt_t i) { return GRHits.at(i).GR(); }

private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  std::vector<TGrandRaidenHit> GRHits;

  ClassDef(TGrandRaiden,1);
};

#endif
