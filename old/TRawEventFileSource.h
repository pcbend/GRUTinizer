#ifndef _TRAWEVENTFILESOURCE_H_
#define _TRAWEVENTFILESOURCE_H_

#include "TRawEventSource.h"

class TRawEventFileSource : public TRawEventByteSource {
public:
  TRawEventFileSource(const std::string& filename, kFileType file_type);
  ~TRawEventFileSource();

  virtual int ReadBytes(char* buf, size_t size);

  virtual std::string SourceDescription() const;
private:
  std::string fFilename;
  FILE* fFile;

  ClassDef(TRawEventFileSource,0);
};

#endif /* _TRAWEVENTFILESOURCE_H_ */
