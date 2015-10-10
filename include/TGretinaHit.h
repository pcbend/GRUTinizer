#ifndef TGRETINAHIT_H
#define TGRETINAHIT_H

#include <TObject.h>
#include <Rtypes.h>
#include <TVector3.h>
#include <TMath.h>

#include <cmath>

#include "TGEBEvent.h"
#include "TDetectorHit.h"

#define MAXHPGESEGMENTS 36

class TGretinaHit : public TDetectorHit {

public:
  TGretinaHit();
  ~TGretinaHit();

  void Copy(TObject& obj) const;

  void BuildFrom(const TRawEvent::GEBBankType1& raw);

  Double_t GetTime()  const     { return (double)fTimeStamp - (double)fWalkCorrection; }
  Int_t    GetAddress()         const { return fAddress;        }
  Int_t    GetCrystalId()       const { return fCrystalId;      }
  Int_t    GetHoleNumber()      const { return fCrystalId/4 -1; } 
  Int_t    GetCrystalNumber()   const { return fCrystalId%4;    }   
  Float_t  GetCoreEnergy()      const { return fCoreEnergy;     }
  Float_t  GetCoreCharge(int i) const { return fCoreCharge[i];  }

  void  Print(Option_t *opt="") const;
  void  Clear(Option_t *opt="");
  const Int_t Size()  { return fNumberOfInteractions; }//fSegmentNumber.size(); }

  double GetX() { return GetPosition().X(); }
  double GetY() { return GetPosition().Y(); }
  double GetZ() { return GetPosition().Z(); }

  double GetPhi() {
    double phi = GetPosition().Phi();
    if(phi<0) {
      return TMath::TwoPi()+phi;
    } else {
      return phi;
    }
  }
  double GetTheta() { return GetPosition().Theta(); }
  double GetPhiDeg() { return GetPhi()*TMath::RadToDeg(); }
  double GetThetaDeg() { return GetTheta()*TMath::RadToDeg(); }


  bool HasInteractions() { return fNumberOfInteractions; }
  //TGretinaHit& operator+=(const TGretinaHit&);
  //TGretinaHit& operator+(const TGretinaHit&);


  double GetDoppler(double beta,const TVector3 *vec=0) {
    if(Size()<1)
      return 0.0;
    bool madevec = false;
    if(vec==0) {
      vec = &BeamUnitVec;
    }
    double tmp = 0.0;
    double gamma = 1/(sqrt(1-pow(beta,2)));
    tmp = fCoreEnergy*gamma *(1 - beta*TMath::Cos(GetPosition().Angle(*vec)));
    return tmp;
  }


  Int_t   GetFirstIntPoint()             const { return fFirstInteraction;     }
  Int_t   GetSecondIntPoint()            const { return fSecondInteraction;    }
  Int_t   NumberOfInteractions()               { return fNumberOfInteractions; }
  Int_t   GetSegmentId(const int &i)           { return fSegmentNumber[i]; }
  Float_t GetSegmentEng(const int &i)          { return fInteractionEnergy[i]; }
  TVector3 GetInteractionPosition(int i) const; //{ return fGlobalInteractionPosition[i]; }
  TVector3 GetLocalPosition(int i) const; 
  //TVector3 GetCrystalPosition(int i)     const { return TVector3(0,0,1): }
  TVector3 GetPosition()                 const { return GetFirstIntPosition(); }

  TVector3 GetFirstIntPosition() const;
  TVector3 GetSecondIntPosition() const;

  bool CheckAddback(const TGretinaHit&) const;
  void AddToSelf(const TGretinaHit& other, double& max_energy);

  //void SetPosition(TVector3 &vec) { fCorePosition = vec; }

private:
  void SortHits();

  Long_t  fTimeStamp;
  Float_t fWalkCorrection;

  Int_t   fAddress;
  Int_t   fCrystalId;
  Float_t fCoreEnergy;
  Int_t   fCoreCharge[4];

  Int_t   fFirstInteraction;
  Int_t   fSecondInteraction;

  Int_t   fNumberOfInteractions;

  /// The number of the segment containing the interaction.
  /**
     Note: This is not equal to the segment number as read from the datastream.
     This is equal to 36*raw.crystal_id + raw.segnum.  ///not anymore pcb.
   */
  Int_t    fSegmentNumber[MAXHPGESEGMENTS];

  /// The position of the interaction point in lab coordinates
  /**
     Note: this is not equal to the position as read from the datastream.
     This has been transformed to lab coordinates.
     To get the crystal coordinate, use TGretinaHit::GetCrystalPosition(int i).
   */
  TVector3 fGlobalInteractionPosition[MAXHPGESEGMENTS];
  TVector3 fLocalInteractionPosition[MAXHPGESEGMENTS];
  Float_t  fInteractionEnergy[MAXHPGESEGMENTS];

  ClassDef(TGretinaHit,1)
};


#endif
