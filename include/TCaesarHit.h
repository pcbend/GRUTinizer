#ifndef _TCAESARDETECTORHIT_H_
#define _TCAESARDETECTORHIT_H_

#include "TMath.h"

#include "TDetectorHit.h"

#include "GValue.h"

class TCaesarHit : public TDetectorHit {
public:
  TCaesarHit() {  Clear(); numHitsContained = 1; is_garbage_addback = false;}
  TCaesarHit(const TCaesarHit&);

  void Clear(Option_t* opt = "");
  void Print(Option_t* opt = "") const;
  void Copy(TObject& obj) const;

  int GetVSN() const		{ return fVsn;		}
  int GetChannel() const	{ return fChannel;	}

  //If SetEnergy() is called, GetCharge() will not return charge,
  //as fCharge is replaced by energy.
  int GetCharge() const		{ return Charge();	}
  int GetDetectorNumber() const { return fDet;          }
  int GetRingNumber() const     { return fRing;         }
  int GetAbsoluteDetectorNumber() const;
  double *GetPos()              { return pos;        }  
  double  GetX()      const     { return pos[0];     }
  double  GetY()      const     { return pos[1];     }
  double  GetZ()      const     { return pos[2];     }
  int GetNumHitsContained() const { return numHitsContained;}


  void SetVSN(int vsn)		{ fVsn = vsn;		}
  void SetChannel(int channel)	{ fChannel = channel;	}
  void SetDet(int det)          { fDet = det;           }
  void SetRing(int ring)        { fRing = ring;         }
  void SetPos(double x, double y, double z){ pos[0] = x; 
                                             pos[1] = y; 
                                             pos[2] = z; }

  //Detector number is not ordered in any reasonable way. This is just
  //all the detectors.

  bool IsValid() const { return (Charge()!=-1 && Time()!=-1); }
  bool IsOverflow() const;
  //For so-called ng events; see is_garbage_addback description
  void IsGarbageAddback() { is_garbage_addback = true;}

  TVector3 GetPosition() const; 
  //z_shift is the target shift relative to the center of the array.
  //Positive values are downstream, and the units should be in cm.
  TVector3 GetPosition(double z_shift) const; 
  
  double GetDoppler(double beta=-1, TVector3 *track=0) const{
    if(track==0) {
      track = (TVector3*)&BeamUnitVec;
    }
    if (beta == -1){
      beta = GValue::Value("BETA");
    }

    if (!beta || beta == -1){
      std::cout << "ERROR: No beta given and no GValue found!" << std::endl;
      return sqrt(-1);
    }
    double tmp = 0.0;
    double gamma = 1/(sqrt(1-pow(beta,2)));
    tmp = GetEnergy()*gamma *(1 - beta*TMath::Cos(GetPosition().Angle(*track)));
    return tmp;
  }
  //Note z_shift must be in cm!
  double GetDoppler(double beta,  double z_shift,TVector3 *track=0) {
    if(track==0) {
      track = (TVector3*)&BeamUnitVec;
    }
    double tmp = 0.0;
    double gamma = 1/(sqrt(1-pow(beta,2)));
    tmp = GetEnergy()*gamma *(1 - beta*TMath::Cos(GetPosition(z_shift).Angle(*track)));
    return tmp;
  }

  //Combine two caesar hits for addback, taking the properties of the higher energy
  //hit.
  void AddToSelf(const TCaesarHit& other);
  //If there are more than two neighboring hits, or if there is a shape
  //where not all possible neighbors in a combination or neighbors with all
  //possibilities. For example, if three detectors in a line fire, the first
  //and the third are not neighbors and therefore the hit cannot be added back
  //conclusively.
  bool is_garbage_addback;

  //TOTAL_DET_IN_PREV_RINGS olds the number of detectors
  //in all previous rings before that element. For example,
  //there are 0 detectors before ring A, but there are 182
  //detectors before ring J. 
  static const int TOTAL_DET_IN_PREV_RINGS[10];

private:
	 
  int fVsn;
  int fChannel;
  int fDet;
  int fRing;
  //num_hits_contained is the number of hits added together to form the current
  //hit for addback purposes. For non-addback, this number is 1.
  int numHitsContained;
  double pos[3];

  

  ClassDef(TCaesarHit,2);
};

#endif /* _TCAESARDETECTORHIT_H_ */
