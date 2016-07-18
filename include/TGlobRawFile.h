#ifndef _TGLOBRAWFILE_H_
#define _TGLOBRAWFILE_H_

#ifndef __CINT__
#include <chrono>
#endif

#include <string>

#include "TRawSource.h"
#include "TMultiRawFile.h"

class TGlobRawFile : public TRawEventSource {
public:
  TGlobRawFile(std::string pattern, kFileType file_type = kFileType::UNKNOWN_FILETYPE);

  virtual std::string SourceDescription(bool long_description=false) const {
    return fWrapped.SourceDescription(long_description);
  }
  virtual std::string Status(bool long_description = false) const {
    return fWrapped.Status(long_description);
  }

  virtual int GetLastErrno() const { return fWrapped.GetLastErrno(); }
  virtual std::string GetLastError() const { return fWrapped.GetLastError(); }

  virtual void Reset() {
    TRawEventSource::Reset();
    fWrapped.Reset();
  }


private:
  virtual int GetEvent(TRawEvent& outevent) {
    CheckForFiles();
    return fWrapped.Read(outevent);
  }

  void CheckForFiles();

  std::string fPattern;
  std::set<std::string> fFilesAdded;
  kFileType fFileType;
#ifndef __CINT__
  std::chrono::system_clock::time_point fPreviousCheck;
#endif

  TMultiRawFile fWrapped;

  ClassDef(TGlobRawFile, 0);
};

#endif /* _TGLOBRAWFILE_H_ */
