#ifndef _TRAWFILE_H_
#define _TRAWFILE_H_

#include "TRawEventSource.h"

class TRawFile : public TRawEventSource {
public:
  TRawFile(const char* filename, kFileType file_type = kFileType::UNKNOWN_FILETYPE);
  ~TRawFile();

  virtual std::string SourceDescription() const {
    return wrapped->SourceDescription();
  }

  virtual std::string Status() const {
    return wrapped->Status();
  }

  virtual int GetLastErrno() const {
    return wrapped->GetLastErrno();
  }

  virtual std::string GetLastError() const {
    return wrapped->GetLastError();
  }


private:
  virtual int GetEvent(TRawEvent& event) {
    return wrapped->Read(event);
  }

  TRawEventSource* wrapped;
};

class TRawFileIn : public TRawFile {
public:
  TRawFileIn(const char* filename, kFileType file_type = kFileType::UNKNOWN_FILETYPE)
    : TRawFile(filename, file_type) { }
};


#endif /* _TRAWFILE_H_ */
