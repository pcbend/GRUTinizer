
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

TTof::TTof() {
  Clear();
}

TTof::~TTof() {
}

void TTof::Copy(TObject &obj) const { 
  TDetectorHit::Copy(obj);
  TTof &tof_ =(TTof&)obj;
  tof_.frf      = frf;
  tof_.fobj     = fobj;
  tof_.fxfp     = fxfp;
  tof_.fsi      = fsi;
  tof_.ftac_obj = ftac_obj; 
  tof_.ftac_xfp = ftac_xfp;
}

void TTof::Clear(Option_t *opt) { 
   frf      = -1;
   fobj     = -1;
   fxfp     = -1;
   fsi      = -1;
   ftac_obj = -1;
   ftac_xfp = -1;
}

void TTof::Print(Option_t *opt) const {  }

/******* End of Ttof.  Beginning of TScintillator **********/

TScintillator::TScintillator() {
  Clear();
}

TScintillator::~TScintillator() {
}

void TScintillator::Copy(TObject &obj) const { 
  TDetectorHit::Copy(obj);
  TScintillator &scinti =(TScintillator&)obj;
  scinti.fID            = fID;
  scinti.fdE_up         = fdE_up;
  scinti.fdE_down       = fdE_down;
  scinti.fTime_up       = fTime_up;
  scinti.fTime_down     = fTime_down;

}

void TScintillator::Clear(Option_t *opt) { 
  fID        = -1;
  fdE_up     = -1;
  fdE_down   = -1;
  fTime_up   = -1;
  fTime_down = -1;
}

void TScintillator::Print(Option_t *opt) const { }


/******* End of TScintillator.  Beginning of TIonChamber **********/

TIonChamber::TIonChamber() {
  Clear();
}

TIonChamber::~TIonChamber() {
}

void TIonChamber::Set(int ch, int data){
  fChan.push_back(ch);
  fData.push_back(data);

}

int TIonChamber::GetData(int ch){
  for(int x = 0; x < fChan.size();x++){
    if(fChan[x]==ch)
      return fData[x];    
  }
  return -1;
}

float TIonChamber::GetdE(){
  for(int x = 0; x< fChan.size();x++){
    fdE+=fData[x];
  }
  fdE = fdE/float(fChan.size());
  return fdE;
}

void TIonChamber::Copy(TObject &obj) const { 
  TDetectorHit::Copy(obj);
  TIonChamber &ic =(TIonChamber&)obj;
  ic.fChan = fChan;
  ic.fData = fData;

}

void TIonChamber::Clear(Option_t *opt) { 
  fChan.clear();
  fData.clear();
}

void TIonChamber::Print(Option_t *opt) const { }

/******* End of TIonChamber.  Beginning of TWhatever **********/


TCrdc::TCrdc() { 
  Clear();
}

TCrdc::~TCrdc() { 
}

int TCrdc::GetWidth() { 
  if(Size()<2)
    return 0;
  return sample.back()-sample.front()+1;
}

void TCrdc::Copy(TObject &obj) const {
  TDetectorHit::Copy(obj);
  TCrdc &c = (TCrdc&)obj;
  c.fId      = fId;
  c.channel  = channel;
  c.sample   = sample;
  c.data     = data;
  c.anode    = anode;
  c.time     = time;
}

void TCrdc::Clear(Option_t *opt) { 
  TDetectorHit::Clear(opt);
  fId   = -1;
  anode = -1;
  time  = -1;
  channel.clear();
  sample.clear();  
  data.clear();    
} 

void TCrdc::Print(Option_t *opt) const { } 









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












