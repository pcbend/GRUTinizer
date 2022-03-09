#ifndef _TMULTIRAWFILE_H_
#define _TMULTIRAWFILE_H_

#include <set>
#ifndef __CINT__
#   include <mutex>
#endif

#include "TObject.h"

#include "TRawEvent.h"
#include "TRawSource.h"

struct FileEvent {
  TRawEventSource* file;
  TRawEvent next_event;

  // Needed for keeping the std::set sorted by timestamp
  bool operator<(const FileEvent& other) const;
};

bool file_event_sorting(const FileEvent& a, const FileEvent& b);

class TMultiRawFile : public TRawEventSource {
public:
  TMultiRawFile();
  ~TMultiRawFile();

  void AddFile(TRawEventSource* infile);
  void AddFile(const char* filename);

  bool IsValid() const { return fIsValid; }

  virtual void Reset();

  virtual std::string SourceDescription(bool long_description = false) const;
  virtual std::string Status(bool long_description = false) const;
  virtual int GetLastErrno() const;
  virtual std::string GetLastError() const;

private:
  TMultiRawFile(const TMultiRawFile& other) { }
  TMultiRawFile& operator=(const TMultiRawFile& other) { return *this; }

  std::set<TRawEventSource*> fFileList; // This list does not get modified frequently
  std::set<FileEvent> fFileEvents; // This list is modified frequently

#ifndef __CINT__
  mutable std::mutex fFileListMutex;
#endif

  virtual int GetEvent(TRawEvent& outevent);
  mutable bool fIsFirstStatus;
  bool fIsValid;


  ClassDef(TMultiRawFile, 0);
};




#endif
