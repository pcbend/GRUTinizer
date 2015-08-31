#include "TMode3.h"

#include "TGEBEvent.h"

ClassImp(TMode3)

bool TMode3::fExtractWaves = false;

TMode3::TMode3(){
  fOwnWave = false;
}

TMode3::~TMode3() {
  if(fOwnWave && wave) {
    wave = 0;
    delete wave;
  }
}


void TMode3::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TMode3& mode3 = (TMode3&)obj;
  mode3.board_id = board_id;
  mode3.energy   = energy;
  mode3.wavesize = wavesize;
  mode3.led      = led;
  mode3.cfd      = cfd;
  if(mode3.wave) { delete wave; }
  if(ExtractWaves() && wavesize>0) { 
    fOwnWave = true;
    mode3.wave = new Short_t[wavesize];
    memcpy(mode3.wave,wave,wavesize*(sizeof(Short_t));
  }
  mode3.raw_data.clear();
}

int TGretina::BuildHits(){
  for(auto& event : raw_data){
    TGEBEvent& geb = (TGEBEvent&)event;
    SetTimestamp(geb.GetTimestamp());
    const TRawEvent::GEBBankType1* raw = (const TRawEvent::GEBBankType1*)geb.GetPayloadBuffer().GetData();
    TGretinaHit hit;
    hit.BuildFrom(*raw);
    InsertHit(hit);
  }
  gretina_hits->At(0)->Print();
  raw_data.clear();

  BuildAddbackHits();

  gretina_hits->At(0)->Print();

  return Size();
}

void TGretina::Print(Option_t *opt) const { }

void TGretina::Clear(Option_t *opt) {
  TDetector::Clear(opt);
  board_id = -1;
  energy   = -1;
  wavesize =  0;
  led      = -1;
  cfd      = -1;
  if(fOwnWave && wave) {
    wave = 0;
    delete wave;
  }
  raw_data.clear();
}
