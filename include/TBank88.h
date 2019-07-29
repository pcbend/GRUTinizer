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

  //void PrintHit(int i){ gretina_hits->At(i)->Print(); }

  Long_t GetTimestamp() { return Timestamp(); }
    //if(Size()>0) return ((TMode3Hit&)GetHit(0)).GetTimestamp(); else return -1; }
 
  double GetCharge()       const { return channels.at(0).Charge(); }  ///this prob shouldn't be zero....
  const TMode3Hit &GetHit(int i) const { return channels.at(i); }
  size_t Size() const { return channels.size(); } 

private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  std::vector<TMode3Hit> channels;

  ClassDef(TBank88,1);
};





#endif
