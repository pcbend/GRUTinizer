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
  : fCurrentlyStallingSource(nullptr),
    fIsFirstStatus(true), fIsValid(true) { }

TMultiRawFile::~TMultiRawFile(){
  for(auto& file : fFileList){
    delete file;
  }
}

void TMultiRawFile::AddFile(TRawEventSource* infile){
  FileEvent f;
  f.file = infile;
  infile->Read(f.next_event);
  fFileEvents.insert(f);
  fFileList.push_back(infile);
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
  // If a source has currently stalling, give it up to a minute to recover.
  if(fCurrentlyStallingSource) {
    FileEvent next;
    next.file = fCurrentlyStallingSource;
    int bytes_read = fCurrentlyStallingSource->Read(next.next_event);
    if(bytes_read > 0) {
      fFileEvents.insert(next);
      fCurrentlyStallingSource = nullptr;
    } else if(std::chrono::system_clock::now() > fStallingGiveUp) {
      fStalledSources.insert(fCurrentlyStallingSource);
      fCurrentlyStallingSource = nullptr;
    } else {
      return -1;
    }
  }

  // Every 10 seconds, check all the stalled sources.
  if (fStalledSources.size() &&
      std::chrono::system_clock::now() > fNextStalledCheck) {

    std::set<TRawEventSource*> stalled_sources(fStalledSources);
    for(auto source : stalled_sources) {
      FileEvent next;
      next.file = source;
      int bytes_read = source->Read(next.next_event);
      if (bytes_read > 0) {
        fFileEvents.insert(next);
        fStalledSources.erase(source);
      }
    }

    // Schedule the next stalled-source check for 10 seconds from now.
    fNextStalledCheck = std::chrono::system_clock::now() + std::chrono::seconds(10);
  }

  // No events remaining means that we are at the end of all files.
  if(fFileEvents.size() == 0){
    return -1;
  }

  // Pop the earliest event, place in output
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
    // Source is now stalling, give it a 60-second grace period.
    fCurrentlyStallingSource = next.file;
    fStallingGiveUp = std::chrono::system_clock::now() + std::chrono::seconds(60);
  }

  return outevent.GetTotalSize();
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
