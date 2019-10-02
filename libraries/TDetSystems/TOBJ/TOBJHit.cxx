
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
  size_t Size = fTrace.size();

  int offset = 2*FastLen + FastGap - 1;
  for(size_t x = offset; x < Size; x++){
    double fsum0 = 0;
    for( size_t y = (x-offset); y < (x-offset+FastLen) && y < Size; y++){
      fsum0 += fTrace.at(y);
    }
    double fsum1 = 0;
    for(size_t y = (x-offset+FastLen+FastGap); y < (x-offset + 2*FastLen+FastGap) && y < Size; y++){
      fsum1 += fTrace.at(y);
    }
    data[x] = (fsum1 - fsum0) / (double)FastLen;
  }
  for( size_t x = 0; x < (size_t)offset; x++)
    data[x] = data[offset];
  fTTrace.reserve(Size);
  copy(data,data+Size,back_inserter(fTTrace));

  delete [] data;
}

void TOBJHit::EnergyFilter(Double_t tpeak,Double_t tgap,Double_t tau){
  if(fTrace.size() <=0) return;

  size_t Size = fTrace.size();
  Double_t *data = new Double_t[Size];
  int SlowFilterRange = 1;
  double Module_ADCMSPS = 250;
  int SL = (int) std::round(tpeak * (double)(Module_ADCMSPS / 2) / std::pow(2.0, (double)SlowFilterRange));
  int SG = (int) std::round(tgap * (double)(Module_ADCMSPS / 2) / std::pow(2.0, (double)SlowFilterRange));
  int SlowLen = SL * (unsigned int) std::pow(2.0,(double) SlowFilterRange);
  int SlowGap = SG * (unsigned int) std::pow(2.0,(double) SlowFilterRange);

  PeakSample = SL+SG-3;

  if(Size<(size_t)((2*SlowLen+SlowGap)*2)) {
    std::cout<<"The length of recoreded trace is too short!"<<std::endl;
    return;
  }

  double deltaT = 1.0/((double)Module_ADCMSPS);
  double b1 = exp(-1.0 * deltaT / tau);
  double c0 = -(1.0 - b1) * std::pow(b1, (double) SlowLen) * 4.0 / (1.0 - std::pow(b1, (double) SlowLen));
  double c1 = (1.0 - b1) * 4.0;
  double c2 = (1.0 - b1) * 4.0 / (1.0 - std::pow(b1, (double)SlowLen));

  // Comupte baseline
  double baseline = 0;
  double bsum0 = 0;
  double bsum1 = 0;
  double bsum2 = 0;
  for( size_t y = 0 ; y<SlowLen; y++) 
    bsum0+=fTrace.at(y);

  for( size_t y = SlowLen ; y<SlowLen+SlowGap; y++) 
    bsum1+=fTrace.at(y);
   
  for( size_t y = (SlowLen+SlowGap) ; y<(2*SlowLen+SlowGap); y++) 
    bsum2+=fTrace.at(y);

  baseline = c0*bsum0+c1*bsum1+c2*bsum2;
  
  int offset = 2*SlowLen + SlowGap - 1;
  for (size_t x = offset; x<Size; x++){
    double esum0 = 0;
    for( size_t y = (x-offset); y < (x-offset+SlowLen) && y < Size; y++){
        esum0 += fTrace.at(y);
    }
    double esum1 = 0;
    for( size_t y = (x-offset+SlowLen); y < (x-offset+SlowLen+SlowGap) && y < Size; y++){
        esum1 += fTrace.at(y);
    }
    double esum2 = 0;
    for( size_t y = (x-offset+SlowLen+SlowGap); y < (x - offset + 2*SlowLen+SlowGap) && y < Size; y++){
        esum2 += fTrace.at(y);
    }
    data[x] = c0*(double)esum0+c1*(double)esum1+c2*(double)esum2 - baseline;
  }
  for(size_t x = 0; x<(size_t)offset; x++){
    data[x] = data[offset];
  }
  fETrace.reserve(Size);
  copy(data,data+Size,back_inserter(fETrace));

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

void TOBJHit::DrawTTrace(Option_t *option){
  if(fTTrace.size() <= 0) return;
  int len = fTTrace.size();
  if(ttrace == nullptr)
    ttrace = new GH1D("ttrace","",len,0,len);
  else if(ttrace->GetNbinsX()!=len){
    ttrace->Delete();
    ttrace = new GH1D("ttrace","",len,0,len);
  }
  for(int i = 0; i<len; i++){
    ttrace->SetBinContent(i+1,fTTrace.at(i));
  }
  if(!gPad){
    TCanvas *c = new TCanvas();
  }
  gPad->cd();
  ttrace->Draw(option);
}

void TOBJHit::DrawETrace(Option_t *option){
  if(fETrace.size() <= 0) return;
  int len = fETrace.size();
  if(etrace == nullptr)
    etrace = new GH1D("etrace","",len,0,len);
  else if(etrace->GetNbinsX()!=len){
    etrace->Delete();
    etrace = new GH1D("etrace","",len,0,len);
  }
  for(int i = 0; i<len; i++){
    etrace->SetBinContent(i+1,fETrace.at(i));
  }
  if(!gPad){
    TCanvas *c = new TCanvas();
  }
  gPad->cd();
  etrace->Draw(option);
}

void TOBJHit::CalOfflineEnergy(double thres){
  if(fTTrace.size() == 0 || fETrace.size() == 0) {
    fEnergy = -1;
    return;
  }

  size_t x = 0;
  for(x = 0; x<fTTrace.size(); x++)
    if(fTTrace.at(x)>=thres)
      break;
  if(x!=fTTrace.size()-1){
    x = x+PeakSample*(unsigned int)std::pow(2.0,1);//1 slowfilter range
    fEnergy = fETrace.at(x);
  }
  else
    fEnergy = -1;
}
