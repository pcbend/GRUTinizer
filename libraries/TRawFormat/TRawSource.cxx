#include "TRawSource.h"

#include <cassert>

#include "TString.h"

#include "TGRUTOptions.h"

ClassImp(TRawEventSource)

int TRawEventSource::Read(TRawEvent& event){
  if(fIsFinished){
    return -6;
  } else {
    int result = GetEvent(event);
    if(result > 0){
      UpdateByteThroughput(event.GetTotalSize());
    } else if (result < 0 && TGRUTOptions::Get()->ExitAfterSorting()) {
      fIsFinished = true;
    }
    return result;
  }
}

int TRawEventSource::Read(TRawEvent* event){
  if(event){
    return Read(*event);
  } else {
    return -7;
  }
}

void TRawEventSource::Reset() {
  fIsFinished = false;
  fLastErrno = 0;
  fLastError = "";
  fBytesGiven = 0;
}

void TRawEventSource::UpdateByteThroughput(int bytes) {
  if(!fBytesPerSecond.size()){
    fBytesPerSecond.push_back(0);
    current_time = time(NULL);
  }

  fBytesGiven += bytes;
  fBytesPerSecond.back() += bytes;

  if(current_time != time(NULL)){
    current_time = time(NULL);
    fBytesPerSecond.push_back(0);
    if(fBytesPerSecond.size() > 5) {
      fBytesPerSecond.pop_front();
    }
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

  TByteSource* byte_source = 0;
  // If it is a ring, open it
  if(is_ring){
    byte_source = new TRingByteSource(filename);
  // If it is an archived file, open it as such
  } else if(hasSuffix(filename,".bz2")){
    byte_source = new TBZipByteSource(filename);
  } else if (hasSuffix(filename,".gz")){
    byte_source = new TGZipByteSource(filename);
  // Otherwise, open it as a normal file.
  } else {
    byte_source = new TFileByteSource(filename);
  }

  TRawEventSource* source = new TRawEventTimestampSource(byte_source, file_type);

  return source;
}

double TRawEventSource::GetAverageRate() const {
  ((TRawEventSource*)this)->UpdateByteThroughput(0);

  int n = 0;
  double sum = 0;
  for(auto val : fBytesPerSecond) {
    n++;
    sum += val;
  }
  return sum/n;
}

ClassImp(TRawEventTimestampSource)

TRawEventTimestampSource::TRawEventTimestampSource(TByteSource* byte_source, kFileType file_type)
  : fByteSource(byte_source), fFileType(file_type),
    fDefaultBufferSize(8192) { }

TRawEventTimestampSource::~TRawEventTimestampSource() {
  if(fByteSource) {
    delete fByteSource;
  }
}

void TRawEventTimestampSource::Reset() {
  TRawEventSource::Reset();
  fCurrentBuffer = TSmartBuffer();
  fByteSource->Reset();
}

std::string TRawEventTimestampSource::Status(bool long_description) const {
  return Form("%s: %s %8.2f MB given %s / %s %8.2f MB total %s  => %s %3.02f MB/s processed %s",
              SourceDescription(long_description).c_str(),
              DCYAN, GetBytesGiven()/1e6, RESET_COLOR,
              BLUE,  GetFileSize()/1e6, RESET_COLOR,
              GREEN, GetAverageRate()/1e6, RESET_COLOR);
}

int TRawEventTimestampSource::GetEvent(TRawEvent& rawevent) {
  switch(fFileType) {
    case kFileType::NSCL_EVT:
    case kFileType::GRETINA_MODE2:
    case kFileType::GRETINA_MODE3:
      break;

    default:
      std::cout << "Unknown file type: " << fFileType << std::endl;
      return -4;
      break;
  }

  rawevent.Clear();
  rawevent.SetFileType(fFileType);

  int bytes_read_header = FillBuffer(sizeof(TRawEvent::RawHeader));
  // If not enough was read, check the errno.
  // If it is 0, we may still receive more data. (e.g. reading from ring)
  // If it is nonzero, we are done. (e.g. end of file)
  if(bytes_read_header == 0) {
    if(GetLastErrno()){
      return -1;
    } else {
      return 0;
    }
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

int TRawEventTimestampSource::FillBuffer(size_t bytes_requested) {
  // //std::cout << "UNBUFFERED READ" << std::endl;
  // char* buf = (char*)malloc(bytes_requested);
  // size_t bytes_read = fByteSource->ReadBytes(buf, bytes_requested);
  // fCurrentBuffer = TSmartBuffer(buf, bytes_requested);
  // if(bytes_read != bytes_requested){
  //   return -2;
  // } else {
  //   return bytes_read;
  // }

  if(fCurrentBuffer.GetSize() >= bytes_requested){
    return bytes_requested;
  }

  size_t bytes_allocating = std::max(fDefaultBufferSize, bytes_requested);
  char* buf = (char*)malloc(bytes_allocating);

  // Copy any leftover bytes from the previous buffer.
  size_t bytes_to_copy = fCurrentBuffer.GetSize();
  memcpy(buf, fCurrentBuffer.GetData(), bytes_to_copy);

  // Read to fill the buffer.
  size_t bytes_to_read = bytes_allocating - bytes_to_copy;
  size_t bytes_read = fByteSource->ReadBytes(buf + bytes_to_copy, bytes_to_read);;

  // Store everything
  fCurrentBuffer = TSmartBuffer(buf, bytes_to_copy + bytes_read);

  // Set the error flags and return code appropriately.
  if(bytes_read == 0 && GetLastErrno()){
    SetLastErrno(0);
    SetLastError("EOF");
    return -1;
  } else if (bytes_read == 0) {
    return 0;
  } else if ((bytes_read+bytes_to_copy) < bytes_requested){
    SetLastErrno(errno);
    SetLastError(strerror(errno));
    return -2;
  } else {
    return bytes_requested;
  }
}
