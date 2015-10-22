#include "TRawEventOnlineFileSource.h"

#include <chrono>
#include <thread>

#include "FileSize.h"
#include "TGRUTLoop.h"

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
    size_t output = single_read(buf, size);
    if(output == -1){
      if(!TGRUTLoop::Get()->IsRunning()){
        return -1;
      }
      std::this_thread::sleep_for(std::chrono::seconds(5));
      SetFileSize(FindFileSize(fFilename.c_str()));
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

    // If we still got something, then we can continue
    if(output){
      return output;
    } else {
      return -1;
    }
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
