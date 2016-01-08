#ifndef TPHOSWALL_H
#define TPHOSWALL_H

#include "TClonesArray.h"
#include "TDetector.h"
#include "TGEBEvent.h"
#include "TPhosWallHit.h"

#define MAXPIXEL 256

class TPhosWall : public TDetector {

public:
  TPhosWall();
  ~TPhosWall();

  virtual void Copy(TObject&) const;
  virtual void Clear(Option_t *opt = "");
  virtual void Print(Option_t *opt = "") const ;

  virtual void InsertHit(const TDetectorHit& hit);


  int  Size() const    { return phoswall_hits.size(); }

  Int_t GetLargestNumber()                  const { return fLargestHit; }
  TPhosWallHit& GetLargetHit()                    { return GetPhosWallHit(GetLargestNumber()); }
        TPhosWallHit& GetPhosWallHit(int i)       { return phoswall_hits.at(i); }
  const TPhosWallHit& GetPhosWallHit(int i) const { return phoswall_hits.at(i); }
  TDetectorHit& GetHit(int i)                     { return phoswall_hits.at(i); }

  //void Draw(Option_t *opt="");
  //void DrawXY(Option_t *opt="");

  //static TVector3 FindWallPosition(const Int_t &pixel) { if(pixel<0||pixel>256) return TDetector::fB;  return fWallPositions[pixel]; }
  //void FindWeightedPosition();
  //void SetWeightedPosition(const TVector3 &temp)  { fWeightedPosition = temp; }

  static TVector3 GetWallPosition(int pixelnumber,double delta=5.0);

private:
  virtual int BuildHits();

  Short_t  fLargestHit;
  Int_t    fNumberOfHits;

  std::vector<TPhosWallHit> phoswall_hits;

  static std::vector<TVector3> fWallPositions;
  static bool      fPositionsSet;
  static void      SetWallPositions();

  ClassDef(TPhosWall,3);
};

#endif
