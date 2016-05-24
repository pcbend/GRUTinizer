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
  virtual unsigned int Size() const { return mode3_hits.size(); }

  virtual void          InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(int i)            { return mode3_hits.at(i); }


  const TMode3Hit& GetMode3Hit(int i) { return mode3_hits.at(i); }
  void PrintHit(int i){ mode3_hits.at(i).Print(); }



private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  //TClonesArray* mode3_hits;//("TMode3Hit");
  std::vector<TMode3Hit> mode3_hits;

  ClassDef(TMode3,3);
};





#endif
