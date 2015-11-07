#include "TRawSource.h"

#include "TGRUTOptions.h"

TRawFile::TRawFile(const char* filename, kFileType file_type) {
  wrapped = TRawEventSource::EventSource(filename,
                                         TGRUTOptions::Get()->IsOnline(),
                                         false, //not a ring
                                         file_type);
  SetNameTitle(filename,filename);
}

TRawFile::~TRawFile() {
  delete wrapped;
}
