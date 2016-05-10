#ifndef TARGONNE_H
#define TARGONNE_H

#include <TObject.h>
#include <TMath.h>

#include <TClonesArray.h>

#include "TDetector.h"
#include "TArgonneHit.h"

class TArgonne : public TDetector {

public:
  TArgonne();
  ~TArgonne();

  virtual void Copy(TObject& obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");

  virtual void          InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(int i)            { return anl_hits.at(i); }

  const TArgonneHit& GetArgonneHit(int i) { return anl_hits.at(i); }
  void PrintHit(int i){ anl_hits.at(i).Print(); }

private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  std::vector<TArgonneHit> anl_hits;

  ClassDef(TArgonne,1);
};





#endif
