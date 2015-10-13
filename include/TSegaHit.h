#ifndef _TSEGAHIT_H_
#define _TSEGAHIT_H_

#include "TDetectorHit.h"

#define MAX_TRACE_LENGTH 100

class TSegaHit : public TDetectorHit {
public:
  TSegaHit() { }

  virtual void Copy(TObject&) const;
  virtual void Clear(Option_t *opt = "");
  virtual void Print(Option_t *opt = "") const;

  int GetChannel() const { return fChannel; }
  int GetSlot() const    { return fSlot; }
  int GetCrate() const   { return fCrate; }
  virtual int Charge() const { return fCharge;}

  void SetChannel(int chan) { fChannel = chan;  }
  void SetSlot(int slot)    { fSlot    = slot;  }
  void SetCrate(int crate)  { fCrate   = crate; }
  void SetCharge(int chg)   { fCharge  = chg;   }

  unsigned int GetTraceLength() const { return fTraceLength; }
  const unsigned short* GetTrace() const { return fTrace; }

  void SetTrace(unsigned int trace_length, const unsigned short* trace);

  long GetTimestamp() const { return fTimestamp; }
  void SetTimestamp(long ts) { fTimestamp = ts; }

private:
  long fTimestamp;
  int fChannel;
  int fCharge;
  int fSlot;
  int fCrate;

  unsigned int fTraceLength;
  unsigned short fTrace[MAX_TRACE_LENGTH];

  ClassDef(TSegaHit,2);
};

#endif /* _TSEGAHIT_H_ */
