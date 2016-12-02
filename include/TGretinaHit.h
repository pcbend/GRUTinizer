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


class interaction_point { 
  public:
  interaction_point():fSeg(-1),fX(sqrt(-1)),fY(sqrt(-1)),fZ(sqrt(-1)),fEng(sqrt(-1)),fFrac(sqrt(-1)) { }
  interaction_point(int seg,float x,float y,float z,float energy,float fraction=100.0)
    : fSeg(seg),fX(x),fY(y),fZ(z),fEng(energy),fFrac(fraction) { }
  virtual ~interaction_point() { }
  int   fSeg;
  float fX;
  float fY;
  float fZ;
  float fEng;
  float fFrac;

  bool operator<(const interaction_point &other) const {
    if(fEng!=other.fEng) {
      return fEng>other.fEng;
    } 
    if(fSeg==other.fSeg) {
      return fFrac>other.fFrac;
    }
    return fSeg<other.fSeg;
  }
  void Print(Option_t *opt="") const { 
    printf("Seg[%02i]\tEng: % 4.2f / % 4.2f  \t(X,Y,Z) % 3.2f % 3.2f % 3.2f\n",
            fSeg,fEng,fFrac,fX,fY,fZ);
  
  };
  ClassDef(interaction_point,1)
};



class TGretinaHit : public TDetectorHit {

public:
  TGretinaHit();
  TGretinaHit(const TGretinaHit& hit){ hit.Copy(*this); }
  ~TGretinaHit();

  void Copy(TObject& obj) const;

  void BuildFrom(TSmartBuffer& raw);

  Double_t GetTime()            const { return (double)Timestamp() - (double)fWalkCorrection; } 
  Int_t    GetCrystalId()       const { return fCrystalId;      }
  Int_t    GetHoleNumber()      const { return fCrystalId/4-1;  }
  Int_t    GetCrystalNumber()   const { return fCrystalId%4;    }
  Float_t  GetCoreEnergy()      const { return fCoreEnergy;     }
  Int_t    GetCoreCharge(int i) const { return fCoreCharge[i];  }
  Float_t  GetCoreEnergy(int i) const;
  virtual Int_t Charge()        const { return GetCoreCharge(3); }
  Int_t GetPad() const { return fPad; }

  const char *GetName() const;

  void  Print(Option_t *opt="") const;
  void  Clear(Option_t *opt="");
  
  Int_t Size()  const { return fSegments.size();  }
  
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
  double GetTheta()    const { return GetPosition().Theta(); }
  double GetPhiDeg()   const { return GetPhi()*TMath::RadToDeg(); }
  double GetThetaDeg() const { return GetTheta()*TMath::RadToDeg(); }

  Int_t Compare(const TObject *obj) const { 
    TGretinaHit *other = (TGretinaHit*)obj;
    if(this->GetCoreEnergy()>other->GetCoreEnergy())
      return -1;
    else if(this->GetCoreEnergy()<other->GetCoreEnergy())
      return 1;  //sort largest to smallest.
    return 0;
  }
  
  bool HasInteractions() { return !fSegments.empty(); }
  
  bool operator<(const TGretinaHit &rhs) const { return fCoreEnergy > rhs.fCoreEnergy; }

  double GetDoppler(double beta,const TVector3 *vec=0,int EngRange=-1) {
    if(Size()<1)
      return 0.0;
    if(vec==0) {
      vec = &BeamUnitVec;
    }
    double tmp = 0.0;
    double gamma = 1/(sqrt(1-pow(beta,2)));
    if(EngRange>0) 
      tmp = GetCoreEnergy(EngRange)*gamma *(1 - beta*TMath::Cos(GetPosition().Angle(*vec)));
    else
      tmp = fCoreEnergy*gamma *(1 - beta*TMath::Cos(GetPosition().Angle(*vec)));
    return tmp;
  } 
  
  double GetDoppler(const TS800 *s800,bool doDTAcorr=false,int EngRange=-1);
  double GetDoppler_dB(double beta,const TVector3 *vec=0, double Dta=0);



  Int_t    NumberOfInteractions()        const { return fNumberOfInteractions; }
  Int_t    GetSegmentId(const int &i)    const { return fSegments.at(i).fSeg;  }
  Float_t  GetSegmentEng(const int &i)   const { return fSegments.at(i).fEng;  }

  TVector3 GetIntPosition(unsigned int i)   const;  // position of the ith segment, Global coor.
  TVector3 GetLocalPosition(unsigned int i) const;  // position of the ith segment, Local coor.
  TVector3 GetPosition()                    const { return GetIntPosition(0); }
  TVector3 GetLastPosition()                const;

  TVector3 GetCrystalPosition()           const; 
  //TVector3 GetSegmentPosition()           const; 
                                                
  void Add(const TGretinaHit& other) {  }
  void SetCoreEnergy(float temp) const { fCoreEnergy = temp; }


private:
  void SortHits();
/* All possible decomp information and
 * where is is stored:
 * -------------------
  Int_t     type;       // endiness identifier; droppped.
  Int_t     crystal_id; //                                      -> TGreintaHit.fCrystalId
  Int_t     num;        // number of interactions of error code -> TGreintaHit.fNumberOfINteractions
  Float_t   tot_e;      // energy used for decomp               -> TGretinaHit.fCoreEnergy   
  Int_t     core_e[4];  // charge reported at dig for each gain -> TGretinaHit.fCoreCharge[4]
  Long_t    timestamp;  // timestamp for the hit                -> TDetectorHit.fTimestamp
  Long_t    trig_time;  // currently unsed (?)
  Float_t   t0;         // rise time as reported by decomp      -> TGretinaHit.fWalkCorrectrion
  Float_t   cfd;        // cfd as reported by the dig.          -> TDetectorHit.fTime
  Float_t   chisq;      // chisq value reported by decomp
  Float_t   norm_chisq; //
  Float_t   baseline;   //
  Float_t   prestep;    // avg trace value before step
  Float_t   poststep;   // avg trave value after step
  Int_t     pad;        // decomp error code.
 *------------------
*/



  Int_t           fCrystalId;
  Int_t           fCoreCharge[4];
  Int_t   fPad;
  Int_t   fNumberOfInteractions;
  
  mutable Float_t fCoreEnergy;
  Float_t         fWalkCorrection;   //also called t0.

  std::vector<interaction_point> fSegments;
  //std::vector<Int_t> fSegmentNumber; //[fNumberOfInteractions]
  //std::vector<Float_t>  fInteractionEnergy;         //[fNumberOfInteractions]
  //std::vector<Float_t>  fInteractionFraction;       //[fNumberOfInteractions]
  //std::vector<TVector3> fLocalInteractionPosition;  //[fNumberOfInteractions]
  ClassDef(TGretinaHit,5)
};


#endif
