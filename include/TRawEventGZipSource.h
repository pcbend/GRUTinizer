#ifndef _TRAWEVENTGZIPSOURCE_H_
#define _TRAWEVENTGZIPSOURCE_H_

#include <zlib.h>

#include "TRawEventSource.h"

class TRawEventGZipSource : public TRawEventByteSource {
public:
  TRawEventGZipSource(const std::string& filename, kFileType file_type);
  ~TRawEventGZipSource();

  virtual int ReadBytes(char* buf, size_t size);

  virtual std::string SourceDescription() const;
private:
  std::string fFilename;
  FILE* fFile;
  gzFile* fGzFile;

  ClassDef(TRawEventGZipSource,0);
};

#endif /* _TRAWEVENTGZIPSOURCE_H_ */
