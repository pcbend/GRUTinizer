#ifndef _TDETECTOR_H_
#define _TDETECTOR_H_

#include <vector>

#include "TNamed.h"

#include "TDetectorHit.h"
#include "TRawEvent.h"
//class TRawEvent;
class TSmartBuffer;


class TDetector : public TNamed {
public:
  TDetector();
  TDetector(const char *name,const char *title="");
  virtual ~TDetector();

  bool AddRawData(const TRawEvent& buf);

  virtual void Copy(TObject& obj) const;
  virtual void Clear(Option_t *opt = "" );
  virtual void Print(Option_t *opt = "" ) const;
  virtual int  Compare(const TObject& obj) const;

  int Build();
  virtual void InsertHit(const TDetectorHit&) { AbstractMethod("InsertHit()"); }
  virtual TDetectorHit& GetHit(int i)         { AbstractMethod("GetHit()"); }
  virtual int  Size() const                   { AbstractMethod("Size()"); }


  Long_t Timestamp() const { return fTimestamp; }
  void   SetTimestamp(Long_t timestamp)  { fTimestamp   = timestamp; }

protected:
  #ifndef __CINT__
  std::vector<TRawEvent> raw_data; //!
  #endif

private:
  virtual int  BuildHits()                  { AbstractMethod("BuildHits()"); }

  Long_t   fTimestamp;

  ClassDef(TDetector,1)
};

#endif
