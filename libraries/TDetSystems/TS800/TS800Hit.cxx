
#include "TS800Hit.h"
#include "TRandom.h"
#include "GCanvas.h"
#include "TH2.h"

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

float TIonChamber::GetdE(){
  float temp =0.0;
  //if(fdE==-1.0) {
    for(int x=0;x<fData.size();x++) {   //std::vector<int>::iterator it=fData.begin();it!=fData.end();it++) {
      //if(fdE==-1.0)
      //  fdE=0.0;
      temp+=fData.at(x); //it->first;
    //}
    //if(fdE!=-1.0)
     // fdE = fdE/((float)fData.size());
  }
  if(temp>0)
    temp = temp/((float)fData.size());
  return temp;                   
}

void TIonChamber::Copy(TObject &obj) const { 
  TDetectorHit::Copy(obj);
  TIonChamber &ic =(TIonChamber&)obj;
  ic.fChan = fChan;
  ic.fData = fData;

}

void TIonChamber::Clear(Option_t *opt) { 
  TDetectorHit::Clear(opt);
  //fdE = -1;
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

float TCrdc::GetPad(){
  int temp = 0;
  int place =0;
  float WeightedSumNum = 0;
  float WeightedSumDen = 0;
  if(!data.size())
    return -1.0;
  
  temp = data.at(0);
  for(int i = 0; i < data.size(); i++){
    /* std::cout << " ------ " << std::endl;
    std::cout << " Data    : " << data.at(i) << " at " << i << std::endl;
    std::cout << " Sample  : " << sample.at(i) << " at " << i << std::endl;
    std::cout << " Channel : " << channel.at(i) << " at " << i << std::endl;
    */
    
    /*if(data.at(i)>0){
      WeightedSumNum += float(data.at(i))*float(channel.at(i));
      WeightedSumDen += float(channel.at(i));
      
    }*/

    if(data.at(i)>temp) {
    temp = data.at(i);
    place = i;
    }
  }
  
  //for(int j = 0; j < 6; j++){
    
  //}
  
  //return float(WeightedSumNum/WeightedSumDen)+gRandom->Uniform();
  return (float)(channel.at(place))+gRandom->Uniform();
  
}

void TCrdc::DrawChannels(Option_t *opt) const {
  GCanvas *c = 0;
  if(!gPad)
    c = new GCanvas();
  else {
    //gPad->Clear();
    c = (GCanvas*)gPad->GetCanvas();
    c->Clear();
  }
  
  std::vector<TH1I> hits;
  512;
  int currentchannel = -1;
  TH1I *currenthist = 0;
  for(int x=0;x<Size();x++) {
    if(channel.at(x)!=currentchannel) {
      
      TH1I hist(Form("channel_%02i",channel.at(x)),Form("channel_%02i",channel.at(x)),512,0,512);
      hits.push_back(hist);
      currentchannel = channel.at(x);
      currenthist = &(hits.back());
    }
    currenthist->Fill(sample.at(x),data.at(x));
  }
  c->Divide(1,hits.size());
  for(int x=0;x<hits.size();x++) {
    c->cd(x+1);
    hits.at(x).DrawCopy();
  }
  return;
}

void TCrdc::DrawHit(Option_t *opt) const {
  GCanvas *c = 0;
  if(!gPad)
    c = new GCanvas();
  else {
    //gPad->Clear();
    c = (GCanvas*)gPad->GetCanvas();
    c->Clear();
  } 
  TH1I *mat = new TH1I("hit_pattern","hit_pattern",256,0,256);
  int currentchannel = -1;
  int currentx = -1;
  int currenty = -1;
  int datasum =0;
  for(int x=0;x<Size();x++) {
    if(channel.at(x)!=currentchannel) {
       if(currentchannel!=-1)
         mat->Fill(currentx,datasum);
       datasum = 0;
       currentchannel = channel.at(x);
       currentx = channel.at(x);
       currenty = channel.at(x)%16;
    } 
    datasum+=data.at(x);
    
  }
  if(currentchannel!=-1)
    mat->Fill(currentx,datasum);

  std::string options = "";
  options.append(opt);
  mat->Draw(options.c_str());

  return;
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
  c.fCRDCXslope = fCRDCXslope;
  c.fCRDCYslope = fCRDCYslope;
  c.fCRDCXoff = fCRDCXoff;
  c.fCRDCYoff = fCRDCYoff;

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



TMTof::TMTof() { Clear(); }

TMTof::~TMTof() { }

TMTof::TMTof(const TMTof &mtof) {
  mtof.Copy(*this);
}


void TMTof::Copy(TObject &obj) const {
  TDetectorHit::Copy(obj);
  
  TMTof &mtof = ((TMTof&)obj);

  mtof.fE1Up      = fE1Up;     
  mtof.fE1Down    = fE1Down;
  mtof.fXfp       = fXfp;
  mtof.fObj       = fObj;
  //mtof.fGalotte   = fGalotte;
  mtof.fCrdc1Anode = fCrdc1Anode;
  mtof.fCrdc2Anode = fCrdc2Anode;
  mtof.fRf        = fRf;
  mtof.fHodoscope = fHodoscope;
  mtof.fRef       = fRef;
}

void TMTof::Clear(Option_t *opt) {

  fE1Up.clear();     
  fE1Down.clear();   
  fXfp.clear();       
  fObj.clear();      
  //fGalotte.clear();
  fCrdc1Anode.clear();
  fCrdc2Anode.clear();
  fRf.clear();       
  fHodoscope.clear();
  fRef.clear();
}

void TMTof::Print(Option_t *opt) const {    }









