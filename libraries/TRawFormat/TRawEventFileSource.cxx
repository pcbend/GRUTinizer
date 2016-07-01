#include "TRawSource.h"

//#include "FileSize.h"

TRawEventFileSource::TRawEventFileSource(const std::string& filename, kFileType file_type)
  : TRawEventByteSource(file_type), fFilename(filename) {
  fFile = fopen(filename.c_str(),"rb");
  SetFileSize(FindFileSize(filename.c_str()));

  if (TGRUTOptions::Get()->FastForwardRawFile() && TGRUTOptions::Get()->StartGUI())  {
    FastForward();
  }
}

TRawEventFileSource::~TRawEventFileSource() {
  fclose(fFile);
}

void TRawEventFileSource::Reset() {
  TRawEventByteSource::Reset();
  fseek(fFile, 0, SEEK_SET);
}

void TRawEventFileSource::FastForward() {
  fseek(fFile, 0, SEEK_END);
}

int TRawEventFileSource::ReadBytes(char* buf, size_t size){
  size_t output = fread(buf, 1, size, fFile);
  if(output != size){
    if(feof(fFile)){
      SetLastErrno(-1);
      SetLastError("EOF");
    } else {
      int error = ferror(fFile);
      SetLastErrno(error);
      SetLastError(strerror(error));
    }
  }
  return output;
}

std::string TRawEventFileSource::SourceDescription() const {
  return "File: " + fFilename;
}
