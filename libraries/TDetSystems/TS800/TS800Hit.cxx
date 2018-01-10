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
  //if(fdE==-1.0) {
  
  for(unsigned int x=0;x<fData.size();x++) {   
      TChannel *c = TChannel::GetChannel(Address(x));
      if (c){
        temp += c->CalEnergy(fData.at(x));
      }
      else{
        temp += fData.at(x);
      }
  }
  if(temp>0)
    temp = temp/((float)fData.size());
  return temp;
}


float TIonChamber::GetSum() const {
  float temp =0.0;
  //if(fdE==-1.0) {
  /*
  for(unsigned int x=0;x<fData.size();x++) {   
      TChannel *c = TChannel::GetChannel(Address(x));
      if (c){
        temp += c->CalEnergy(fData.at(x));
      }
      else{
        temp += fData.at(x);
      }
  }
  if(temp>0)
  temp = temp/((float)fData.size());*/

  for(int x=0;x<Size();x++){
    TChannel *c = TChannel::GetChannel(Address(x));
    if(c){
      //printf("I AM HERE!!!\n"); fflush(stdout);
      temp+=c->CalEnergy(GetData(x));
    }else{
      temp+=GetData(x);
    }
  }
  return temp;
}

//FROM GRROOT
//bool Calibration::BuildIonChamber(GIonChamber* in, TRACK* track, IC* out){
//  out->Clear();
//  out->SetCal(ICCal(in));
//  double sum = ICSum(out->GetCal());
//  double IC_corg = 1;
//  double IC_coro = 0;
//  if(fIC_cor[0]!=NULL)
//    IC_corg = fIC_cor[0]->GetBinContent(fevent/10000+1);
//  if(fIC_cor[1]!=NULL)
//    IC_coro = fIC_cor[1]->GetBinContent(fevent/10000+1);
//  sum *= IC_corg;
//  sum += IC_coro;
//  out->SetSum(sum);
//  out->SetDE(ICDE(sum,track));
//  if(!isnan(sum) && sum>fSett->ICThresh()){
//    fichctr++;
//    return true;
//  }
//  return false;
//}
//Float_t Calibration::ICDE(Float_t sum, TRACK* track){
//  Float_t x = track->GetXFP();
//  Float_t y = track->GetYFP();
//  if(!isnan(sum) && !isnan(track->GetAFP())){
//    if(!isnan(y))
//      sum += sum*fSett->dE_ytilt()*y;
//    if(!isnan(x) && x < fSett->dE_x0tilt())
//      sum *= exp(fSett->dE_xtilt()* (fSett->dE_x0tilt() -x) );
//    fs800valid = 0;
//    return sum * fde_slope + fde_offset;
//  } else {
//    return sqrt(-1.0);
//  }
//}

//We already have ICSum, it's our current getdE() unction. We need to somehow get the 
//track from the crdc into this function, and figure out what IC_corg is.

//TODO: We need to change this function to correct the sum for each event
//      based on the track through the CRDCs

float TIonChamber::GetdE(){
  //std::cout << "GetdE() NOT IMPLEMENTED! Just returning GetSum()" << std::endl;
  return GetSum();
}

//Calculate energy loss in Ion Chamber corrected
//for particle track
float TIonChamber::GetdECorr(TCrdc *crdc){
  float sum = GetdE();
  float x   = crdc->GetDispersiveX();
  float y   = crdc->GetNonDispersiveY();

  float xtilt  = GValue::Value("IC_DE_XTILT");
  float ytilt  = GValue::Value("IC_DE_YTILT");
  float x0tilt = GValue::Value("IC_DE_X0TILT");

  /*  std::cout << "---------------------" << std::endl;
  std::cout << " xtilt = " << xtilt << std::endl;
  std::cout << " ytilt = " << ytilt << std::endl;
  std::cout << " x0tilt = " << x0tilt << std::endl;
  std::cout << BLUE << " SUM = " << sum << RESET_COLOR << std::endl;
  */
  sum += sum * ytilt * y;
  //  std::cout << GREEN << " SUM2 = " << sum << RESET_COLOR << std::endl;
  sum *= TMath::Exp(xtilt*(x0tilt-x));
  //std::cout << RED << " SUM3 = " << sum << RESET_COLOR << std::endl;
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

TF1 *TCrdc::fgaus = new TF1("fgaus","gaus");


int TCrdc::GetMaxPad() const {
//  double temp = 0;
//  int    place =0;
  if(!data.size())
    return -1.0;

  //std::cout << " Data has size? = " << data.size() << std::endl;
 std::map<int,double> sum; 

//  temp = data.at(0);
  for(unsigned int i = 0; i < data.size(); i++){
    /*
    std::cout << " ------ " << std::endl;
    std::cout << " Data    : " << data.at(i) << " at " << i << std::endl;
    std::cout << " Sample  : " << sample.at(i) << " at " << i << std::endl;
    std::cout << " Channel : " << channel.at(i) << " at " << i << std::endl;
    */
    

    /*if(data.at(i)>0){
      WeightedSumNum += float(data.at(i))*float(channel.at(i));
      WeightedSumDen += float(channel.at(i));

    }*/

    if(!IsGoodSample(i)) {
      continue;
    }

    TChannel *c = TChannel::GetChannel(Address(i));
    double cal_data;
    if(c){
      cal_data = c->CalEnergy(data.at(i));
    }
    else{
      cal_data = (double)data.at(i);
    }
    sum[channel.at(i)] += cal_data;
  }

  std::map<int,double>::iterator  it;
  int max = -1;
  double maxd =-1;
  for(it = sum.begin();it!=sum.end();it++) {
    if(it->second > maxd){
      max = it->first;
      maxd = it->second;
    }
  }
  //return (float)(channel.at(place))+gRandom->Uniform();
  return max;
}

int TCrdc::GetMaxPadSum() const{
  if(!data.size())
    return -1.0;

 std::map<int,double> sum; 

  for(unsigned int i = 0; i < data.size(); i++){
    bool good = false;
    if (i == 0 && data.size()>1) {
      if(channel.at(i) == channel.at(i+1))
        good = true;
    }
    else if(i == data.size()-1 && data.size()>2) {
      if(channel.at(i) == channel.at(i-1))
        good = true;
    } else if(data.size()>2) {
      if((channel.at(i) == channel.at(i-1)) ||
         (channel.at(i) == channel.at(i+1)))
        good = true;
    }

    if(!good){
      continue;
    }

    TChannel *c = TChannel::GetChannel(Address(i));
    double cal_data;
    if(c){
        cal_data = c->CalEnergy(data.at(i));
    }
    else{
      cal_data = (double)data.at(i);
    }

    sum[channel.at(i)] += cal_data;
  }

  std::map<int,double>::iterator  it;
  double maxd =-1.0;
  for(it = sum.begin();it!=sum.end();it++) {
    if(it->second > maxd){
      maxd = it->second;
    }
  }
  //return (float)(channel.at(place))+gRandom->Uniform();
  return maxd; 
}

void TCrdc::DrawChannels(Option_t *opt,bool calibrate) const {
  if(!gPad)
    new GCanvas();
  else {
    //gPad->Clear();
    //GCanvas* c = (GCanvas*)gPad->GetCanvas();
    //c->Clear();
  }

  std::vector<TH1I> hits;
  //int currentchannel = -1;
  //TH2I *currenthist = 0;
  TH2I hist(Form("crdc_%i",fId),Form("crdc_%i",fId),224,0,224,128,0,128);
  for(int x=0;x<this->Size();x++) {
    //if(channel.at(x)!=currentchannel) {

      //TH1I hist(Form("channel_%02i",channel.at(x)),Form("channel_%02i",channel.at(x)),512,0,512);
      //hits.push_back(hist);
      //currentchannel = channel.at(x);
      //currenthist = &(hits.back());
      double cal_data;
      //printf("channel.size() = %i \t ",channel.size());
      //printf("address = %08x \t ",Address(x));
      //printf("channel = %i \t ",channel.at(x));
      //printf("x = %i  ",x);
      TChannel *c = TChannel::GetChannel(Address(x));
      if(c && calibrate) {
        cal_data = c->CalEnergy(data.at(x));
//        printf("cal_data[%03i]  = %f\n",channel.at(x),cal_data);
      } else {
        if(!c)
          printf("failed to find TChannel for 0x%08x\n",Address(x));
        cal_data = data.at(x);
        //printf("cal_data[%03i]  = %f\n",channel.at(x),cal_data);
      }
      hist.Fill(channel.at(x),sample.at(x),cal_data);
    //}
    //hist.Fill(sample.at(x),data.at(x));
  }
  //c->Divide(1,hits.size());
  //for(int x=0;x<hits.size();x++) {
  //  c->cd(x+1);
  //  hits.at(x).DrawCopy();
  //}
  //printf("GetMaxPad()     = %i\n",GetMaxPad());
  //printf("GetDipersiveX() = %.02f\n",GetDispersiveX());
  hist.DrawCopy("colz");
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
  //int currenty = -1;
  int datasum =0;
  for(int x=0;x<Size();x++) {
    if(channel.at(x)!=currentchannel) {
       if(currentchannel!=-1)
         mat->Fill(currentx,datasum);
       datasum = 0;
       currentchannel = channel.at(x);
       currentx = channel.at(x);
       //currenty = channel.at(x)%16;
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
//c.fCRDCXslope = fCRDCXslope;
//c.fCRDCYslope = fCRDCYslope;
//c.fCRDCXoff = fCRDCXoff;
//c.fCRDCYoff = fCRDCYoff;
}

void TCrdc::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fId   = -1;
  anode = 0;
  time  = 0;
  channel.clear();
  sample.clear();
  data.clear();
}

/*
float TCrdc::GetDispersiveX() const{
  if (GetMaxPad() ==-1){
    return sqrt(-1);
  }

  float x_slope = sqrt(-1);
  float x_offset = sqrt(-1);
  if(fId==0) {
    x_slope = GValue::Value("CRDC1_X_SLOPE");
    x_offset = GValue::Value("CRDC1_X_OFFSET");
  } else if(fId==1) {
    x_slope = GValue::Value("CRDC2_X_SLOPE");
    x_offset = GValue::Value("CRDC2_X_OFFSET");
  }

  std::map<int,int> datamap;
  int mpad = GetMaxPad();
  for(int i=0;i<Size();i++) {
    if((channel.at(i) <( mpad-10)) || (channel.at(i)>(mpad+10)))
      continue;
    datamap[channel.at(i)] += GetData(i);
  }


  int i=0;
  std::map<int,int>::iterator it;
  TGraph g(datamap.size());
  for(it=datamap.begin();it!=datamap.end();it++) {
    g.SetPoint(i++,it->first,it->second);
  }
  TVirtualFitter::SetMaxIterations(10);
  g.Fit(fgaus,"qgoff"); //  "gaus","q","goff");
  TVirtualFitter::SetMaxIterations(5000);
  //new GCanvas;
  //g->Draw("AC");
  //new GCanvas;
  //printf("fgaus->GetParameter(1) = %.02f\n",fgaus->GetParameter(1));
  //return (GetMaxPad()*x_slope+x_offset);
  double pad = fgaus->GetParameter(1);
  //fgaus->Reset();
  return (pad*x_slope+x_offset);
}
*/

//float TCrdc::GetDispersiveX() const{
//  if (GetMaxPad() ==-1){
//    return sqrt(-1);
//  }

//  float x_slope = sqrt(-1);
//  float x_offset = sqrt(-1);
//  if(fId==0) {
//    x_slope = GValue::Value("CRDC1_X_SLOPE");
//    x_offset = GValue::Value("CRDC1_X_OFFSET");
//  } else if(fId==1) {
//    x_slope = GValue::Value("CRDC2_X_SLOPE");
//    x_offset = GValue::Value("CRDC2_X_OFFSET");
//  }
//  if(std::isnan(x_slope))
//    x_slope = 1.0;
//  if(std::isnan(x_offset))
//    x_offset = 0.0;
//  


//  std::map<int,int> datamap;
//  int mpad = GetMaxPad();
//  double datasum = 0;
//  for(int i=0;i<Size();i++) {
//    if((channel.at(i) <( mpad-10)) || (channel.at(i)>(mpad+10)))
//      continue;
//    datamap[channel.at(i)] += GetData(i);
//    datasum += GetData(i);
//  }
//  std::map<int,int>::iterator it;
//  double wchansum = 0.0;
//  for(it=datamap.begin();it!=datamap.end();it++) {
//    wchansum += it->first*(it->second/datasum);
//  }
//  return (wchansum*x_slope+x_offset);
//}

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
  //std::cout << " Before Max Pad Return " << std::endl;
  if (maxpad ==-1){
    return sqrt(-1);
  }
  //std::cout << " After Max Pad Return " << std::endl;

  float x_slope = sqrt(-1);
  float x_offset = sqrt(-1);
  if(fId==0) {
    x_slope = GValue::Value("CRDC1_X_SLOPE");
    x_offset = GValue::Value("CRDC1_X_OFFSET");
  } else if(fId==1) {
    x_slope = GValue::Value("CRDC2_X_SLOPE");
    x_offset = GValue::Value("CRDC2_X_OFFSET");
  }
  if(std::isnan(x_slope))
    x_slope = 1.0;
  if(std::isnan(x_offset))
    x_offset = 0.0;
  
  std::map<int,double> datamap;
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
    }
    else{
      cal_data = (double)GetData(i);
    }
    
    datasum += cal_data;
    weighted_sum += channel.at(i)*cal_data;
  }

  // + 0.5 so that we take the middle of the pad, not the left edge.
  double mean_chan = weighted_sum/datasum + 0.5;
  return (mean_chan*x_slope+x_offset);
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

  if(std::isnan(y_slope))
    y_slope = 1.0;
  if(std::isnan(y_offset))
    y_offset = 0.0;

  // std::cout << " ------------------ "  << std::endl;
  // std::cout << " 2 Slope = " << y_slope << std::endl;
  // std::cout << " 2 Offst = " << y_offset << std::endl;
    
  //  return ((GetTimeRand()*y_slope+y_offset));
  float tmp = ((float)time)*y_slope+y_offset;
  //printf("fId[%i]:  %.05f * %.03f + %.05f = %.05f\n",fId,y_slope,(float)time,y_offset,tmp);
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

  fCorrelatedXFP=-1;
  fCorrelatedOBJ=-1;
  fCorrelatedE1=-1;
  fCorrelatedXFP_Ch15=-1;
  fCorrelatedOBJ_Ch15=-1;
  fCorrelatedE1_Ch15=-1;
  
}

void TMTof::Print(Option_t *opt) const {    }
