
#include <TOBJHit.h>
#include <TCanvas.h>

ClassImp(TOBJHit)


void TOBJHit::SetTrace(unsigned int trace_length, const unsigned short* trace) {
  fTrace.reserve(trace_length);
  copy(trace,trace+trace_length,back_inserter(fTrace));
  //printf("%s called\t%i\n",__PRETTY_FUNCTION__,Size()); fflush(stdout);
}

void TOBJHit::EnergyFilter(Double_t tpeak,Double_t tgap,Double_t tau){
  if(fTrace.size() <=0) return;
  
}

void TOBJHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fTrace.clear();
  fExternalTimestamp = -1;
}

void TOBJHit::DrawTrace(Option_t *option){
  if(fTrace.size() <= 0) return;
  int len = fTrace.size();
  if(htrace == nullptr)
    htrace = new GH1D("trace","",len,0,len);
  else if(htrace->GetNbinsX()!=len){
    htrace->Delete();
    htrace = new GH1D("trace","",len,0,len);
  }
  for(int i = 0; i<len; i++){
    htrace->SetBinContent(i+1,fTrace.at(i));
  }
  if(!gPad){
    TCanvas *c = new TCanvas();
  }
  gPad->cd();
  htrace->Draw(option);
}


