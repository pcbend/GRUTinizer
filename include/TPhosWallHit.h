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

  const Int_t  A()     { return fACharge; }
  const Int_t  B()     { return fBCharge; }
  const Int_t  C()     { return fCCharge; }
  const Int_t  Pixel() { return fPixel; }
  const Int_t  Time()  { return fTime; }



private:
  Short_t  fACharge;
  Short_t  fBCharge;
  Short_t  fCCharge;
  Short_t  fPixel;
  Short_t  fTime;

  TVector3  fPosition;

  ClassDef(TPhosWallHit,1)

};

#endif
