#ifndef TGRETINA_H
#define TGRETINA_H

#include <TObject.h>
#include <TMath.h>

#include <TClonesArray.h>

#include "TDetector.h"
#include "TGretinaHit.h"

class TGretina : public TDetector {

public:
  TGretina();
  ~TGretina();

  virtual void Copy(TObject& obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");

  virtual void          InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(int i)            { return gretina_hits.at(i); }

  const TGretinaHit& GetGretinaHit(int i) { return gretina_hits.at(i); }
  //const TGretinaHit& GetAddbackHit(int i) { return *(TGretinaHit*)addback_hits.at(i); }
  void PrintHit(int i){ gretina_hits.at(i).Print(); }

  static TVector3 CrystalToGlobal(int cryId,
                                  Float_t localX=0,Float_t localY=0,Float_t localZ=0);
  static TVector3 GetSegmentPosition(int cryid,int segment); //return the position of the segemnt in the lab system
  static TVector3 GetCrystalPosition(int cryid); //return the position of the crysal in the lab system

  static void DrawDopplerGamma(Double_t Beta=0.1,Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,TChain *chain=0);
  static void DrawDopplerBeta(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,TChain *chain=0);
  static void DrawEnVsTheta(Double_t Beta=0.1,Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,TChain *chain=0);
  static void DrawCoreSummary(Option_t *gate="",Option_t *opt="",Long_t entries=kMaxLong,TChain *chain=0);



private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);
  //void BuildAddbackHits();

  std::vector<TGretinaHit> gretina_hits;
  //TClonesArray* addback_hits;//("TGretinaHit");

  static Float_t crmat[32][4][4][4];
  static Float_t m_segpos[2][36][3];
  static void SetCRMAT();
  static void SetSegmentCRMAT();
  static bool fCRMATSet;


  ClassDef(TGretina,3);
};





#endif
