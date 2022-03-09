#ifndef TORDEREDRAWFILE_H
#define TORDEREDRAWFILE_H

#include <map>

#include "TRawEvent.h"
#include "TRawSource.h"
#include "TString.h"

class TOrderedRawFile : public TRawEventSource {

public:
  TOrderedRawFile(TRawEventSource* unordered);
  ~TOrderedRawFile();

  void SetDepth(size_t depth) { this->depth = depth; }
  size_t GetDepth() const { return depth; }
  virtual void Reset() { unordered->Reset(); }

  virtual std::string SourceDescription(bool long_description = false) const {
    return unordered->SourceDescription(long_description);
  }
  virtual std::string Status(bool long_description = false) const {
    static bool returnonce = false;
    if(unordered->IsFinished()) {
      if(!returnonce) {
        printf("\n");
        returnonce=true;
      }
      return Form("  %lu left to sort...          \r",event_queue.size());

    } else {
      return unordered->Status(long_description);
    }
  }
  virtual int GetLastErrno()              const { return unordered->GetLastErrno(); }
  virtual std::string GetLastError()      const { return unordered->GetLastError(); }

private:
  virtual int GetEvent(TRawEvent& event);

  TRawEventSource* unordered;

  size_t depth;
  long oldest_timestamp,newest_timestamp;
  bool finished;

  int fillqueue();

  std::multimap<long,TRawEvent> event_queue;

  ClassDef(TOrderedRawFile,0)
};


#endif
