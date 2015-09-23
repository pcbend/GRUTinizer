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
  fEventCounter = -1;
}

int TS800::BuildHits(){
  //printf("In S800 build events.\n");
  for(auto& event : raw_data) {
    TGEBEvent geb(event);
    SetTimestamp(geb.GetTimestamp());
    int ptr = 0;
    const TRawEvent::GEBS800Header *head = ((const TRawEvent::GEBS800Header*)geb.GetPayload());
    ptr += sizeof(TRawEvent::GEBS800Header);
    //std::cout << *head << std::endl;
    SetEventCounter(head->GetEventNumber());
    //       *((Long_t*)(geb.GetPayload()+26)) & 0x0000ffffffffffff);
    //geb.Print("all0x5804");
    
    









  }
  return 0;
}

void TS800::InsertHit(const TDetectorHit& hit){
  return;
}

int TS800::Size(){
  return 0;
}

TDetectorHit& TS800::GetHit(int i){
  TDetectorHit *hit = new TDetectorHit;
  return *hit;
}




