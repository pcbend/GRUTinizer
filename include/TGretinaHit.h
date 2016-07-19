#ifndef TGRETINAHIT_H
#define TGRETINAHIT_H

#include <TObject.h>
#include <Rtypes.h>
#include <TVector3.h>
#include <TMath.h>
#include <TChain.h>


#include <cmath>

//#include "TGEBEvent.h"
#include "TDetectorHit.h"

#define MAXHPGESEGMENTS 36

class TSmartBuffer;
class TS800;

class TGretinaHit : public TDetectorHit {

public:
  TGretinaHit();
  ~TGretinaHit();

  void Copy(TObject& obj) const;

  void BuildFrom(TSmartBuffer& raw);

  Long_t   GetTimestamp()       const { return fTimeStamp; }
  Double_t GetTime()            const { return (double)fTimeStamp - (double)fWalkCorrection; }
  Int_t    GetAddress()         const { return fAddress;        }
  Int_t    GetCrystalId()       const { return fCrystalId;      }
  Int_t    GetHoleNumber()      const { return fCrystalId/4-1;  }
  Int_t    GetCrystalNumber()   const { return fCrystalId%4;    }
  Float_t  GetCoreEnergy()      const { return fCoreEnergy;     }
  Int_t    GetCoreCharge(int i) const { return fCoreCharge[i];  }
  Float_t  GetCoreEnergy(int i) const; // { return fCoreCharge[i];  }
  virtual Int_t Charge()        const { return fCoreCharge[3];  }

  const char *GetName() const;

  Int_t GetPad() const { return fPad; }

  void  Print(Option_t *opt="") const;
  void  Clear(Option_t *opt="");
  Int_t Compare(const TObject *obj) const { 
    TGretinaHit *other = (TGretinaHit*)obj;
    if(this->GetCoreEnergy()>other->GetCoreEnergy())
      return -1;
    else if(this->GetCoreEnergy()<other->GetCoreEnergy())
      return 1;  //sort largest to smallest.
    return 0;
  }
  
  Int_t Size() const { return fNumberOfInteractions; }//fSegmentNumber.size(); }

  double GetX() const { return GetPosition().X(); }
  double GetY() const { return GetPosition().Y(); }
  double GetZ() const { return GetPosition().Z(); }

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
  bool operator<(const TGretinaHit &rhs) const { return fCoreEnergy > rhs.fCoreEnergy; }


  double GetDoppler(double beta,const TVector3 *vec=0) {
    if(Size()<1)
      return 0.0;
    if(vec==0) {
      vec = &BeamUnitVec;
    }
    double tmp = 0.0;
    double gamma = 1/(sqrt(1-pow(beta,2)));
    tmp = fCoreEnergy*gamma *(1 - beta*TMath::Cos(GetPosition().Angle(*vec)));
    return tmp;
  } 
  double GetDoppler(const TS800 *s800,bool doDTAcorr=false,int EngRange=-1);
  double GetDoppler(int EngRange, double beta,const TVector3 *vec=0) {
    if(Size()<1)
      return 0.0;
    if(vec==0) {
      vec = &BeamUnitVec;
    }
    double tmp = 0.0;
    double gamma = 1/(sqrt(1-pow(beta,2)));
    tmp = GetCoreEnergy(EngRange)*gamma *(1 - beta*TMath::Cos(GetPosition().Angle(*vec)));
    return tmp;
  }



  double GetDoppler_dB(double beta,const TVector3 *vec=0, double Dta=0);



  Int_t    GetFirstIntPoint()             const { return fFirstInteraction;     }
  Int_t    GetSecondIntPoint()            const { return fSecondInteraction;    }
  Int_t    NumberOfInteractions()         const { return fNumberOfInteractions; }
  Int_t    GetSegmentId(const int &i)     const { return fSegmentNumber.at(i); }
  Float_t  GetSegmentEng(const int &i)    const { return fInteractionEnergy.at(i); }
  TVector3 GetInteractionPosition(int i)  const; //{ return fGlobalInteractionPosition[i]; }
  TVector3 GetLocalPosition(int i) const;
  //TVector3 GetCrystalPosition(int i)     const { return TVector3(0,0,1): }
  TVector3 GetPosition()                  const { return GetFirstIntPosition(); }
  TVector3 GetPosition_2()                const { return GetFirstIntPosition_2(); }

  TVector3 GetCrystalPosition()           const; 
  TVector3 GetSegmentPosition()           const; 
                                                


  TVector3 GetFirstIntPosition() const;
  TVector3 GetFirstIntPosition_2() const;
  TVector3 GetSecondIntPosition() const;

  void AddToSelf(const TGretinaHit& other);

  //void SetPosition(TVector3 &vec) { fCorePosition = vec; }


  void   SetCoreEnergy(float temp) const { fCoreEnergy = temp; }


private:
  void SortHits();

  Long_t  fTimeStamp;
  Float_t fWalkCorrection;

  Int_t   fAddress;
  Int_t   fCrystalId;
  mutable Float_t fCoreEnergy;
  Int_t   fCoreCharge[4];

  Int_t   fPad;

  Int_t   fFirstInteraction;
  Int_t   fSecondInteraction;

  Int_t   fNumberOfInteractions;

  /// The number of the segment containing the interaction.
  /**
     Note: This is not equal to the segment number as read from the datastream.
     This is equal to 36*raw.crystal_id + raw.segnum.  ///not anymore pcb.
   */
  std::vector<Int_t> fSegmentNumber; //[fNumberOfInteractions]

  /// The position of the interaction point in lab coordinates
  /**
     Note: this is not equal to the position as read from the datastream.
     This has been transformed to lab coordinates.
     To get the crystal coordinate, use TGretinaHit::GetCrystalPosition(int i).
   */
  std::vector<TVector3> fGlobalInteractionPosition; //[fNumberOfInteractions]
  std::vector<TVector3> fLocalInteractionPosition; //[fNumberOfInteractions]
  std::vector<Float_t>  fInteractionEnergy; //[fNumberOfInteractions]
  std::vector<Float_t>  fInteractionFraction; //[fNumberOfInteractions]

  ClassDef(TGretinaHit,4)
};


#endif
