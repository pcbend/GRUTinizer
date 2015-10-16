
#include "TS800Hit.h"


TTrigger::TTrigger() {
  Clear();
}

TTrigger::~TTrigger() {
}

void TTrigger::Copy(TObject &obj) const { 
  TDetectorHit::Copy(obj);
  TTrigger &trig =(TTrigger&)obj;
  trig.fregistr  = fregistr;
  trig.fs800source      = fs800source;     
  trig.fexternalsource1 = fexternalsource1;
  trig.fexternalsource2 = fexternalsource2;
  trig.fsecondarysource = fsecondarysource;
}

void TTrigger::Clear(Option_t *opt) { 
  fregistr         = -1;
  fs800source      = -1;     
  fexternalsource1 = -1;
  fexternalsource2 = -1;
  fsecondarysource = -1;
}

void TTrigger::Print(Option_t *opt) const {  }









TCrdcPad::TCrdcPad()  { Clear(); }

TCrdcPad::TCrdcPad(int chan)  { 
  fChannel = chan;  
  fNumSamples =0;
  Clear(); 
  for(int x=0;x<MAXCRDC;x++)  {
    fSample[x] = 0;
    fValue[x]  = 0;
  }
}

TCrdcPad::~TCrdcPad() {  }

int TCrdcPad::Charge() const {
  int temp = 0;
  //for(std::map<int,int>::iterator it=fTrace.begin();it!=fTrace.end();it++) {
  for(int x=0;x<fNumSamples;x++)
    temp += fValue[x];
  return temp;
}

void TCrdcPad::Clear(Option_t *opt) {

  fChannel = -1;
  for(int x=0;x<fNumSamples;x++) {
    fSample[x] = 0;
    fValue[x]  = 0;
  }
  fNumSamples =0;
}  

void TCrdcPad::Print(Option_t *opt) const { 
  printf("I print.\n");
}

void TCrdcPad::Copy(TObject &obj) const {
  TDetectorHit::Copy(obj);

  ((TCrdcPad&)obj).fChannel   = fChannel;
  ((TCrdcPad&)obj).fNumSamples = fNumSamples;
  for(int x=0;x<MAXCRDC;x++) {
    ((TCrdcPad&)obj).fSample[x] = fSample[x];
    ((TCrdcPad&)obj).fValue[x]  = fValue[x];
  }

}












