#include "TDetector.h"

#include <TRawEvent.h>
#include <iostream>

#include <TClass.h>
#include <TBuffer.h>

ClassImp(TDetector)

TDetector::TDetector() {
  Clear();
}

TDetector::TDetector(const char* name,const char* title) {
  Clear();
  SetNameTitle(name,title);
}

TDetector::~TDetector() { }

void TDetector::Clear(Option_t *opt) {
  fBuilt = false;
  fTimestamp = -1;
  fSize = 0;
  fRunStart = 0;
  fRawData.clear();
}

void TDetector::Print(Option_t *opt) const { }

void TDetector::Copy(TDetector& det) const {
  det.fTimestamp = fTimestamp;
  det.fSize = fSize;
  det.fRunStart = fRunStart;
  det.fBuilt = fBuilt;
  det.fName = fName;
  det.fTitle = fTitle;
}

int TDetector::Compare(const TDetector& det) const {
  if(fTimestamp < det.fTimestamp) {
    return -1;
  } else if (fTimestamp > det.fTimestamp) {
    return +1;
  } else {
    return 0;
  }
}

int TDetector::Build(std::vector<TRawEvent>& raw_data){
  int output = BuildHits(raw_data);
  //if(output>0){
  fBuilt = true;  // if we called build on it, assume it is built whether or not it actually made any hits.  pcb.
  //}
  return output;
}




int TDetector::Build() {
  //this is a debug hack added by pcb on 5/1/16
  return BuildHits(fRawData);
}

int TDetector::BuildHits(std::vector<TRawEvent*> &raw_data) { 
  //this is a debug hack added by pcb on 5/1/16
  std::vector<TRawEvent> event;
  for(auto it : fRawData) {
    event.push_back(*it);
  }
  return BuildHits(event);
}


void TDetector::Streamer(TBuffer &r_b) {
  if(r_b.IsReading()) {
    r_b.ReadClassBuffer(TDetector::Class(),this);
  } else { 
    r_b.WriteClassBuffer(TDetector::Class(),this);
  }
}

