#include "TRawEventGZipSource.h"


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

int TRawEventGZipSource::ReadBytes(char* buf, size_t size){
  size_t output = gzread(*fGzFile, buf, size);
  if(output != size){
    SetLastErrno(errno);
    SetLastError(strerror(GetLastErrno()));
  }
  return output;
}

std::string TRawEventGZipSource::SourceDescription() const {
  return "GZip File: " + fFilename;
}
