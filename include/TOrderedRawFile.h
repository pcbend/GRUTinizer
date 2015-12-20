#ifndef TORDEREDRAWFILE_H
#define TORDEREDRAWFILE_H

#include <map>

#include "TRawEvent.h"
#include "TRawSource.h"

class TOrderedRawFile : public TRawEventSource {

public:
  TOrderedRawFile(TRawEventSource* unordered);
  ~TOrderedRawFile();

  void SetDepth(int depth) { this->depth = depth; }
  int GetDepth() { return depth; }

  virtual std::string SourceDescription() const { return unordered->SourceDescription(); }
  virtual std::string Status()            const { return unordered->Status(); }
  virtual int GetLastErrno()              const { return unordered->GetLastErrno(); }
  virtual std::string GetLastError()      const { return unordered->GetLastError(); }

private:
  virtual int GetEvent(TRawEvent& event);

  TRawEventSource* unordered;

  int depth;
  long oldest_timestamp,newest_timestamp;
  bool finished;

  int fillqueue();

  std::multimap<long,TRawEvent> event_queue;

  ClassDef(TOrderedRawFile,0)
};


#endif
