#ifndef TCAGRA_H
#define TCAGRA_H

#include <TObject.h>
#include <TMath.h>

#include "TDetector.h"
#include "TCAGRAHit.h"

#include "TVector3.h"

class TCAGRA : public TDetector {

public:
  TCAGRA();
  ~TCAGRA();

  virtual void Copy(TObject& obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");

  virtual void          InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(int i)            { return cagra_hits.at(i); }

  const TCAGRAHit& GetCAGRAHit(int i) { return cagra_hits.at(i); }
  void PrintHit(int i){ cagra_hits.at(i).Print(); }

  static TVector3 GetSegmentPosition(int detnum, char subpos, int segnum);
  // Allows for looping over all hits with for(auto& hit : cagra) { }
  std::vector<TCAGRAHit>::iterator begin() { return cagra_hits.begin(); }
  std::vector<TCAGRAHit>::iterator end() { return cagra_hits.end(); }


private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);
  static void LoadDetectorPositions();

  std::vector<TCAGRAHit> cagra_hits;
  static std::map<int,TVector3> detector_positions;

  ClassDef(TCAGRA,1);
};





#endif
