#include "TByteSource.h"

#include <cstring>

TPipeByteSource::TPipeByteSource(const std::string& command)
  : fCommand(command) {
  fPipe = popen(command.c_str(),"r");
}

TPipeByteSource::~TPipeByteSource() {
  pclose(fPipe);
}

void TPipeByteSource::Reset() {
  pclose(fPipe);
  fPipe = popen(fCommand.c_str(),"r");
}

int TPipeByteSource::ReadBytes(char* buf, size_t size){
  size_t output = fread(buf, 1, size, fPipe);
  if(output != size){
    SetLastErrno(ferror(fPipe));
    SetLastError(strerror(GetLastErrno()));
  }
  return output;
}

std::string TPipeByteSource::SourceDescription(bool /*long_description*/) const {
  return "Pipe: " + fCommand;
}
