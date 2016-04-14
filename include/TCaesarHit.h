#ifndef _TCAESARDETECTORHIT_H_
#define _TCAESARDETECTORHIT_H_

#include "TMath.h"

#include "TDetectorHit.h"

class TCaesarHit : public TDetectorHit {
public:
  TCaesarHit() { Clear(); }
  TCaesarHit(const TCaesarHit&);

  //int GetDetnum() const;

  void Clear(Option_t* opt = "");
  void Print(Option_t* opt = "") const;
  void Copy(TObject& obj) const;

  int GetVSN() const		{ return fVsn;		}
  int GetChannel() const	{ return fChannel;	}
  int GetCharge() const		{ return Charge();	}
//  int GetTime() const		{ return Time();        }
  int GetDetectorNumber() const { return fDet;          }
  int GetRingNumber() const     { return fRing;         }
  double *GetPos()              { return pos;        }  
  double  GetX()      const     { return pos[0];     }
  double  GetY()      const     { return pos[1];     }
  double  GetZ()      const     { return pos[2];     }


  void SetVSN(int vsn)		{ fVsn = vsn;		}
  void SetChannel(int channel)	{ fChannel = channel;	}
  void SetDet(int det)          { fDet = det;           }
  void SetRing(int ring)        { fRing = ring;         }
  void SetPos(double x, double y, double z){ pos[0] = x; 
                                             pos[1] = y; 
                                             pos[2] = z; }

  //Detector number is not ordered in any reasonable way. This is just
  //all the detectors.

  bool IsValid() const { return (fCharge!=-1 && fTime!=-1); }

  TVector3 GetPosition() const; 
  TVector3 GetPosition(double z_shift) const; 
 
  double GetDoppler(double beta, TVector3 *track=0) {
    if(track==0) {
      track = (TVector3*)&BeamUnitVec;
    }
    double tmp = 0.0;
    double gamma = 1/(sqrt(1-pow(beta,2)));
    tmp = GetEnergy()*gamma *(1 - beta*TMath::Cos(GetPosition().Angle(*track)));
    return tmp;
  }
  double GetDoppler(double beta,  double z_shift,TVector3 *track=0) {
    if(track==0) {
      track = (TVector3*)&BeamUnitVec;
    }
    double tmp = 0.0;
    double gamma = 1/(sqrt(1-pow(beta,2)));
    tmp = GetEnergy()*gamma *(1 - beta*TMath::Cos(GetPosition(z_shift).Angle(*track)));
    return tmp;
  }

  void AddToSelf(const TCaesarHit& other);

private:
	 
  int fVsn;
  int fChannel;
  int fDet;
  int fRing;
  double pos[3];

  

  ClassDef(TCaesarHit,2);
};

#endif /* _TCAESARDETECTORHIT_H_ */
