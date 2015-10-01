#include "TRawEventBZipSource.h"

TRawEventBZipSource::TRawEventBZipSource(const std::string& filename, kFileType file_type)
  : TRawEventPipeSource("bzip2 -dc " + filename, file_type),
    fFilename(filename) { }

std::string TRawEventBZipSource::SourceDescription() const {
  return "BZip File: " + fFilename;
}
