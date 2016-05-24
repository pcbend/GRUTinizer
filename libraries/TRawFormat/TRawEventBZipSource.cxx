#include "TRawSource.h"

#include "TGRUTUtilities.h"

TRawEventBZipSource::TRawEventBZipSource(const std::string& filename, kFileType file_type)
  : TRawEventPipeSource("bzip2 -dc " + filename, file_type),
    fFilename(filename) { }

std::string TRawEventBZipSource::SourceDescription(bool long_description) const {
  if(long_description) {
    return fFilename;
  } else {
    return get_short_filename(fFilename);
  }
}
