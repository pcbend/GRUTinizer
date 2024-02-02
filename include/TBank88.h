#ifndef TBANK88_H
#define TBANK88_H

#include <TObject.h>
#include <TMath.h>

#include <TClonesArray.h>

#include "TDetector.h"
#include "TMode3.h"

class TBank88 : public TDetector {

public:
  TBank88();
  ~TBank88();

  virtual void Copy(TObject& obj) const;
  virtual void Clear(Option_t *opt = "");
  //virtual void Compare(const TObject&) const;
  virtual void Print(Option_t *opt = "") const;

  virtual void          InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(int i)        { return channels.at(i); }
  virtual TMode3Hit&    GetMode3Hit(int i)   { return channels.at(i); }

  Long_t GetTimestamp() { return Timestamp(); }
    //if(Size()>0) return ((TMode3Hit&)GetHit(0)).GetTimestamp(); else return -1; }

private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);
  std::vector<TMode3Hit> channels;
  ClassDef(TBank88,1);
};

#endif
