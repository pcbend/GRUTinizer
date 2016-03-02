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


std::vector<short> TS800::fmaxcoefficient;                      
std::vector<std::vector<short> > TS800::forder;                 
std::vector<std::vector<std::vector<short> > > TS800::fexponent;
std::vector<std::vector<float> > TS800::fcoefficient;           
short TS800::fmaxorder;                                         
float TS800::fbrho;                                             
int TS800::fmass;                                               
int TS800::fcharge;                                             

TS800::TS800() {
  Clear();
}

void TS800::ReadInverseMap(const char *mapfile) {
  static std::atomic_bool InvMapFileRead(false);
  std::string filename = mapfile; // TGRUTOptions::Get()->S800InverseMapFile();
  if(!InvMapFileRead){
    //   Quick little mutex to make sure that multiple threads
    // don't try to read the map file at the same time.
    static std::mutex inv_map_mutex;
    std::lock_guard<std::mutex> lock(inv_map_mutex);
    if(!InvMapFileRead){
      InvMapFileRead=ReadMap_SpecTCL(filename);
      std::cout << " SPECTCL INV MAP LOADED!!!" << std::endl;
    }
  }
}


TS800::~TS800(){
}

void TS800::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TS800& other = (TS800&)obj;
  other.fAtaTCL1 = fAtaTCL1;
  other.fYtaTCL1 = fYtaTCL1;
  other.fBtaTCL1 = fBtaTCL1;
  other.fDtaTCL1 = fDtaTCL1;
  other.fAtaTCL2 = fAtaTCL2;
  other.fYtaTCL2 = fYtaTCL2;
  other.fBtaTCL2 = fBtaTCL2;
  other.fDtaTCL2 = fDtaTCL2;
  other.fAtaTCL3 = fAtaTCL3;
  other.fYtaTCL3 = fYtaTCL3;
  other.fBtaTCL3 = fBtaTCL3;
  other.fDtaTCL3 = fDtaTCL3;
  other.fAtaTCL4 = fAtaTCL4;
  other.fYtaTCL4 = fYtaTCL4;
  other.fBtaTCL4 = fBtaTCL4;
  other.fDtaTCL4 = fDtaTCL4;
  other.fAtaTCL5 = fAtaTCL5;
  other.fYtaTCL5 = fYtaTCL5;
  other.fBtaTCL5 = fBtaTCL5;
  other.fDtaTCL5 = fDtaTCL5;
  other.fAtaTCL6 = fAtaTCL6;
  other.fYtaTCL6 = fYtaTCL6;
  other.fBtaTCL6 = fBtaTCL6;
  other.fDtaTCL6 = fDtaTCL6;

  for(int i = 0; i<3; i++){
    scint[i].Copy(other.scint[i]);
  }
  trigger.Copy(other.trigger);
  tof.Copy(other.tof);
  mtof.Copy(other.mtof);
  ion.Copy(other.ion);
  for(int i=0; i<2; i++){
    crdc[i].Copy(other.crdc[i]);
  }
}

float TS800::MapCalc_SpecTCL(int calcorder,int parameter,float *input){
  float cumul=0;
  float multiplicator;
  for(int index=0; index<fmaxcoefficient[parameter]; index++){
    if (calcorder < forder[parameter][index]) break;
    multiplicator = 1;
    for(int nex=0; nex<6; nex++){
      if(fexponent[parameter][nex][index] != 0){
	multiplicator *= pow(input[nex], fexponent[parameter][nex][index]);
      }
    }
    cumul += multiplicator * fcoefficient[parameter][index];
  }
  
  return cumul;
}

Float_t TS800::GetAta_Spec(int i){
  float Shift_ata = 0;
  if(GValue::FindValue("ATA_SHIFT"))
    Shift_ata = GValue::FindValue("ATA_SHIFT")->GetValue();
  
  switch(i){
  case 1: return (fAtaTCL1+Shift_ata);
    break;
  case 2: return (fAtaTCL2+Shift_ata);
    break;
  case 3: return (fAtaTCL3+Shift_ata);
    break;
  case 4: return (fAtaTCL4+Shift_ata);
    break;
  case 5: return (fAtaTCL5+Shift_ata);
    break;
  default: return (fAtaTCL6+Shift_ata);
    break;
  }
}

Float_t TS800::GetBta_Spec(int i){
  float Shift_bta = 0;
  if(GValue::FindValue("BTA_SHIFT"))
    Shift_bta = GValue::FindValue("BTA_SHIFT")->GetValue();
  
  switch(i){
  case 1: return (fBtaTCL1+Shift_bta);
    break;
  case 2: return (fBtaTCL2+Shift_bta);
    break;
  case 3: return (fBtaTCL3+Shift_bta);
    break;
  case 4: return (fBtaTCL4+Shift_bta);
    break;
  case 5: return (fBtaTCL5+Shift_bta);
    break;
  default: return (fBtaTCL6+Shift_bta);
    break;
  }
}

Float_t TS800::GetYta_Spec(int i){
  float Shift_yta = 0;
  if(GValue::FindValue("YTA_SHIFT"))
    Shift_yta = GValue::FindValue("YTA_SHIFT")->GetValue();
  
  switch(i){
  case 1: return (fYtaTCL1+Shift_yta);
    break;
  case 2: return (fYtaTCL2+Shift_yta);
    break;
  case 3: return (fYtaTCL3+Shift_yta);
    break;
  case 4: return (fYtaTCL4+Shift_yta);
    break;
  case 5: return (fYtaTCL5+Shift_yta);
    break;
  default: return (fYtaTCL6+Shift_yta);
    break;
  }
}

Float_t TS800::GetDta_Spec(int i){
  float Shift_dta = 0;
  if(GValue::FindValue("DTA_SHIFT"))
    Shift_dta = GValue::FindValue("DTA_SHIFT")->GetValue();
  
  switch(i){
  case 1: return (fDtaTCL1+Shift_dta);
    break;
  case 2: return (fDtaTCL2+Shift_dta);
    break;
  case 3: return (fDtaTCL3+Shift_dta);
    break;
  case 4: return (fDtaTCL4+Shift_dta);
    break;
  case 5: return (fDtaTCL5+Shift_dta);
    break;
  default: return (fDtaTCL6+Shift_dta);
    break;
  }
}

bool TS800::ReadMap_SpecTCL(std::string filename){
  //std::string filename = TGRUTOptions::Get()->S800InverseMapFile();
  fmaxcoefficient.resize(6);
  forder.resize(6);
  fexponent.resize(6);
  fcoefficient.resize(6);
  for(short i=0;i<6;i++){
    forder[i].resize(200);
    fcoefficient[i].resize(200);
    fexponent[i].resize(6);
    for(short k=0;k<6;k++){
      fexponent[i][k].resize(200);
    }
  }
  fmaxorder = 0;
  FILE* file;
  char line[80];
  int index, par, exp[6];
  int ord;
  float co;
  char title[120];
  char *ret=NULL;
  file = fopen(filename.c_str(), "r");
  if(file == NULL){
    std::cout << "Sorry I couldn't find the map file: " << filename << std::endl;
    std::cout << "Will continue without the map file" << std::endl;
    return false;
  }
  ret = fgets(title, 120, file);
  sscanf(title, "S800 inverse map - Brho=%g - M=%d - Q=%d", &fbrho, &fmass, &fcharge);
  //if(fSett->VLevel()>0)
  //std::cout << "brho " << fbrho << " mass " << fmass << " charge " << fcharge << std::endl;
  while(strstr(line, "COEFFICIENT") == NULL)
    ret = fgets(line, 80, file);
  par = 0;
  while(!feof(file)){
    ret = fgets(line, 80, file);
    while (strstr(line, "------------------") == NULL){
      sscanf(line, "%d %g %d %d %d %d %d %d %d", &index, &co, &ord, &exp[0], &exp[1], &exp[2], &exp[3], &exp[4], &exp[5]);
      if(index > 200){
	std::cout << "Too many coefficients in map.  Increase TS800_TRACK_COEFFICIENTS." << std::endl;
	break;
      }
      if(par > 6){
	std::cout << "Too many parameters in map.  Increase TS800_TRACK_PARAMETERS." << std::endl;
	break;
      }
      fmaxcoefficient[par] = index;
      forder[par][index-1] = ord;
      fcoefficient[par][index-1] = co;
      //std::cout << "max coef " << fmaxcoefficient[par] << " order " << forder[par][index-1] << " coef " << fcoefficient[par][index-1] << std::endl;
      for(int k=0; k<6; k++){
	fexponent[par][k][index-1] = exp[k];
	//std::cout << "exp["<<k<<"] " << exp[k] << " fexponent["<<par<<"]["<<k<<"]["<<index-1<<"] " << fexponent[par][k][index-1] << std::endl;
      }
      ret = fgets(line, 80, file);
    }
    if(ord > fmaxorder)
      fmaxorder = ord;
    par++;
  }
  if(ret){}
  //std::cout << "Done reading map from " << filename << "." << std::endl;
  //std::cout << "Title: " << title << std::endl;
  //std::cout << "Order: " << fmaxorder << std::endl;
  fclose(file);
  return true;
}

TVector3 TS800::ExitTargetVect_Spec(int order){
  TVector3 track;
  double xsin = 0;
  double ysin = 0;
  xsin = GetAta_Spec(order);
  ysin = GetBta_Spec(order);
  double phi   = 0;
  double theta = 0;

  xsin = TMath::Sin(xsin);
  ysin = TMath::Sin(ysin);
  
  if(xsin>0 && ysin>0)      phi = 2.0*TMath::Pi()-TMath::ATan(ysin/xsin);
  else if(xsin<0 && ysin>0) phi = TMath::Pi()+TMath::ATan(ysin/TMath::Abs(xsin));
  else if(xsin<0 && ysin<0) phi = TMath::Pi()-TMath::ATan(TMath::Abs(ysin)/TMath::Abs(xsin));
  else if(xsin>0 && ysin<0) phi = TMath::ATan(TMath::Abs(ysin)/xsin);
  else                      phi = 0;

  theta = TMath::ASin(TMath::Sqrt(xsin*xsin+ysin*ysin));
  track.SetPtThetaPhi(1,theta,phi);
  return track;
}

TVector3 TS800::CRDCTrack(){
  /*TVector3 crdc1,crdc2;

  crdc1.SetXYZ(crdc[0].GetDispersiveX(),crdc[0].GetNonDispersiveY(),0);
  crdc2.SetXYZ(crdc[1].GetDispersiveX(),crdc[1].GetNonDispersiveY(),1000);

  TVector3 track = crdc2-crdc1;*/
  TVector3 track;
  track.SetTheta(TMath::ATan((GetCrdc(0).GetDispersiveX()-GetCrdc(1).GetDispersiveX())/1073.0)); // rad
  track.SetPhi(TMath::ATan((GetCrdc(0).GetNonDispersiveY()-GetCrdc(1).GetNonDispersiveY())/1073.0)); // rad
  //track.SetPt(1) // need to do this.

  return track;
}

float TS800::GetAFP() const{
  if (GetCrdc(0).GetId() == -1 || GetCrdc(1).GetId() == -1){
    return sqrt(-1);
  }
  float AFP = TMath::ATan((GetCrdc(1).GetDispersiveX()-GetCrdc(0).GetDispersiveX())/1073.0);
  return AFP;

}

float TS800::GetBFP() const{
  if (GetCrdc(0).GetId() == -1 || GetCrdc(1).GetId() == -1){
    return sqrt(-1);
  }
  float BFP = TMath::ATan((GetCrdc(1).GetNonDispersiveY()-GetCrdc(0).GetNonDispersiveY())/1073.0);
  return BFP;
}

void TS800::Clear(Option_t* opt){
  TDetector::Clear(opt);
  crdc[0].Clear();
  crdc[1].Clear();

  scint[0].Clear();
  scint[1].Clear();
  scint[2].Clear();

  tof.Clear();
  mtof.Clear();
  trigger.Clear();
  ion.Clear();

  fAtaTCL1 = sqrt(-1);
  fYtaTCL1 = sqrt(-1);
  fBtaTCL1 = sqrt(-1);
  fDtaTCL1 = sqrt(-1);
  fAtaTCL2 = sqrt(-1);
  fYtaTCL2 = sqrt(-1);
  fBtaTCL2 = sqrt(-1);
  fDtaTCL2 = sqrt(-1);
  fAtaTCL3 = sqrt(-1);
  fYtaTCL3 = sqrt(-1);
  fBtaTCL3 = sqrt(-1);
  fDtaTCL3 = sqrt(-1);
  fAtaTCL4 = sqrt(-1);
  fYtaTCL4 = sqrt(-1);
  fBtaTCL4 = sqrt(-1);
  fDtaTCL4 = sqrt(-1);
  fAtaTCL5 = sqrt(-1);
  fYtaTCL5 = sqrt(-1);
  fBtaTCL5 = sqrt(-1);
  fDtaTCL5 = sqrt(-1);
  fAtaTCL6 = sqrt(-1);
  fYtaTCL6 = sqrt(-1);
  fBtaTCL6 = sqrt(-1);
  fDtaTCL6 = sqrt(-1);

}

int TS800::BuildHits(){
  if(raw_data.size() != 1){
    std::cout << "Data buffers: " << raw_data.size() << std::endl;
  }
  for(auto& event : raw_data) { // should only be one..
    SetTimestamp(event.GetTimestamp());
    // TGEBEvent* geb = (TGEBEvent*)&event;
    // SetTimestamp(geb->GetTimestamp());
    int ptr = 0;
    //    const TRawEvent::GEBS800Header *head = ((const TRawEvent::GEBS800Header*)geb->GetPayload());
    const TRawEvent::GEBS800Header *head = ((const TRawEvent::GEBS800Header*)event.GetPayload());
    ptr += sizeof(TRawEvent::GEBS800Header);
      //  Here, we are now pointing at the size of the next S800 thing.  Inclusive in shorts.
      //  std::string buffer = Form("all0x%04x",*((unsigned short*)(geb->GetPayload()+ptr+2)));
    //printf("head.total_size == %i\n",head->total_size); fflush(stdout);
    //printf("sizeof(TRawEvent::GEBS800Header) == %i \n",sizeof(TRawEvent::GEBS800Header));
    unsigned short *data = (unsigned short*)(event.GetPayload()+ptr);
    //printf("\t0x%04x\t0x%04x\t0x%04x\t0x%04x\n",*data,*(data+1),*(data+2),*(data+3));
    //while(ptr<((head->total_size*2)-2)) {
    std::string toprint = "all";
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
      //unsigned short size_in_bytes = (*((unsigned short*)(geb->GetPayload()+ptr))*2);
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
  //Raytracing(
  float input[6];
  input[0] = -GetCrdc(0).GetDispersiveX()/1000.0; // divide to get to units of meters
  input[1] = -GetAFP(); // this is in radians.
  input[2] = GetCrdc(0).GetNonDispersiveY()/1000.0;
  input[3] = GetBFP();
  //input[2] = GetCrdc(0).GetNonDispersiveY()/1000.0 - 0.033;
  //input[3] = GetBFP() + 0.034;
  input[4] = 0;
  input[5] = 0;

  //printf("-----------------------  --------------------\n");
  //std::cout << " BFP : " << input[3] << "  " << GetBFP() << std::endl;
  
  fAtaTCL1 = MapCalc_SpecTCL(1,0,input);
  fYtaTCL1 = MapCalc_SpecTCL(1,1,input);
  fBtaTCL1 = MapCalc_SpecTCL(1,2,input);
  fDtaTCL1 = MapCalc_SpecTCL(1,3,input);

  fAtaTCL2 = MapCalc_SpecTCL(2,0,input);
  fYtaTCL2 = MapCalc_SpecTCL(2,1,input);
  fBtaTCL2 = MapCalc_SpecTCL(2,2,input);
  fDtaTCL2 = MapCalc_SpecTCL(2,3,input);

  fAtaTCL3 = MapCalc_SpecTCL(3,0,input);
  fYtaTCL3 = MapCalc_SpecTCL(3,1,input);
  fBtaTCL3 = MapCalc_SpecTCL(3,2,input);
  fDtaTCL3 = MapCalc_SpecTCL(3,3,input);

  fAtaTCL4 = MapCalc_SpecTCL(4,0,input);
  fYtaTCL4 = MapCalc_SpecTCL(4,1,input);
  fBtaTCL4 = MapCalc_SpecTCL(4,2,input);
  fDtaTCL4 = MapCalc_SpecTCL(4,3,input);

  fAtaTCL5 = MapCalc_SpecTCL(5,0,input);
  fYtaTCL5 = MapCalc_SpecTCL(5,1,input);
  fBtaTCL5 = MapCalc_SpecTCL(5,2,input);
  fDtaTCL5 = MapCalc_SpecTCL(5,3,input);

  fAtaTCL6 = MapCalc_SpecTCL(6,0,input);
  fYtaTCL6 = MapCalc_SpecTCL(6,1,input);
  fBtaTCL6 = MapCalc_SpecTCL(6,2,input);
  fDtaTCL6 = MapCalc_SpecTCL(6,3,input);

  //printf("-----------------------\n");
  return 0;
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
  if((*data)<3)
    current_crdc = &crdc[*data];
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
  //for(;x<subsize;x+=2) {
  while(x<subsize){
    unsigned short word1 = *(data+x); x++;
    //std::cout << std::hex << " word 1 " << word1 << std::endl;
    if((word1&0x8000)!=0x8000) { continue; }
    unsigned short word2 = *(data+x);
    //std::cout << std::hex << " word 2 " << word2 << std::endl;


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

/*
bool TS800::HandleHODOPacket(char *data,unsigned short size) {
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

/*
bool TS800::HandleTOFPacket(char *data,unsigned short size) {
  //some number of times read from both a Phillips 7186H TDC and an Ortec 566 TAC digitized by a Phillips 7164 ADC.
  if(size<2)
    return false;
  for(int i=0;i<size;i+=2) {
    TTOFHit *tof = (TTOFHit*)time_of_flight->ConstructedAt(time_of_flight->GetEntries());
    tof->Set(*((unsigned short*)(data+i)));
  }
  return true;
}

bool TS800::HandleFPScintPacket(char *data,unsigned short size) {
  //Energies and times of the focal plane scintillator, read from a Lecroy 4300B FERA and a Phillips 7186H TDC, should come in pairs.
  //printf("in handle crdcc.\n");
  for(int i=0;i<size;i+=4) {
    unsigned short *temp = ((unsigned short*)(data+i));
    if(((*(temp+1))&0xf000) != (*temp)&0xf000) {
      fprintf(stderr,"fp_scint: energy and time do not match.");
      return false;
    }
    TFPScint *hit = (TFPScint*)fp_scint->ConstructedAt(fp_scint->GetEntries());
    hit->SetCharge(*temp);
    hit->SetCharge(*(temp+1));
  }
  return true;
}

bool TS800::HandleIonChamberPacket(char *data,unsigned short size) {
  //Zero suppressed energies from the ionization chamber, read from a Phillips 7164 ADC.
  //Note, this data is in a "sub-packet".
  int x =0;
  if(*(data+2) == 0x5821)
    x+=4;
  for(;x<size;x+=2) {
    unsigned short *temp = ((unsigned short*)(data+x));
    TIonChamber *ion = (TIonChamber*)ion_chamber->ConstructedAt(ion_chamber->GetEntries());
    ion->Set(*temp);
  }
  return true;

}

bool TS800::HandleCRDCPacket(char *data,unsigned short size) {
  int ptr = 0;
  short id      = *((short*)data); ptr += 2;
  short subsize = *((short*)(data+ptr)); ptr += 2;
  short subtype = *((short*)(data+ptr)); ptr += 2;
  short zerobuffer = *((short*)(data+ptr)); ptr += 2;
  int lastsampe = 0;

  //for(int x=0;x<crdc1->GetSize();x++)
  //  ((TCrdcPad*)crdc1->At(x))->Clear();
  //for(int x=0;x<crdc2->GetSize();x++)
  //  ((TCrdcPad*)crdc2->At(x))->Clear();

  std::map<int,std::map<int,int> > pad;

  for(int x=1;x<subsize;x+=2) {
    unsigned short word1 = *((unsigned short*)(data+ptr)); ptr += 2;
    if((word1&0x8000)!=0x8000) { continue; }
    unsigned short word2 = *((unsigned short*)(data+ptr)); ptr += 2;

    //printf("[%i]\t0x%04x\t0x%04x\n",x,word1,word2);
    int sample_number    = (word1&(0x7fc0)) >> 6;
    int channel_number   =  word1&(0x003f);
    int connector_number = (word2&(0x0c00)) >> 10;
    int databits         = (word2&(0x03ff));

    int real_channel = (connector_number << 6) + channel_number;
    pad[real_channel][sample_number] = databits;
  }

  std::map<int,std::map<int,int> >::iterator it1;
  std::map<int,int>::iterator it2;

  for(it1=pad.begin();it1!=pad.end();it1++) {
    //printf("channel[%03i]\n",it1->first);
    TCrdcPad *crdcpad = 0;
    if(id==0) {
      crdcpad = (TCrdcPad*)crdc1->ConstructedAt(crdc1->GetEntries());
    } else if(id==1) {
      crdcpad = (TCrdcPad*)crdc2->ConstructedAt(crdc2->GetEntries());
    }
    if(crdcpad) {
      crdcpad->SetChannel(it1->first);
      for(it2=it1->second.begin();it2!=it1->second.end();it2++) {
        //printf("\t%i\t%i\n",it2->first,it2->second);
        crdcpad->SetPoint(it2->first,it2->second);
      }
    } else {
      return false;
    }
  }

  unsigned short word1 = *((unsigned short*)(data+ptr)); ptr += 2;
  unsigned short word2 = *((unsigned short*)(data+ptr)); ptr += 2;
  if(id==0) {
    crdc1_charge = word1;
    crdc1_time   = word2;
  } else if(id==1) {
    crdc2_charge = word1;
    crdc2_time   = word2;
  } else {
    return false;
  }
  //printf("after crdc: %i | %i     0x%04x\t0x%04x\t\t",crdc1->GetEntries(),crdc2->GetEntries(),word1,word2);
  //std::cout << " ++++++++++++++++++++++++++++++++++++\n";

  return true;
}

bool TS800::HandleIntermediatePPACPacket(char *data,unsigned short size) {
  int ptr = 0;
  short id      = *((short*)data); ptr += 2;
  short subsize = *((short*)(data+ptr)); ptr += 2;
  short subtype = *((short*)(data+ptr)); ptr += 2;
  short zerobuffer = *((short*)(data+ptr)); ptr += 2;
  int lastsampe = 0;

  //std::map<int,int> ChargeMap;

  std::map<int,std::map<int,int> > pad;

  for(int x=1;x<subsize;x+=2) {
    unsigned short word1 = *((unsigned short*)(data+ptr)); ptr += 2;
    if((word1&0x8000)!=0x8000) { continue; }
    unsigned short word2 = *((unsigned short*)(data+ptr)); ptr += 2;
    int sample_number    = (word1&(0x7fc0)) >> 6;
    int channel_number   =  word1&(0x003f);
    int connector_number = (word2&(0x0c00)) >> 10;
    int databits         = (word2&(0x03ff));

    int real_channel = (connector_number << 6) + channel_number;
    pad[real_channel][sample_number] = databits;
  }

  std::map<int,std::map<int,int> >::iterator it1;
  std::map<int,int>::iterator it2;

  for(it1=pad.begin();it1!=pad.end();it1++) {
    //printf("channel[%03i]\n",it1->first);
    //printf("\t%i\t%i\n",it2->first,it2->second);
    TCrdcPad *ppac = 0;
    if(id==0) {
      ppac = (TCrdcPad*)ppac1->ConstructedAt(crdc1->GetEntries());
    } else if(id==1) {
      ppac = (TCrdcPad*)ppac2->ConstructedAt(crdc2->GetEntries());
    }
    if(ppac) {
      ppac->SetChannel(it1->first);
      for(it2=it1->second.begin();it2!=it1->second.end();it2++)
        ppac->SetPoint(it2->first,it2->second);
    } else {
      return false;
    }
  }
  unsigned short word1 = *((unsigned short*)(data+ptr)); ptr += 2;
  unsigned short word2 = *((unsigned short*)(data+ptr)); ptr += 2;
  if(id==0) {
    ppac1_charge = word1;
    ppac1_time   = word2;
  } else if(id==1) {
    ppac2_charge = word1;
    ppac2_time   = word2;
  } else {
    return false;
  }
  //printf("after crdc: %i | %i     0x%04x\t0x%04x\t\t",crdc1->GetEntries(),crdc2->GetEntries(),word1,word2);
  //std::cout << " ++++++++++++++++++++++++++++++++++++\n";

  return true;
}
*/

void TS800::InsertHit(const TDetectorHit& hit){
  return;
}

TDetectorHit& TS800::GetHit(int i){
  TDetectorHit *hit = new TS800Hit;
  return *hit;
}

float TS800::GetTofE1_TAC(float c1,float c2)  const {
  if (GetCrdc(0).GetId() == -1) {
    return sqrt(-1);
  }
  return GetTof().GetTacOBJ() + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX();
}


float TS800::GetTofE1_TDC(float c1,float c2)  const {
  if (GetCrdc(0).GetId() == -1) {
    return sqrt(-1);
  }

  return GetTof().GetOBJ() - GetScint().GetTimeUp() + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX();
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

float TS800::GetRawOBJ_MESY(int i) const {
  return (mtof.fObj.at(i));
}

float TS800::GetRawE1_MESY(int i) const {
  return (mtof.fE1Up.at(i));
}

float TS800::GetRawXF_MESY(int i) const {
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

float TS800::MCorrelatedOBJ() const{
  if(mtof.fCorrelatedOBJ>-1) return mtof.fObj.at(mtof.fCorrelatedOBJ);
  else return 0;
}

float TS800::MCorrelatedXFP() const{
  if(mtof.fCorrelatedXFP>-1) return mtof.fXfp.at(mtof.fCorrelatedXFP);
  else return 0;
}

float TS800::MCorrelatedE1() const{
  if(mtof.fCorrelatedE1>-1) return mtof.fE1Up.at(mtof.fCorrelatedE1);
  else return 0;
}

float TS800::MCorrelatedOBJ_E1(bool corrected) const{
  if(mtof.fCorrelatedOBJ>-1 && mtof.fCorrelatedE1>-1)
    return (mtof.fObj.at(mtof.fCorrelatedOBJ)-mtof.fE1Up.at(mtof.fCorrelatedE1));
  else if(mtof.fCorrelatedE1>-1){
    double OBJLow  = GValue::Value("MOBJ_CORR_LOW");
    double OBJHigh = GValue::Value("MOBJ_CORR_HIGH");
    
    double afp_cor = GValue::Value("OBJTAC_TOF_CORR_AFP");
    double xfp_cor = GValue::Value("OBJTAC_TOF_CORR_XFP");
    
    if(corrected==false){
      afp_cor = 0;
      xfp_cor = 0;
    }

    if(std::isnan(afp_cor)){
      std::cout << " afp cor = nan" << std::endl;
      return 0;
    }
    if(std::isnan(xfp_cor)){
      std::cout << " xfp cor = nan" << std::endl;
      return 0;
    }
    if(std::isnan(OBJLow)){
      std::cout << " OBJ Low = nan" << std::endl;
      return 0;
    }
    if(std::isnan(OBJHigh)){
      std::cout << " OBJ Hig = nan" << std::endl;
      std::cout << " ELSE IF " << std::endl;
      return 0;
    }

    std::vector<float> val2;
    float val;
    for(unsigned int y=0;y<mtof.fObj.size();y++) {
      val = (mtof.fObj.at(y) - mtof.fE1Up.at(mtof.fCorrelatedE1) + afp_cor * GetAFP() + xfp_cor  * GetCrdc(0).GetDispersiveX()) ;
      if(val<OBJHigh && val>OBJLow){
	val2.push_back(val);      
	mtof.fCorrelatedOBJ=y;
      }
    }
    if(val2.size()==1)
      return val2.at(0);
    mtof.fCorrelatedOBJ =-1;
  }
  else{
    double OBJLow  = GValue::Value("MOBJ_CORR_LOW");
    double OBJHigh = GValue::Value("MOBJ_CORR_HIGH");

    double afp_cor = GValue::Value("OBJTAC_TOF_CORR_AFP");
    double xfp_cor = GValue::Value("OBJTAC_TOF_CORR_XFP");
    
    if(corrected==false){
      afp_cor = 0;
      xfp_cor = 0;
    }

    if(std::isnan(afp_cor)){
      std::cout << " afp cor = nan" << std::endl;
      return 0;
    }
    if(std::isnan(xfp_cor)){
      std::cout << " xfp cor = nan" << std::endl;
      return 0;
    }
    if(std::isnan(OBJLow)){
      std::cout << " OBJ Low = nan" << std::endl;
      return 0;
    }
    if(std::isnan(OBJHigh)){
      std::cout << " OBJ Hig = nan" << std::endl;
      std::cout << " ELSE " << std::endl;
      return 0;
    }

    /*if(std::isnan(afp_cor))  return 0;
    if(std::isnan(xfp_cor))  return 0;
    if(std::isnan(OBJLow))   return 0;
    if(std::isnan(OBJHigh))  return 0;
    */
    std::vector<float> val2;
    float val;
    for(unsigned int x=0;x<mtof.fE1Up.size();x++) {
      for(unsigned int y=0;y<mtof.fObj.size();y++) {
	
	val = mtof.fObj.at(y) - mtof.fE1Up.at(x);
	if(val<OBJHigh && val>OBJLow){
	  val2.push_back(val);      
	  mtof.fCorrelatedOBJ=y;
	  mtof.fCorrelatedE1=x;
	}
      }
    }
    
    if(val2.size()==1)
      return val2.at(0);
    mtof.fCorrelatedOBJ =-1;
    mtof.fCorrelatedE1  =-1;
  }

  return 0;
}

float TS800::MCorrelatedXFP_E1() const{
  if(mtof.fCorrelatedXFP>-1 && mtof.fCorrelatedE1>-1)
    return (mtof.fXfp.at(mtof.fCorrelatedXFP)-mtof.fE1Up.at(mtof.fCorrelatedE1));
  else if(mtof.fCorrelatedE1>-1){
    double XFLow = GValue::Value("MXF_CORR_LOW");
    double XFHigh = GValue::Value("MXF_CORR_HIGH");
    

    if(std::isnan(XFLow))   return 0;
    if(std::isnan(XFHigh))  return 0;

    std::vector<float> val2;
    float val;
    for(unsigned int y=0;y<mtof.fXfp.size();y++) {
      val = mtof.fXfp.at(y) - mtof.fE1Up.at(mtof.fCorrelatedE1);
      if(val<XFHigh && val>XFLow){
	val2.push_back(val);      
	mtof.fCorrelatedXFP=y;
      }
    }
    if(val2.size()==1)
      return val2.at(0);
    mtof.fCorrelatedXFP =-1;
  }
  else{
    double XFLow = GValue::Value("MXF_CORR_LOW");
    double XFHigh = GValue::Value("MXF_CORR_HIGH");

    if(std::isnan(XFLow))   return 0;
    if(std::isnan(XFHigh))  return 0;

    std::vector<float> val2;
    float val;
    for(unsigned int x=0;x<mtof.fE1Up.size();x++) {
      for(unsigned int y=0;y<mtof.fXfp.size();y++) {
	val = mtof.fXfp.at(y) - mtof.fE1Up.at(x);
	if(val<XFHigh && val>XFLow){
	  val2.push_back(val);      
	  mtof.fCorrelatedXFP=y;
	  mtof.fCorrelatedE1=x;
	}
      }
    }
    if(val2.size()==1)
      return val2.at(0);
    mtof.fCorrelatedXFP =-1;
    mtof.fCorrelatedE1  =-1;
  }

  return 0;
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
  double afp_cor = GValue::Value("OBJ_MTOF_CORR_AFP");
  double xfp_cor = GValue::Value("OBJ_MTOF_CORR_XFP");
  return GetTofE1_MTDC(afp_cor,xfp_cor,i);
}

//float TS800::GetCorrTOF_XFP(){
//  double afp_cor = GValue::Value("XFP_TOF_CORR_AFP");
//  double xfp_cor = GValue::Value("XFP_TOF_CORR_XFP");
//  return GetTofE1_(afp_cor,xfp_cor);
//}

//float TS800::GetCorrTOF_XFPTAC(){
//  double afp_cor = GValue::Value("XFPTAC_TOF_CORR_AFP");
//double xfp_cor = GValue::Value("XFPTAC_TOF_CORR_XFP");
//return GetTofE1_TAC(afp_cor,xfp_cor);
//}
//
//



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

