#include "TSequentialRawFile.h"


TSequentialRawFile::TSequentialRawFile()
  : active_source(0) { }

TSequentialRawFile::~TSequentialRawFile() {
  for(auto source : sources){
    delete source;
  }
}

void TSequentialRawFile::Add(TRawEventSource* infile) {
  sources.push_back(infile);
}

void TSequentialRawFile::Reset() {
  for(auto source : sources){
    source->Reset();
  }
  active_source = 0;
}

std::string TSequentialRawFile::SourceDescription() const {
  if(active_source < sources.size()){
    return sources[active_source]->SourceDescription();
  } else {
    return "";
  }
}

std::string TSequentialRawFile::Status() const {
  if(active_source < sources.size()){
    return sources[active_source]->Status();
  } else {
    return "";
  }
}

int TSequentialRawFile::GetLastErrno() const {
  if(sources.size()){
    return sources[0]->GetLastErrno();
  } else {
    return 0;
  }
}

std::string TSequentialRawFile::GetLastError() const {
  if(active_source < sources.size()){
    return sources[active_source]->GetLastError();
  } else {
    return 0;
  }
}

int TSequentialRawFile::GetEvent(TRawEvent& event) {
  // No sources remaining, we are done.
  if(active_source >= sources.size()) {
    return -1;
  }

  // Try to read from the active source
  int res = sources[active_source]->Read(event);
  if(res >= 0) {
    return res;
  }

  // Active source is invalid, move on to the next one.
  active_source++;

  // Try again with the next source.
  return GetEvent(event);
}
