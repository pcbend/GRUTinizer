#include "TJanus.h"

#include <cassert>
#include <iostream>

#include "JanusDataFormat.h"
#include "TNSCLEvent.h"

TJanus::TJanus() { }

TJanus::~TJanus(){ }

void TJanus::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TJanus& janus = (TJanus&)obj;
  janus.janus_hits = janus_hits;
  janus.raw_data.clear();
}

void TJanus::Clear(Option_t* opt){
  TDetector::Clear(opt);

  janus_hits.clear();
}

int TJanus::BuildHits(){
  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    Build_VMUSB_Read(nscl.GetPayloadBuffer());
  }
  return Size();
}

void TJanus::InsertHit(const TDetectorHit& hit){
  janus_hits.push_back((const TJanusHit&)hit);
  fSize++;
}

TJanusHit& TJanus::GetJanusHit(int i){
  return janus_hits.at(i);
}

TDetectorHit& TJanus::GetHit(int i){
  return janus_hits.at(i);
}


void TJanus::Build_VMUSB_Read(TSmartBuffer buf){
  const char* data = buf.GetData();

  const VMUSB_Header* vmusb_header = (VMUSB_Header*)data;
  data += sizeof(VMUSB_Header);

  // vmusb_header.size() returns the number of 16-bit words in the payload.
  // Each adc entry is a 32-bit word.
  // 3 additional 16-bit words for the timestamp
  int num_adc_channels = vmusb_header->size()/2 - 3;

  const VME_Timestamp* vme_timestamp = (VME_Timestamp*)(data + num_adc_channels*sizeof(CAEN_ADC));
  long timestamp = vme_timestamp->ts1()*20;

  int i;
  for(i=0; i<num_adc_channels; i++){
    const CAEN_ADC* adc = (CAEN_ADC*)data;
    data += sizeof(CAEN_ADC);

    TJanusHit hit;
    hit.SetEntryType((char)adc->entry_type());
//    hit.SetTimestamp(timestamp);

    if(adc->IsValid()){
      int id = 32*(adc->card_num()-5) + adc->channel_num();
      hit.SetAnalogChannel(id);
      hit.SetOverflowBit(adc->overflow());
      hit.SetUnderflowBit(adc->underflow());
      hit.SetCharge(adc->adcvalue());
      InsertHit(hit);
    }
  }

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
