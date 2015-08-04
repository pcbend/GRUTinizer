#include "TSegaHit.h"

#include <algorithm>
#include <iostream>

void TSegaHit::Copy(TObject& obj) const{
  TDetectorHit::Copy(obj);

  TSegaHit& sega = (TSegaHit&)obj;
  sega.fChannel = fChannel;
  sega.fSlot = fSlot;
  sega.fCrate = fCrate;
  sega.fTraceLength = fTraceLength;
  memcpy(sega.fTrace, fTrace, fTraceLength * sizeof(unsigned short));
}

void TSegaHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
}

void TSegaHit::Print(Option_t *opt) const {
  std::cout << "TSegaHit:\n"
            << "\tChannel: " << GetChannel() << "\n"
            << "\tSlot: " << GetSlot() << "\n"
            << "\tCrate: " << GetCrate() << "\n"
            << "\tCharge: " << Charge() << "\n"
            << "\tTrace length: " << GetTraceLength() << "\n"
            << std::flush;
}

void TSegaHit::SetTrace(unsigned int trace_length, const unsigned short* trace) {
  if(!trace){
    fTraceLength = 0;
    return;
  }

  fTraceLength = trace_length;
  unsigned int max_length = MAX_TRACE_LENGTH;
  for(unsigned int i=0; i<std::min(trace_length, max_length); i++){
    fTrace[i] = trace[i];
  }
}
