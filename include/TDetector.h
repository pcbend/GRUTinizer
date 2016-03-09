#ifndef _TDETECTOR_H_
#define _TDETECTOR_H_

#include <vector>

#include "TNamed.h"

#include "TDetectorHit.h"
#ifndef __CINT__
#include "TRawEvent.h"
#else
class TRawEvent;
#endif

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
  virtual void InsertHit(const TDetectorHit&) = 0;
  virtual TDetectorHit& GetHit(int i)         = 0;

  int  Size() const { return fSize; }


  Long_t Timestamp() const { return fTimestamp; }
  void   SetTimestamp(Long_t timestamp)  { fTimestamp   = timestamp; }

protected:
  #ifndef __CINT__
  std::vector<TRawEvent> raw_data; //!
  #endif

  int fSize;
  Long_t   fTimestamp;

private:
  virtual int  BuildHits() = 0;


  ClassDef(TDetector,1)
};

#endif
