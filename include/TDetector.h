#ifndef _TDETECTOR_H_
#define _TDETECTOR_H_

#include <vector>
#include <cmath>

#include "TBuffer.h"
#include "TDetectorHit.h"
#include "TString.h"


#ifndef __CINT__
class TRawEvent;
#else
#include <TRawEvent.h>
#endif
class TSmartBuffer;

class TDetector {
public:
  TDetector();
  TDetector(const char *name,const char *title="");
  virtual ~TDetector();

  void Copy(TDetector& obj) const;
  virtual void Clear(Option_t *opt = "" );
  virtual void Print(Option_t *opt = "" ) const;
  int  Compare(const TDetector& obj) const;

  int Build(std::vector<TRawEvent>& raw_data);
  virtual void InsertHit(const TDetectorHit&) = 0;
  virtual TDetectorHit& GetHit(int i)         = 0;

  //virtual size_t Size() const { return (size_t)fSize; }
  virtual size_t Size() const = 0; //{ return (size_t)fSize; }
  //virtual unsigned long Size() const = 0; //{ return (size_t)fSize; }
  //virtual unsigned int Size() const = 0; //{ return (size_t)fSize; }


  Long_t Timestamp() const { return fTimestamp; }
  void   SetTimestamp(Long_t timestamp)  { fTimestamp = timestamp; }

  bool IsBuilt() const { return fBuilt; }
  bool IsUnbuilt() const { return !fBuilt; }

  const char* GetName() const { return fName.Data(); }
  const char* GetTitle() const { return fTitle.Data(); }
  void SetName(const char* name) { fName = name ? name : ""; }
  void SetTitle(const char* title) { fTitle = title ? title : ""; }
  void SetNameTitle(const char* name, const char* title) { SetName(name); SetTitle(title); }
  
  unsigned int RunStart() const { return fRunStart; }
  virtual void SetRunStart(unsigned int unix_time) { fRunStart = unix_time; }

   
  
  void AddRawData(TRawEvent *event) { fRawData.push_back(event); }
  int Build(); // build from transient data member.


protected:
  int fSize;
  Long_t fTimestamp;

  /// Start of the run, in unix time.
  /**
     This comes from the BEGIN_RUN packet in NSCL data.
     It is the number of seconds since January 1, 1970.
     If unavailable, will be 0.
   **/
  unsigned int fRunStart; 
  bool fBuilt;
  TString fName;
  TString fTitle;


private:
  virtual int  BuildHits(std::vector<TRawEvent>& raw_data) = 0;
  virtual int  BuildHits(std::vector<TRawEvent*> &raw_data);
  std::vector<TRawEvent*> fRawData;  //!

  ClassDef(TDetector,2)
};

#endif
