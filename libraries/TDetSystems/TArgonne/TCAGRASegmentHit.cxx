#include "TCAGRASegmentHit.h"

#include <algorithm>
#include <iostream>

void TCAGRASegmentHit::Copy(TObject& obj) const{
  TDetectorHit::Copy(obj);

  TCAGRASegmentHit& cagra = (TCAGRASegmentHit&)obj;
}

void TCAGRASegmentHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
}

void TCAGRASegmentHit::Print(Option_t *opt) const {
  std::cout << "TCAGRASegmentHit:\n"
            << "\tCharge: " << Charge() << "\n"
            << std::flush;
}

int TCAGRASegmentHit::GetDetnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetArrayPosition();
  } else {
    std::cout << "Unknown address: " << std::hex << fAddress << std::dec
              << std::endl;
    return -1;
  }
}
char TCAGRASegmentHit::GetLeaf() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return *chan->GetArraySubposition();
  } else {
    std::cout << "Unknown address: " << std::hex << fAddress << std::dec
              << std::endl;
    return (char)-1;
  }
}

int TCAGRASegmentHit::GetSegnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetSegment();
  } else {
    return -1;
  }
}

// int TCAGRASegmentHit::GetCrate() const {
//   return (fAddress&0x00ff0000)>>16;
// }

int TCAGRASegmentHit::GetBoardID() const {
  return (fAddress&0x0000ff00)>>8;
}

int TCAGRASegmentHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}

Int_t TCAGRASegmentHit::Charge() const {
  if(fCharge > 30000) {
    return fCharge - 32768;
  } else {
    return fCharge;
  }
}
