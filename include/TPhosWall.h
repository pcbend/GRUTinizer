#ifndef TPHOSWALL_H
#define TPHOSWALL_H

#include <cmath>

#include "TChain.h"
#include "TDetector.h"
#include "TGEBEvent.h"
#include "TPhosWallHit.h"
#include "TNucleus.h"

#define MAXPIXEL 256

class TNucleus;
class TCutG;

class TPhosWall : public TDetector {

public:
  TPhosWall();
  ~TPhosWall();

  virtual void Copy(TObject&) const;
  virtual void Clear(Option_t *opt = "");
  virtual void Print(Option_t *opt = "") const ;

  virtual void InsertHit(const TDetectorHit& hit);


  unsigned int Size()       const { return phoswall_hits.size(); }
  int Pixel(int i) const { return phoswall_hits.at(i).Pixel(); }
  int A(int i)     const { return phoswall_hits.at(i).A(); }
  int B(int i)     const { return phoswall_hits.at(i).B(); }
  int C(int i)     const { return phoswall_hits.at(i).C(); }
  TVector3 Position(int i) const { return GetWallPosition(Pixel(i)); }

  Int_t GetLargestNumber()                  const { return fLargestHit; }
  TPhosWallHit& GetLargestHit()                   { return GetPhosWallHit(GetLargestNumber()); }
        TPhosWallHit& GetPhosWallHit(int i)       { return phoswall_hits.at(i); }
  const TPhosWallHit& GetPhosWallHit(int i) const { return phoswall_hits.at(i); }
  TDetectorHit& GetHit(int i)                     { return phoswall_hits.at(i); }

  void Draw(Option_t *opt="");
  static void DrawPID(Option_t *gate="",Option_t *opt="BC",Long_t entries=kMaxLong,TChain *chain=0); 

  TVector3 GetHitPosition() 
    { if(fLargestHit>-1) return GetWallPosition(Pixel(fLargestHit)); else return TVector3(sqrt(-1),sqrt(-1),sqrt(-1)); }

  TVector3 GetKinVector(Double_t E_ejec,Double_t E_beam=30.0,const char *beam="18O",const char *recoil="30Si", const char *ejec="a");
  TVector3 GetKinVector(Double_t E_ejec,Double_t E_beam,TNucleus &beam,TNucleus &recoil,TNucleus &ejec);


  //void DrawXY(Option_t *opt="");
  //static TVector3 FindWallPosition(const Int_t &pixel) { if(pixel<0||pixel>256) return TDetector::fB;  return fWallPositions[pixel]; }
  //void FindWeightedPosition();
  //void SetWeightedPosition(const TVector3 &temp)  { fWeightedPosition = temp; }

  static TVector3 GetWallPosition(int pixelnumber,double delta=5.0);


  static void   AddGate(TCutG* cut)   { gates.Add((TObject*)cut); }
  static int    LoadGates(const char* filename);
  static int    SaveGates(const char* filename="");
  static TList *GetGates()            { return &gates;  }
         int    IsInside(Option_t *opt = "BC") const; 
  static const char* GetGateName(int i) { return gates.At(i)->GetName(); }

  void SortHits();

private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  Short_t  fLargestHit;
  Int_t    fNumberOfHits;

  std::vector<TPhosWallHit> phoswall_hits;

  static std::vector<TVector3> fWallPositions;
  static bool      fPositionsSet;
  static void      SetWallPositions();

  static TList gates; //!
  
  ClassDef(TPhosWall,3);
};

#endif
