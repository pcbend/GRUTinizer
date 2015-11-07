
#include <string>
#include <algorithm>

#include "TRawSource.h"
#include "TGRUTOptions.h"

TRawFile::TRawFile(const char* filename, kFileType file_type) {
  wrapped = TRawEventSource::EventSource(filename,
                                         TGRUTOptions::Get()->IsOnline(),
                                         false, //not a ring
                                         file_type);

  std::string name = filename;
  std::replace(name.begin(),name.end(),'/','_');

  SetNameTitle(name.c_str(),name.c_str());
}

TRawFile::~TRawFile() {
  delete wrapped;
}
