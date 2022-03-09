#include "TBank29.h"

#include "TGEBEvent.h"
#include "TGRUTOptions.h"

ClassImp(TBank29)

TBank29::TBank29(){
}

TBank29::~TBank29() {
}

void TBank29::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TBank29& bank = (TBank29&)obj;
  bank.channels = channels;
}

void TBank29::InsertHit(const TDetectorHit& hit){
  channels.emplace_back((TMode3Hit&)hit);
  fSize++;
}

int TBank29::BuildHits(std::vector<TRawEvent>& raw_data){
  for(auto& event : raw_data){
    TGEBEvent* geb = (TGEBEvent*)&event;
    SetTimestamp(geb->GetTimestamp());
    TMode3Hit hit;
    TSmartBuffer buf = geb->GetPayloadBuffer();
    hit.BuildFrom(buf);
    InsertHit(hit);
  }
  if(Size()) {
    SetTimestamp(channels.at(0).GetLed());
  }
  return Size();
}

void TBank29::Print(Option_t *opt) const { }

void TBank29::Clear(Option_t *opt) {
  TDetector::Clear(opt);
  channels.clear();
}
