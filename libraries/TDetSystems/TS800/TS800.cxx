#include "TS800.h"
#include "TS800Hit.h"

#include <cassert>
#include <iostream>

#include "TGEBEvent.h"

TS800::TS800() {
  time_of_flight = new TClonesArray("TS800TOF",10);
  Clear();
}

TS800::~TS800(){
  time_of_flight->Delete();
}

void TS800::Copy(TObject& obj) const {
  TDetector::Copy(obj);
}

void TS800::Clear(Option_t* opt){
  TDetector::Clear(opt);

  fEventCounter   = -1;
  fTriggerPattern = -1;
  fTrigger        = -1;

  time_of_flight->Clear();
  rf_tdc   = -1;
  obj_tdc  = -1;
  obj_tac  = -1;
  xfp_tdc  = -1;
  xfp_tac  = -1;
  si_tdc   = -1;

  e1up_energy   = -1;
  e1up_time     = -1;
  e2up_energy   = -1;
  e2up_time     = -1;
  e3up_energy   = -1;
  e3up_time     = -1;
  e1down_energy = -1;
  e1down_time   = -1;
  e2down_energy = -1;
  e2down_time   = -1;
  e3down_energy = -1;
  e3down_time   = -1;




}

int TS800::BuildHits(){
  //printf("In S800 build events.\n");
  for(auto& event : raw_data) {
    TGEBEvent* geb = (TGEBEvent*)&event;
    SetTimestamp(geb->GetTimestamp());
    int ptr = 0;
    const TRawEvent::GEBS800Header *head = ((const TRawEvent::GEBS800Header*)geb->GetPayload());
    ptr += sizeof(TRawEvent::GEBS800Header);     //  Here, we are now pointing at the size of the next S800 thing.  Inclusive in shorts.
    //std::string buffer = Form("all0x%04x",*((unsigned short*)(geb->GetPayload()+ptr+2)));
    char data[2048];
    
    //printf("head.total_size == %i\n",head->total_size); fflush(stdout);

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
          HandleFPICPacket(data+4,size_in_bytes-4);
          break;
      };


      if(ptr>=(head->total_size*2)) 
        break;
      //buffer += Form("0x%04x",type);//*((unsigned short*)(geb->GetPayload()+ptr+2)));
       
    }


    //printf("buffer.c_str == %s\n",buffer.c_str());
    //std::cout << *head << std::endl;
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
        //fprintf(stderr,"Trigger from !S800, External1!!");
        break;
      case 0xa000:  //External2 source
        //fprintf(stderr,"Trigger from !S800, External2!!");
        break;
      case 0xb000:  //Secondary source
        //fprintf(stderr,"Trigger from !S800, Secondary source!!");
        break;
    };
  }

  return true;
}

bool TS800::HandleTOFPacket(char *data,unsigned short size) {
  //some number of times read from both a Phillips 7186H TDC and an Ortec 566 TAC digitized by a Phillips 7164 ADC.
  if(size<2)
    return false;
  for(int i=0;i<size;i+=2) {
    TS800TOF *tof = (TS800TOF*)time_of_flight->ConstructedAt(time_of_flight->GetEntries());
    tof->Set(*((unsigned short*)(data+i))); 
  } 
  return true;
}

bool TS800::HandleFPScintPacket(char *data,unsigned short size) {
  //Energies and times of the focal plane scintillator, read from a Lecroy 4300B FERA and a Phillips 7186H TDC, should come in pairs.
  for(int i=0;i<size;i+=4) {
    unsigned short *temp = ((unsigned short*)(data+i));
    if(((*(temp+2))&0xf000) != (*temp)&0xf000) {
      fprintf(stderr,"fp_scint: energy and time do not match.");
      return false;
    }
    switch((*temp)&0xf000) {
      case 0x0000: //E1 up
        e1up_energy = (*temp)&0x0fff;
        e1up_time   = (*(temp+2)&0x0fff);
        break;
      case 0x0001: //E1 down
        e1down_energy = (*temp)&0x0fff;
        e1down_time   = (*(temp+2)&0x0fff);
        break;
      case 0x0002: //E2 up 
        e2up_energy = (*temp)&0x0fff;
        e2up_time   = (*(temp+2)&0x0fff);
        break;
      case 0x0003: //E2 down
        e2down_energy = (*temp)&0x0fff;
        e2down_time   = (*(temp+2)&0x0fff);
        break;
      case 0x0004: //E3 up
        e3up_energy = (*temp)&0x0fff;
        e3up_time   = (*(temp+2)&0x0fff);
        break;
      case 0x0005: //E3 down
        e3down_energy = (*temp)&0x0fff;
        e3down_time   = (*(temp+2)&0x0fff);
        break;
    };
  }
  return true;
}

bool TS800::HandleFPICPacket(char *data,unsigned short size) {
  //Zero suppressed energies from the ionization chamber, read from a Phillips 7164 ADC.
  //Note, this data is in a "sub-packet".  
  int ptr = 0;



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
