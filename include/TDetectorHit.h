#ifndef _TDETECTORHIT_H_
#define _TDETECTORHIT_H_

#include "TChannel.h"
#include "TObject.h"
#include "TVector3.h"

class TDetectorHit : public TObject {
public:
  TDetectorHit();
  virtual ~TDetectorHit();

  virtual const char* GetName() const;

  virtual Int_t Compare(const TObject *obj) const; //needed for root containers
  virtual bool IsSortable() const { return true; }

  virtual void  Copy(TObject&) const;
  virtual void  Clear(Option_t *opt = "" );
  virtual void  Print(Option_t *opt = "" ) const;

  Int_t  Address()   const      { return fAddress; }
  virtual Int_t  Charge() const { return fCharge;  }
  Int_t  Time() const           { return fTime; }
  long   Timestamp() const      { return fTimestamp; }

  void SetAddress(int address)      { fAddress = address; }
  void SetCharge(int charge)        { fCharge = charge;  }
  void SetTime(int time)            { fTime = time; }
  void SetTimestamp(long timestamp) { fTimestamp = timestamp; }

  double GetEnergy() const;

  static const TVector3 BeamUnitVec; //!

protected:
  Int_t fAddress;
  Int_t fCharge;
  Int_t fTime;
  long fTimestamp;

  ClassDef(TDetectorHit,2)
};

#endif
