#ifndef _TGENERICDDASHIT_H_
#define _TGENERICDDASHIT_H_

#include "TDetectorHit.h"
#include "TMath.h"


class TGenericDDASHit : public TDetectorHit {
public:
  TGenericDDASHit();

  virtual void Copy(TObject&) const;
  virtual void Clear(Option_t *opt = "");
  virtual void Print(Option_t *opt = "") const;
  virtual void Draw(Option_t* opt = "");

  std::vector<unsigned short>* GetTrace()     { return &fTrace; }

  void SetTrace(unsigned int trace_length, const unsigned short* trace);
  void SetDetectorNumber(int dnum) { fDetector = dnum; }

  virtual Int_t Charge() const;

  int GetDetnum() const;
  int GetSlot() const;
  int GetCrate() const;
  int GetChannel() const;

  private:
    std::vector<unsigned short> fTrace;
    int fDetector;
  ClassDef(TGenericDDASHit,4);
};

#endif /* _TGENERICDDASHIT_H_ */
