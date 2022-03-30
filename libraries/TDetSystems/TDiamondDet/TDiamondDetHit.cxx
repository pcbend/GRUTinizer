#include "TDiamondDetHit.h"

#include <algorithm>
#include <iostream>

#include "TString.h"

#include "GCanvas.h"
#include "GValue.h"
#include "TDiamond.h"

#include <GH1D.h>

TDiamondDetHit::TDiamondDetHit() {
  Clear();
}

void TDiamondDetHit::Copy(TObject& obj) const{
  TDetectorHit::Copy(obj);
  TDiamondDetHit& diamond = (TDiamondDetHit&)obj;
}

void TDiamondDetHit::Draw(Option_t* opt) {
}

void TDiamondDetHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
}

void TDiamondDetHit::Print(Option_t *opt) const {
  std::cout << "TDiamondDetHit:\n" << "\tChannel: " << GetChannel() << "\n" << "\tCharge: " << Charge() << "\n" << std::flush;
}

int TDiamondDetHit::GetDetnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  int output = -1;
  if(chan && fAddress!=-1){
    output = chan->GetArrayPosition();
  } else {
    // std::cout << "Unknown address: " << std::hex << fAddress << std::dec << std::endl;
    output = -1;
  }

  if(output == -1 && chan){
    // std::cout << "Chan with det=-1: " << chan->GetName() << std::endl;
    // std::cout << "address: " << fAddress << std::endl;
  }
  return output;
}

int TDiamondDetHit::GetCrate() const {
  return (fAddress&0x00ff0000)>>16;
}

int TDiamondDetHit::GetSlot() const {
  return (fAddress&0x0000ff00)>>8;
}

int TDiamondDetHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}

Int_t TDiamondDetHit::Charge() const {
  if(fCharge > 30000) {
    return fCharge - 32768;
  } else {
    return fCharge;
  }
}
