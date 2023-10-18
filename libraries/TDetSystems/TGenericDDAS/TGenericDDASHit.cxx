#include "TGenericDDASHit.h"
#include "GValue.h"

#include <algorithm>
#include <iostream>
#include "TString.h"


TGenericDDASHit::TGenericDDASHit() {
  Clear();
}

void TGenericDDASHit::Copy(TObject& obj) const{
  TDetectorHit::Copy(obj);
//  TGenericDDASHit& ddas = (TGenericDDASHit&)obj;
}

void TGenericDDASHit::Draw(Option_t* opt) {
}

void TGenericDDASHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
}

void TGenericDDASHit::Print(Option_t *opt) const {
  std::cout << "TGenericDDASHit:\n" << "\tChannel: " << GetChannel() << "\n" << "\tCharge: " << Charge() << "\n" << std::flush;
}

void TGenericDDASHit::SetTrace(unsigned int trace_length, const unsigned short* trace) {
  fTrace.clear();
  fTrace.reserve(trace_length);
  copy(trace,trace+trace_length,back_inserter(fTrace));
}

int TGenericDDASHit::GetDetnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  int output = -1;
  if(chan && fAddress!=-1){
    output = chan->GetArrayPosition();
  } else {
    output = -1;
  }
  return output;
}

int TGenericDDASHit::GetCrate() const {
  return (fAddress&0x00ff0000)>>16;
}

int TGenericDDASHit::GetSlot() const {
  return (fAddress&0x0000ff00)>>8;
}

int TGenericDDASHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}

Int_t TGenericDDASHit::Charge() const {
  if(fCharge > 30000) {
    return fCharge - 32768;
  } else {
    return fCharge;
  }
}
