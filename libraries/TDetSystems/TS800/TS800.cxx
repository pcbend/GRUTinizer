#include "TS800.h"

#include <cassert>
#include <iostream>

#include "TGEBEvent.h"

TS800::TS800() {
  Clear();
}

TS800::~TS800(){
}

void TS800::Copy(TObject& obj) const {
  TDetector::Copy(obj);

}

void TS800::Clear(Option_t* opt){
  TDetector::Clear(opt);

}

int TS800::BuildHits(){
  for(auto& event : raw_data) { // should only be one..
    TGEBEvent* geb = (TGEBEvent*)&event;
    SetTimestamp(geb->GetTimestamp());
    int ptr = 0;
    const TRawEvent::GEBS800Header *head = ((const TRawEvent::GEBS800Header*)geb->GetPayload());
    ptr += sizeof(TRawEvent::GEBS800Header);
      //  Here, we are now pointing at the size of the next S800 thing.  Inclusive in shorts.
      //  std::string buffer = Form("all0x%04x",*((unsigned short*)(geb->GetPayload()+ptr+2)));
    //printf("head.total_size == %i\n",head->total_size); fflush(stdout);
    //printf("sizeof(TRawEvent::GEBS800Header) == %i \n",sizeof(TRawEvent::GEBS800Header));
    unsigned short *data = (unsigned short*)(geb->GetPayload()+ptr);
    //printf("\t0x%04x\t0x%04x\t0x%04x\t0x%04x\n",*data,*(data+1),*(data+2),*(data+3));
    //while(ptr<((head->total_size*2)-2)) {
    std::string toprint = "all";
    int x = 0;
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
	tof.Clear();
	HandleTOFPacket(dptr+1,sizeleft);
	break;
      case 0x5810:  // S800 Scint
	break;
      case 0x5820:  // S800 Ion Chamber
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
	break;
      case 0x58c0:  // VME ADC 
	break;
      case 0x58d0:  // Galotte
	break;
      case 0x58e0:
	break;
      case 0x58f0:
	break;
      default:
	fprintf(stderr,"unknown data S800 type: 0x%04x\n",*dptr);
	return 0;
      };
    }
    SetEventCounter(head->GetEventNumber());
    //geb->Print(toprint.c_str());
  }
  return 0;
}

bool TS800::HandleTrigPacket(unsigned short *data,int size) {
  if(size<2)
    return false;

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
  TCrdc *current_crdc=0;
  if((*data)<3)
    current_crdc = &crdc[*data];
  if(!current_crdc)
    return false;
  current_crdc->SetId(*data);
  int x =1;
  int subsize = *(data+x); 
  x++;
  int subtype = *(data+x);
  x++;

  std::map<int,std::map<int,int> > pad;
  for(;x<subsize;x+=2) {
    unsigned short word1 = *(data+x); 
    if((word1&0x8000)!=0x8000) { continue; }
    unsigned short word2 = *(data+x); 

    int sample_number    = (word1&(0x7fc0)) >> 6;
    int channel_number   =  word1&(0x003f);
    int connector_number = (word2&(0x0c00)) >> 10;
    int databits         = (word2&(0x03ff));
    int real_channel = (connector_number << 6) + channel_number;
    pad[real_channel][sample_number] = databits;
  }
  x+=2;
  std::map<int,std::map<int,int> >::iterator it1;
  std::map<int,int>::iterator it2;

  for(it1=pad.begin();it1!=pad.end();it1++) {
    //printf("channel[%03i]\n",it1->first);
    for(it2=it1->second.begin();it2!=it1->second.end();it2++) {
      //printf("\t%i\t%i\n",it2->first,it2->second);
      crdc->AddPoint(it1->first,it2->first,it2->second);
    }
  }
  if(x>=size)
      return true;
  subsize = *(data+x);
  x++;
  subtype = *(data+x);
  x++;
  current_crdc->SetAnode(*(data+x));
  x++;
  current_crdc->SetTime(*(data+x));

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

bool TS800::HandleMTDCPacket(char *data,unsigned short size) {
  return true;
}

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

int TS800::Size(){
  return 0;
}

TDetectorHit& TS800::GetHit(int i){
  TDetectorHit *hit = new TS800Hit;
  return *hit;
}
