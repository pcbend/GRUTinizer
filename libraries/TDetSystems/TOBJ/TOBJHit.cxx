
#include <TOBJHit.h>
#include <TCanvas.h>

ClassImp(TOBJHit)


void TOBJHit::SetTrace(unsigned int trace_length, const unsigned short* trace) {
  fTrace.reserve(trace_length);
  copy(trace,trace+trace_length,back_inserter(fTrace));
  //printf("%s called\t%i\n",__PRETTY_FUNCTION__,Size()); fflush(stdout);
}


// modified according to XIA pixie-16 algorithm
void TOBJHit::TrigFilter(Double_t tpeak,Double_t tgap){
  if(fTrace.size()<=0) return;
  Double_t *data = new Double_t[fTrace.size()];
  Double_t FastFilterRange = 0;
  int Module_ADCMSPS = 250; // 250MHz ADC
  Double_t FL = (unsigned int) std::round(tpeak * (double) (Module_ADCMSPS/2)/std::pow(2.0,FastFilterRange));
  Double_t FG = (unsigned int) std::round(tgap * (double) (Module_ADCMSPS/2)/std::pow(2.0,FastFilterRange));
  int FastLen = FL * (unsigned int)std::pow(2.0,(double)FastFilterRange);
  int FastGap = FG * (unsigned int)std::pow(2.0,(double)FastFilterRange);

  int offset = 2*FastLen + FastGap - 1;
  for(size_t x = offset; x < fTrace.size(); x++){
    double fsum0 = 0;
    for( size_t y = (x-offset); y < (x-offset+FastLen); y++){
      fsum0 += fTrace.at(y);
    }
    double fsum1 = 0;
    for(size_t y = (x-offset+FastLen+FastGap); y < (x-offset + 2*FastLen+FastGap); y++){
      fsum1 += fTrace.at(y);
    }
    data[x] = (fsum1 - fsum0) / (double)FastLen;
  }
  for( size_t x = 0; x < (size_t)offset; x++)
    data[x] = data[offset];
  fTTrace.reserve(fTrace.size());
  copy(data,data+fTrace.size(),back_inserter(fTTrace));

  delete [] data;
}

void TOBJHit::EnergyFilter(Double_t tpeak,Double_t tgap,Double_t tau){
  if(fTrace.size() <=0) return;

  Double_t *data = new Double_t[fTrace.size()];
  int SlowFilterRange = 1;
  double Module_ADCMSPS = 250;
  int SL = (int) std::round(tpeak * (double)(Module_ADCMSPS / 2) / std::pow(2.0, (double)SlowFilterRange));
  int SG = (int) std::round(tgap * (double)(Module_ADCMSPS / 2) / std::pow(2.0, (double)SlowFilterRange));
  int SlowLen = SL * (unsigned int) std::pow(2.0,(double) SlowFilterRange);
  int SlowGap = SG * (unsigned int) std::pow(2.0,(double) SlowFilterRange);

  double deltaT = 1.0/((double)Module_ADCMSPS);
  double b1 = exp(-1.0 * deltaT / tau);
  double c0 = -(1.0 - b1) * std::pow(b1, (double) SlowLen) * 4.0 / (1.0 - std::pow(b1, (double) SlowLen));
  double c1 = (1.0 - b1) * 4.0;
  double c2 = (1.0 - b1) * 4.0 / (1.0 - std::pow(b1, (double)SlowLen));

  int offset = 2*SlowLen + SlowGap - 1;
  for (size_t x = 0; x<fTrace.size(); x++){
    double esum0 = 0;
    for( size_t y = (x-offset); y < (x-offset+SlowLen); y++){
      if(y > 0)
        esum0 += fTrace.at(y);
    }
    double esum1 = 0;
    for( size_t y = (x-offset+SlowLen); y < (x-offset+SlowLen+SlowGap); y++){
      if(y>0) 
        esum1 += fTrace.at(y);
    }
    double esum2 = 0;
    for( size_t y = (x-offset+SlowLen+SlowGap); y < (x - offset + 2*SlowLen+SlowGap); y++){
      if(y>0)
        esum2 += fTrace.at(y);
    }
    data[x] = c0*(double)esum0+c1*(double)esum1+c2*(double)esum2;
  }
  fETrace.reserve(fTrace.size());
  copy(data,data+fTrace.size(),back_inserter(fETrace));

  delete [] data;
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


