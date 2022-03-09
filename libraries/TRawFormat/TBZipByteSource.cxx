#include "TRawSource.h"

#include "TGRUTUtilities.h"

TBZipByteSource::TBZipByteSource(const std::string& filename)
  : TPipeByteSource("bzip2 -dc " + filename),
    fFilename(filename) { }

std::string TBZipByteSource::SourceDescription(bool long_description) const {
  if(long_description) {
    return fFilename;
  } else {
    return get_short_filename(fFilename);
  }
}
