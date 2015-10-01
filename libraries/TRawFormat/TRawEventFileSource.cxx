#include "TRawEventFileSource.h"

#include "FileSize.h"

TRawEventFileSource::TRawEventFileSource(const std::string& filename, kFileType file_type)
  : TRawEventByteSource(file_type), fFilename(filename) {
  fFile = fopen(filename.c_str(),"rb");
  SetFileSize(FindFileSize(filename.c_str()));
}

TRawEventFileSource::~TRawEventFileSource() {
  fclose(fFile);
}

int TRawEventFileSource::ReadBytes(char* buf, size_t size){
  size_t output = fread(buf, 1, size, fFile);
  if(output != size){
    SetLastErrno(ferror(fFile));
    SetLastError(strerror(GetLastErrno()));
  }
  return output;
}

std::string TRawEventFileSource::SourceDescription() const {
  return "File: " + fFilename;
}
