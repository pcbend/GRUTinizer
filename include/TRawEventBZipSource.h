#ifndef _TRAWEVENTBZIPSOURCE_H_
#define _TRAWEVENTBZIPSOURCE_H_

#include <string>

#include "TRawEventSource.h"

class TRawEventBZipSource : public TRawEventPipeSource {
public:
  TRawEventBZipSource(const std::string& filename, kFileType file_type);
  ~TRawEventBZipSource() { }

  virtual std::string SourceDescription() const;

private:
  std::string fFilename;

  ClassDef(TRawEventBZipSource,0);
};

#endif /* _TRAWEVENTBZIPSOURCE_H_ */
