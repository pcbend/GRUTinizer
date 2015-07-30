#ifndef _TMULTIRAWFILE_H_
#define _TMULTIRAWFILE_H_

#include <set>
#ifndef __CINT__
#   include <mutex>
#endif

#include "TObject.h"

#include "TGEBEvent.h"
#include "TRawFile.h"

struct FileEvent {
  TRawFileIn* file;
  TGEBEvent next_event;

  // Needed for keeping the std::set sorted by timestamp
  bool operator<(const FileEvent& other) const;
};

bool file_event_sorting(const FileEvent& a, const FileEvent& b);

class TMultiRawFile : public TObject, public TRawEventSource {
public:
  TMultiRawFile();
  ~TMultiRawFile();
  TMultiRawFile(const TMultiRawFile& other) { MayNotUse("TMultiRawFile()"); }
  TMultiRawFile& operator=(const TMultiRawFile& other) { MayNotUse("TMultiRawFile::operator=()"); }

  void AddFile(TRawFileIn* infile);
  void AddFile(const char* filename);

  virtual bool IsFinished() const;
  virtual std::string SourceDescription() const;
  virtual std::string Status() const;
  virtual int GetLastErrno() const;
  virtual std::string GetLastError() const;

private:
  std::set<TRawFileIn*> fFileList; // This list does not get modified frequently
  std::set<FileEvent> fFileEvents; // This list is modified frequently

#ifndef __CINT__
  mutable std::mutex fFileListMutex;
#endif

  virtual int GetEvent(TRawEvent* outevent);
  mutable bool fIsFirstStatus;


  ClassDef(TMultiRawFile, 0);
};




#endif
