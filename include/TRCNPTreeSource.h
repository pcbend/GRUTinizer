#ifndef _TRCNPTREESOURCE_H_
#define _TRCNPTREESOURCE_H_

#include <set>
#ifndef __CINT__
#   include <mutex>
#endif

#include "TObject.h"
#include "TRawEvent.h"
#include "TRawSource.h"
#include "TChain.h"
#include "TTree.h"
#include "rootalyze.h"

class TRCNPTreeSource : public TRawEventSource {
public:
  TRCNPTreeSource(const char* filename);
  ~TRCNPTreeSource();


  void Add (const char* filename) {
    fChain.Add(filename);
  }

  bool IsValid() const { return fIsValid; }

  virtual void Reset();

  virtual std::string SourceDescription() const; // required
  virtual std::string Status() const; // required
  virtual int GetLastErrno() const;
  virtual std::string GetLastError() const;

private:
  TRCNPTreeSource(const TRCNPTreeSource& other) { }
  TRCNPTreeSource& operator=(const TRCNPTreeSource& other) { return *this; }

  //std::vector<string> fFileList; // This list does not get modified frequently
  TChain fChain;
  std::set<RCNPTREE*> fFileEvents; // This list is modified frequently

#ifndef __CINT__
  mutable std::mutex fFileListMutex;
#endif


  virtual int GetEvent(TRawEvent& outevent); // required
  bool fIsValid;


  ClassDef(TRCNPTreeSource, 0);
};




#endif
