#include "TMultiRawFile.h"

#include <iomanip>
#include <sstream>

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

  static TRawEventSource* stalled_source = nullptr;
  if (stalled_source) {
    FileEvent next;
    int bytes_read = stalled_source->Read(next.next_event);
    if (bytes_read > 0) {
      next.file = stalled_source;
      fFileEvents.insert(next);
      stalled_source = nullptr;
    }
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
    stalled_source = next.file;
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

std::string TMultiRawFile::SourceDescription(bool long_description) const{
  std::lock_guard<std::mutex> lock(fFileListMutex);

  std::stringstream ss;
  ss << "Multi file: ";
  int i=0;
  for(auto& file : fFileList){
    ss << i << " = (" << file->SourceDescription(long_description) << ") ";
    i++;
  }
  return ss.str();
}

std::string TMultiRawFile::Status(bool long_description) const{
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
    max_length = std::max(max_length, file->SourceDescription(long_description).length());
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
