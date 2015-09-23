#ifndef _TSEIGHTHUNDRAD_H_
#define _TSEIGHTHUNDRAD_H_

#include "TClonesArray.h"

#include "TDetector.h"

class TS800 : public TDetector {
public:
  TS800();
  virtual ~TS800();

  void Copy(TObject& obj) const;

  virtual void Clear(Option_t* opt = "");

  virtual void InsertHit(const TDetectorHit&);
  virtual TDetectorHit& GetHit(int i);
  virtual int Size();

  Long_t GetEventCounter() { return fEventCounter;}
  void   SetEventCounter(Long_t counter) { fEventCounter = counter; }

private:
  virtual int  BuildHits();

  void Build_VMUSB_Read(TSmartBuffer buf);

  //TClonesArray* janus_hits; //->

  Long_t fEventCounter;

  ClassDef(TS800,1);
};

#endif /* _TJANUS_H_ */
