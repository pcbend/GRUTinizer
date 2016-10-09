#ifndef TS800SIM_H
#define TS800SIM_H

#include <TObject.h>
#include <TMath.h>

#include <TClonesArray.h>

#include "TDetector.h"
#include "TS800SimHit.h"
#include "TGEBEvent.h"

class TS800Sim : public TDetector {

public:
  TS800Sim();
  ~TS800Sim();

  virtual void Copy(TObject& obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");
  virtual UInt_t Size() const { return s800sim_hits.size(); }
 
  virtual void          InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(int i)            { return s800sim_hits.at(i); }

  const TS800SimHit& GetS800SimHit(int i) { return s800sim_hits.at(i); }
  void PrintHit(int i){ s800sim_hits.at(i).Print(); }

 private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);
  void BuildFrom(TGEBEvent &event);

  std::vector<TS800SimHit> s800sim_hits;

  ClassDef(TS800Sim,1);
};





#endif
