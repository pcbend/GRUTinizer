#include "TSegaSegmentHit.h"

#include <algorithm>
#include <iostream>

void TSegaSegmentHit::Copy(TObject& obj) const{
  TDetectorHit::Copy(obj);

  TSegaSegmentHit& sega = (TSegaSegmentHit&)obj;
  sega.fTimestamp = fTimestamp;
  sega.fCharge   = fCharge;
  sega.fCfd = fCfd;
  sega.fTrace = fTrace;
}

void TSegaSegmentHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fTimestamp = -1;
  fCharge  = -1;
  fCfd = -1;
  fTrace.clear();
}

void TSegaSegmentHit::Print(Option_t *opt) const {
  std::cout << "TSegaSegmentHit:\n"
            << "\tCharge: " << Charge() << "\n"
            << std::flush;
}

void TSegaSegmentHit::SetTrace(unsigned int trace_length, const unsigned short* trace) {
  if(!trace){
    fTrace.clear();
    return;
  }

  fTrace.clear();
  fTrace.reserve(trace_length);
  for(unsigned int i=0; i<trace_length; i++){
    fTrace.push_back(trace[i]);
  }
}

int TSegaSegmentHit::GetDetnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetArrayPosition();
  } else {
    return -1;
  }
}

int TSegaSegmentHit::GetSegnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetSegment();
  } else {
    return -1;
  }
}

int TSegaSegmentHit::GetCrate() const {
  return (fAddress&0x00ff0000)>>16;
}

int TSegaSegmentHit::GetSlot() const {
  return (fAddress&0x0000ff00)>>8;
}

int TSegaSegmentHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}
