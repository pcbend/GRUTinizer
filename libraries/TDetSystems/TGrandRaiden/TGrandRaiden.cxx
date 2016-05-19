#include "TGrandRaiden.h"
#include "TRawEvent.h"

TGrandRaiden::TGrandRaiden(){
  Clear();
}

TGrandRaiden::~TGrandRaiden() {

}

void TGrandRaiden::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TGrandRaiden& detector = (TGrandRaiden&)obj;
  detector.GRHits = GRHits;
}

void TGrandRaiden::InsertHit(const TDetectorHit& hit){
  GRHits.emplace_back((TGrandRaidenHit&)hit);
  fSize++;
}

int TGrandRaiden::BuildHits(std::vector<TRawEvent>& raw_data){
  for(auto& event : raw_data){
    SetTimestamp(event.GetTimestamp());
    TGrandRaidenHit hit;
    auto buf = event.GetPayloadBuffer();
    hit.BuildFrom(buf);
    hit.SetTimestamp(event.GetTimestamp());
    InsertHit(hit);
  }
  return Size();
}

void TGrandRaiden::Print(Option_t *opt) const { }

void TGrandRaiden::Clear(Option_t *opt) {
  GRHits.clear();
}
