#ifndef _TCAGRASEGMENTHIT_H_
#define _TCAGRASEGMENTHIT_H_

#include "TDetectorHit.h"

class TCAGRASegmentHit : public TDetectorHit {
public:
  TCAGRASegmentHit() { }

  virtual void Copy(TObject&) const;
  virtual void Clear(Option_t *opt = "");
  virtual void Print(Option_t *opt = "") const;

  virtual Int_t Charge() const;

  int GetDetnum() const;
  char GetLeaf() const;
  int GetSegnum() const;

  int GetBoardID() const;
  int GetChannel() const;

private:

  ClassDef(TCAGRASegmentHit,1);
};

#endif /* _TCAGRASEGMENTHIT_H_ */
