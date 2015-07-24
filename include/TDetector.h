#ifndef _TDETECTOR_H_
#define _TDETECTOR_H_

#include <vector>

#include "TDetectorHit.h"
#include "TNamed.h"

struct RawData {
  char* data;
  size_t data_size;
};

class TDetector : public TNamed {
public:
  TDetector();
  TDetector(const char *name,const char *title="");
  virtual ~TDetector();

  virtual void AddRawData(RawData raw_data) { AbstractMethod("AddRawData"); }
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
