#include "TS800.h"


#include <atomic>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include "TGEBEvent.h"
#include "TGRUTOptions.h"
#include "GH2I.h"
#include "GRootCommands.h"
#include "GCanvas.h"

#include "TChain.h"
#include "TPad.h"
#include "TROOT.h"

#include "TInverseMap.h"

bool TS800::fGlobalReset =false;



static double f_mafp_cor = GValue::Value("OBJ_MTOF_CORR_AFP");
static double f_mxfp_cor = GValue::Value("OBJ_MTOF_CORR_XFP");



TS800::TS800() {
  Clear();
}


TS800::~TS800(){
}

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


Float_t TS800::GetAta(int i) const {
  //float Shift_ata = 0;
  float ata = TInverseMap::Get()->Ata(i,this);
  if(!std::isnan(GValue::Value("ATA_SHIFT"))) {
    ata += GValue::Value("ATA_SHIFT");
  }
  return ata;

}

Float_t TS800::GetBta(int i) const {
  float bta = TInverseMap::Get()->Bta(i,this);
  if(!std::isnan(GValue::Value("BTA_SHIFT"))) {
    bta += GValue::Value("BTA_SHIFT");
  }
  return bta;
}

Float_t TS800::GetYta(int i) const {
  float yta = TInverseMap::Get()->Yta(i,this);
  if(!std::isnan(GValue::Value("YTA_SHIFT"))) {
    yta += GValue::Value("YTA_SHIFT");
  }
  return yta;
}

Float_t TS800::GetDta(int i) const {
  float dta = TInverseMap::Get()->Dta(i,this);
  if(!std::isnan(GValue::Value("DTA_SHIFT"))) {
    dta += GValue::Value("DTA_SHIFT");
  }
  return dta;
}

float TS800::AdjustedBeta(float beta) const {
  double gamma = 1.0/(sqrt(1.-beta*beta));
  double dp_p = gamma/(1.+gamma) * GetDta();
  beta *=(1.+dp_p/(gamma*gamma));
  return beta;
}


Float_t TS800::Azita(int order){
  float xsin = TMath::Sin(GetAta(order));
  float ysin = TMath::Sin(GetBta(order));
  float azita = 0.0;
  if(xsin>0 && ysin>0){
    azita = TMath::ATan(ysin/xsin);
  } else if(xsin<0 && ysin>0){
    azita = TMath::Pi()-TMath::ATan(ysin/TMath::Abs(xsin));
  } else if(xsin<0 && ysin<0){
    azita = TMath::Pi()+TMath::ATan(TMath::Abs(ysin)/TMath::Abs(xsin));
  } else if(xsin>0 && ysin<0){
    azita = 2.0*TMath::Pi()-TMath::ATan(TMath::Abs(ysin)/xsin);
  } else{
    azita = 0;
  }
  return azita;
}

TVector3 TS800::Track(double sata,double sbta) const {
  //TVector3 track(0,0,1);  // set to input beam trajectory 
  //track.RotateY(GetAta(3));
  //track.Rotate( GetBta(3),-track.Cross(TVector3(0,1,0)) );
  //  TVector3 track(TMath::Sin(GetAta()),-TMath::Sin(GetBta()),1);
  double ata = TMath::Sin(GetAta()+sata);
  double bta = TMath::Sin(GetBta()+sbta);

  TVector3 track(ata,-bta,sqrt(1-ata*ata-bta*bta));
  //TVector3 track(ata,-bta,1.0);
  
  return track;//.Unit();
}


TVector3 TS800::ExitTargetVect(int order){
  TVector3 track;
  double sin_ata = 0;
  double sin_bta = 0;
  sin_ata = GetAta(order); //* TMath::DegToRad() ;
  sin_bta = GetBta(order); // * TMath::DegToRad() ;
  
  sin_ata = TMath::Sin(sin_ata);
  sin_bta = TMath::Sin(sin_bta);

  double phi   = 0;
  double theta = 0;
  
   if(sin_ata>0 && sin_bta>0)      phi = 2.0*TMath::Pi()-TMath::ATan(sin_bta/sin_ata);
   else if(sin_ata<0 && sin_bta>0) phi = TMath::Pi()+TMath::ATan(sin_bta/TMath::Abs(sin_ata));
   else if(sin_ata<0 && sin_bta<0) phi = TMath::Pi()-TMath::ATan(TMath::Abs(sin_bta)/TMath::Abs(sin_ata));
   else if(sin_ata>0 && sin_bta<0) phi = TMath::ATan(TMath::Abs(sin_bta)/sin_ata);
   else                      phi = 0;

   theta = TMath::ASin(TMath::Sqrt(sin_ata*sin_ata+sin_bta*sin_bta));
   //   track.SetMagThetaPhi(1,theta,phi);
   track.SetXYZ(TMath::Sin(theta)*TMath::Cos(phi),TMath::Sin(theta)*TMath::Sin(phi),TMath::Cos(phi));
   //track.SetXYZ(sin_ata,-sin_bta,1);
   return track; 
}

TVector3 TS800::CRDCTrack(){
  TVector3 track;
  track.SetTheta(TMath::ATan((GetCrdc(0).GetDispersiveX()-GetCrdc(1).GetDispersiveX())/1073.0)); // rad
  track.SetPhi(TMath::ATan((GetCrdc(0).GetNonDispersiveY()-GetCrdc(1).GetNonDispersiveY())/1073.0)); // rad
  //track.SetPt(1) // need to do this.

  return track;
}

float TS800::GetXFP(int i) const {
  return GetCrdc(i).GetDispersiveX();
}

float TS800::GetYFP(int i) const {
  float ypos = GetCrdc(i).GetNonDispersiveY();

  // // Bit 2 in registr is coincidence event.
  // // These have a timing offset, which corresponds to a spatial offset.
  // if(trigger.GetRegistr() & 2) {
  //   double slope = GValue::Value( (i==0) ? "CRDC1_Y_SLOPE" : "CRDC2_Y_SLOPE" );
  //   double coinc_timediff = 10;
  //   ypos -= slope*coinc_timediff;
  // }

  return ypos;
}

float TS800::GetAFP() const{
   /*  if (GetCrdc(0).GetId() == -1 || GetCrdc(1).GetId() == -1){
     return sqrt(-1);
  }*/
  if(GetCrdc(0).Size()==0||GetCrdc(1).Size()==0){
    return sqrt(-1);
  }
  float AFP = TMath::ATan((GetXFP(1)-GetXFP(0))/1073.0);
  return AFP;

}

float TS800::GetBFP() const{
  /*if (GetCrdc(0).GetId() == -1 || GetCrdc(1).GetId() == -1) {
    return sqrt(-1);
    }*/
  if(GetCrdc(0).Size()==0||GetCrdc(1).Size()==0){
    return sqrt(-1);
  }
  float BFP = TMath::ATan((GetYFP(1)-GetYFP(0))/1073.0);
  return BFP;
}

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
    //std::string toprint = "all";
    size_t x = 0;
    while(x<(head->total_size-sizeof(TRawEvent::GEBS800Header)+16)) {  //total size is inclusive.
      int size             = *(data+x);
      unsigned short *dptr = (data+x+1);
      //toprint.append(Form("0x%04x",*dptr));
      x+=size;
      //if(size==0) {
      //  geb->Print(toprint.c_str());
      //  printf("head size = %i\n",sizeof(head));
      //  exit(0);
      //}
      int sizeleft = size-2;
      //ptr +=  (*((unsigned short*)(geb->GetPayload()+ptr))*2);
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
	fprintf(stderr,"unknown data S800 type: 0x%04x\n",*dptr);
	return 0;
      };
    }
    SetEventCounter(head->GetEventNumber());
    //geb->Print(toprint.c_str());
  }
  return 1;
}

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








bool TS800::HandleTrigPacket(unsigned short *data,int size) {
  if(size<1){
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


bool TS800::HandleCRDCPacket(unsigned short *data,int size) {
  //std::cout << "----------------------" << std::endl;
  //std::cout << " In Handle CRDC " << std::endl;

  TCrdc *current_crdc=0;
  if((*data)<3){
    if(*data==0) current_crdc = &crdc1;
    else if(*data==1) current_crdc = &crdc2;
    else return false;
  }
  if(!current_crdc)
    return false;

  //printf("crdc = [%i]\t0x%08x\n",(*data),crdc+(*data));

  current_crdc->SetId(*data);
  /*std::cout << std::hex << " data : " <<  *data << std::endl;
  std::cout << std::hex << " ID   : " << current_crdc->GetId() << std::endl;
  std::dec;
  */


  int x =1;
  int subsize = *(data+x);
  x++;
  //int subtype = *(data+x);
  x++;

  //std::cout << " subsize : " << std::hex << subsize << std::endl;
  //std::cout << " subtype : " << std::hex << subtype << std::endl;
  current_crdc->SetAddress((0x58<<24) + (1<<16) + (current_crdc->GetId() <<8) + 0);

  std::map<int,std::map<int,int> > pad;

  // This is deliberately different from SpecTcl,
  //   in how it handles multiple word2 occurring in a row.
  // We talked to Daniel, and this is when it has read out
  //   the same sample/channel on multiple connectors.
  // Therefore, in this case, we should use the same word1 (top bit set)
  //   with all the word2 (top bit unset) instances that follow.
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

      /*std::cout << " sample Number    : " << std::dec << sample_number << std::endl;
        std::cout << " channel Number   : " << std::dec << channel_number << std::endl;
        std::cout << " connector Number : " << std::dec << connector_number << std::endl;
        std::cout << " data bits        : " << std::dec << databits << std::endl;
        std::cout << " real channel     : " << std::dec << real_channel << std::endl;
        std::dec;
      */
      pad[real_channel][sample_number] = databits;
    }
  }
  x+=1;
  std::map<int,std::map<int,int> >::iterator it1;
  std::map<int,int>::iterator it2;

  for(it1=pad.begin();it1!=pad.end();it1++) {
    //printf("channel[%03i]\n",it1->first);
    for(it2=it1->second.begin();it2!=it1->second.end();it2++) {
      //printf("\t%i\t%i\n",it2->first,it2->second);
      current_crdc->AddPoint(it1->first,it2->first,it2->second);
    }
  }
  //printf("\nchannel.size() = %i\n\n\n",current_crdc->Size());
  //printf("\t0x%08x\t%i\n",currentcrdc,currentcrdc->

  if(x>=size)
      return true;
  subsize = *(data+x);
  x++;
  //subtype = *(data+x);
  x++;
  current_crdc->SetAnode(*(data+x));
  x++;
  current_crdc->SetTime(*(data+x));

  /*std::cout << " subsize   : " << std::hex << subsize << std::endl;
  std::cout << " subtype   : " << subtype << std::endl;
  std::cout << " ID   : " << current_crdc->GetId() << std::endl;
  std::cout << " CRDC Time : " << current_crdc->GetTime() << std::endl;
  std::cout << " CRDC Anod : " << current_crdc->GetAnode() << std::endl;
  std::dec;*/
  return true;
}

bool TS800::HandleScintPacket(unsigned short* data, int size){

  for(int x = 0; x<size;x+=2){
    unsigned short current = *(data+x);
    unsigned short current_p1 = *(data+x+1);

    if(((current)&(0xf000))==((current_p1)&(0xf000))){
      switch(current&0xf000){
      case 0x0000:
	scint[0].SetdE_Up(current&0x0fff);
	scint[0].SetTime_Up(current_p1&0x0fff);
	scint[0].SetID(1);

	//std::cout << " Channel 1 Up " << std::endl;
	//std::cout << " Energy : " << scint[0].GetdE_Up() << std::endl;
	//std::cout << " Time : " << scint[0].GetTime_Up() << std::endl;
	break;
      case 0x1000:
	scint[0].SetdE_Down(current&0x0fff);
	scint[0].SetTime_Down(current_p1&0x0fff);
	scint[0].SetID(1);

	//std::cout << " Channel 1 Down " << std::endl;
	//std::cout << " Energy : " << scint[0].GetdE_Down() << std::endl;
	//std::cout << " Time : " << scint[0].GetTime_Down() << std::endl;
	break;
      case 0x2000:
	scint[1].SetdE_Up(current&0x0fff);
	scint[1].SetTime_Up(current_p1&0x0fff);
	scint[1].SetID(2);

	//std::cout << " Channel 2 Up " << std::endl;
	//std::cout << " Energy : " << scint[1].GetdE_Up() << std::endl;
	//std::cout << " Time : " << scint[1].GetTime_Up() << std::endl;
	break;
      case 0x3000:
	scint[1].SetdE_Down(current&0x0fff);
	scint[1].SetTime_Down(current_p1&0x0fff);
	scint[1].SetID(2);

	//std::cout << " Channel 2 Down " << std::endl;
	//std::cout << " Energy : " << scint[1].GetdE_Down() << std::endl;
	//	std::cout << " Time : " << scint[1].GetTime_Down() << std::endl;
	break;
      case 0x4000:
	scint[2].SetdE_Up(current&0x0fff);
	scint[2].SetTime_Up(current_p1&0x0fff);
	scint[2].SetID(3);

	//std::cout << " Channel 3 Up " << std::endl;
	//std::cout << " Energy : " << scint[2].GetdE_Up() << std::endl;
	//std::cout << " Time : " << scint[2].GetTime_Up() << std::endl;
	break;
      case 0x5000:
	scint[2].SetdE_Down(current&0x0fff);
	scint[2].SetTime_Down(current_p1&0x0fff);
	scint[2].SetID(3);

	//std::cout << " Channel 3 Down " << std::endl;
	//std::cout << " Energy : " << scint[2].GetdE_Down() << std::endl;
	//std::cout << " Time : " << scint[2].GetTime_Down() << std::endl;
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



bool TS800::HandleIonCPacket(unsigned short* data, int size){
  //  std::cout << "-------------------------------------" << std::endl;
  //std::cout << "   In Handle Ion Chamber Packet " << std::endl;
  int x = 0;

  ion.SetAddress((0x58<<24) + (0<<16) + (0<<8) + 0);

  while(x<size){
    int sub_size = (*(data+x)&(0xffff)); x++;
    //std::cout << " Sub packet size : " << sub_size << std::endl;
    //std::cout << " data+x          : " << std::hex << *(data+x) << std::endl;
    //std::dec;
    switch(*(data+x++)){
    case 0x5821:
      for(; x<sub_size;x++){
	unsigned short current = *(data+x);
	int ch  = (current&0xf000)>>12;
	int dat = (current&0x0fff);
	ion.Set(ch,dat);
	//std::cout << " --- " << std::endl;
	//std::cout << " Channel : " << std::hex << ch << std::endl;
	//std::cout << " Energy  : " << std::hex << dat << std::endl;
	//std::cout << " Channel : " << std::hex << ((current&0xf000)>>12) << std::endl;
	//std::cout << " Energy  : " << std::hex << ion.GetData(ch) << std::endl;
	//std::dec;
      }
      break;
    case 0x5822: // Old Style
      for(; x<sub_size;x++){
	unsigned short current = *(data+x);
	ion.Set((current&0xf000),(current&0x0fff));
	//std::cout << " Channel : " << std::hex << (current&0xf000) << std::endl;
	//std::cout << " Energy  : " << std::dec << ion.GetData((current&0xf000)) << std::endl;
      }
      break;
    default:
      return false;
      break;
    }
  }
  //printf("i am here.\n");
  //for(int x=0;x<ion.Size();x++) {
  // printf("\t[%02i]\t=\t%i\n",ion.GetChannel(x),ion.GetData(x));
  //}


  return true;
}


//Hodoscope packets are described in detail on the following website:
//https://wikihost.nscl.msu.edu/S800Doc/doku.php?id=event_filter#hodoscope_packet
bool TS800::HandleHodoPacket(unsigned short *data,int size) {
  if(!size)
    return false;
  
  //note: size is size left after subtracting out length of packet size and packet tag

  //ID is the Hodoscope Sub-pcket "Energy" tag
  //If ID == 0, then we are dealing with channels (0,15), if ID == 1, then we
  //are dealing with channels (16,31)
  int x = 0;
  int id = *(data+x); x += 1;
  while (x < size){
    if (id == 2){
//    hit_reg1 = *(data+x);
//    hit_reg2 = *(data+x+1);
//    tac = *(data+x+2);
      x += 3;
      break;
    }
//    unsigned short cur_packet = *data; ++data; size -= 1; 
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
  }//x < size
  return true;
}
/*
bool TS800::HandleHODOPacket(unsigned short *data,int size) {
  if(!size)
    return false;
  //printf("HODO id: 0x%04x\n",(*((unsigned short*)data)));
  int id = (*((unsigned short*)data));
  for(int x=2;x<size;x+=2) {
    THodoHit *hit =0;
    unsigned short temp = (*((unsigned short*)data));
    //printf("\tHODO : 0x%04x\n",(*((unsigned short*)data)));
    switch(id) {
      case 0:
        hit = (THodoHit*)hodo->ConstructedAt(hodo->GetEntries());
        hit->SetChannel((temp*0xf000)>>12);
        hit->Set(temp);
        break;
      case 1:
        hit = (THodoHit*)hodo->ConstructedAt(hodo->GetEntries());
        hit->SetChannel(((temp*0xf000)>>12)+16);
        hit->Set(temp);
        break;
      case 2:
        hodo_hit_pattern1 = temp;
        hodo_hit_pattern2 = (*((unsigned short*)(data+2)));
        hodo_hit_time     = (*((unsigned short*)(data+4)));
        return true;
    }
  }

  return true;
}
*/
bool TS800::HandleMTDCPacket(unsigned short *data,int size) {
  int x = 0;
  while(x<size){
    //printf("0x%04x  ",*(data+x));
    //x++;
    //if((x%8)==0)
    //  printf("\n");

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
  //printf("\n\n");

  return true;
}


void TS800::InsertHit(const TDetectorHit& hit){
  return;
}

TDetectorHit& TS800::GetHit(int i){
  TDetectorHit *hit = new TS800Hit;
  return *hit;
}

float TS800::GetTofE1_TAC(float c1,float c2)  const {
  /*if (GetCrdc(0).GetId() == -1) {
    return sqrt(-1);
    }*/
  /*----------------*\
  | AFP returns nan  |
  | if both crdc's   |
  | are not present  |
  \*----------------*/

  if(GetTof().GetTacOBJ()>-1)
    return GetTof().GetTacOBJ() + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX();
  return sqrt(-1);

}


float TS800::GetTofE1_TDC(float c1,float c2)  const {
  /*----------------*\
  | AFP returns nan  |
  | if both crdc's   |
  | are not present  |
  \*----------------*/
  if(GetTof().GetOBJ()>-1)
    return GetTof().GetOBJ() - GetScint().GetTimeUp() + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX();
  return sqrt(-1);
}

float TS800::GetTofE1_MTDC(float c1,float c2,int i) const {

  std::vector<float> result;
  // TODO: This check is always false.  Commented it out, but was there some reason for it?
  // if(mtof.fObj.size()<0)
  //   std::cout << " In GetTOF MTDC, Size = " << mtof.fObj.size() << std::endl;
  for(unsigned int x=0;x<mtof.fObj.size();x++) {
    for(unsigned int y=0;y<mtof.fE1Up.size();y++) {
      result.push_back( mtof.fObj.at(x) - mtof.fE1Up.at(y) + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX());
    }
  }

  if(result.size()>(unsigned int)i)
    return result.at(i);
  return sqrt(-1.0);
}

float TS800::GetOBJRaw_TAC() const {
  return (GetTof().GetTacOBJ());
}

float TS800::GetOBJ_E1Raw() const {
  return (GetTof().GetOBJ() - GetScint().GetTimeUp()); // Time in OBJ - Time in E1
}

float TS800::GetOBJ_E1Raw_MESY(int i) const {
  std::vector<float> result;
  for(unsigned int x=0;x<mtof.fObj.size();x++) {
    for(unsigned int y=0;y<mtof.fE1Up.size();y++) {
      result.push_back( mtof.fObj.at(x) - mtof.fE1Up.at(y));
      }
    }

  if(result.size()>(unsigned int)i)
    return result.at(i);
  return sqrt(-1.0);
}

float TS800::GetOBJ_E1Raw_MESY_Ch15(int i) const {
  std::vector<float> result;
  for(unsigned int x=0;x<mtof.fObj.size();x++) {
    for(unsigned int y=0;y<mtof.fRef.size();y++) {
      result.push_back( mtof.fObj.at(x) - mtof.fRef.at(y));
      }
    }

  if(result.size()>(unsigned int)i)
    return result.at(i);
  return sqrt(-1.0);
}

float TS800::GetRawOBJ_MESY(unsigned int i) const {
  if(i>=mtof.fObj.size())
    return sqrt(-1);
  return (mtof.fObj.at(i));
}

float TS800::GetRawE1_MESY(unsigned int i) const {
  if(i>=mtof.fE1Up.size())
    return sqrt(-1);
  return (mtof.fE1Up.at(i));
}

float TS800::GetRawE1_MESY_Ch15(unsigned int i) const {
  if(i>=mtof.fRef.size())
    return sqrt(-1);
  return (mtof.fRef.at(i));
}

float TS800::GetRawXF_MESY(unsigned int i) const {
  if(i>=mtof.fXfp.size())
    return sqrt(-1);
  return (mtof.fXfp.at(i));
}

float TS800::GetXFRaw_TAC() const {
  return (GetTof().GetTacXFP());
}
float TS800::GetXF_E1Raw() const {
  return (GetTof().GetXFP() - GetScint().GetTimeUp()); // Time in XF - Time in E1
}

float TS800::GetXF_E1Raw_MESY(int i) const {
  std::vector<float> result;
  for(unsigned int x=0;x<mtof.fXfp.size();x++) {
    for(unsigned int y=0;y<mtof.fE1Up.size();y++) {
      result.push_back( mtof.fXfp.at(x) - mtof.fE1Up.at(y));
    }
  }
  if(result.size()>(unsigned int)i)
    return result.at(i);
  return sqrt(-1.0);
}

float TS800::GetXF_E1Raw_MESY_Ch15(int i) const {
  std::vector<float> result;
  for(unsigned int x=0;x<mtof.fXfp.size();x++) {
    for(unsigned int y=0;y<mtof.fRef.size();y++) {
      result.push_back( mtof.fXfp.at(x) - mtof.fRef.at(y));
    }
  }
  if(result.size()>(unsigned int)i)
    return result.at(i);
  return sqrt(-1.0);
}

float TS800::GetCorrTOF_OBJTAC() const {
  double afp_cor = GValue::Value("OBJTAC_TOF_CORR_AFP");
  double xfp_cor = GValue::Value("OBJTAC_TOF_CORR_XFP");
//std::cout << "TOF OBJTAC AFP COR" << afp_cor << std::endl;
//std::cout << "TOF OBJTAC xfp COR" << xfp_cor << std::endl;
  return GetTofE1_TAC(afp_cor,xfp_cor);
}
float TS800::GetCorrTOF_OBJ() const {
  double afp_cor = GValue::Value("OBJ_TOF_CORR_AFP");
  double xfp_cor = GValue::Value("OBJ_TOF_CORR_XFP");
  return GetTofE1_TDC(afp_cor,xfp_cor);
}

//std::vector<float> TS800::GetCorrTOF_OBJ_MESY() const {
float TS800::GetCorrTOF_OBJ_MESY(int i) const {
  //static double f_afp_cor = GValue::Value("OBJ_MTOF_CORR_AFP");
  //static double f_xfp_cor = GValue::Value("OBJ_MTOF_CORR_XFP");
  //if(fGlobalReset) {
    f_mafp_cor = GValue::Value("OBJ_MTOF_CORR_AFP");
    f_mxfp_cor = GValue::Value("OBJ_MTOF_CORR_XFP");
  //}
  return GetTofE1_MTDC(f_mafp_cor,f_mxfp_cor,i);
}

void TS800::DrawPID(Option_t *gate,Option_t *opt,Long_t nentries,TChain *chain) {
  TString OptString = opt;
  if(!chain)
    chain = gChain;
  if(!chain || !chain->GetBranch("TS800"))
    return;
  if(!gPad || !gPad->IsEditable()) {
    gROOT->MakeDefCanvas();
  } else {
    if(!OptString.Contains("Tune"))
      gPad->GetCanvas()->Clear();
  }

  std::string name = Form("%s_PID",Class()->GetName()); //_%s",opt);
  std::string title = Form("%s PID AFP=%.01f XFP=%.02f",Class()->GetName(),GValue::Value("OBJTAC_TOF_CORR_AFP"),GValue::Value("OBJTAC_TOF_CORR_XFP")); //_%s",opt);
  GH2I *h = (GH2I*)gROOT->FindObject(name.c_str());
  if(!h)
    h = new GH2I(name.c_str(),"GetIonChamber()->GetSum():GetCorrTOF_OBJTAC()",4096,0,4096,4000,0,4000);
  chain->Project(name.c_str(),"GetIonChamber()->GetSum():GetCorrTOF_OBJTAC()","","colz",nentries);
  h->GetXaxis()->SetTitle("Corrected TOF (objtac)");
  h->GetYaxis()->SetTitle("Ion Chamber Energy loss (arb. units)");
  h->Draw("colz");
}


void TS800::DrawAFP(Option_t *gate,Option_t *opt,Long_t nentries,TChain *chain) {
  TString OptString = opt;
  if(!chain)
    chain = gChain;
  if(!chain || !chain->GetBranch("TS800"))
    return;
  if(!gPad || !gPad->IsEditable()) {
    gROOT->MakeDefCanvas();
  } else {
    if(!OptString.Contains("Tune"))
      gPad->GetCanvas()->Clear();
  }

  std::string name = Form("%s_AFP",Class()->GetName()); //_%s",opt);
  std::string title = Form("%s AFP AFP=%.01f XFP=%.02f",Class()->GetName(),GValue::Value("OBJTAC_TOF_CORR_AFP"),GValue::Value("OBJTAC_TOF_CORR_XFP")); //_%s",opt);
  GH2I *h = (GH2I*)gROOT->FindObject(name.c_str());
  if(!h)
    h = new GH2I(name.c_str(),title.c_str(),2048,0,2048,4000,-0.1,0.1);
  chain->Project(name.c_str(),"GetAFP():GetCorrTOF_OBJTAC()","","colz",nentries);
  h->GetXaxis()->SetTitle("Corrected TOF (objtac)");
  h->GetYaxis()->SetTitle("Corrected AFP (objtac)");
  h->Draw("colz");
}


void TS800::DrawDispX(Option_t *gate,Option_t *opt,Long_t nentries,TChain *chain) {

  TString OptString = opt;

  if(!chain)
    chain = gChain;
  if(!chain || !chain->GetBranch("TS800"))
    return;
  if(!gPad || !gPad->IsEditable()) {
    gROOT->MakeDefCanvas();
  } else {
    if(!OptString.Contains("Tune"))
       gPad->GetCanvas()->Clear();
  }

  std::string name = Form("%s_DispX",Class()->GetName()); //_%s",opt);
  std::string title = Form("%s DispX AFP=%.01f XFP=%.02f",Class()->GetName(),GValue::Value("OBJTAC_TOF_CORR_AFP"),GValue::Value("OBJTAC_TOF_CORR_XFP")); //_%s",opt);
  GH2I *h = (GH2I*)gROOT->FindObject(name.c_str());
  if(!h)
    h = new GH2I(name.c_str(),title.c_str(),4096,-2047,2048,4000,-300,300);
  h->SetTitle(title.c_str());
  chain->Project(name.c_str(),"GetCrdc(0)->GetDispersiveX():GetCorrTOF_OBJTAC()","","colz",nentries);
  h->GetXaxis()->SetTitle("Corrected TOF (objtac)");
  h->GetYaxis()->SetTitle("Dispersive X (objtac)");
  h->Draw("colz");
}

void TS800::DrawPID_Tune(Long_t nentries,TChain *chain){
  if(!chain) chain=gChain;
  if(!chain || !chain->GetBranch("TS800")) return;
  if(!gPad || !gPad->IsEditable()){
    gROOT->MakeDefCanvas()->Divide(3,1);
  }
  else{
    gPad->GetCanvas()->Clear();
    gPad->GetCanvas()->Divide(3,1);
  }

  gPad->GetCanvas()->cd(1);
  DrawDispX("","Tune",nentries);

  gPad->GetCanvas()->cd(2);
  DrawAFP("","Tune",nentries);

  gPad->GetCanvas()->cd(3);
  DrawPID("","Tune",nentries);


}

void TS800::DrawPID_Mesy(Option_t *gate,Option_t *opt,Long_t nentries,int i,TChain *chain) {
  TString OptString = opt;
  if(!chain)
    chain = gChain;
  if(!chain || !chain->GetBranch("TS800"))
    return;
  if(!gPad || !gPad->IsEditable()) {
    gROOT->MakeDefCanvas();
  } else {
    if(!OptString.Contains("Tune"))
      gPad->GetCanvas()->Clear();
  }

  std::string name = Form("%s_PID",Class()->GetName()); //_%s",opt);
  std::string title = Form("%s PID AFP=%.01f XFP=%.02f",Class()->GetName(),GValue::Value("OBJ_MTOF_CORR_AFP"),GValue::Value("OBJ_MTOF_CORR_XFP")); //_%s",opt);
  GH2I *h = (GH2I*)gROOT->FindObject(name.c_str());
  if(!h)
    h = new GH2I(name.c_str(),"GetIonChamber()->GetSum():GetCorrTOF_OBJ_MESY()",4096,-4096,4096,4000,-4000,4000);
  chain->Project(name.c_str(),Form("GetIonChamber()->GetSum():GetCorrTOF_OBJ_MESY(%i)",i),"","colz",nentries);
  h->GetXaxis()->SetTitle("Corrected TOF (Mesy)");
  h->GetYaxis()->SetTitle("Ion Chamber Energy loss (arb. units)");
  h->Draw("colz");
}


void TS800::DrawAFP_Mesy(Option_t *gate,Option_t *opt,Long_t nentries,int i,TChain *chain) {
  TString OptString = opt;
  if(!chain)
    chain = gChain;
  if(!chain || !chain->GetBranch("TS800"))
    return;
  if(!gPad || !gPad->IsEditable()) {
    gROOT->MakeDefCanvas();
  } else {
    if(!OptString.Contains("Tune"))
      gPad->GetCanvas()->Clear();
  }

  std::string name = Form("%s_AFP",Class()->GetName()); //_%s",opt);
  std::string title = Form("%s AFP AFP=%.01f XFP=%.02f",Class()->GetName(),GValue::Value("OBJ_MTOF_CORR_AFP"),GValue::Value("OBJ_MTOF_CORR_XFP")); //_%s",opt);
  GH2I *h = (GH2I*)gROOT->FindObject(name.c_str());
  if(!h)
    h = new GH2I(name.c_str(),title.c_str(),2048,-4096,2048,4000,-0.1,0.1);
  chain->Project(name.c_str(),Form("GetAFP():GetCorrTOF_OBJ_MESY(%i)",i),"","colz",nentries);
  h->GetXaxis()->SetTitle("Corrected TOF (Mesy)");
  h->GetYaxis()->SetTitle("Corrected AFP (Mesy)");
  h->Draw("colz");
}


void TS800::DrawDispX_Mesy(Option_t *gate,Option_t *opt,Long_t nentries,int i,TChain *chain) {

  TString OptString = opt;

  if(!chain)
    chain = gChain;
  if(!chain || !chain->GetBranch("TS800"))
    return;
  if(!gPad || !gPad->IsEditable()) {
    gROOT->MakeDefCanvas();
  } else {
    if(!OptString.Contains("Tune"))
       gPad->GetCanvas()->Clear();
  }

  std::string name = Form("%s_DispX",Class()->GetName()); //_%s",opt);
  std::string title = Form("%s DispX AFP=%.01f XFP=%.02f",Class()->GetName(),GValue::Value("OBJ_MTOF_CORR_AFP"),GValue::Value("OBJ_MTOF_CORR_XFP")); //_%s",opt);
  GH2I *h = (GH2I*)gROOT->FindObject(name.c_str());
  if(!h)
    h = new GH2I(name.c_str(),title.c_str(),4096,-4096,4096,4000,-300,300);
  h->SetTitle(title.c_str());
  chain->Project(name.c_str(),Form("GetCrdc(0)->GetDispersiveX():GetCorrTOF_OBJ_MESY(%i)",i),"","colz",nentries);
  h->GetXaxis()->SetTitle("Corrected TOF (Mesy)");
  h->GetYaxis()->SetTitle("Dispersive X (Mesy)");
  h->Draw("colz");
}

void TS800::DrawPID_Mesy_Tune(Long_t nentries,int i,TChain *chain){
  if(!chain) chain=gChain;
  if(!chain || !chain->GetBranch("TS800")) return;
  if(!gPad || !gPad->IsEditable()){
    gROOT->MakeDefCanvas()->Divide(3,1);
  }
  else{
    gPad->GetCanvas()->Clear();
    gPad->GetCanvas()->Divide(3,1);
  }

  gPad->GetCanvas()->cd(1);
  DrawDispX_Mesy("","Tune",nentries,i);

  gPad->GetCanvas()->cd(2);
  DrawAFP_Mesy("","Tune",nentries,i);

  gPad->GetCanvas()->cd(3);
  DrawPID_Mesy("","Tune",nentries,i);


}


double TS800::GetMTofObjE1() const {
  // I return the correlated gvalue corrected time-of-flight obj to e1.
  double afp_cor = GValue::Value("OBJ_MTOF_CORR_AFP");
  double xfp_cor = GValue::Value("OBJ_MTOF_CORR_XFP");
  if(std::isnan(afp_cor) || std::isnan(xfp_cor)) {
    printf(ALERTTEXT "Attmepting to do mtof obj correction without values!" RESET_COLOR "\n");
    fflush(stdout);
    return sqrt(-1);
  }
  return GetMTofObjE1(afp_cor, xfp_cor);
}

double TS800::GetMTofObjE1(double afp_cor, double xfp_cor) const {
  // I return the correlated gvalue corrected time-of-flight obj to e1.
  return(GetMTof().GetCorrelatedObjE1()
         + afp_cor * GetAFP() + xfp_cor  * GetCrdc(0).GetDispersiveX());
}


double TS800::GetMTofXfpE1() const {
  // I return the correlated gvalue corrected time-of-flight xfp to e1.
  double afp_cor = GValue::Value("XFP_MTOF_CORR_AFP");
  double xfp_cor = GValue::Value("XFP_MTOF_CORR_XFP");
  if(std::isnan(afp_cor) || std::isnan(xfp_cor)) {
    printf(ALERTTEXT "Attmepting to do mtof xfp correction without values!" RESET_COLOR "\n");
    fflush(stdout);
    return sqrt(-1);
  }
  return GetMTofXfpE1(afp_cor, xfp_cor);
}

double TS800::GetMTofXfpE1(double afp_cor, double xfp_cor) const {
  // I return the correlated gvalue corrected time-of-flight xfp to e1.
  return(GetMTof().GetCorrelatedXfpE1()
         + afp_cor * GetAFP() + xfp_cor  * GetCrdc(0).GetDispersiveX());
}



/*
float TS800::GetMTOF_ObjE1(unsigned int i,bool find_best) const { 
  if(!find_best) 
    return GetCorrTOF_OBJ_MESY(i); 
  double target = GValue::Value("MTOF_ObjE1");
  if(std::isnan(target)) {
    target = GValue::Value("TARGET_MTOF_ObjE1");
    if(std::isnan(target)) {
      return GetCorrTOF_OBJ_MESY(i); 
    }
  }
  double value = GetMTOF_ObjE1(0,0);
  for(int i=1;i<mtof.fObj.size();i++) {    
    double newvalue = GetMTOF_ObjE1(i,0);
    if(std::abs(target - newvalue) < std::abs(target - value)) {
      value = newvalue;
    }
  }
  return value;
}


float TS800::GetMTOF_XfpE1(unsigned int i,bool find_best) const { 
  if(!find_best) 
    return GetRawXF_MESY(i); 
  double target = GValue::Value("MTOF_XfpE1");
  if(std::isnan(target)) {
    target = GValue::Value("TARGET_MTOF_XfpE1");
    //std::cout << target << std::endl;
    if(std::isnan(target)) {
      return GetRawXF_MESY(i);
    }
  }
  double value = GetMTOF_XfpE1(0,0);
  for(int i=1;i<mtof.fXfp.size();i++) {    
    double newvalue = GetMTOF_XfpE1(i,0);
    if(std::abs(target - newvalue) < std::abs(target - value)) {
      value = newvalue;
    }
  }
  return value;
}
*/
