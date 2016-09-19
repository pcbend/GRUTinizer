#ifndef TGRETSIM_H
#define TGRETSIM_H

#include <TObject.h>
#include <TMath.h>

#include <TClonesArray.h>

#include "TDetector.h"
#include "TGretSimHit.h"
#include "TGEBEvent.h"

class TGretSim : public TDetector {

public:
  TGretSim();
  ~TGretSim();

  virtual void Copy(TObject& obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");

  virtual UInt_t Size() const { return gretsim_hits.size(); }
  virtual void          InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(int i)            { return gretsim_hits.at(i); }

  const TGretSimHit& GetGretinaSimHit(int i) { return gretsim_hits.at(i); }
  void PrintHit(int i){ gretsim_hits.at(i).Print(); }

  


  /* I dont think we need these, but we can model some
     useful stuff off of this maybe
  static TVector3 CrystalToGlobal(int cryId,
                                  Float_t localX=0,Float_t localY=0,Float_t localZ=0);
  static TVector3 GetSegmentPosition(int cryid,int segment); //return the position of the segemnt in the lab system
  static TVector3 GetCrystalPosition(int cryid); //return the position of the crysal in the lab system*/

private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);
  //int Build_From(TNSCLEvent &event,bool Zero_Suppress = false);
  void BuildFrom(TGEBEvent &event);

  //void BuildAddbackHits();

  std::vector<TGretSimHit> gretsim_hits;
  //TClonesArray* addback_hits;//("TGretSimHit");

  /* for now I dont know if we need these
     so Ill take it out for simplicity
  static Float_t crmat[32][4][4][4];
  static Float_t m_segpos[2][36][3];
  static void SetCRMAT();
  static void SetSegmentCRMAT();
  static bool fCRMATSet;*/

  ClassDef(TGretSim,1);
};





#endif
