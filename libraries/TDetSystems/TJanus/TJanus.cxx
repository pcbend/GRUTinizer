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

void TJanus::Clear(Option_t* opt){
  janus_hits->Clear(opt);
  raw_data.clear();
}

bool TJanus::AddRawData(TSmartBuffer buf){
  raw_data.push_back(buf);
  return true;
}

int TJanus::BuildHits(){
  for(auto buf : raw_data){
    Build_VMUSB_Read(buf);
  }
  raw_data.clear();

  return Size();
}

void TJanus::InsertHit(const TDetectorHit& hit){
  if(!hit.InheritsFrom("TJanusHit")){
    return;
  }

  TJanusHit* new_hit = (TJanusHit*)janus_hits->ConstructedAt(Size());
  new_hit->Copy(hit);
}

int TJanus::Size(){
  return janus_hits->GetEntries();
}

TJanusHit& TJanus::GetJanusHit(int i){
  return *(TJanusHit*)janus_hits->At(i);
}

TDetectorHit& TJanus::GetHit(int i){
  return *(TJanusHit*)janus_hits->At(i);
}


void TJanus::Build_VMUSB_Read(TSmartBuffer buf){
  const char* data = buf.GetData();

  const VMUSB_Header* vmusb_header = (VMUSB_Header*)data;
  data += sizeof(VMUSB_Header);

  // vmusb_header.size() returns the number of 16-bit words in the payload.
  // Each adc entry is a 32-bit word.
  // 3 additional 16-bit words for the timestamp
  int num_adc_channels = vmusb_header->size()/2 - 3;

  int i;
  for(i=0; i<num_adc_channels; i++){
    const CAEN_ADC* adc = (CAEN_ADC*)data;
    data += sizeof(CAEN_ADC);

    TJanusHit hit;
    hit.SetEntryType((char)adc->entry_type());

    if(adc->IsValid()){
      int id = 32*(adc->card_num()-5) + adc->channel_num();
      hit.SetAnalogChannel(id);
      hit.SetOverflowBit(adc->overflow());
      hit.SetUnderflowBit(adc->underflow());
      hit.SetCharge(adc->adcvalue());
      hit.SetTime(0);
    }

    InsertHit(hit);
  }

  const VME_Timestamp* vme_timestamp = (VME_Timestamp*)data;
  data += sizeof(VME_Timestamp);

  //assert(data == buf.GetData() + buf.GetSize());
  if(data != buf.GetData() + buf.GetSize()){
    std::cerr << "End of janus read not equal to size of buffer given:\n"
              << "\tBuffer Start: " << (void*)buf.GetData() << "\tBuffer Size: " << buf.GetSize()
              << "\n\tBuffer End: " << (void*)(buf.GetData() + buf.GetSize())
              << "\n\tNum ADC chan: " << num_adc_channels
              << "\n\ti: " << i
              << "\n\tPtr at end of read: " << (void*)(data)
              << "\n\tDiff: " << (buf.GetData() + buf.GetSize()) - data
              << std::endl;

    buf.Print("all");
  }
}
