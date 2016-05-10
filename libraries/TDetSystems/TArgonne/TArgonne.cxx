#include "TArgonne.h"
#include "TGEBEvent.h"

TArgonne::TArgonne(){
  Clear();
}

TArgonne::~TArgonne() {

}

void TArgonne::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TArgonne& detector = (TArgonne&)obj;
  detector.anl_hits = anl_hits;
}

void TArgonne::InsertHit(const TDetectorHit& hit){
  anl_hits.emplace_back((TArgonneHit&)hit);
  fSize++;
}

int TArgonne::BuildHits(std::vector<TRawEvent>& raw_data){
  for(auto& event : raw_data){
    SetTimestamp(event.GetTimestamp());
    TArgonneHit hit;
    auto buf = event.GetPayloadBuffer();
    hit.BuildFrom(buf);
    hit.SetTimestamp(event.GetTimestamp());
    InsertHit(hit);
  }
  return Size();
}

void TArgonne::Print(Option_t *opt) const { }

void TArgonne::Clear(Option_t *opt) {
  anl_hits.clear();
}
