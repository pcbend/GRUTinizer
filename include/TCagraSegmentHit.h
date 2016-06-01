#ifndef _TCagraSEGMENTHIT_H_
#define _TCagraSEGMENTHIT_H_

#include "TDetectorHit.h"

class TCagraSegmentHit : public TDetectorHit {
public:
  TCagraSegmentHit() { }

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

  ClassDef(TCagraSegmentHit,1);
};

#endif /* _TCagraSEGMENTHIT_H_ */
