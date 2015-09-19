#ifndef _TDETECTORHIT_H_
#define _TDETECTORHIT_H_

#include "TObject.h"
#include "TVector3.h"

class TDetectorHit : public TObject {
public:
  TDetectorHit();
  virtual ~TDetectorHit();

  virtual Int_t Compare(const TObject *obj) const; //needed for root containers
  virtual bool IsSortable() const { return true; }

  virtual void  Copy(TObject&) const;
  virtual void  Clear(Option_t *opt = "" );
  virtual void  Print(Option_t *opt = "" ) const;

  Int_t  Address()   const { return fAddress; }
  Int_t  Charge()    const { return fCharge;  }
  double GetEnergy() const;

  Int_t  GetSystem()  const { return ((fAddress&0xff000000)>>24); }
  Int_t  GetType()    const { return ((fAddress&0x00ff0000)>>16); }
  Int_t  GetChannel() const { return ((fAddress&0x0000ffff)    ); }

  Int_t  GetHole()      const { return ((fAddress&0xff000000)>>24); }
  Int_t  GetCrystal()   const { return ((fAddress&0x00ff0000)>>16); }
  Int_t  GetSegmentId() const { return ((fAddress&0x0000ffff)    ); }

  void   SetAddress(unsigned char system,unsigned char type,unsigned short channel) {
    fAddress =
      (((unsigned int)system ) << 24) +
      (((unsigned int)type   ) << 16) +
      (((unsigned int)channel) << 0);
  }
  void   SetCharge(Int_t chg) { fCharge = chg; }

  static const TVector3 BeamUnitVec; //!

private:              //      System | Type  | Element
  Int_t    fAddress;  //   0x   ff   |  ff   |  ffff
  Int_t    fCharge;


  ClassDef(TDetectorHit,1)

};

#endif
