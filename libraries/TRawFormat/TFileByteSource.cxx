#include "TByteSource.h"

#include "TGRUTUtilities.h"

TFileByteSource::TFileByteSource(const std::string& filename)
  : fFilename(filename) {
  fFile = fopen(filename.c_str(),"rb");
  SetFileSize(FindFileSize(filename.c_str()));
}

TFileByteSource::~TFileByteSource() {
  fclose(fFile);
}

void TFileByteSource::Reset() {
  fseek(fFile, 0, SEEK_SET);
}

int TFileByteSource::ReadBytes(char* buf, size_t size){
  size_t output = fread(buf, 1, size, fFile);
  if(output == size){
    // Clear EOF if there was a sccuessful read.
    if(GetLastErrno() < 0) {
      SetLastErrno(0);
      SetLastError("");
    }
  } else if(feof(fFile)){
    SetLastErrno(-1);
    SetLastError("EOF");
    clearerr(fFile); // otherwise subsequent calls will automatically fail
  } else {
    int error = ferror(fFile);
    SetLastErrno(error);
    SetLastError(strerror(error));
  }

  return output;
}

std::string TFileByteSource::SourceDescription(bool long_description) const {
  if(long_description) {
    return fFilename;
  } else {
    return get_short_filename(fFilename);
  }
}
