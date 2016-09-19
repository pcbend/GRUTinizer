#ifndef TGRETSIMHIT_H
#define TGRETSIMHIT_H

#include <TObject.h>
#include <Rtypes.h>
#include <TVector3.h>
#include <TMath.h>
#include <TRandom.h>

#include <cmath>

#include "TGEBEvent.h"
#include "TDetectorHit.h"

#define MAXHPGESEGMENTS 36

class TGretSimHit : public TDetectorHit {

public:
  TGretSimHit();
  ~TGretSimHit();

  void Copy(TObject& obj) const;

  //  void BuildFrom(const TRawEvent::GEBBankType1& raw); // ??

  virtual Int_t Charge()        const { return fEnergy;  }

  const char *GetName() const;



  void  Print(Option_t *opt="") const;
  void  Clear(Option_t *opt="");

  int    GetEn()     const   { return fEnergy; }
  double GetBeta()   const   { return fBeta; }
  double GetX()      const   { return fPosit.X(); }
  double GetY()      const   { return fPosit.Y(); }
  double GetZ()      const   { return fPosit.Z(); }
  int    IsFEP()     const   { return fIsFull; }
  int    TotalHits() const   { return fTotalHits; }
  int    HitNum()    const   { return fHitNum; }
  

  double GetDoppler(double sigma=1, const TVector3 *vec=0) {

    if(vec==0) {
      vec = &BeamUnitVec;
    }
    double tmp = 0.0;
    double gamma = 1/(sqrt(1-pow(GetBeta(),2)));
    double Energy = gRandom->Gaus(fEnergy,sigma);
    tmp = Energy*gamma *(1 - GetBeta()*TMath::Cos(fPosit.Angle(*vec)));
    return tmp;
  }


  /*
  double GetPhi(int id=0) {
    double phi = fPosit.at(0).Phi();
    if(phi<0) {
      return TMath::TwoPi()+phi;
    } else {
      return phi;
    }
  }
  double GetTheta(int id=0)    const { return fPosit.at(id).Theta(); }
  double GetPhiDeg(int id=0)   const { return GetPhi(id)*TMath::RadToDeg(); }
  double GetThetaDeg(int id=0) const { return GetTheta(id)*TMath::RadToDeg(); }
  */

  
  //void SetPosition(TVector3 &vec) { fCorePosition = vec; }

  

  int      fEnergy;
  TVector3 fPosit;
  TVector3 fInteraction;
  double   fBeta;
  int      fIsFull;
  int      fTotalHits;
  int      fHitNum;

private:
  ClassDef(TGretSimHit,1)
};


#endif
