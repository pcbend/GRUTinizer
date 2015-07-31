#ifndef _TSEGAHIT_H_
#define _TSEGAHIT_H_

#include "TDetectorHit.h"

class TSegaHit : public TDetectorHit {
  static const unsigned int MAX_TRACE_LENGTH = 100;

public:
  TSegaHit() { }

  virtual void Copy(TObject&) const;
  virtual void Clear(Option_t *opt = "");
  virtual void Print(Option_t *opt = "") const;

  int GetChannel() const { return fChannel; }
  int GetSlot() const { return fSlot; }
  int GetCrate() const { return fCrate; }

  void SetChannel(int chan) { fChannel = chan; }
  void SetSlot(int slot) { fSlot = slot; }
  void SetCrate(int crate) { fCrate = crate; }

  unsigned int GetTraceLength() const { return fTraceLength; }
  const unsigned short* GetTrace() const { return fTrace; }

  void SetTrace(unsigned int trace_length, const unsigned short* trace);

  long GetTimestamp() const { return fTimestamp; }
  void SetTimestamp(long ts) { fTimestamp = ts; }

private:
  long fTimestamp;
  int fChannel;
  int fSlot;
  int fCrate;

  unsigned int fTraceLength;
  unsigned short fTrace[MAX_TRACE_LENGTH];

  ClassDef(TSegaHit,1);
};

#endif /* _TSEGAHIT_H_ */
