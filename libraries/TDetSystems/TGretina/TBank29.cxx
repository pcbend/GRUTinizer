#include "TBank29.h"

#include "TGEBEvent.h"
#include "TGRUTOptions.h"

ClassImp(TBank29)

TBank29::TBank29(){
  channels = new TClonesArray("TMode3Hit");
}

TBank29::~TBank29() {
  delete channels;
}

void TBank29::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TBank29& bank = (TBank29&)obj;
  channels->Copy(*(bank.channels));
  bank.raw_data.clear();
}

void TBank29::InsertHit(const TDetectorHit& hit){
  TMode3* new_hit = (TMode3*)channels->ConstructedAt(Size());
  hit.Copy(*new_hit);
  fSize++;
}

int TBank29::BuildHits(){
  for(auto& event : raw_data){
    TGEBEvent* geb = (TGEBEvent*)&event;
    SetTimestamp(geb->GetTimestamp());
    TMode3Hit hit;
    hit.BuildFrom(geb->GetPayloadBuffer());
    InsertHit(hit);
  }
  if(Size())
    SetTimestamp(((TMode3Hit*)channels->At(0))->GetLed());
  return Size();
}

void TBank29::Print(Option_t *opt) const { }

void TBank29::Clear(Option_t *opt) {
  TDetector::Clear(opt);
  channels->Clear(opt);//("TMode3Hit");
  raw_data.clear();
}
