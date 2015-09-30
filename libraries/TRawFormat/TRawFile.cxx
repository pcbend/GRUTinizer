#include "TRawFile.h"

#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include <Rtypes.h>

#include "TString.h"

#include "FileSize.h"
#include "TRawEvent.h"
#include "TGRUTOptions.h"

ClassImp(TRawEventSource);

int TRawEventSource::Read(TRawEvent& event){
  if(fIsFinished){
    return 0;
  } else {
    int result = GetEvent(event);
    if(result > 0){
      fBytesGiven += event.GetTotalSize();
    } else {
      fIsFinished = true;
    }
    return result;
  }
}

namespace{
  int hasSuffix(const char *name,const char *suffix) {
    const char *s = strstr(name,suffix);
    if(s==NULL)
      return 0;
    return (s-name)+strlen(suffix) == strlen(name);
  }
}

TRawEventSource* TRawEventSource::EventSource(const char* filename, kFileType file_type,
                                              bool is_online, bool is_ring){
  if(file_type == kFileType::UNKNOWN_FILETYPE){
    file_type = TGRUTOptions::Get()->DetermineFileType(filename);
  }

  if(hasSuffix(filename,".bz2")){
    return new TRawEventBZipSource(filename, file_type);
  } else if (hasSuffix(filename,".gz")){
    return new TRawEventGZipSource(filename, file_type);
  } else {
    return new TRawEventFileSource(filename, file_type);
  }
}


ClassImp(TRawEventByteSource);

TRawEventByteSource::TRawEventByteSource(kFileType file_type)
  : fFileType(file_type), fFileSize(-1),
    fDefaultBufferSize(8192) {
  clock.Start();
}

std::string TRawEventByteSource::Status() const {
  double runtime = clock.RealTime();
  clock.Continue();
  return Form("%s %8.2f MB given %s / %s %8.2f MB total %s  => %s %3.02f MB/s processed %s",
              DCYAN, GetBytesGiven()/1e6, RESET_COLOR,
              BLUE,  GetFileSize()/1e6, RESET_COLOR,
              GREEN, GetBytesGiven()/(1e6*runtime), RESET_COLOR);
}

int TRawEventByteSource::GetEvent(TRawEvent& rawevent) {
  switch(fFileType) {
    case kFileType::NSCL_EVT:
    case kFileType::GRETINA_MODE2:
    case kFileType::GRETINA_MODE3:
      break;

    default:
      std::cout << "Unknown file type: " << fFileType << std::endl;
      return 0;
      break;
  }

  rawevent.Clear();
  rawevent.SetFileType(fFileType);

  int bytes_read_header = FillBuffer(sizeof(TRawEvent::RawHeader));
  if(bytes_read_header < 0){
    return -1;
  }

  memcpy(rawevent.GetRawHeader(), fCurrentBuffer.GetData(), sizeof(TRawEvent::RawHeader));
  fCurrentBuffer.Advance(sizeof(TRawEvent::RawHeader));

  if(!rawevent.IsGoodSize()) {
    SetLastErrno(-1);
    SetLastError("Invalid event size");
    return -3;
  }

  size_t body_size = rawevent.GetBodySize();
  int bytes_read_body = FillBuffer(body_size);
  if(bytes_read_body < 0){
    return -2;
  }

  rawevent.SetData(fCurrentBuffer.BufferSubset(0, body_size));
  fCurrentBuffer.Advance(body_size);

  size_t total_bytes = sizeof(TRawEvent::RawHeader) + body_size;
  return total_bytes;
}

int TRawEventByteSource::FillBuffer(size_t bytes_requested) {
  if(fCurrentBuffer.GetSize() >= bytes_requested){
    return 0;
  }

  size_t bytes_allocating = std::max(fDefaultBufferSize, bytes_requested);
  char* buf = (char*)malloc(bytes_allocating);

  // Copy any leftover bytes from the previous buffer.
  size_t bytes_to_copy = fCurrentBuffer.GetSize();
  memcpy(buf, fCurrentBuffer.GetData(), bytes_to_copy);

  // Read to fill the buffer.
  size_t bytes_to_read = bytes_allocating - bytes_to_copy;
  size_t bytes_read = ReadBytes(buf + bytes_to_copy, bytes_to_read);;

  // Store everything
  fCurrentBuffer = TSmartBuffer(buf, bytes_to_copy + bytes_read);

  // Set the error flags and return code appropriately.
  if(bytes_read == 0){
    SetLastErrno(0);
    SetLastError("EOF");
    return -1;
  } else if ((bytes_read+bytes_to_copy) < bytes_requested){
    SetLastErrno(errno);
    SetLastError(strerror(errno));
    return -2;
  } else {
    return bytes_read;
  }
}

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

TRawEventPipeSource::TRawEventPipeSource(const std::string& command, kFileType file_type)
  : TRawEventByteSource(file_type), fCommand(command) {
  fPipe = popen(command.c_str(),"r");
}

TRawEventPipeSource::~TRawEventPipeSource() {
  pclose(fPipe);
}

int TRawEventPipeSource::ReadBytes(char* buf, size_t size){
  size_t output = fread(buf, 1, size, fPipe);
  if(output != size){
    SetLastErrno(ferror(fPipe));
    SetLastError(strerror(GetLastErrno()));
  }
  return output;
}

std::string TRawEventPipeSource::SourceDescription() const {
  return "Pipe: " + fCommand;
}

TRawEventBZipSource::TRawEventBZipSource(const std::string& filename, kFileType file_type)
  : TRawEventPipeSource("bzip2 -dc " + filename, file_type),
    fFilename(filename) { }

std::string TRawEventBZipSource::SourceDescription() const {
  return "BZip File: " + fFilename;
}
