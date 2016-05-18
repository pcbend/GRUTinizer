#include "TRawSource.h"

#include "TGRUTUtilities.h"

TRawEventGZipSource::TRawEventGZipSource(const std::string& filename, kFileType file_type)
  : TRawEventByteSource(file_type), fFilename(filename) {
  fFile = fopen(filename.c_str(),"rb");
  fGzFile = new gzFile;
  *fGzFile = gzdopen(fileno(fFile),"rb");
}

TRawEventGZipSource::~TRawEventGZipSource() {
  gzclose(*fGzFile);
  delete fGzFile;
  fclose(fFile);
}

void TRawEventGZipSource::Reset() {
  TRawEventByteSource::Reset();
  gzseek(*fGzFile, 0, SEEK_SET);
}

int TRawEventGZipSource::ReadBytes(char* buf, size_t size){
  int output = gzread(*fGzFile, buf, size);
  if(size_t(output) != size){
    if(gzeof(*fGzFile)){
      SetLastErrno(-1);
      SetLastError("EOF");
    } else {
      int errnum;
      gzerror(*fGzFile, &errnum);
      SetLastErrno(errnum);
      SetLastError(strerror(GetLastErrno()));
    }
  }
  return output;
}

std::string TRawEventGZipSource::SourceDescription(bool long_description) const {
  if(long_description) {
    return fFilename;
  } else {
    return get_short_filename(fFilename);
  }
}
