#include "TMode3.h"

#include "TGEBEvent.h"

TMode3::TMode3(){
  //mode3_hits = new TClonesArray("TMode3Hit");
  Clear();
}

TMode3::~TMode3() {
  //delete mode3_hits;
}

void TMode3::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TMode3& mode3 = (TMode3&)obj;
  mode3.mode3_hits = mode3_hits;
  //mode3_hits->Copy(*mode3.mode3_hits);
  mode3.raw_data.clear();
}

void TMode3::InsertHit(const TDetectorHit& hit){
  //TMode3Hit* new_hit = (TMode3Hit*)mode3_hits->ConstructedAt(Size());
  //hit.Copy(*new_hit);
  mode3_hits.emplace_back((TMode3Hit&)hit);
  fSize++;
}

int TMode3::BuildHits(){
  for(auto& event : raw_data){
    TGEBEvent* geb = (TGEBEvent*)&event;
    SetTimestamp(geb->GetTimestamp());
    TMode3Hit hit;
    hit.BuildFrom(geb->GetPayloadBuffer());
    InsertHit(hit);
  }
  return Size();
}

void TMode3::Print(Option_t *opt) const { }

void TMode3::Clear(Option_t *opt) {
  //TDetector::Clear(opt);
  mode3_hits.clear(); //->Clear(opt);//("TMode3Hit");
  raw_data.clear();
}
