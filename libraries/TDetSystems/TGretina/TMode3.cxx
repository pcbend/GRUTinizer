#include "TMode3.h"

#include "TGEBEvent.h"

ClassImp(TMode3)

bool TMode3::fExtractWaves = false;

TMode3::TMode3(){
  fOwnWave = false;
  wave = NULL;
}

TMode3::~TMode3() {
  if(fOwnWave && wave) {
    wave = 0;
    delete wave;
  }
}



void TMode3::BuildFrom(TSmartBuffer& buf, bool read_waveform){
  Clear();

  auto header = (TRawEvent::GEBMode3Head*)buf.GetData();
  TRawEvent::SwapMode3Head(*header);
  buf.Advance(sizeof(TRawEvent::GEBMode3Head));

  board_id = header->board_id;

  auto data   = (TRawEvent::GEBMode3Data*)buf.GetData();
  TRawEvent::SwapMode3Data(*data);
  buf.Advance(sizeof(TRawEvent::GEBMode3Data));

  led = data->GetLed();
  hit.SetCharge(data->GetEnergy(header->GetChannel()));
  hit.SetAddress(GetHole(),GetCrystal(),GetSegmentId());
  cfd = data->GetCfd();

  size_t wave_bytes = header->GetLength()*4 - sizeof(*header) + 4 - sizeof(*data);
  if(read_waveform){
    wavesize = wave_bytes/sizeof(short);
    wave = (short*)malloc(wave_bytes);
    fOwnWave = true;
    memcpy((char*)wave, buf.GetData(), wave_bytes);
 
    for(int i=0; i<wavesize; i+=2){
      short tmp = TRawEvent::SwapShort(wave[i+1]);
      wave[i+1] = TRawEvent::SwapShort(wave[i]);
      wave[i]   = tmp;
    }
  } else {
    wave = NULL;
    wavesize = 0;
  }

  buf.Advance(wave_bytes);
}





void TMode3::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TMode3& mode3 = (TMode3&)obj;
  
  hit.Copy(mode3.hit);
  
  mode3.board_id = board_id;
  //mode3.energy   = energy;
  mode3.wavesize = wavesize;
  mode3.led      = led;
  mode3.cfd      = cfd;
  if(mode3.wave) { delete wave; }
  if(ExtractWaves() && wavesize>0) { 
    fOwnWave = true;
    mode3.wave = new Short_t[wavesize];
    memcpy(mode3.wave,wave,wavesize*(sizeof(Short_t)));
  }
  mode3.raw_data.clear();
}

int TMode3::BuildHits(){
/*
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
*/
  return 0;
}

void TMode3::Print(Option_t *opt) const { }

void TMode3::Clear(Option_t *opt) {
  TDetector::Clear(opt);
  hit.Clear(opt);
  board_id = -1;
  //energy   = -1;
  wavesize =  0;
  led      = -1;
  cfd      = -1;
  if(fOwnWave && wave) {
    delete wave;
  }
  wave = 0;
  raw_data.clear();
}
