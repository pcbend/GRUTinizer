#ifndef TGRETINAHIT_H
#define TGRETINAHIT_H

#include <TObject.h>
#include <Rtypes.h>
#include <TVector3.h>
#include <TMath.h>
#include <TChain.h>

#include <cmath>

#include "TDetectorHit.h"

#define MAXHPGESEGMENTS 36

class TSmartBuffer;


class TInteractionPoint {
  public:
    TInteractionPoint() { } 
    TInteractionPoint(const TInteractionPoint &IP);
    TInteractionPoint(int seg,float eng,float frac,TVector3 lpos) :
                      fSegNum(seg),fEng(eng),fDecompEng(frac),fLPosition(lpos) { }
    virtual ~TInteractionPoint() { }
   
    virtual void Copy(const TInteractionPoint&);
    void Add(TInteractionPoint&);

    virtual int   GetSegNum()              const { return fSegNum;    }
    virtual float GetPreampE()             const { return fEng;       }
    virtual float GetDecompE()             const { return fDecompEng; }
    virtual float GetAssignE()             const { return fAssignedEng; }
    virtual int   GetOrder()               const { return fOrder; } 
    virtual TVector3 GetPosition(int xtal) const; // { return TGretina::CrystalToGlobal(xtal,
    TVector3 GetLocalPosition()            const { return fLPosition; }
    void SetOrder(int o)     { fOrder=o; }
    void SetAssignE(float e) { fAssignedEng = e; }

    void Print(Option_t *opt="") const;
    void Clear(Option_t *opt="");

    void SetSegNum(int seg) { fSegNum = seg; }

    int Wedge() const { return ((GetSegNum()-1)%6); }

    bool operator == (const TInteractionPoint &rhs) const {
      return (GetDecompE() == rhs.GetDecompE());
    }
    bool operator < (const TInteractionPoint &rhs) const{
      return (GetDecompE() > rhs.GetDecompE());
    }

  private:
    int   fSegNum;
    float fEng;          // energy as recorded by preamp.  energy in mode2 format
    float fDecompEng;    // energy as assigned by decomp.  fraction in mode2 format
    float fAssignedEng;  // percent eng assigned by decomp scaled to the core. not in mode2 format
    int   fOrder;        // interaction order
    //TVector3 fPosition;  // in global coordinates
    TVector3 fLPosition; // in local coordinates.
  ClassDef(TInteractionPoint,1)

};



class TGretinaHit : public TDetectorHit {

public:
  TGretinaHit();
  TGretinaHit(const TGretinaHit& hit){ hit.Copy(*this); }
  ~TGretinaHit();

  void Copy(TObject& obj) const;

  void BuildFrom(TSmartBuffer& raw);

  Double_t GetTime()            const { return (double)Timestamp() - (double)fT0; }
  Double_t GetT0()              const { return fT0; }
  Int_t    GetAddress()         const { return fAddress;        }
  Int_t    GetXtalId()          const { return fCrystalId;      }
  Int_t    GetCrystalId()       const { return fCrystalId;      }
  Int_t    GetHoleNumber()      const { return fCrystalId/4;  }
  Int_t    GetCrystalNumber()   const { return fCrystalId%4;    }
  Float_t  GetCoreEnergy()      const { return fCoreEnergy;     }
  Int_t    GetCoreCharge(int i) const { return fCoreCharge[i];  }
  Float_t  GetCoreEnergy(int i) const; // { return fCoreCharge[i];  }
  virtual Int_t    Charge()     const { return fCoreCharge[3];  }
  virtual Double_t GetEnergy()  const { return fCoreEnergy;     } 

  const char *GetName()   const { return TDetectorHit::GetName(); }
  int         GetNumber() const { return TDetectorHit::GetNumber(); }

  Int_t GetPad() const { return fPad; }

  void  Print(Option_t *opt="") const;
  void  Clear(Option_t *opt="");
  Int_t Compare(const TObject *obj) const; 
  
  Int_t Size() const { return fInteractions.size(); }//fSegmentNumber.size(); }

  double GetX() const { return GetPosition().X(); }
  double GetY() const { return GetPosition().Y(); }
  double GetZ() const { return GetPosition().Z(); }

  double GetPhi() const {
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
  bool operator<(const TGretinaHit &rhs) const { return fCoreEnergy > rhs.fCoreEnergy; }

  double GetDoppler(double beta,const TVector3 *vec=0);
  double GetDoppler_dB(double beta,const TVector3 *vec=0, double Dta=0);

  TVector3 GetPosition() const; //                  const { return GetIntPosition(0); }


  int      CleanInteractions();
  TInteractionPoint GetInteractionPoint(int i) const { return fInteractions.at(i); }

  //Int_t    NumberOfInteractions()     const { return fNumberOfInteractions; }
  Int_t    NumberOfInteractions()     const { return fInteractions.size(); }
  Int_t    GetSegmentId(int i)        const { return GetInteractionPoint(i).GetSegNum(); }
  Float_t  GetIntPreampEng(int i)     const { return GetInteractionPoint(i).GetPreampE(); }
  Float_t  GetIntDecomEng(int i)      const { return GetInteractionPoint(i).GetDecompE(); }
  Float_t  GetIntAssignEng(int i)     const { return GetInteractionPoint(i).GetAssignE(); }
  TVector3 GetIntPosition(int i)      const { return GetInteractionPoint(i).GetPosition(GetCrystalId()); }
  TVector3 GetLocalIntPosition(int i) const { return GetInteractionPoint(i).GetLocalPosition(); }
  TVector3 GetCrystalPosition()       const;// { return TGretina::GetCrystalPosition(fCrystalId); }

  
  void   SetCoreEnergy(float temp) const { fCoreEnergy = temp; }

  double GetIntMag(int i)      const { return GetIntPosition(i).Mag(); }
  double GetIntTheta(int i)    const { return GetIntPosition(i).Theta(); } 
  double GetIntThetaDeg(int i) const { return GetIntTheta(i)*TMath::RadToDeg(); }
  double GetIntPhi(int i) const {
    double phi = GetIntPosition(i).Phi();
    if(phi<0) {
      return TMath::TwoPi()+phi;
    } else {
      return phi;
    }
  }
  double GetIntPhiDeg(int i) const { return GetIntPhi(i)*TMath::RadToDeg(); }


private:
  //void SortHits();
  void SortInts();

  Float_t fT0; //WalkCorrection;
  Int_t   fCrystalId;
  mutable Float_t fCoreEnergy;
  Int_t   fCoreCharge[4];

  Int_t   fPad;
  Int_t   fNumberOfInteractions;
  
  std::vector<TInteractionPoint> fInteractions; //[fNumberOfInteractions];

  ClassDef(TGretinaHit,7)
};







#endif




