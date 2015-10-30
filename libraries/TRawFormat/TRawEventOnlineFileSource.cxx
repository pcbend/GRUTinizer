#include "TRawEventOnlineFileSource.h"

#include <chrono>
#include <thread>

#include "FileSize.h"

TRawEventOnlineFileSource::TRawEventOnlineFileSource(const std::string& filename, kFileType file_type)
  : TRawEventByteSource(file_type), fFilename(filename) {
  fFile = fopen(filename.c_str(),"rb");
  SetFileSize(FindFileSize(filename.c_str()));
}

TRawEventOnlineFileSource::~TRawEventOnlineFileSource() {
  fclose(fFile);
}

int TRawEventOnlineFileSource::ReadBytes(char* buf, size_t size){
  while(true){
    int output = single_read(buf, size);
    if(output == -1){
      SetFileSize(FindFileSize(fFilename.c_str()));
      return 0;
    } else {
      return output;
    }
  }
}

int TRawEventOnlineFileSource::single_read(char* buf, size_t size){
  size_t output = fread(buf, 1, size, fFile);

  if(feof(fFile)){
    // Attempted to read past the end of the file
    clearerr(fFile);
    return output;
  }

  // Reading less than expected for any other reason is an error
  if(output != size){
    SetLastErrno(ferror(fFile));
    SetLastError(strerror(GetLastErrno()));
  }

  // Successful read of all bytes available
  return output;
}

std::string TRawEventOnlineFileSource::SourceDescription() const {
  return "Online File: " + fFilename;
}
