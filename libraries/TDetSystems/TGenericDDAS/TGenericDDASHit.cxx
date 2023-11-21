#include "TGenericDDASHit.h"
#include "GValue.h"

#include <algorithm>
#include <iostream>
#include "TString.h"


TGenericDDASHit::TGenericDDASHit() {
  Clear();
}

/*******************************************************************************/
/* Copies hit ******************************************************************/
/*******************************************************************************/
void TGenericDDASHit::Copy(TObject& obj) const{
  TDetectorHit::Copy(obj);
}

/*******************************************************************************/
/* Clear hit *******************************************************************/
/*******************************************************************************/
void TGenericDDASHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
}

/*******************************************************************************/
/* Basic Print Function ********************************************************/
/*******************************************************************************/
void TGenericDDASHit::Print(Option_t *opt) const {
  std::cout << "TGenericDDASHit:\n" << "\tChannel: " << GetChannel() << "\n" << "\tCharge: " << Charge() << "\n" << std::flush;
}

/*******************************************************************************/
/* Sets trace information if present in data ***********************************/
/*******************************************************************************/
void TGenericDDASHit::SetTrace(unsigned int trace_length, const unsigned short* trace) {
  fTrace.clear();
  fTrace.reserve(trace_length);
  copy(trace,trace+trace_length,back_inserter(fTrace));
}

/*******************************************************************************/
/* Returns detector number based on channels.cal file definition ***************/
/*******************************************************************************/
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

/*******************************************************************************/
/* Returns DDAS crate/slot channel number **************************************/
/*******************************************************************************/
int TGenericDDASHit::GetCrate() const {
  return (fAddress&0x00ff0000)>>16;
}

int TGenericDDASHit::GetSlot() const {
  return (fAddress&0x0000ff00)>>8;
}

int TGenericDDASHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}

/*******************************************************************************/
/* Uncalibrated DDAS energies **************************************************/
/*******************************************************************************/
Int_t TGenericDDASHit::Charge() const {
  if(fCharge > 30000) {
    return fCharge - 32768;
  } else {
    return fCharge;
  }
}
