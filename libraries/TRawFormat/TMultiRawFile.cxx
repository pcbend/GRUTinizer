#include "TMultiRawFile.h"

#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>

#include "Globals.h"

bool FileEvent::operator<(const FileEvent& other) const{
  if(next_event.GetTimestamp() != other.next_event.GetTimestamp()){
    return next_event.GetTimestamp() < other.next_event.GetTimestamp();
  }

  return file < other.file;
}

TMultiRawFile::TMultiRawFile()
  : fIsFirstStatus(true), fIsValid(true) { }

TMultiRawFile::~TMultiRawFile(){
  for(auto& val : fFileEvents){
    delete val.file;
  }
}

void TMultiRawFile::AddFile(TRawEventSource* infile){
  FileEvent f;
  f.file = infile;
  infile->Read(f.next_event);
  fFileEvents.insert(f);
  fFileList.insert(infile);
}

void TMultiRawFile::AddFile(const char* filename){
  TRawEventSource* file = TRawEventSource::EventSource(filename);
  if(file->GetLastErrno() == 0){
    AddFile(file);
  } else {
    std::cerr << "Error opening file " << filename << ": " << file->GetLastError()
              << " (Errno=" << file->GetLastErrno() << ")" << std::endl;
    delete file;
    fIsValid = false;
  }
}

int TMultiRawFile::GetEvent(TRawEvent& outevent){
  if(fFileEvents.begin() == fFileEvents.end()){
    return -1;
  }

  // Pop the event, place in output
  FileEvent output = *fFileEvents.begin();
  fFileEvents.erase(fFileEvents.begin());
  outevent = output.next_event;

  // If another event exists, put it back into the list
  FileEvent next;
  next.file = output.file;
  int bytes_read = next.file->Read(next.next_event);
  if(bytes_read > 0){
    fFileEvents.insert(next);
  } else if (!TGRUTOptions::Get()->ExitAfterSorting()) {
    // if online (not exiting immediately) and no bytes were read from source
    while (bytes_read <= 0) { // stall until source provides next event
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      bytes_read = next.file->Read(next.next_event);
    }
  } else { // otherwise delete the source from the file list
    std::lock_guard<std::mutex> lock(fFileListMutex);
    delete output.file;
    fFileList.erase(output.file);
  }

  return output.next_event.GetTotalSize();
}

void TMultiRawFile::Reset() {
  std::lock_guard<std::mutex> lock(fFileListMutex);

  for(auto& file : fFileList){
    file->Reset();
  }
}

std::string TMultiRawFile::SourceDescription() const{
  std::lock_guard<std::mutex> lock(fFileListMutex);

  std::stringstream ss;
  ss << "Multi file: ";
  int i=0;
  for(auto& file : fFileList){
    ss << i << " = (" << file->SourceDescription() << ") ";
    i++;
  }
  return ss.str();
}

std::string TMultiRawFile::Status() const{
  std::lock_guard<std::mutex> lock(fFileListMutex);

  std::stringstream ss;
  if(!fIsFirstStatus){
    for(unsigned int i = 0; i<fFileList.size(); i++) {
      ss << CURSOR_UP;
    }
  }
  fIsFirstStatus = false;

  size_t max_length = 0;
  for(auto& file : fFileList){
    max_length = std::max(max_length, file->SourceDescription().length());
  }

  for(auto& file : fFileList){
    ss << std::setw(max_length) << file->SourceDescription() << " " << file->Status() << "\n";
  }
  return ss.str();
}

int TMultiRawFile::GetLastErrno() const{
  return 0;
}
std::string TMultiRawFile::GetLastError() const{
  return "";
}
