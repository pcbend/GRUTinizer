#include <TLendaHit.h>

ClassImp(TLendaHit)


void TLendaHit::SetTrace(unsigned int trace_length, const unsigned short* trace) {
  fTrace.clear();
  fTrace.reserve(trace_length);
  copy(trace,trace+trace_length,back_inserter(fTrace));
}

void TLendaHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fTrace.clear();
}

int TLendaHit::GetCrate() const {
  return (fAddress&0x00ff0000)>>16;
}

int TLendaHit::GetSlot() const {
  return (fAddress&0x0000ff00)>>8;
}

int TLendaHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}
