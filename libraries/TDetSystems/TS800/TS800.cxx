#include "TS800.h"
#include "TS800Hit.h"

#include <cassert>
#include <iostream>

#include "TGEBEvent.h"

TS800::TS800() {
  time_of_flight = new TClonesArray("TTOFHit",10);
  fp_scint       = new TClonesArray("TFPScint",10);
  ion_chamber    = new TClonesArray("TIonChamber",20);
  crdc1          = new TClonesArray("TCrdcPad",20);
  crdc2          = new TClonesArray("TCrdcPad",20);
  ppac1          = new TClonesArray("TCrdcPad",20);
  ppac2          = new TClonesArray("TCrdcPad",20);
  hodo           = new TClonesArray("THodoHit",10);
  Clear();
}

TS800::~TS800(){
  time_of_flight->Delete();
  fp_scint->Delete();
  ion_chamber->Delete();
  crdc1->Delete("C");
  crdc2->Delete("C");
  ppac1->Delete("C");
  ppac2->Delete("C");
  hodo->Delete();
}

void TS800::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  time_of_flight->Copy((*((TS800&)obj).time_of_flight)) ;
  fp_scint->Copy((*((TS800&)obj).time_of_flight));
  ion_chamber->Copy((*((TS800&)obj).time_of_flight));
  crdc1->Copy((*((TS800&)obj).crdc1));
  crdc2->Copy((*((TS800&)obj).crdc2));
  ppac1->Copy((*((TS800&)obj).ppac1));
  ppac2->Copy((*((TS800&)obj).ppac2));
  hodo->Copy((*((TS800&)obj).hodo));

  ((TS800&)obj).crdc1_charge = crdc1_charge;
  ((TS800&)obj).crdc2_charge = crdc2_charge;
  ((TS800&)obj).crdc1_time = crdc1_time;
  ((TS800&)obj).crdc2_time = crdc2_time;

  ((TS800&)obj).ppac1_charge = ppac1_charge;
  ((TS800&)obj).ppac2_charge = ppac2_charge;
  ((TS800&)obj).ppac1_time = ppac1_time;
  ((TS800&)obj).ppac2_time = ppac2_time;


  ((TS800&)obj).hodo_hit_pattern1 = hodo_hit_pattern1;
  ((TS800&)obj).hodo_hit_pattern2 = hodo_hit_pattern2;
  ((TS800&)obj).hodo_hit_time     = hodo_hit_time;
}

void TS800::Clear(Option_t* opt){
  TDetector::Clear(opt);

  //printf("S800 CLEAR.\n"); fflush(stdout);

  fEventCounter   = -1;
  fTriggerPattern = -1;
  fTrigger        = -1;

  time_of_flight->Clear();
  fp_scint->Clear();
  ion_chamber->Clear();
  ppac1->Clear();
  ppac2->Clear();
  crdc1->Clear();
  crdc2->Clear();
  hodo->Clear();

  crdc1_charge = -1;
  crdc2_charge = -1;
  crdc1_time   = -1;
  crdc2_time   = -1;

  ppac1_charge = -1;
  ppac2_charge = -1;
  ppac1_time   = -1;
  ppac2_time   = -1;

  hodo_hit_pattern1 = 0;
  hodo_hit_pattern2 = 0;
  hodo_hit_time     = -1;
}

int TS800::BuildHits(){
  //printf("In S800 build events.\n");
  //Clear();
  for(auto& event : raw_data) {
    TGEBEvent* geb = (TGEBEvent*)&event;
    SetTimestamp(geb->GetTimestamp());
    int ptr = 0;
    const TRawEvent::GEBS800Header *head = ((const TRawEvent::GEBS800Header*)geb->GetPayload());
    ptr += sizeof(TRawEvent::GEBS800Header);     //  Here, we are now pointing at the size of the next S800 thing.  Inclusive in shorts.
    //std::string buffer = Form("all0x%04x",*((unsigned short*)(geb->GetPayload()+ptr+2)));

    //printf("head.total_size == %i\n",head->total_size); fflush(stdout);

    char data[2048];
    while(ptr<((head->total_size*2)-2)) {
      //printf("\tptr == %i\t0x%04x\t0x%04x\n",ptr,(*((unsigned short*)(geb->GetPayload()+ptr))) , (*((unsigned short*)(geb->GetPayload()+ptr+2)))       ); fflush(stdout);

      unsigned short size_in_bytes = (*((unsigned short*)(geb->GetPayload()+ptr))*2);
      unsigned short type          = *((unsigned short*)(geb->GetPayload()+ptr+2));

      memcpy(data,geb->GetPayload()+ptr,size_in_bytes);//(*((unsigned short*)(geb->GetPayload()+ptr))*2));
      ptr +=  (*((unsigned short*)(geb->GetPayload()+ptr))*2);

      switch(type) {
        case 0x5801:
          HandleTriggerPacket(data+4,size_in_bytes-4);
          break;
        case 0x5802:
          HandleTOFPacket(data+4,size_in_bytes-4);
          break;
        case 0x5810:
          HandleFPScintPacket(data+4,size_in_bytes-4);
          break;
        case 0x5820:
          HandleIonChamberPacket(data+4,size_in_bytes-4);
          break;
        case 0x5840:
          //HandleCRDCPacket(data+4,size_in_bytes-4);
          break;
        case 0x5850:
        case 0x5860:
          printf("S800 packet: 0x%04x.\n",type);
          break;
        case 0x5870:
          //HandleIntermediatePPACPacket(data+4,size_in_bytes-4);
          break;
        case 0x5880:
        case 0x5890:
          //HandleIntermidatePPACPacket(data+4,size_in_bytes-4);
          printf("S800 packet: 0x%04x.\n",type);
          break;
        case 0x58a0:
          break;
        case 0x58b0:
          HandleHODOPacket(data+4,size_in_bytes-4);
          break;
        case 0x58c0:
        case 0x58d0:
        case 0x58e0:
          break;
        case 0x58f0:
          HandleMTDCPacket(data+4,size_in_bytes-4);
          break;
      };


      if(ptr>=(head->total_size*2))
        break;
      //buffer += Form("0x%04x",type);//*((unsigned short*)(geb->GetPayload()+ptr+2)));

    }


    //printf("buffer.c_str == %s\n",buffer.c_str());
    SetEventCounter(head->GetEventNumber());
    //       *((Long_t*)(geb.GetPayload()+26)) & 0x0000ffffffffffff);
    //geb->Print("all0x5800");
    //geb->Print(buffer.c_str());
  }
  return 0;
}

bool TS800::HandleTriggerPacket(char *data,unsigned short size) {

  //printf(BLUE "0x%04x\t0x%04x" RESET_COLOR "\n",*((unsigned short*)data),*((unsigned short*)(data+2)) );
  //  Data read from the Lecroy 2367 ULM module and a Phillips 7186H TDC. One trigger pattern followed by up to four times.
  if(size<4) //min size to fit the above(2 shorts).
    return false;
  fTriggerPattern = *((unsigned short*)data);
  for(int i=2;i<size;i+=2) {
    unsigned short *temp = ((unsigned short*)(data+i));
    switch((*temp)&0xf000) {
      case 0x8000:  //S800
        fTrigger  = *temp;
        break;
      case 0x9000:  //External1 source
        fTrigger  = *temp;
        //fprintf(stderr,"Trigger from !S800, External1!!");
        break;
      case 0xa000:  //External2 source
        fTrigger  = *temp;
        //fprintf(stderr,"Trigger from !S800, External2!!");
        break;
      case 0xb000:  //Secondary source
        fTrigger  = *temp;
        //fprintf(stderr,"Trigger from !S800, Secondary source!!");
        break;
    };
  }

  return true;
}


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
