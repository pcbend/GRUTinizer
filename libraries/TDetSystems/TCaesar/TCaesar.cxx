#include "TCaesar.h"

#include "TNSCLEvent.h"

#define FERA_TIME_ID   0x2301
#define FERA_ENERGY_ID 0x2302

TCaesar::TCaesar() {
  Clear();
}

TCaesar::~TCaesar(){ }

void TCaesar::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TCaesar& caesar = (TCaesar&)obj;
  caesar.caesar_hits = caesar_hits;
  caesar.raw_data.clear();
}

void TCaesar::Clear(Option_t* opt){
  TDetector::Clear(opt);

  fUlm = -1;
  caesar_hits.clear();
}

int TCaesar::BuildHits(){
  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    Build_Single_Read(nscl.GetPayloadBuffer());
  }
  return caesar_hits.size();
}

TCaesarHit& TCaesar::GetCaesarHit(int i){
  return caesar_hits.at(i);
}

TDetectorHit& TCaesar::GetHit(int i){
  return caesar_hits.at(i);
}

void TCaesar::Build_Single_Read(TSmartBuffer buf){
  const char* data = buf.GetData();
  const char* data_end = data + buf.GetSize();
  TRawEvent::CAESARHeader* header = (TRawEvent::CAESARHeader*)data;
  data += sizeof(TRawEvent::CAESARHeader);

  // FERA_VERSION_00 does not have a ULM register
  if(header->version == 0x0001){
    data -= sizeof(Short_t);
    SetULM(-1);
  } else {
    SetULM(header->ulm);
  }

  while(data < data_end){
    TRawEvent::CAESARFeraHeader* fera_header = (TRawEvent::CAESARFeraHeader*)data;
    if(fera_header->tag != FERA_ENERGY_ID && fera_header->tag != FERA_TIME_ID){
      data += fera_header->size * 2; // Size is inclusive number of 16-bit values.
      continue;
    }
    const char* fera_end = data + fera_header->size*2;
    data += sizeof(TRawEvent::CAESARFeraHeader);


    while(data < fera_end){
      TRawEvent::CAESARFera* fera = (TRawEvent::CAESARFera*)data;
      int nchan = fera->number_chans();
      if(nchan==0){
	nchan = 16;
      }

      for(int i=0; i<nchan; i++){
	if(fera_header->tag == FERA_ENERGY_ID){
	  SetCharge(fera->vsn(), fera->items[i].channel(), fera->items[i].value());
	} else { //FERA_TIME_ID
	  SetTime(fera->vsn(), fera->items[i].channel(), fera->items[i].value());
	}
      }

      data += 2*fera->number_chans() + 2;
    }
  }
}

TCaesarHit& TCaesar::GetHit_VSNChannel(int vsn, int channel){
  for(auto& hit : caesar_hits){
    if(hit.GetVSN() == vsn &&
       hit.GetChannel() == channel){
      return hit;
    }
  }

  // No such thing existing right now, make one
  caesar_hits.emplace_back();
  TCaesarHit& output = caesar_hits.back();
  output.SetVSN(vsn);
  output.SetChannel(channel);
  output.SetAddress( (37<<24) +
		     (vsn<<16) +
		     (channel) );
  return output;
}


void TCaesar::SetCharge(int vsn, int channel, int data) {
  TCaesarHit& hit = GetHit_VSNChannel(vsn, channel);
  hit.SetCharge(data);
}

void TCaesar::SetTime(int vsn, int channel, int data) {
  TCaesarHit& hit = GetHit_VSNChannel(vsn, channel);
  hit.SetTime(data);
}

void TCaesar::InsertHit(const TDetectorHit& hit) {
  caesar_hits.emplace_back((TCaesarHit&)hit);
  fSize++;
}
