#include "TJanus.h"

#include <cassert>
#include <iostream>

#include "JanusDataFormat.h"
#include "TNSCLEvent.h"

TJanus::TJanus() {
  janus_hits = new TClonesArray("TJanusHit");
}

TJanus::~TJanus(){
  delete janus_hits;
}

void TJanus::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TJanus& janus = (TJanus&)obj;
  janus_hits->Copy(*janus.janus_hits);
  janus.raw_data.clear();
}

void TJanus::Clear(Option_t* opt){
  TDetector::Clear(opt);

  janus_hits->Clear(opt);
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
  TJanusHit* new_hit = (TJanusHit*)janus_hits->ConstructedAt(Size());
  hit.Copy(*new_hit);
  fSize++;
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

  const VME_Timestamp* vme_timestamp = (VME_Timestamp*)(data + num_adc_channels*sizeof(CAEN_DataPacket));
  long timestamp = vme_timestamp->ts1() * 20;

	std::map<int, TJanusHit> channels;
  for(int i=0; i<num_adc_channels; i++){
    const CAEN_DataPacket* adc = (CAEN_DataPacket*)data;
    data += sizeof(CAEN_DataPacket);

		if(!adc->IsValid()){
			 continue;
		}

		// ADCs are in slots 5-8, TDCs in slots 9-12
		bool is_tdc = adc->card_num() >= 9;
		int id = 32*(adc->card_num() - 5 - 4*is_tdc) + adc->channel_num();
		
		TJanusHit& hit = channels[id];
		hit.SetAnalogChannel(id);
    hit.SetTimestamp(timestamp);

		if(is_tdc){
			 hit.SetTDCEntryType((char)adc->entry_type());
		} else {
			 hit.SetADCEntryType((char)adc->entry_type());
		}

    if(adc->IsValid()){
			if(is_tdc){
				 hit.SetTDCOverflowBit(adc->overflow());
				 hit.SetTDCUnderflowBit(adc->underflow());
				 hit.SetTDC(adc->adcvalue());
				 // std::cout << "Channel: " << id << "\t" << "TDC:" << adc->adcvalue()
				 // 					 << std::endl;
			} else {
				 hit.SetADCOverflowBit(adc->overflow());
				 hit.SetADCUnderflowBit(adc->underflow());
				 hit.SetCharge(adc->adcvalue());
			}			
    }
  }

	for(auto& elem : channels) {
		 InsertHit(elem.second);
	}

  data += sizeof(VME_Timestamp);

  //assert(data == buf.GetData() + buf.GetSize());
  if(data != buf.GetData() + buf.GetSize()){
    std::cerr << "End of janus read not equal to size of buffer given:\n"
              << "\tBuffer Start: " << (void*)buf.GetData() << "\tBuffer Size: " << buf.GetSize()
              << "\n\tBuffer End: " << (void*)(buf.GetData() + buf.GetSize())
              << "\n\tNum ADC chan: " << num_adc_channels
              << "\n\tPtr at end of read: " << (void*)(data)
              << "\n\tDiff: " << (buf.GetData() + buf.GetSize()) - data
              << std::endl;

    buf.Print("all");
  }
}
