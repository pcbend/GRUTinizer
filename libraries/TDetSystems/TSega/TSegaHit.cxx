#include "TSegaHit.h"

#include <algorithm>
#include <iostream>

TSegaHit::TSegaHit() {
  Clear();
}

void TSegaHit::Copy(TObject& obj) const{
  TDetectorHit::Copy(obj);

  TSegaHit& sega = (TSegaHit&)obj;
  sega.fTimestamp = fTimestamp;
  sega.fCharge   = fCharge;
  sega.fCfd = fCfd;
  sega.fTrace = fTrace;
}

void TSegaHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fTimestamp = -1;
  fCharge  = -1;
  fCfd = -1;
  fTrace.clear();
}

void TSegaHit::Print(Option_t *opt) const {
  std::cout << "TSegaHit:\n"
            << "\tChannel: " << GetChannel() << "\n"
            << "\tCharge: " << Charge() << "\n"
            << std::flush;
}

void TSegaHit::SetTrace(unsigned int trace_length, const unsigned short* trace) {
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

int TSegaHit::GetDetnum() {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetArrayPosition();
  } else if(fSegments.size()) {
    return fSegments[0].GetDetnum();
  } else {
    return -1;
  }
}

int TSegaHit::GetCrate() const {
  return (fAddress&0x00ff0000)>>16;
}

int TSegaHit::GetSlot() const {
  return (fAddress&0x0000ff00)>>8;
}

int TSegaHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}

TSegaSegmentHit& TSegaHit::MakeSegmentByAddress(unsigned int address){
  // for(auto& segment : fSegments){
  //   if(segment.Address() == address){
  //     return segment;
  //   }
  // }

  fSegments.emplace_back();
  TSegaSegmentHit& output = fSegments.back();
  output.SetAddress(address);
  return output;
}
