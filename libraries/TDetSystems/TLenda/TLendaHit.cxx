
#include <TLendaHit.h>

ClassImp(TLendaHit)


void TLendaHit::SetTrace(unsigned int trace_length, const unsigned short* trace) {
  fTrace.reserve(trace_length);
  copy(trace,trace+trace_length,back_inserter(fTrace));
  //printf("%s called\t%i\n",__PRETTY_FUNCTION__,Size()); fflush(stdout);
}


void TLendaHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fTrace.clear(); 
}




