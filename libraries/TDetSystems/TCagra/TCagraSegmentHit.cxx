#include "TCagraSegmentHit.h"

#include <algorithm>
#include <iostream>

void TCagraSegmentHit::Copy(TObject& obj) const{
  TDetectorHit::Copy(obj);

  TCagraSegmentHit& cagra = (TCagraSegmentHit&)obj;
  cagra.fTrace = fTrace;
}

void TCagraSegmentHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fTrace.clear();
}

void TCagraSegmentHit::SetTrace(std::vector<Short_t>& trace) {
  fTrace.clear();
  fTrace.swap(trace);
}

void TCagraSegmentHit::Print(Option_t *opt) const {
  std::cout << "TCagraSegmentHit:\n"
            << "\tCharge: " << Charge() << "\n"
            << std::flush;
}

int TCagraSegmentHit::GetDetnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetArrayPosition();
  } else {
    std::cout << "Unknown address: " << std::hex << fAddress << std::dec
              << std::endl;
    return -1;
  }
}
char TCagraSegmentHit::GetLeaf() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return *chan->GetArraySubposition();
  } else {
    std::cout << "Unknown address: " << std::hex << fAddress << std::dec
              << std::endl;
    return (char)-1;
  }
}

int TCagraSegmentHit::GetSegnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetSegment();
  } else {
    return -1;
  }
}

// int TCagraSegmentHit::GetCrate() const {
//   return (fAddress&0x00ff0000)>>16;
// }

int TCagraSegmentHit::GetBoardID() const {
  return (fAddress&0x0000ff00)>>8;
}

int TCagraSegmentHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}

Int_t TCagraSegmentHit::Charge() const {
  if(fCharge > 30000) {
    return fCharge - 32768;
  } else {
    return fCharge;
  }
}
