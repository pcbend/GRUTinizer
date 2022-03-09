#ifndef _TSEGASEGMENTHIT_H_
#define _TSEGASEGMENTHIT_H_

#include "TDetectorHit.h"

#define MAX_TRACE_LENGTH 100

class TSegaSegmentHit : public TDetectorHit {
public:
  TSegaSegmentHit() { }

  virtual void Copy(TObject&) const;
  virtual void Clear(Option_t *opt = "");
  virtual void Print(Option_t *opt = "") const;

  virtual Int_t Charge() const;

  int GetDetnum() const;
  int GetSegnum() const;

  //Mapped Numbers
  int GetMapnum() const;
  int GetPairnum() const;
  int GetSlicenum() const;

  std::vector<unsigned short>& GetTrace() { return fTrace; }

  void SetTrace(unsigned int trace_length, const unsigned short* trace);

  int GetSlot() const;
  int GetCrate() const;
  int GetChannel() const;

private:
  std::vector<unsigned short> fTrace;

  ClassDef(TSegaSegmentHit,3);
};

#endif /* _TSEGASEGMENTHIT_H_ */
