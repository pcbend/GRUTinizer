#include "Globals.h"

#include "TS800Hit.h"
#include "TRandom.h"
#include "GCanvas.h"
#include "TH2.h"
#include "TGraph.h"
#include "TF1.h"
#include "TVirtualFitter.h"

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

float TIonChamber::GetAve(){
  float temp =0.0;
  for(unsigned int x=0;x<fData.size();x++) {
      TChannel *c = TChannel::GetChannel(Address(x));
      if (c){
        temp += c->CalEnergy(fData.at(x));
      } else{
        temp += fData.at(x);
      }
  }
  if(temp > 0) temp = temp/((float)fData.size());
  return temp;
}


float TIonChamber::GetSum() const {
  float temp =0.0;
  for(int x=0;x<Size();x++){
    TChannel *c = TChannel::GetChannel(Address(x));
    if(c){
      temp+=c->CalEnergy(GetData(x));
    } else{
      temp+=GetData(x);
    }
  }
  return temp;
}

float TIonChamber::GetdE(TCrdc *crdc){
  float x   = crdc->GetDispersiveX();
  float y   = crdc->GetNonDispersiveY();

  return GetdE(x,y);
}

float TIonChamber::GetdE(double crdc_1_x, double crdc_1_y){
  float sum = GetAve();

  float xtilt  = GValue::Value("IC_DE_XTILT");
  float ytilt  = GValue::Value("IC_DE_YTILT");
  float x0tilt = GValue::Value("IC_DE_X0TILT");

  if (isnan(xtilt) || isnan(ytilt) || isnan(x0tilt)){
    std::cout << "Define  IC_DE_XTILT, IC_DE_YTILT, and IC_DE_X0TILT before using TIonChamber::GetdE()!\n";
  }
  sum += sum * ytilt * crdc_1_y;
  if (crdc_1_x < x0tilt){
    sum *= TMath::Exp(xtilt*(x0tilt-crdc_1_x));
  }
  return sum;
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

int TCrdc::GetMaxPad() const {
  if(!data.size()) return -1.0;
  int maxp = 0;
  float maxd = 0;
  for(unsigned int i = 0; i < data.size(); i++){
    if(!IsGoodSample(i)) {
      continue;
    }
    TChannel *c = TChannel::GetChannel(Address(i));
    double cal_data;
    if(c){
      cal_data = c->CalEnergy(data.at(i));
    } else{
      cal_data = (double)data.at(i);
    }
    if(cal_data > maxd) {
      maxp = channel.at(i);
      maxd = cal_data;
    }
  }
  return maxp;
}

int TCrdc::GetMaxPadSum() const{
  if(!data.size()) return -1.0;
  float maxd = 0;
  for(unsigned int i = 0; i < data.size(); i++){
    if(!IsGoodSample(i)) {
      continue;
    }
    TChannel *c = TChannel::GetChannel(Address(i));
    double cal_data;
    if(c){
      cal_data = c->CalEnergy(data.at(i));
    } else{
      cal_data = (double)data.at(i);
    }
    if(cal_data > maxd) {
      maxd = cal_data;
    }
  }
  return maxd;
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
  anode = 0;
  time  = 0;
  channel.clear();
  sample.clear();
  data.clear();
  has_cached_dispersive_x = false;
  cached_dispersive_x = std::sqrt(-1);
}


bool TCrdc::IsGoodSample(int i) const {
  if (i == 0 && data.size()>1) {
    return channel.at(i) == channel.at(i+1);
  } else if(i == (int)data.size()-1 && data.size()>2) {
    return channel.at(i) == channel.at(i-1);
  } else if(data.size()>2) {
    return (channel.at(i) == channel.at(i-1) ||
	    (channel.at(i) == channel.at(i+1)));
  }
  return false;
}

float TCrdc::GetDispersiveX() const{
  int maxpad = GetMaxPad();
  if (maxpad ==-1){
    has_cached_dispersive_x = true;
    return sqrt(-1);
  }
  float x_slope = sqrt(-1);
  float x_offset = sqrt(-1);
  if(fId == 0) {
    x_slope = GValue::Value("CRDC1_X_SLOPE");
    x_offset = GValue::Value("CRDC1_X_OFFSET");
  } else {
    x_slope = GValue::Value("CRDC2_X_SLOPE");
    x_offset = GValue::Value("CRDC2_X_OFFSET");
  }
  if(std::isnan(x_slope))
    x_slope = 1.0;
  if(std::isnan(x_offset))
    x_offset = 0.0;

  const int GRAVITY_WIDTH = 14;//determines how many pads one uses in averaging
  const int NUM_PADS = 224;
  int lowpad = maxpad - GRAVITY_WIDTH/2;
  int highpad = lowpad + GRAVITY_WIDTH;
  if (lowpad < 0){
    lowpad = 0;
  }
  if (highpad >= NUM_PADS){
    highpad = NUM_PADS-1;
  }

  double datasum = 0;
  double weighted_sum = 0;
  for(int i=0;i<Size();i++) {
    if((channel.at(i) < lowpad)||(channel.at(i)>highpad) ||
       !IsGoodSample(i)) {
      continue;
    }

    TChannel *c = TChannel::GetChannel(Address(i));
    double cal_data;
    if (c){
      cal_data = c->CalEnergy(GetData(i));
    } else{
      cal_data = (double)GetData(i);
    }
    datasum += cal_data;
    weighted_sum += channel.at(i)*cal_data;
  }

  // + 0.5 so that we take the middle of the pad, not the left edge.
  double mean_chan = weighted_sum/datasum + 0.5;
  double output = (mean_chan*x_slope+x_offset);

  has_cached_dispersive_x = true;
  cached_dispersive_x = output;

  return output;
}


float TCrdc::GetNonDispersiveY() {
  if (GetMaxPad() ==-1){
    return sqrt(-1);
  }
  float y_slope = sqrt(-1);
  float y_offset = sqrt(-1);
  if(fId==0) {
    y_slope = GValue::Value("CRDC1_Y_SLOPE");
    y_offset = GValue::Value("CRDC1_Y_OFFSET");
  } else if(fId==1) {
    y_slope = GValue::Value("CRDC2_Y_SLOPE");
    y_offset = GValue::Value("CRDC2_Y_OFFSET");
  }

  if(std::isnan(y_slope)) y_slope = 1.0;
  if(std::isnan(y_offset)) y_offset = 0.0;

  float tmp = ((float)time)*y_slope+y_offset;
  return tmp;
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

THodoscope::THodoscope() { Clear(); }
THodoscope::~THodoscope() { Clear(); }
THodoscope::THodoscope(const THodoscope &hodoscope) {
  hodoscope.Copy(*this);
}

void THodoscope::Copy(TObject &obj) const {
  TDetectorHit::Copy(obj);
  THodoscope &hodo = (THodoscope&)obj;
  hodo.hodo_hits = hodo_hits;
}

void THodoscope::Clear(Option_t *opt){
  TDetectorHit::Clear(opt);
  hodo_hits.clear();
}

void THodoscope::Print(Option_t *opt) const {
  printf("Number of Hits: %zu\n",Size());
  for(unsigned int i=0;i<Size();i++) {
    printf("\t");
    GetHodoHit(i).Print();
  }
  printf("---------------------------------------\n");
}


void THodoscope::InsertHit(const TDetectorHit& hit){
  hodo_hits.emplace_back((THodoHit&)hit);
}

TDetectorHit& THodoscope::GetHit(int i){
  return hodo_hits.at(i);
}

const THodoHit& THodoscope::GetHodoHit(int i) const {
  return hodo_hits.at(i);
}

THodoHit& THodoscope::GetHodoHit(int i) {
  return hodo_hits.at(i);
}

THodoHit::THodoHit(const THodoHit &hit){
  hit.Copy(*this);
}

void THodoHit::Copy(TObject &obj) const {
  TDetectorHit::Copy(obj);
  THodoHit &hit = (THodoHit&)obj;
  hit.fChannel = fChannel;
}

void THodoHit::Print(Option_t *opt) const {
  printf("Channel: %d Charge: %d\n", GetChannel(), GetCharge());
}
void THodoHit::Clear(Option_t *opt){
  TDetectorHit::Clear(opt);
  fChannel = sqrt(-1);
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
  fCrdc1Anode.clear();
  fCrdc2Anode.clear();
  fRf.clear();
  fHodoscope.clear();
  fRef.clear();

  fCorrelatedXFPE1=sqrt(-1);
  fCorrelatedOBJE1=sqrt(-1);
}

double TMTof::GetCorrelatedObjE1(int channel) const {
  double target = GValue::Value("TARGET_MTOF_OBJE1");
  if (std::isnan(target)){
    std::cout << "TARGET_MTOF_OBJE1 not defined! Use fObj.at(0) if you want first.\n";
    fCorrelatedOBJE1 = sqrt(-1);
    return fCorrelatedOBJE1 = sqrt(-1);
  }

  //shift allows "shifting" of TOF to line up different runs. Necessary when,
  //e.g., the voltage on a scintillator changes during an experiment
  double shift = GValue::Value("SHIFT_MTOF_OBJE1");

  std::vector<unsigned short> refvec = GetMTofVector(channel);
  if(fObj.size() && refvec.size()){
    fCorrelatedOBJE1 = std::numeric_limits<double>::max();
    for(size_t i=0;i<fObj.size();i++) {
      for (size_t j=0; j < refvec.size(); j++){
        double newvalue = fObj.at(i) - refvec.at(j);
        if (!std::isnan(shift)){
          newvalue += shift;
        }
        if(std::abs(target - newvalue) < std::abs(target - fCorrelatedOBJE1)) {
          fCorrelatedOBJE1 = newvalue;
        }
      }
    }
  }
  return fCorrelatedOBJE1;
}

double TMTof::GetCorrelatedXfpE1(int channel) const{
  double target = GValue::Value("TARGET_MTOF_XFPE1");
  if (std::isnan(target)){
    std::cout << "TARGET_MTOF_XFPE1 not defined! Use fXfp.at(0) if you want first.\n";
    fCorrelatedXFPE1 = sqrt(-1);
    return fCorrelatedXFPE1;
  }

  std::vector<unsigned short> refvec = GetMTofVector(channel);
  if(fXfp.size() && refvec.size()){
    fCorrelatedXFPE1 = std::numeric_limits<double>::max();
    for(size_t i=0;i<fXfp.size();i++) {
      for (size_t j=0; j < refvec.size(); j++){
        double newvalue = fXfp.at(i)-refvec.at(j);
        if(std::abs(target - newvalue) < std::abs(target - fCorrelatedXFPE1)) {
          fCorrelatedXFPE1 = newvalue;
        }
      }
    }
  }
  return fCorrelatedXFPE1;
}


double TMTof::GetCorrelatedTof(int ch1, int ch2, double target, double shift) const{
  double fCorr = -1;
  std::vector<unsigned short> refvec1 = GetMTofVector(ch1);
  std::vector<unsigned short> refvec2 = GetMTofVector(ch2);
  if(refvec1.size() && refvec1.size()){
    if(std::isnan(target)) return (refvec1.at(0) - refvec2.at(0));
    fCorr = std::numeric_limits<double>::max();
    for(size_t i = 0; i < refvec1.size(); i++) {
      for (size_t j = 0; j < refvec2.size(); j++){
        double newvalue = refvec1.at(i) - refvec2.at(j);
	if(!std::isnan(shift)) {
	  newvalue += shift;
        }
        if(std::abs(target - newvalue) < std::abs(target - fCorr)) {
          fCorr = newvalue;
        }
      }
    }
  } else return sqrt(-1);
  return fCorr;
}

//Used to selected reference channel for MToF, defaults to E1 up
std::vector<unsigned short> TMTof::GetMTofVector(int channel) const {
  switch(channel) {
    case 0  : //E1 up
      return  fE1Up;
      break;
    case 1  : //E1 down
      return  fE1Down;
      break;
    case 2  : //Xfp
      return  fXfp;
      break;
    case 3  : //Obj
      return  fObj;
      break;
    case 5  : //Rf
      return  fRf;
      break;
    case 6  : //CRDC1 anode
      return  fCrdc1Anode;
      break;
    case 7  : //CRDC2 anode
      return  fCrdc2Anode;
      break;
    case 12 : //Hodoscope
      return  fHodoscope;
      break;
    case 15 : //Refrence
      return  fRef;
      break;
    default : //E1 Up
      return  fE1Up;
      break;
  }

  return fE1Up;
}


double TMTof::GetCorrelatedTof(std::string ch1, std::string ch2, double target, double shift) const{
  double fCorr = -1;
  std::vector<unsigned short> refvec1 = GetMTofVectorFromString(ch1);
  std::vector<unsigned short> refvec2 = GetMTofVectorFromString(ch2);

  if(refvec1.size() && refvec1.size()){
    if(std::isnan(target)) return (refvec1.at(0) - refvec2.at(0));
    fCorr = std::numeric_limits<double>::max();
    for(size_t i = 0; i < refvec1.size(); i++) {
      for (size_t j = 0; j < refvec2.size(); j++){
        double newvalue = refvec1.at(i) - refvec2.at(j);
	if(!std::isnan(shift)) {
	  newvalue += shift;
        }
        if(std::abs(target - newvalue) < std::abs(target - fCorr)) {
          fCorr = newvalue;
        }
      }
    }
  } else return sqrt(-1);
  return fCorr;
}
//Used to selected reference channel for MToF, defaults to E1 up
std::vector<unsigned short> TMTof::GetMTofVectorFromString(std::string vecname) const {
  if(vecname == "E1Up")        return fE1Up;
  else if(vecname == "E1Down") return  fE1Down;
  else if(vecname == "Xfp")    return  fXfp;
  else if(vecname == "Obj")    return  fObj;
  else if(vecname == "Rf")    return  fRf;
  else if(vecname == "Crdc1")  return  fCrdc1Anode;
  else if(vecname == "Crdc2")  return  fCrdc2Anode;
  else if(vecname == "Hodo")   return  fHodoscope;
  else if(vecname == "Ref")    return  fRef;
  else {
    std::cout << "Vector Name Not Recognised using fE1Up" << std::endl;
    std::cout << "Recognised Names: E1Up E1Down Xfp Obj Rf Crdc1 Crdc2 Hodo Ref" << std::endl;
    return  fE1Up;
  }
}

double TMTof::GetCorrelatedObjE1() const {
  double target = GValue::Value("TARGET_MTOF_OBJE1");
  if (std::isnan(target)){
    std::cout << "TARGET_MTOF_OBJE1 not defined! Use fObj.at(0) if you want first.\n";
    fCorrelatedOBJE1 = sqrt(-1);
    return fCorrelatedOBJE1 = sqrt(-1);
  }

  //shift allows "shifting" of TOF to line up different runs. Necessary when,
  //e.g., the voltage on a scintillator changes during an experiment
  double shift = GValue::Value("SHIFT_MTOF_OBJE1");

  if(fObj.size() && fE1Up.size()){
    fCorrelatedOBJE1 = std::numeric_limits<double>::max();
    for(size_t i=0;i<fObj.size();i++) {
      for (size_t j=0; j < fE1Up.size(); j++){
        double newvalue = fObj.at(i) - fE1Up.at(j);
        if (!std::isnan(shift)){
          newvalue += shift;
        }
        if(std::abs(target - newvalue) < std::abs(target - fCorrelatedOBJE1)) {
          fCorrelatedOBJE1 = newvalue;
        }
      }
    }
  }
  return fCorrelatedOBJE1;
}

double TMTof::GetCorrelatedXfpE1() const{
  double target = GValue::Value("TARGET_MTOF_XFPE1");
  if (std::isnan(target)){
    std::cout << "TARGET_MTOF_XFPE1 not defined! Use fXfp.at(0) if you want first.\n";
    fCorrelatedXFPE1 = sqrt(-1);
    return fCorrelatedXFPE1;
  }

  if(fXfp.size() && fE1Up.size()){
    fCorrelatedXFPE1 = std::numeric_limits<double>::max();
    for(size_t i=0;i<fXfp.size();i++) {
      for (size_t j=0; j < fE1Up.size(); j++){
        double newvalue = fXfp.at(i)-fE1Up.at(j);
        if(std::abs(target - newvalue) < std::abs(target - fCorrelatedXFPE1)) {
          fCorrelatedXFPE1 = newvalue;
        }
      }
    }
  }
  return fCorrelatedXFPE1;
}


void TMTof::Print(Option_t *opt) const {    }
