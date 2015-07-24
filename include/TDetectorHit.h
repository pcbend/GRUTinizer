#ifndef _TDETECTORHIT_H_
#define _TDETECTORHIT_H_

#include "TObject.h"
#include "TVector3.h"

class TDetectorHit : public TObject {
public:
  TDetectorHit();
  virtual ~TDetectorHit();

  virtual Int_t Compare(const TObject *obj) const { AbstractMethod("Compare()"); } //needed for root containers

  virtual void  Copy(const TObject&);
  virtual void  Clear(Option_t *opt = "" );
  virtual void  Print(Option_t *opt = "" );

  Int_t  Address() const { return fAddress; }
  Int_t  Charge()  const { return fCharge;  }
  Long_t Time()    const { return fTime;    }

  Int_t  GetSystem()  const { return ((fAddress&0xff000000)>>24); }
  Int_t  GetType()    const { return ((fAddress&0x00ff0000)>>16); }
  Int_t  GetChannel() const { return ((fAddress&0x0000ffff)    ); }

  void   SetAddress(const Int_t system,const Int_t type,const Int_t channel) {
    fAddress = ((system&0x000000ff)<<24)+((type&0x00ff0000)<<24)+((channel&0x0000ffff));
  }
  void   SetCharge(const Int_t chg) { fCharge = chg; }
  void   SetTime(const Long_t tim)  { fTime   = tim; }

  virtual TVector3 *Position() { return &fBeamUnitVec; }

private:              //      System | Type  | Element
  Int_t    fAddress;  //   0x   ff   |  ff   |  ffff
  Int_t    fCharge;
  Long_t   fTime;

  static TVector3 fBeamUnitVec; //!

  ClassDef(TDetectorHit,1)

};

#endif
