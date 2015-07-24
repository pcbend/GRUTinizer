#include "TJanus.h"

#include <cassert>
#include <iostream>

#include "JanusDataFormat.h"

TJanus::TJanus() {
  janus_hits = new TClonesArray("TJanusHit");
}

TJanus::~TJanus(){
  delete janus_hits;
}

void TJanus::AddRawData(RawData raw){
  raw_data.push_back(raw);
}

void TJanus::BuildHits(){
  for(auto raw : raw_data){
    Build_VMUSB_Read(raw);
  }
  raw_data.clear();
}

void TJanus::InsertHit(const TDetectorHit& hit){
  if(!hit.InheritsFrom("TJanusHit")){
    std::cout << "Attempted to copy " << hit.ClassName() << " into TJanusHit" << std::endl;
    return;
  }

  TJanusHit* new_hit = (TJanusHit*)janus_hits->ConstructedAt(Size());
  new_hit->Copy(hit);
}

int TJanus::Size(){
  return janus_hits->GetEntries();
}

TDetectorHit& TJanus::GetHit(int i){
  return *(TJanusHit*)janus_hits->At(i);
}


void TJanus::Build_VMUSB_Read(RawData raw){
  const char* data = raw.data;

  VMUSB_Header* vmusb_header = (VMUSB_Header*)data;
  data += sizeof(VMUSB_Header);

  // vmusb_header.size() returns the number of 16-bit words in the payload.
  // Each adc entry is a 32-bit word.
  // 3 additional 16-bit words for the timestamp
  int num_adc_channels = vmusb_header->size()/2 - 3;

  for(int i=0; i<num_adc_channels; i++){
    CAEN_ADC* adc = (CAEN_ADC*)data;
    data += sizeof(CAEN_ADC);

    if(adc->IsValid()){
      TJanusHit hit;
      int id = 32*(adc->card_num()-5) + adc->channel_num();
      hit.SetAnalogChannel(id);
      hit.SetOverflowBit(adc->overflow());
      hit.SetUnderflowBit(adc->underflow());
      hit.SetCharge(adc->adcvalue());
      hit.SetTime(0);

      InsertHit(hit);
    }
  }

  VME_Timestamp* vme_timestamp = (VME_Timestamp*)data;
  data += sizeof(VME_Timestamp);

  assert(data == raw.data+raw.data_size);
}
