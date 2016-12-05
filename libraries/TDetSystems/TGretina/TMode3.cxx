#include "TMode3.h"

#include "TGEBEvent.h"


#include "GH1D.h"
#include "GCanvas.h"

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
}

void TMode3::InsertHit(const TDetectorHit& hit){
  //TMode3Hit* new_hit = (TMode3Hit*)mode3_hits->ConstructedAt(Size());
  //hit.Copy(*new_hit);
  mode3_hits.emplace_back((TMode3Hit&)hit);
  fSize++;
}

int TMode3::BuildHits(std::vector<TRawEvent>& raw_data){
  if(raw_data.size()<1)
    return Size();
  long smallest_time = 0x3fffffffffffffff;
  for(auto& event : raw_data){
    if(event.GetTimestamp()<smallest_time)
      smallest_time=event.GetTimestamp();
    TMode3Hit hit;
    TSmartBuffer buf = event.GetPayloadBuffer();
    hit.BuildFrom(buf);
    hit.SetTimestamp(event.GetTimestamp());
    InsertHit(hit);
  }
  SetTimestamp(smallest_time);
  std::sort(mode3_hits.begin(),mode3_hits.end());
  return Size();
}

void TMode3::Print(Option_t *opt) const { }

void TMode3::Clear(Option_t *opt) {
  //TDetector::Clear(opt);
  mode3_hits.clear(); //->Clear(opt);//("TMode3Hit");
}

void TMode3::Draw(Option_t *opt) const {
  if(!Size())
    return;
  TString option = opt;
  if(!gPad || option.Contains("new",TString::kIgnoreCase)) {
    new GCanvas;
  } else {
    gPad->Clear();
  }
  int total_bins =0;
  std::vector<double> data;
  for(auto i=0;i<Size();i++) {
    TMode3Hit hit = GetMode3Hit(i);
    for(auto j=0;j<hit.Size();j++) {
      data.push_back(hit.GetWave().at(j));
    }
  }
  GH1D g("hist","hist",data.size(),0,data.size());
  for(auto i=0;i<data.size();i++) {
    g.Fill(i,data.at(i));
  }
  g.DrawCopy(opt);

}


