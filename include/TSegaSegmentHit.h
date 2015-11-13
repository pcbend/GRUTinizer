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

  int GetDetnum() const;
  int GetSegnum() const;

  virtual int Charge() const { return fCharge;}

  void SetCharge(int chg)   { fCharge  = chg;   }

  std::vector<unsigned short>& GetTrace() { return fTrace; }

  void SetTrace(unsigned int trace_length, const unsigned short* trace);

  long GetTimestamp() const { return fTimestamp; }
  void SetTimestamp(long ts) { fTimestamp = ts; }

  int GetSlot() const;
  int GetCrate() const;
  int GetChannel() const;

private:
  long fTimestamp;
  int fCharge;
  int fCfd;

  std::vector<unsigned short> fTrace;


  ClassDef(TSegaSegmentHit,2);
};

#endif /* _TSEGASEGMENTHIT_H_ */
