#ifndef _TDETECTOR_H_
#define _TDETECTOR_H_

#include <vector>

#include "TNamed.h"

#include "TDetectorHit.h"
#include "TSmartBuffer.h"

class TDetector : public TNamed {
public:
  TDetector();
  TDetector(const char *name,const char *title="");
  virtual ~TDetector();

  virtual void AddRawData(TSmartBuffer buf) { AbstractMethod("AddRawData"); }
  virtual void BuildHits()                  { AbstractMethod("BuildHits()"); }

  virtual void InsertHit(const TDetectorHit&) { AbstractMethod("InsertHit()"); }
  virtual TDetectorHit& GetHit(int i)         { AbstractMethod("GetHit()"); }
  virtual int  Size()                         { AbstractMethod("Size()"); }

  virtual void Copy(const TDetector&);
  virtual void Clear(Option_t *opt = "" );
  virtual void Print(Option_t *opt = "" );

private:
  ClassDef(TDetector,1)
};

#endif
