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
  virtual Int_t  Charge() const;
  Int_t  Time() const           { return fTime; }
  long   Timestamp() const      { return fTimestamp; }

  void SetAddress(int address)      { fAddress = address; }
  void SetCharge(int charge);
  void SetTime(int time)            { fTime = time; }
  void SetTimestamp(long timestamp) { fTimestamp = timestamp; }

  double GetEnergy() const; //applies TChannel ENERGYCOEFF to Charge
  void SetEnergy(double energy);
  double GetTime() const;   //applies TChannel TIMECOEFF to Time()

  void AddEnergy(double eng);

  static const TVector3 BeamUnitVec; //!

protected:
  Int_t fAddress;
  Float_t fCharge;
  Int_t fTime;
  long fTimestamp;

  unsigned char fFlags;

  enum EHitFlags {
    kIsEnergy = BIT(0),
    kUnused1  = BIT(1),
    kUnused2  = BIT(2),
    kUnused3  = BIT(3),
    kUnused4  = BIT(4),
    kUnused5  = BIT(5),
    kUnused6  = BIT(6),
    kUnused7  = BIT(7)
  };

  ClassDef(TDetectorHit,4)
};

#endif
