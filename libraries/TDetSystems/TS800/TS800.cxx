#include "TS800.h"

#include <atomic>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include "TGEBEvent.h"
#include "TGRUTOptions.h"
#include "TInverseMap.h"


bool TS800::fGlobalReset =false;

static double f_mafp_cor = GValue::Value("OBJ_MTOF_CORR_AFP");
static double f_mxfp_cor = GValue::Value("OBJ_MTOF_CORR_XFP");

/*******************************************************************************/
/* TS800 ***********************************************************************/
/* For the unpacking and storage of the S800 and its ancillary detectors *******/
/*******************************************************************************/
TS800::TS800() {
  Clear();
}

TS800::~TS800(){
}

/*******************************************************************************/
/* Copy S800 hits TS800 ********************************************************/
/*******************************************************************************/
void TS800::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TS800& other = (TS800&)obj;
  for(int i = 0; i<3; i++){
    scint[i].Copy(other.scint[i]);
  }
  trigger.Copy(other.trigger);
  tof.Copy(other.tof);
  mtof.Copy(other.mtof);
  ion.Copy(other.ion);
  crdc1.Copy(other.crdc1);
  crdc2.Copy(other.crdc2);
  hodo.Copy(other.hodo);
}

/*******************************************************************************/
/* Uses TInverse map to calculate the x-angle at the target position (rad) *****/
/*******************************************************************************/
Float_t TS800::GetAta(int i) const {
  float ata = TInverseMap::Get()->Ata(i,this);
  if(!std::isnan(GValue::Value("ATA_SHIFT"))) {
    ata += GValue::Value("ATA_SHIFT");
  }
  return ata;
}

Float_t TS800::GetAta(float xfp, float afp, float yfp, float bfp, int i) const {
  float ata = TInverseMap::Get()->Ata(i, xfp, afp, yfp, bfp);
  if(!std::isnan(GValue::Value("ATA_SHIFT"))) {
    ata += GValue::Value("ATA_SHIFT");
  }
  return ata;
}

/*******************************************************************************/
/* Uses TInverse map to calculate the y-angle at the target position (rad) *****/
/*******************************************************************************/
Float_t TS800::GetBta(int i) const {
  float bta = TInverseMap::Get()->Bta(i,this);
  if(!std::isnan(GValue::Value("BTA_SHIFT"))) {
    bta += GValue::Value("BTA_SHIFT");
  }
  return bta;
}

Float_t TS800::GetBta(float xfp, float afp, float yfp, float bfp, int i) const {
  float bta = TInverseMap::Get()->Bta(i, xfp, afp, yfp, bfp);
  if(!std::isnan(GValue::Value("BTA_SHIFT"))) {
    bta += GValue::Value("BTA_SHIFT");
  }
  return bta;
}

/*******************************************************************************/
/* Uses TInverse map to calculate the y offset at the target position (mm) *****/
/*******************************************************************************/
Float_t TS800::GetYta(int i) const {
  float yta = TInverseMap::Get()->Yta(i,this);
  if(!std::isnan(GValue::Value("YTA_SHIFT"))) {
    yta += GValue::Value("YTA_SHIFT");
  }
  return yta;
}

Float_t TS800::GetYta(float xfp, float afp, float yfp, float bfp, int i) const {
  float yta = TInverseMap::Get()->Yta(i, xfp, afp, yfp, bfp);
  if(!std::isnan(GValue::Value("YTA_SHIFT"))) {
    yta += GValue::Value("YTA_SHIFT");
  }
  return yta;
}
/*******************************************************************************/
/* Uses TInverse map to calculate dE/E of outgoing particle relative ***********/
/* to central B-Rho ************************************************************/
/*******************************************************************************/
Float_t TS800::GetDta(int i) const {
  float dta = TInverseMap::Get()->Dta(i,this);
  if(!std::isnan(GValue::Value("DTA_SHIFT"))) {
    dta += GValue::Value("DTA_SHIFT");
  }
  return dta;
}

Float_t TS800::GetDta(float xfp, float afp, float yfp, float bfp, int i) const {
  float dta = TInverseMap::Get()->Dta(i, xfp, afp, yfp, bfp);
  if(!std::isnan(GValue::Value("DTA_SHIFT"))) {
    dta += GValue::Value("DTA_SHIFT");
  }
  return dta;
}

/*******************************************************************************/
/* Calculates projectile scattering angle (mrad) *******************************/
/*******************************************************************************/
Float_t TS800::GetScatteringAngle(float ata, float bta) const {
  return asin(sqrt( pow(sin(ata),2) + pow(sin(bta),2) )) * 1000; //in mrad
}

/*******************************************************************************/
/* Calculates Phi-Polar angle at target position (rad) *************************/
/*******************************************************************************/
Float_t TS800::GetAzita(float ata, float bta) const {
  float xsin = TMath::Sin(ata);
  float ysin = TMath::Sin(bta);
  float azita = 0.0;
  if(xsin>0 && ysin>0){
    azita = TMath::ATan(ysin/xsin);
  } else if(xsin<0 && ysin>0){
    azita = TMath::Pi()-TMath::ATan(ysin/TMath::Abs(xsin));
  } else if(xsin<0 && ysin<0){
    azita = TMath::Pi()+TMath::ATan(TMath::Abs(ysin)/TMath::Abs(xsin));
  } else if(xsin>0 && ysin<0){
    azita = 2.0*TMath::Pi()-TMath::ATan(TMath::Abs(ysin)/xsin);
  } else {
    azita = 0;
  }
  return azita;
}

/*******************************************************************************/
/* Calulate DTA corrected beam velocity ****************************************/
/*******************************************************************************/
float TS800::AdjustedBeta(float beta, float dta) const {
  double gamma = 1.0/(sqrt(1.-beta*beta));
  double dp_p = gamma/(1.+gamma) * dta;;
  beta *=(1.+dp_p/(gamma*gamma));
  return beta;
}

/*******************************************************************************/
/* Returns position vector of projectile used for doppler correction ***********/
/*******************************************************************************/
TVector3 TS800::Track(double sata,double sbta) const {
  double ata = TMath::Sin(GetAta()+sata);
  double bta = TMath::Sin(GetBta()+sbta);

  TVector3 track(ata,-bta,sqrt(1-ata*ata-bta*bta));
  return track;
}

TVector3 TS800::Track(float Ata, float Bta, double sata,double sbta) const {
  double ata = TMath::Sin(Ata + sata);
  double bta = TMath::Sin(Bta + sbta);

  TVector3 track(ata,-bta,sqrt(1-ata*ata-bta*bta));
  return track;
}


/*******************************************************************************/
/* Unused function - may be removed at later date ******************************/
/*******************************************************************************/
TVector3 TS800::CRDCTrack(){
  TVector3 track;
  track.SetTheta(TMath::ATan((GetCrdc(0).GetDispersiveX()-GetCrdc(1).GetDispersiveX())/1073.0)); // rad
  track.SetPhi(TMath::ATan((GetCrdc(0).GetNonDispersiveY()-GetCrdc(1).GetNonDispersiveY())/1073.0)); // rad
  return track;
}

/*******************************************************************************/
/* Returns X position of the CRDCs *********************************************/
/*******************************************************************************/
float TS800::GetXFP(int i) const {
  return GetCrdc(i).GetDispersiveX();
}

/*******************************************************************************/
/* Returns Y position of the CRDCs *********************************************/
/*******************************************************************************/
float TS800::GetYFP(int i) const {
  return GetCrdc(i).GetNonDispersiveY();
}

/*******************************************************************************/
/* Dispersive X angle in the Focal Plane calculated from CRDCs positions *******/
/*******************************************************************************/
float TS800::GetAFP() const{
  if(GetCrdc(0).Size()==0||GetCrdc(1).Size()==0){
    return sqrt(-1);
  }
  return TMath::ATan((GetXFP(1)-GetXFP(0))/1073.0);
}

float TS800::GetAFP(float xfp0, float xfp1) const{
  return TMath::ATan((xfp1-xfp0)/1073.0);
}

/*******************************************************************************/
/* Dispersive Y angle in the Focal Plane calculated from CRDCs positions *******/
/*******************************************************************************/
float TS800::GetBFP() const{
   if(GetCrdc(0).Size()==0||GetCrdc(1).Size()==0){
    return sqrt(-1);
  }
  return TMath::ATan((GetYFP(1)-GetYFP(0))/1073.0);
}

float TS800::GetBFP(float yfp0, float yfp1) const{
  return TMath::ATan((yfp1-yfp0)/1073.0);
}

/*******************************************************************************/
/* Clears Hits *****************************************************************/
/*******************************************************************************/
void TS800::Clear(Option_t* opt){
  TDetector::Clear(opt);
  crdc1.Clear();
  crdc2.Clear();

  scint[0].Clear();
  scint[1].Clear();
  scint[2].Clear();

  tof.Clear();
  mtof.Clear();
  trigger.Clear();
  ion.Clear();

  hodo.Clear();
}

/*******************************************************************************/
/* Unpacks S800 data from GRETINA event files **********************************/
/*******************************************************************************/
int TS800::BuildHits(std::vector<TRawEvent>& raw_data){
  if(raw_data.size() != 1){
    std::cout << "Trying to combine multiple (" <<  raw_data.size() <<"s800 buffers: " << std::endl;
    return 0;
  }
  for(auto& event : raw_data) { // should only be one..
    SetTimestamp(event.GetTimestamp());
    int ptr = 0;
    const TRawEvent::GEBS800Header *head = ((const TRawEvent::GEBS800Header*)event.GetPayload());
    ptr += sizeof(TRawEvent::GEBS800Header);
    //Here, we are now pointing at the size of the next S800 thing.  Inclusive in shorts.
    unsigned short *data = (unsigned short*)(event.GetPayload()+ptr);
    size_t x = 0;
    while(x<(head->total_size-sizeof(TRawEvent::GEBS800Header)+16)) {  //total size is inclusive.
      int size             = *(data+x);
      unsigned short *dptr = (data+x+1);
      x+=size;
      int sizeleft = size-2;
      switch(*dptr) {
      case 0x5801:  //S800 TriggerPacket.
	HandleTrigPacket(dptr+1,sizeleft);
	break;
      case 0x5802:  // S800 TOF - No Longer present in S800 DAQ
	//event.Print("all0x5802");
	HandleTOFPacket(dptr+1,sizeleft);
	break;
      case 0x5810:  // S800 Scint - No Longer present in S800 DAQ
	HandleScintPacket(dptr+1,sizeleft);
	break;
      case 0x5820:  // S800 Ion Chamber
	HandleIonCPacket(dptr+1,sizeleft);
	break;
      case 0x5840:  // CRDC Packet
  	HandleCRDCPacket(dptr+1,sizeleft);
	break;
      case 0x5850:  // II CRDC Packet
	break;
      case 0x5860:  // TA Pin Packet
	break;
      case 0x5870:  // II Track Packet
	break;
      case 0x5880:  // II PPAC
	break;
      case 0x5890:  // Obj Scint
	break;
      case 0x58a0:  // Obj Pin Packet
	break;
      case 0x58b0:  // S800 Hodoscope
  	HandleHodoPacket(dptr+1, sizeleft);
	break;
      case 0x58c0:  // VME ADC
	break;
      case 0x58d0:  // Galotte
	break;
      case 0x58e0:
	break;
      case 0x58f0: //MTDC packet - contains Time information
        HandleMTDCPacket(dptr+1,sizeleft);
	break;
      case 0x5805:
	break;
      default:
	fprintf(stderr,"unknown data S800 type: 0x%04x\n",*dptr);
	return 0;
      };
    }
    SetEventCounter(head->GetEventNumber());
    //geb->Print(toprint.c_str());
  }
  return 1;
}

/*******************************************************************************/
/* Unpacks S800 data from GRETINA event files **********************************/
/* Unsure when this function would be called ***********************************/
/*******************************************************************************/
int TS800::BuildHits(UShort_t eventsize,UShort_t *dptr,Long64_t timestamp) {  //std::vector<TRawEvent>& raw_data){
  SetTimestamp(timestamp);
  //int ptr = 0;
  //const TRawEvent::GEBS800Header *head = ((const TRawEvent::GEBS800Header*)event.GetPayload());
  //ptr += sizeof(TRawEvent::GEBS800Header);

  //Here, we are now pointing at the size of the next S800 thing.  Inclusive in shorts.
  //std::string toprint = "all";

  unsigned short *data = dptr+2; //unsigned short*)(event.GetPayload()+ptr);
  size_t x = 0;
  //printf(" new s800 event\n"); fflush(stdout);
  while(x<eventsize) { //(head->total_size-sizeof(TRawEvent::GEBS800Header)+16)) {  //total size is inclusive.
    int size             = *(data+x);
    unsigned short *dptr = (data+x+1);
    //toprint.append(Form("0x%04x",*dptr));
    x+=size;
    if(x>eventsize) {
      //std::cout << "x is oor." << std::endl;
      return -1;
    }
    if(size==0) {
      std::cout << "size is zero." << std::endl;\
      return -1;
    //  geb->Print(toprint.c_str());
    //  printf("head size = %i\n",sizeof(head));
    //  exit(0);
    }
    int sizeleft = size-2;
    //ptr +=  (*((unsigned short*)(geb->GetPayload()+ptr))*2);
    //printf(" x [0x%04x] at %i\n",*dptr,x); fflush(stdout);
    switch(*dptr) {
    case 0x5801:  //S800 TriggerPacket.
      HandleTrigPacket(dptr+1,sizeleft);
      break;
    case 0x5802:  // S800 TOF.
      //event.Print("all0x5802");
      HandleTOFPacket(dptr+1,sizeleft);
      break;
    case 0x5810:  // S800 Scint
      //event.Print("all0x5810");
      HandleScintPacket(dptr+1,sizeleft);
      break;
    case 0x5820:  // S800 Ion Chamber
      //event.Print("all0x5820");
      HandleIonCPacket(dptr+1,sizeleft);
      break;
    case 0x5840:  // CRDC Packet
      //event.Print("all0x58400x5845");
      HandleCRDCPacket(dptr+1,sizeleft);
      break;
    case 0x5850:  // II CRDC Packet
      break;
    case 0x5860:  // TA Pin Packet
      break;
    case 0x5870:  // II Track Packet
      break;
    case 0x5880:  // II PPAC
      break;
    case 0x5890:  // Obj Scint
      break;
    case 0x58a0:  // Obj Pin Packet
      //end of s800 tag!
      x += size +1;
      break;
    case 0x58b0:  // S800 Hodoscope
      HandleHodoPacket(dptr+1, sizeleft);
      break;
    case 0x58c0:  // VME ADC
      break;
    case 0x58d0:  // Galotte
      break;
    case 0x58e0:
      break;
    case 0x58f0:
      HandleMTDCPacket(dptr+1,sizeleft);
      break;
    default:
      //fprintf(stderr,"unknown data S800 type: 0x%04x  @ x = %i \n",*dptr,x);
      return 0;
    };
  }
  //SetEventCounter(head->GetEventNumber());
  //geb->Print(toprint.c_str());
  return 1;
}

/*******************************************************************************/
/* Unpacks Trigger packet into TTrigger class **********************************/
/*******************************************************************************/
bool TS800::HandleTrigPacket(unsigned short *data,int size) {
  if(size < 1){
    static int i=0;
    i++;
    std::cout << "Encountered " << i << " events with empty trig packet" << std::endl;
    return false;
  }

  trigger.SetRegistr(*data);
  for(int x=1;x<size;x++) {
    unsigned short current = *(data+x);
    switch(current&0xf000) {
    case 0x8000:  //S800
      trigger.SetS800Source(current&0x0fff);
      break;
    case 0x9000:  //External1 source
      trigger.SetExternalSource1(current&0x0fff);
      break;
    case 0xa000:  //External2 source
      trigger.SetExternalSource2(current&0x0fff);
      break;
    case 0xb000:  //Secondary source
      trigger.SetSecondarySource(current&0x0fff);
      break;
    };
  }
  return true;
}

/*******************************************************************************/
/* Unpacks ToF Packet **********************************************************/
/* No longer Present in S800 DAQ may be removed at later date ******************/
/*******************************************************************************/
bool TS800::HandleTOFPacket(unsigned short *data ,int size){

  for(int x = 0; x < size; x++){
    unsigned short current = *(data+x);
    switch(current&0xf000){
    case 0xc000: // RF
      tof.SetRF(current&0x0fff);
      break;
    case 0xd000: // Object Scin
      tof.SetOBJ(current&0x0fff);
      break;
    case 0xe000: // XFP scint
      tof.SetXFP(current&0x0fff);
      break;
    case 0xf000: // Si
      tof.SetSI(current&0x0fff);
      break;
    case 0x4000: // TAC XFP scint
      tof.SetTacXFP(current&0x0fff);
      break;
    case 0x5000: // TAC Object Scint
      tof.SetTacOBJ(current&0x0fff);
      break;
    default:
      return false;
      break;
    }
  }
  return true;
}

/*******************************************************************************/
/* Unpacks CRDC data packets ***************************************************/
/*******************************************************************************/
bool TS800::HandleCRDCPacket(unsigned short *data,int size) {
  TCrdc *current_crdc=0;
  if((*data)<3){
    if(*data==0) current_crdc = &crdc1;
    else if(*data==1) current_crdc = &crdc2;
    else return false;
  }
  if(!current_crdc) return false;

  current_crdc->SetId(*data);

  int x = 1;
  int subsize = *(data+x);
  x++;
  x++;

  current_crdc->SetAddress((0x58<<24) + (1<<16) + (current_crdc->GetId() <<8) + 0);
  std::map<int,std::map<int,int> > pad;

  // This is deliberately different from SpecTcl, in how it handles multiple word2 occurring
  // in a row. We talked to Daniel, and this is when it has read out the same sample/channel
  // on multiple connectors. Therefore, in this case, we should use the same word1 (top bit set)
  // with all the word2 (top bit unset) instances that follow.
  unsigned short word1 = 0;
  while(x<subsize){
    unsigned short current_word = *(data+x); x++;
    // Remember the header.
    if(current_word & 0x8000) {
      word1 = current_word;
    } else if (word1 != 0) {
      // Not a header, so it is
      unsigned short word2 = current_word;
      int sample_number    = (word1&(0x7fc0)) >> 6;
      int channel_number   =  word1&(0x003f);
      int connector_number = (word2&(0x0c00)) >> 10;
      int databits         = (word2&(0x03ff));
      int real_channel = (connector_number << 6) + channel_number;
      if(real_channel > 223) continue;
      pad[real_channel][sample_number] = databits;
    }
  }
  x+=1;
  std::map<int,std::map<int,int> >::iterator it1;
  std::map<int,int>::iterator it2;

  for(it1 = pad.begin(); it1 != pad.end(); it1++) {
    for(it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
      current_crdc->AddPoint(it1->first,it2->first,it2->second);
    }
  }

  if(x >= size) return true;
  subsize = *(data+x);
  x++;
  x++;
  current_crdc->SetAnode(*(data+x));
  x++;
  current_crdc->SetTime(*(data+x));
  return true;
}

/*******************************************************************************/
/* Unpacks ToF Packet **********************************************************/
/* No longer Present in S800 DAQ may be removed at later date ******************/
/*******************************************************************************/
bool TS800::HandleScintPacket(unsigned short* data, int size){

  for(int x = 0; x < size; x += 2){
    unsigned short current = *(data+x);
    unsigned short current_p1 = *(data+x+1);

    if(((current)&(0xf000))==((current_p1)&(0xf000))){
      switch(current&0xf000){
      case 0x0000:
	scint[0].SetdE_Up(current&0x0fff);
	scint[0].SetTime_Up(current_p1&0x0fff);
	scint[0].SetID(1);
	break;
      case 0x1000:
	scint[0].SetdE_Down(current&0x0fff);
	scint[0].SetTime_Down(current_p1&0x0fff);
	scint[0].SetID(1);
	break;
      case 0x2000:
	scint[1].SetdE_Up(current&0x0fff);
	scint[1].SetTime_Up(current_p1&0x0fff);
	scint[1].SetID(2);
	break;
      case 0x3000:
	scint[1].SetdE_Down(current&0x0fff);
	scint[1].SetTime_Down(current_p1&0x0fff);
	scint[1].SetID(2);
	break;
      case 0x4000:
	scint[2].SetdE_Up(current&0x0fff);
	scint[2].SetTime_Up(current_p1&0x0fff);
	scint[2].SetID(3);
	break;
      case 0x5000:
	scint[2].SetdE_Down(current&0x0fff);
	scint[2].SetTime_Down(current_p1&0x0fff);
	scint[2].SetID(3);
	break;
      default:
	return false;
	break;
      }
    }
    else return false;
  }
  return true;
}

/*******************************************************************************/
/* Unpacks IC data packet ******************************************************/
/*******************************************************************************/
bool TS800::HandleIonCPacket(unsigned short* data, int size){

  int x = 0;
  ion.SetAddress((0x58<<24) + (0<<16) + (0<<8) + 0);

  while(x<size){
    int sub_size = (*(data+x)&(0xffff)); x++;
    switch(*(data+x++)){
    case 0x5821:
      for(; x<sub_size;x++){
	unsigned short current = *(data+x);
	int ch  = (current&0xf000)>>12;
	int dat = (current&0x0fff);
	ion.Set(ch,dat);
      }
      break;
    case 0x5822: // Old Style
      for(; x<sub_size;x++){
	unsigned short current = *(data+x);
	ion.Set((current&0xf000),(current&0x0fff));
      }
      break;
    default:
      return false;
      break;
    }
  }
  return true;
}

/*******************************************************************************/
/* Unpacks hodoscope data packet ***********************************************/
/*******************************************************************************/
bool TS800::HandleHodoPacket(unsigned short *data,int size) {
  if(!size) return false;
  //note: size is size left after subtracting out length of packet size and packet tag

  //ID is the Hodoscope Sub-pcket "Energy" tag
  //If ID == 0, then we are dealing with channels (0,15), if ID == 1, then we
  //are dealing with channels (16,31)
  int x = 0;
  int id = *(data+x); x += 1;
  while (x < size){
    if (id == 2){
      x += 3;
      break;
    }

    //Energy values are 16-bit integers, where the 13th bit is the channel number
    //(0,15) and the first 12 bits are the energy.
    unsigned short charge = (*(data+x)) & 0x0fff;
    unsigned short channel = id*16 + (*(data+x) >> 12);
    x += 1;

    THodoHit hit;
    hit.SetChannel(channel);
    hit.SetCharge(charge);
    hit.SetAddress((0x58<<24) + (4<<16) + (4<<8) + channel);
    hodo.InsertHit(hit);
  } //x < size
  return true;
}


/*******************************************************************************/
/* Unpacks MTDC data packet ****************************************************/
/*******************************************************************************/
bool TS800::HandleMTDCPacket(unsigned short *data,int size) {
  int x = 0;
  while(x<size){

    unsigned short word1 = *(data+x);
    unsigned short word2 = *(data+x+1);
    x+=2;
    switch((word1&0x000f)) {
      case 0: //E1up
        mtof.fE1Up.push_back(word2);
        break;
      case 1: //E1down
        mtof.fE1Down.push_back(word2);
        break;
      case 2: //Xfp
        mtof.fXfp.push_back(word2);
        break;
      case 3: //Obj
        mtof.fObj.push_back(word2);
        break;
      case 5: //Rf
        mtof.fRf.push_back(word2);
        break;
      case 6: //crdc1 anode
        mtof.fCrdc1Anode.push_back(word2);
        break;
      case 7: //crdc2 anode
        mtof.fCrdc2Anode.push_back(word2);
        break;
      case 12: //Hodoscope
        mtof.fHodoscope.push_back(word2);
        break;
      case 15: //Ref. (E1up)
        mtof.fRef.push_back(word2);
        break;
      default:
        //printf("unknown..\n");
        break;
    };
  }
  return true;
}


/*******************************************************************************/
/* Inserts Hit - Blank function needed for compatibility with TDetector ********/
/*******************************************************************************/
void TS800::InsertHit(const TDetectorHit& hit){
  return;
}

/*******************************************************************************/
/* Gets Hit - No Functionality needed for compatibility with TDetector *********/
/*******************************************************************************/
TDetectorHit& TS800::GetHit(int i){
  TDetectorHit *hit = new TS800Hit;
  return *hit;
}

/*******************************************************************************/
/* Below functions are related to TTOF *****************************************/
/* This module is no lonnger present in S800 DAQ and these functions may be ****/
/* removed at a later date *****************************************************/
/*******************************************************************************/
float TS800::GetTofE1_TAC(float c1,float c2)  const {
  if(GetTof().GetTacOBJ()>-1) return GetTof().GetTacOBJ() + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX();
  return sqrt(-1);
}

float TS800::GetTofXFPE1_TAC(float c1,float c2)  const {
  if(GetTof().GetTacXFP()>-1) return GetTof().GetTacXFP() + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX();
  return sqrt(-1);
}

float TS800::GetTofE1_TDC(float c1,float c2)  const {
  if(GetTof().GetOBJ()>-1) return GetTof().GetOBJ() - GetScint().GetTimeUp() + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX();
  return sqrt(-1);
}

float TS800::GetTofXFP_E1_TDC(float c1,float c2)  const {
  if(GetTof().GetXFP()>-1) return GetTof().GetXFP() - GetScint().GetTimeUp() + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX();
  return sqrt(-1);
}

float TS800::GetOBJRaw_TAC() const {
  return (GetTof().GetTacOBJ());
}

float TS800::GetXFRaw_TAC() const {
  return (GetTof().GetTacXFP());
}

float TS800::GetOBJ_E1Raw() const {
  return (GetTof().GetOBJ() - GetScint().GetTimeUp());
}

float TS800::GetXF_E1Raw() const {
  return (GetTof().GetXFP() - GetScint().GetTimeUp());
}

float TS800::GetCorrTOF_OBJTAC() const {
  double afp_cor = GValue::Value("OBJTAC_TOF_CORR_AFP");
  double xfp_cor = GValue::Value("OBJTAC_TOF_CORR_XFP");
  return GetTofE1_TAC(afp_cor,xfp_cor);
}

float TS800::GetCorrTOF_XFPTAC() const {
  double afp_cor = GValue::Value("XFPTAC_TOF_CORR_AFP");
  double xfp_cor = GValue::Value("XFPTAC_TOF_CORR_XFP");
  return GetTofXFPE1_TAC(afp_cor,xfp_cor);
}

float TS800::GetCorrTOF_OBJ() const {
  double afp_cor = GValue::Value("OBJ_TOF_CORR_AFP");
  double xfp_cor = GValue::Value("OBJ_TOF_CORR_XFP");
  return GetTofE1_TDC(afp_cor,xfp_cor);
}

float TS800::GetCorrTOF_XFP() const {
  double afp_cor = GValue::Value("XFP_TOF_CORR_AFP");
  double xfp_cor = GValue::Value("XFP_TOF_CORR_XFP");
  return GetTofXFP_E1_TDC(afp_cor,xfp_cor);
}

float TS800::GetCorrTOF_OBJ_MESY(int i) const {
  f_mafp_cor = GValue::Value("OBJ_MTOF_CORR_AFP");
  f_mxfp_cor = GValue::Value("OBJ_MTOF_CORR_XFP");
  return GetTofE1_MTDC(f_mafp_cor,f_mxfp_cor,i);
}
/*******************************************************************************/
/* End of Tof Functions ********************************************************/
/*******************************************************************************/

/*******************************************************************************/
/* Returns value of MTof vectors ***********************************************/
/* Could probably be replaced with more general function ***********************/
/*******************************************************************************/
float TS800::GetRawOBJ_MESY(unsigned int i) const {
  if(i >= mtof.fObj.size()) return sqrt(-1);
  return (mtof.fObj.at(i));
}

float TS800::GetRawE1_MESY(unsigned int i) const {
  if(i >= mtof.fE1Up.size()) return sqrt(-1);
  return (mtof.fE1Up.at(i));
}

float TS800::GetRawE1_MESY_Ch15(unsigned int i) const {
  if(i >= mtof.fRef.size()) return sqrt(-1);
  return (mtof.fRef.at(i));
}

float TS800::GetRawXF_MESY(unsigned int i) const {
  if(i >= mtof.fXfp.size()) return sqrt(-1);
  return (mtof.fXfp.at(i));
}

/*******************************************************************************/
/* ToF functions ***************************************************************/
/* Do not appear to be currently used and could be replaced with ***************/
/* TMTof.GetCorrelatedTof() ****************************************************/
/*******************************************************************************/
float TS800::GetOBJ_E1Raw_MESY(int i) const {
  std::vector<float> result;
  for(unsigned int x = 0; x < mtof.fObj.size(); x++) {
    for(unsigned int y = 0; y < mtof.fE1Up.size(); y++) {
      result.push_back( mtof.fObj.at(x) - mtof.fE1Up.at(y));
    }
  }
  if(result.size() > (unsigned int)i) return result.at(i);
  return sqrt(-1.0);
}

float TS800::GetOBJ_E1Raw_MESY_Ch15(int i) const {
  std::vector<float> result;
  for(unsigned int x = 0; x < mtof.fObj.size(); x++) {
    for(unsigned int y = 0; y < mtof.fRef.size(); y++) {
      result.push_back( mtof.fObj.at(x) - mtof.fRef.at(y));
      }
    }
  if(result.size()>(unsigned int)i) return result.at(i);
  return sqrt(-1.0);
}

float TS800::GetXF_E1Raw_MESY(int i) const {
  std::vector<float> result;
  for(unsigned int x = 0; x < mtof.fXfp.size(); x++) {
    for(unsigned int y = 0; y < mtof.fE1Up.size(); y++) {
      result.push_back( mtof.fXfp.at(x) - mtof.fE1Up.at(y));
    }
  }
  if(result.size() > (unsigned int)i) return result.at(i);
  return sqrt(-1.0);
}

float TS800::GetXF_E1Raw_MESY_Ch15(int i) const {
  std::vector<float> result;
  for(unsigned int x = 0; x < mtof.fXfp.size(); x++) {
    for(unsigned int y = 0; y < mtof.fRef.size(); y++) {
      result.push_back( mtof.fXfp.at(x) - mtof.fRef.at(y));
    }
  }
  if(result.size() > (unsigned int)i) return result.at(i);
  return sqrt(-1.0);
}

float TS800::GetTofE1_MTDC(float c1,float c2,int i) const {
  std::vector<float> result;
  for(unsigned int x = 0; x < mtof.fObj.size(); x++) {
    for(unsigned int y = 0;y < mtof.fE1Up.size(); y++) {
      result.push_back( mtof.fObj.at(x) - mtof.fE1Up.at(y) + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX());
    }
  }
  if(result.size() > (unsigned int)i) return result.at(i);
  return sqrt(-1.0);
}

/*******************************************************************************/
/* Corrected ToF between OBJ and E1 ********************************************/
/* Should use more general GetMTofCorr() ***************************************/
/*******************************************************************************/
double TS800::GetMTofObjE1() const {
  double afp_cor = GValue::Value("OBJ_MTOF_CORR_AFP");
  double xfp_cor = GValue::Value("OBJ_MTOF_CORR_XFP");
  static int line_displayed = 0;
  if(std::isnan(afp_cor) || std::isnan(xfp_cor)) {
    if(line_displayed < 10) {
      printf(ALERTTEXT "Attmepting to do mtof obj correction without values!" RESET_COLOR "\n");
      fflush(stdout);
      line_displayed++;
    }
    return sqrt(-1);
  }
  return GetMTofObjE1(afp_cor, xfp_cor);
}

double TS800::GetMTofObjE1(double afp_cor, double xfp_cor) const {
  // I return the correlated gvalue corrected time-of-flight obj to e1.
  return(GetMTof().GetCorrelatedObjE1()
         + afp_cor * GetAFP() + xfp_cor  * GetCrdc(0).GetDispersiveX());
}

/*******************************************************************************/
/* Corrected ToF between XFP and E1 ********************************************/
/* Should use more general GetMTofCorr() ***************************************/
/*******************************************************************************/
double TS800::GetMTofXfpE1() const {
  double afp_cor = GValue::Value("XFP_MTOF_CORR_AFP");
  double xfp_cor = GValue::Value("XFP_MTOF_CORR_XFP");
  static int line_displayed = 0;
  if(std::isnan(afp_cor) || std::isnan(xfp_cor)) {
    if(line_displayed < 10) {
      printf(ALERTTEXT "Attmepting to do mtof xfp correction without values!" RESET_COLOR "\n");
      fflush(stdout);
      line_displayed++;
    }
    return sqrt(-1);
  }
  return GetMTofXfpE1(afp_cor, xfp_cor);
}

double TS800::GetMTofXfpE1(double afp_cor, double xfp_cor) const {
  return(GetMTof().GetCorrelatedXfpE1()
         + afp_cor * GetAFP() + xfp_cor  * GetCrdc(0).GetDispersiveX());
}
/*******************************************************************************/
/* General purpose function to calculate corrected time of flight between any **/
/* two channels - Replaces GetMTofOBjE1 and GetMTofXFPE1 ***********************/
/*******************************************************************************/
double TS800::GetMTofCorr(double correlatedtof, double afp, double xp, double afp_cor, double xfp_cor) const {
  static int line_displayed;
  if(std::isnan(afp_cor) || std::isnan(xfp_cor)) {
    if(line_displayed < 10) {
      printf(ALERTTEXT "Attmepting to do mtof corrections without values!" RESET_COLOR "\n");
      fflush(stdout);
      line_displayed++;
    }
    return sqrt(-1);
  }
  return (correlatedtof + afp_cor*afp + xfp_cor*xp);
}

/*******************************************************************************/
/* TS800Track ******************************************************************/
/* Single function to calculate and store all S800 angles **********************/
/*******************************************************************************/
TS800Track::TS800Track() {
  Clear();
}


TS800Track::~TS800Track(){
}

/*******************************************************************************/
/* Clears all values ***********************************************************/
/*******************************************************************************/
void TS800Track::Clear() {
  xfp[0] = -1;
  xfp[1] = -1;
  yfp[0] = -1;
  yfp[1] = -1;
  afp = -1;
  bfp = -1;
  ata = -1;
  yta = -1;
  bta = -1;
  dta = -1;
  azita = -1;
}

/*******************************************************************************/
/* Calulates everything from the CRDC postions and inverse maps with minimum ***/
/* number of function calls ****************************************************/
/*******************************************************************************/
void TS800Track::CalculateTracking(const TS800 *s800, int i) {

  int maxp0 = s800->GetCrdc(0).GetMaxPad();
  int maxp1 = s800->GetCrdc(1).GetMaxPad();

//  int maxp0 = s800->GetCrdc(0).GetMaxPadFast();
//  int maxp1 = s800->GetCrdc(1).GetMaxPadFast();

  xfp[0] = s800->GetCrdc(0).GetDispersiveX(maxp0);
  xfp[1] = s800->GetCrdc(1).GetDispersiveX(maxp1);
  yfp[0] = s800->GetCrdc(0).GetNonDispersiveY(maxp0);
  yfp[1] = s800->GetCrdc(1).GetNonDispersiveY(maxp1);

  afp = s800->GetAFP(xfp[0], xfp[1]);
  bfp = s800->GetBFP(yfp[0], yfp[1]);

  ata = s800->GetAta(xfp[0], afp, yfp[0], bfp, i);
  bta = s800->GetBta(xfp[0], afp, yfp[0], bfp, i);
  yta = s800->GetYta(xfp[0], afp, yfp[0], bfp, i);
  dta = s800->GetDta(xfp[0], afp, yfp[0], bfp, i);

  scatter = s800->GetScatteringAngle(ata, bta);
  azita = s800->GetAzita(ata, bta);
  track = s800->Track(ata, bta);
}
