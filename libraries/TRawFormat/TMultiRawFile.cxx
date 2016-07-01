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
  //   // // if in online mode and the current source is at its end, delete it
  //   // std::cout << next.file->Status() << std::endl;
  //   // TRawEventFileSource* filein = dynamic_cast<TRawEventFileSource*>(next.file);
  //   // if (filein) {
  //   //         std::cout << filein->SourceDescription() << " : " << (filein->GetFileSize() - filein->GetBytesGiven()) << std::endl;
  //   //   if ((filein->GetFileSize() - filein->GetBytesGiven()) == 0) {
  //   //     std::lock_guard<std::mutex> lock(fFileListMutex);
  //   //     delete output.file;
  //   //     fFileList.erase(output.file);
  //   //   }
  //   // } else { // otherwise it is a stalled source and we should wait until data is available for coincidence building
  //   //   stalled_source = next.file;
  //   // }
    stalled_source = next.file;
  }
  else { // otherwise delete the source from the file list
    std::cout << "######################################\n";
    std::cout << std::endl << "Deleting source: " << next.file->SourceDescription() << std::endl;
    std::cout << next.file->Status() << std::endl << std::endl;
    std::cout << "######################################\n";

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
