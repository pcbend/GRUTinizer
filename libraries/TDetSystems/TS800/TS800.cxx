#include "TS800.h"


#include <atomic>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include "TGEBEvent.h"
#include "TGRUTOptions.h"

std::vector<TS800::S800_InvMapLine> TS800::fIML_sec1;
std::vector<TS800::S800_InvMapLine> TS800::fIML_sec2;
std::vector<TS800::S800_InvMapLine> TS800::fIML_sec3;
std::vector<TS800::S800_InvMapLine> TS800::fIML_sec4;
short TS800::fMaxOrder;
float TS800::fBrho;
int TS800::fMass;
int TS800::fCharge;

TS800::TS800() {
  Clear();
  static std::atomic_bool InvMapFileRead(false);
  if(!InvMapFileRead){
    //   Quick little mutex to make sure that multiple threads
    // don't try to read the map file at the same time.
    static std::mutex inv_map_mutex;
    std::lock_guard<std::mutex> lock(inv_map_mutex);
    if(!InvMapFileRead){
      ReadInvMap();
      InvMapFileRead=true;
    }
  }

  GetCrdc(0).SetXslope(2.54);
  GetCrdc(0).SetYslope(-0.1074866936);
  GetCrdc(0).SetXoffset(-281.94);
  GetCrdc(0).SetYoffset(95.9966);
  //GetCrdc(0).SetYoffset(129.1513428524);

  GetCrdc(1).SetXslope(2.54);
  GetCrdc(1).SetYslope(0.1110700359);
  GetCrdc(1).SetXoffset(-281.94);
  GetCrdc(1).SetYoffset(-122.8);
  //GetCrdc(1).SetYoffset(-131.4070990297);


  //  std::cout << " Slopes and offsets are SET" << std::endl;
  //std::cout << " For Example : " <<     GetCrdc(0).GetYslope() << std::endl;
}

TS800::~TS800(){
}

void TS800::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TS800& other = (TS800&)obj;
  other.fAta = fAta;
  other.fYta = fYta;
  other.fBta = fBta;
  other.fDta = fDta;

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

bool TS800::ReadInvMap(){
  std::string filename = TGRUTOptions::Get()->S800InverseMapFile();

  std::string eat,I,COEF,ORDEXP;
  int index;
  std::stringstream ssTest;
  int par = 0;
  ifstream inFile;
  inFile.open(filename);
  getline(inFile,eat);
  //std::cout << eat << std::endl;
  sscanf(eat.c_str(), "S800 inverse map - Brho=%g - M=%d - Q=%d", &fBrho, &fMass, &fCharge);

  inFile >> I >> COEF >> ORDEXP;
  getline(inFile,eat);

  if(I!="I" && COEF!="COEFFICIENT"){
    std::cout << " *** Bad S800 Inv Map File Format!!! " << std::endl;
    std::cout << " *** Inv Map File Not Read !!! " << std::endl;
    return false;
  }

  while(!(inFile.eof())){

    if(ssTest.fail() == true){
      std::cout << " *** sstream Failed while reading file : " << filename << std::endl;
      std::cout << " *** File not loaded!! " << std::endl;
      return false;
    }
    ssTest.str(""); eat = "";
    getline(inFile,eat); ssTest << eat;


    if(ssTest.str() == "    ----------------------------------------------\r"){
      par++; continue;
    }
    else if (ssTest.str() == "     I  COEFFICIENT            ORDER EXPONENTS\r") continue;
    else
      ssTest >> index >> fIML.coef >> fIML.order >> fIML.exp[0]>> fIML.exp[1]>> fIML.exp[2]>> fIML.exp[3]>> fIML.exp[4] >> fIML.exp[5];


    switch(par){
    case 0:
      fIML_sec1.push_back(fIML);
      break;
    case 1:
      fIML_sec2.push_back(fIML);
      break;
    case 2:
      fIML_sec3.push_back(fIML);
      break;
    case 3:
      fIML_sec4.push_back(fIML);
      break;
    case 4:
      // you get to 4 before the eof.
      break;
    default:
      std::cout << " *** More than 4 sections!!" << std::endl
		<< "     Leaving Now!!! File Not Read!! "<< std::endl;
      return false;
      break;
    }
  }
  inFile.close();
  return true;
}


void TS800::MapCalc(float *input){
  float cumul=0;
  float multi;
  int CalcOrder = 6; // Standard order in inv file.
  std::vector<S800_InvMapLine> current_;
  for(int param = 0; param<4;param++){
    switch(param){
    case 0: current_ = fIML_sec1;  break;
    case 1: current_ = fIML_sec2;  break;
    case 2: current_ = fIML_sec3;  break;
    case 3: current_ = fIML_sec4;  break;
    default: printf(" *** Wrong Param passed to S800::MapCalc!!\n"); return;break;
    }
    for(size_t i=0; i<current_.size();i++){
      if(CalcOrder<current_.at(i).order) break;
      multi = 1;
      //std::cout << " current coef : " << current_.at(i).coef << std::endl;
      for(int j=0;j<CalcOrder;j++){
	if(current_.at(i).exp[j]!=0){
	  multi *= pow(input[j],current_.at(i).exp[j]);
	}
      }
      cumul += multi*current_.at(i).coef;
    }
    switch(param){
    case 0: fAta = cumul-0.004406; break; std::cout << " HAVE A HARDCODED SI28 VALUE HERE"<<std::endl;
    case 1: fYta = cumul; break;
    case 2: fBta = cumul-0.008173; break;
    case 3: fDta = cumul; break;
    }
  }
  //return cumul;
  return;
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
  float AFP = TMath::ATan((GetCrdc(1).GetDispersiveX()-GetCrdc(0).GetDispersiveX())/1073.0);
  return AFP;

}

float TS800::GetBFP() const{
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
  fMaxOrder = 0;
  fMass     = 0;
  fBrho     = -1;
  fCharge   = 0;

  fAta = sqrt(-1);
  fYta = sqrt(-1);
  fBta = sqrt(-1);
  fDta = sqrt(-1);


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
  MapCalc(input);

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
  return GetTof().GetTacOBJ() + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX();
}


float TS800::GetTofE1_TDC(float c1,float c2)  const {

  return GetTof().GetOBJ() - GetScint().GetTimeUp() + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX();
}

float TS800::GetTofE1_MTDC(float c1,float c2)   {

  std::vector<float> result;
  // TODO: This check is always false.  Commented it out, but was there some reason for it?
  // if(mtof.fObj.size()<0)
  //   std::cout << " In GetTOF MTDC, Size = " << mtof.fObj.size() << std::endl;
  for(unsigned int x=0;x<mtof.fObj.size();x++) {
    if(mtof.fObj.at(x)>10200) {
      for(unsigned int y=0;y<mtof.fE1Up.size();y++) {
        if((mtof.fE1Up.at(y) <13500) && (mtof.fE1Up.at(y) >12500))
          result.push_back( mtof.fObj.at(x) - mtof.fE1Up.at(y) + c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX());
      }
    }
  }
  if(result.size()==1)
    return result.at(0);
  return -1.0;
  //return GetTof().GetOBJ() - GetScint().GetTimeUp() - c1 * GetAFP() + c2  * GetCrdc(0).GetDispersiveX();
}
