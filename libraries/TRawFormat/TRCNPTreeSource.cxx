#include "TRCNPTreeSource.h"

#include <iomanip>
#include <sstream>

#include "Globals.h"

TRCNPTreeSource::TRCNPTreeSource(const char* filename)
  : fIsFirstStatus(true), fIsValid(true) { fChain.Add(filename); }

TRCNPTreeSource::~TRCNPTreeSource(){
  // for(auto& val : fFileEvents){
  //   delete val.file;
  // }
}

int TRCNPTreeSource::GetEvent(TRawEvent& outevent){

}

void TRCNPTreeSource::Reset() {

}

std::string TRCNPTreeSource::SourceDescription() const{
  return "TRCNPTreeSource::SourceDescription needs definition";
}

std::string TRCNPTreeSource::Status() const{
  return "TRCNPTreeSource::Status needs definition";
}

int TRCNPTreeSource::GetLastErrno() const{
  return 0;
}
std::string TRCNPTreeSource::GetLastError() const{
  return "";
}
