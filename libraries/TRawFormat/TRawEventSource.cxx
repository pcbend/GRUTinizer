#include "TRawEventSource.h"

#include <cassert>

#include "TString.h"

#include "TGRUTOptions.h"
#include "TRawEventBZipSource.h"
#include "TRawEventFileSource.h"
#include "TRawEventGZipSource.h"
#include "TRawEventRingSource.h"
#include "TRawEventOnlineFileSource.h"

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

int TRawEventSource::Read(TRawEvent* event){
  if(event){
    return Read(*event);
  } else {
    return -1;
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

kFileType TRawEventSource::DefaultFileType() {
  return TGRUTOptions::Get()->DefaultFileType();
}

TRawEventSource* TRawEventSource::EventSource(const char* filename,
                                              bool is_online, bool is_ring,
                                              kFileType file_type){
  if(file_type == kFileType::UNKNOWN_FILETYPE){
    if(is_ring){
      std::cerr << "File type determination does not work for ring sources" << std::endl;
      assert(false);
    }
    file_type = TGRUTOptions::Get()->DetermineFileType(filename);
  }

  // If it is a ring, open it
  if(is_ring){
    return new TRawEventRingSource(filename, file_type);
  // If it is an archived file, open it as such
  } else if(hasSuffix(filename,".bz2")){
    return new TRawEventBZipSource(filename, file_type);
  } else if (hasSuffix(filename,".gz")){
    return new TRawEventGZipSource(filename, file_type);
  // If it is an in-progress file, open it that way
  } else if (is_online) {
    return new TRawEventOnlineFileSource(filename, file_type);
  // Otherwise, open it as a normal file.
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
