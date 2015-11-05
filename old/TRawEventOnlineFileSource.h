#ifndef _TRAWEVENTONLINEFILESOURCE_H_
#define _TRAWEVENTONLINEFILESOURCE_H_

#include "TRawEventSource.h"

class TRawEventOnlineFileSource : public TRawEventByteSource {
public:
  TRawEventOnlineFileSource(const std::string& filename, kFileType file_type);
  ~TRawEventOnlineFileSource();

  virtual int ReadBytes(char* buf, size_t size);

  virtual std::string SourceDescription() const;
private:
  int single_read(char* buf, size_t size);

  std::string fFilename;
  FILE* fFile;

  ClassDef(TRawEventOnlineFileSource,0);
};

#endif /* _TRAWEVENTONLINEFILESOURCE_H_ */
