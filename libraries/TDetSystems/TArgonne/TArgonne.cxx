#include "TArgonne.h"

#include "TGEBEvent.h"

TArgonne::TArgonne(){
  //anl_hits = new TClonesArray("TArgonneHit");
  Clear();
}

TArgonne::~TArgonne() {
  //delete anl_hits;
}

void TArgonne::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TArgonne& detector = (TArgonne&)obj;
  detector.anl_hits = anl_hits;
  //anl_hits->Copy(*detector.anl_hits);
  detector.raw_data.clear();
}

void TArgonne::InsertHit(const TDetectorHit& hit){
  //TArgonneHit* new_hit = (TArgonneHit*)anl_hits->ConstructedAt(Size());
  //hit.Copy(*new_hit);
  anl_hits.emplace_back((TArgonneHit&)hit);
  fSize++;
}

int TArgonne::BuildHits(){
  for(auto& event : raw_data){
    SetTimestamp(event.GetTimestamp());
    TArgonneHit hit;
    hit.BuildFrom(event.GetPayloadBuffer());
    hit.SetTimestamp(event.GetTimestamp());
    InsertHit(hit);
  }
  return Size();
}

void TArgonne::Print(Option_t *opt) const { }

void TArgonne::Clear(Option_t *opt) {
  //TDetector::Clear(opt);
  anl_hits.clear(); //->Clear(opt);//("TArgonneHit");
  raw_data.clear();
}
