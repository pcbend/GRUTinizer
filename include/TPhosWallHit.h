#ifndef _TPHOSWALLHIT_H_
#define _TPHOSWALLHIT_H_

#include <TVector3.h>

#include "TDetectorHit.h"
#include "TRawEvent.h"

class TPhosWallHit : public TDetectorHit {
public:
  TPhosWallHit();
  TPhosWallHit(TRawEvent::PWHit *hit);
  virtual ~TPhosWallHit();

  virtual void Copy(TObject &obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");

  Int_t  A()     const { return fACharge; }
  Int_t  B()     const { return fBCharge; }
  Int_t  C()     const { return fCCharge; }
  Int_t  Pixel() const { return fPixel; }
  Int_t  Time()  const { return fTime; }
  
  virtual Int_t Charge() const { return B(); }

  Int_t IsInside(Option_t *opt="") const;

  bool operator<(const TPhosWallHit& rhs) const { return fBCharge > rhs.fBCharge; }


private:
  Short_t  fACharge;
  Short_t  fBCharge;
  Short_t  fCCharge;
  Short_t  fPixel;
  Short_t  fTime;

  TVector3  fPosition;

  ClassDef(TPhosWallHit,2)

};

#endif
