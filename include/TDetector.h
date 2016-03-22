#ifndef _TDETECTOR_H_
#define _TDETECTOR_H_

#include <vector>

#include "TNamed.h"

#include "TDetectorHit.h"

class TRawEvent;
class TSmartBuffer;

class TDetector : public TNamed {
public:
  TDetector();
  TDetector(const char *name,const char *title="");
  virtual ~TDetector();

  virtual void Copy(TObject& obj) const;
  virtual void Clear(Option_t *opt = "" );
  virtual void Print(Option_t *opt = "" ) const;
  virtual int  Compare(const TObject& obj) const;

  int Build(std::vector<TRawEvent>& raw_data);
  virtual void InsertHit(const TDetectorHit&) = 0;
  virtual TDetectorHit& GetHit(int i)         = 0;

  int  Size() const { return fSize; }


  Long_t Timestamp() const { return fTimestamp; }
  void   SetTimestamp(Long_t timestamp)  { fTimestamp = timestamp; }

  enum EDetectorStatus { kUnbuilt = BIT(15) };
  
  unsigned int RunStart() const { return fRunStart; }
  virtual void SetRunStart(unsigned int unix_time) { fRunStart = unix_time; }

protected:
  int fSize;
  Long_t fTimestamp;

  /// Start of the run, in unix time.
  /**
     This comes from the BEGIN_RUN packet in NSCL data.
     It is the number of seconds since January 1, 1970.
     If unavailable, will be 0.
   */
  unsigned int fRunStart;

private:
  virtual int  BuildHits(std::vector<TRawEvent>& raw_data) = 0;


  ClassDef(TDetector,2)
};

#endif
