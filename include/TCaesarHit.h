#ifndef _TCAESARDETECTORHIT_H_
#define _TCAESARDETECTORHIT_H_

#include "TDetectorHit.h"

class TCaesarHit : public TDetectorHit {
public:
  TCaesarHit() { Clear(); }

  //int GetDetnum() const;

  void Clear(Option_t* opt = "");
  void Copy(TObject& obj) const;

  int GetVSN() const		{ return fVsn;		}
  int GetChannel() const	{ return fChannel;	}
  int GetCharge() const		{ return fCharge;	}
  int GetTime() const		{ return fTime;		}

  void SetVSN(int vsn)		{ fVsn = vsn;		}
  void SetChannel(int channel)	{ fChannel = channel;	}
  void SetCharge(int charge)	{ fCharge = charge;	}
  void SetTime(int time)	{ fTime = time;		}

  int GetFullChannel() const {
    return GetVSN()*16 + GetChannel();
  }

  bool IsValid() const { return fCharge!=-1 && fTime!=-1; }

  //TVector3 GetPosition() const;

private:
	 
  int fVsn;
  int fChannel;
      	 
  int fCharge;
  int fTime;

  ClassDef(TCaesarHit,1);
};

#endif /* _TCAESARDETECTORHIT_H_ */
