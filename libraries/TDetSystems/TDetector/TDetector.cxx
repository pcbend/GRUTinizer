#include "TDetector.h"

#include <iostream>

#include "TRawEvent.h"

#include "TBuffer.h"
#include "TClass.h"

ClassImp(TDetector)

TDetector::TDetector() {
  Class()->CanIgnoreTObjectStreamer();
  Clear();
}

TDetector::TDetector(const char* name,const char* title) {
  Clear();
  TNamed::SetNameTitle(name,title);
  Class()->CanIgnoreTObjectStreamer();
}

TDetector::~TDetector() { }

void TDetector::Clear(Option_t *opt) {
  TNamed::Clear(opt);
  SetBit(kUnbuilt,1);
  fTimestamp = -1;
  fSize = 0;
  fRunStart = 0;
  fRawData.clear();
}

void TDetector::Print(Option_t *opt) const { }

void TDetector::Copy(TObject& obj) const {
  TNamed::Copy(obj);

  TDetector& det = (TDetector&)obj;
  det.fTimestamp = fTimestamp;
  det.fSize = fSize;
  det.fRunStart = fRunStart;
}

int TDetector::Compare(const TObject& obj) const {
  TDetector& det = (TDetector&)obj;
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
  SetBit(kUnbuilt,0);  // if we called build on it, assume it is built whether or not it actually made any hits.  pcb.
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
