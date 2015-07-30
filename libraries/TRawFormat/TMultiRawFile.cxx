#include "TMultiRawFile.h"

bool FileEvent::operator<(const FileEvent& other) const{
  if(next_event.GetTimestamp() != other.next_event.GetTimestamp()){
    return next_event.GetTimestamp() < other.next_event.GetTimestamp();
  }

  return file < other.file;
}

TMultiRawFile::TMultiRawFile() { }

TMultiRawFile::~TMultiRawFile(){
  for(auto& val : fFileEvents){
    delete val.file;
  }
}

void TMultiRawFile::AddFile(TRawFileIn* infile){
  FileEvent f;
  f.file = infile;
  infile->Read(&f.next_event);
  fFileEvents.insert(f);
}

void TMultiRawFile::AddFile(const char* filename){
  TRawFileIn* file = new TRawFileIn(filename);
  AddFile(file);
}

int TMultiRawFile::Read(TGEBEvent* outevent){
  if(fFileEvents.begin() == fFileEvents.end()){
    return -1;
  }

  if(!outevent){
    return -2;
  }

  // Pop the event, place in output
  FileEvent output = *fFileEvents.begin();
  fFileEvents.erase(fFileEvents.begin());
  *outevent = output.next_event;

  // If another event exists, put it back into the list
  FileEvent next;
  next.file = output.file;
  int bytes_read = next.file->Read(&next.next_event);
  if(bytes_read > 0){
    fFileEvents.insert(next);
  } else {
    delete output.file;
  }

  return output.next_event.GetTotalSize();
}
