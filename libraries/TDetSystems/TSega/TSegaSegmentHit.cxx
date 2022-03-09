#include "TSegaSegmentHit.h"
#include "TSega.h"

#include <algorithm>
#include <iostream>

void TSegaSegmentHit::Copy(TObject& obj) const{
  TDetectorHit::Copy(obj);

  TSegaSegmentHit& sega = (TSegaSegmentHit&)obj;
  sega.fTrace = fTrace;
}

void TSegaSegmentHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
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
  return;
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
    std::cout << "Unknown address: " << std::hex << fAddress << std::dec
              << std::endl;
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

//Mapped Numbers
int TSegaSegmentHit::GetMapnum() const {return TSega::MappedSegnum(GetDetnum(),GetSegnum());}
int TSegaSegmentHit::GetPairnum() const {return TSega::MappedPairnum(GetDetnum(),GetSegnum());}
int TSegaSegmentHit::GetSlicenum() const {return TSega::MappedSlicenum(GetDetnum(),GetSegnum());}

int TSegaSegmentHit::GetCrate() const {
  return (fAddress&0x00ff0000)>>16;
}

int TSegaSegmentHit::GetSlot() const {
  return (fAddress&0x0000ff00)>>8;
}

int TSegaSegmentHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}

Int_t TSegaSegmentHit::Charge() const {
  if(fCharge > 30000) {
    return fCharge - 32768;
  } else {
    return fCharge;
  }
}
