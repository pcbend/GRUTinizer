#ifdef RCNP
#ifndef _TRCNPSOURCE_H_
#define _TRCNPSOURCE_H_

#include <queue>
#include <cassert>
#include <fstream>
#include <sstream>
#include <queue>
#include <stdexcept>

#ifndef __CINT__
#include <thread>
#include <future>
#include <functional>
#endif

#include "TObject.h"
#include "TRawEvent.h"
#include "TRawSource.h"
#include "TChain.h"
#include "TFile.h"

#include "RCNPEvent.h"
#include "ThreadsafeQueue.h"

class TRCNPSource : public TRawEventSource {
public:

  TRCNPSource(const char* Command, kFileType file_type);
  TRCNPSource(const TRCNPSource& source) { }
  ~TRCNPSource() {;}

  virtual std::string Status() const;
  virtual std::string SourceDescription() const;
  kFileType GetFileType() const { return fFileType; }
  long GetFileSize() const { return fFileSize; }
  virtual void Reset() {;}

protected:
  void SetFileSize(long file_size) { fFileSize = file_size; }
  void LoadFakeTimestamps();

private:
  TRCNPSource() {;}
  virtual int GetEvent(TRawEvent& event);

  const std::string fCommand;
  kFileType fFileType;
  long fFileSize;
  #ifndef __CINT__
  std::future<void> fFuture;
  #endif
  ThreadsafeQueue<RCNPEvent*> rcnp_queue;

  std::queue<Long_t> timestamps;

  ClassDef(TRCNPSource,0);
};


#endif













#else // if RCNP is not defined


#ifndef _TRCNPSOURCE_H_
#define _TRCNPSOURCE_H_
#include "TObject.h"
#include "TRawEvent.h"
#include "TRawSource.h"
class TRCNPSource : public TRawEventSource {
public:
  TRCNPSource(const char* Command, kFileType file_type) {}
  TRCNPSource(const TRCNPSource& source) { }
  ~TRCNPSource() {;}
  virtual std::string Status() const { return ""; }
  virtual std::string SourceDescription() const { return ""; }
  kFileType GetFileType() const { return kFileType::UNKNOWN_FILETYPE; }
  long GetFileSize() const { return 0; }
  virtual void Reset() {;}
protected:
  void SetFileSize(long file_size) { ; }
private:
  TRCNPSource() {;}
  virtual int GetEvent(TRawEvent& event) { event.SetFragmentTimestamp(0); return -1; }
  ClassDef(TRCNPSource,0);
};
#endif


#endif // RCNP
