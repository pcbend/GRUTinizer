#ifndef _TDIAMONDDETHIT_H_
#define _TDIAMONDDETHIT_H_

#include "TDetectorHit.h"
#include "TMath.h"


class TDiamondDetHit : public TDetectorHit {
public:
  TDiamondDetHit();

  virtual void Copy(TObject&) const;
  virtual void Clear(Option_t *opt = "");
  virtual void Print(Option_t *opt = "") const;
  virtual void Draw(Option_t* opt = "");

  virtual Int_t Charge() const;

  int GetDetnum() const;
  int GetSlot() const;
  int GetCrate() const;
  int GetChannel() const;

private:
  ClassDef(TDiamondDetHit,4);
};

#endif /* _TDIAMONDDETHIT_H_ */
