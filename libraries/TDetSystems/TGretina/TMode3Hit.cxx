#include "TMode3Hit.h"

#include "TGEBEvent.h"
#include "TGRUTOptions.h"

ClassImp(TMode3Hit)

bool TMode3Hit::fExtractWaves = true;

TMode3Hit::TMode3Hit(){
  //fOwnWave = false;
  wave = NULL;
  wavesize = 0;
  for(int i=0;i<MAXTRACE;i++) {wavebuffer[i]=0;}
}

TMode3Hit::~TMode3Hit() {
  //if(fOwnWave && wave) {
  //  wave = 0;
  //  delete wave;
  //}
}



void TMode3Hit::BuildFrom(TSmartBuffer buf){
  Clear();

  // // DANGER, uncomment and fix
  // bool read_waveform = TGRUTOptions::Get()->ExtractWaves();

  // auto header = (TRawEvent::GEBMode3Head*)buf.GetData();
  // buf.Advance(sizeof(TRawEvent::GEBMode3Head));

  // board_id = header->board_id;

  // auto data   = (TRawEvent::GEBMode3Data*)buf.GetData();
  // buf.Advance(sizeof(TRawEvent::GEBMode3Data));

  // led = data->GetLed();
  // charge = data->GetEnergy(*header);
  // //SetCharge(data->GetEnergy(*header));
  // SetAddress(GetHole(),GetCrystal(),GetSegmentId());
  // cfd = data->GetCfd();

  // //std::cout << "wavesize:  " << wavesize << std::endl;
  // //buf.Print("all");
  // //std::cout << std::endl;
  // //std::cout << std::endl;

  // size_t wave_bytes = header->GetLength()*4 - sizeof(*header) + 4 - sizeof(*data);
  // if(read_waveform & (wavesize<MAXTRACE)){
  //   wavesize = wave_bytes/sizeof(short);
  //   //wave = (short*)malloc(wave_bytes);
  //   //fOwnWave = true;
  //   memcpy((char*)wavebuffer, buf.GetData(), wave_bytes);

  //   for(int i=0; i<wavesize; i+=2){
  //     short tmp      = TRawEvent::SwapShort(wavebuffer[i+1]);
  //     wavebuffer[i+1] = TRawEvent::SwapShort(wavebuffer[i]);
  //     wavebuffer[i]   = tmp;
  //   }
  //   wave = &wavebuffer[0];
  // } else {
  //   wave = NULL;
  //   wavesize = 0;
  // }

  // buf.Advance(wave_bytes);
}





void TMode3Hit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TMode3Hit& mode3 = (TMode3Hit&)obj;


  mode3.board_id = board_id;
  //mode3.energy   = energy;
  mode3.charge   = charge;
  mode3.wavesize = wavesize;
  mode3.led      = led;
  mode3.cfd      = cfd;
  //if(mode3.wave) { delete wave; }
  if(ExtractWaves() && wavesize>0) {
    //fOwnWave = true;
    //mode3.wave = new Short_t[wavesize];
    memcpy(mode3.wavebuffer,wavebuffer,wavesize*(sizeof(Short_t)));
    mode3.wave = &(mode3.wavebuffer[0]);
  } else {
    mode3.wave=0;
  }
  //mode3.raw_data.clear();
}


void TMode3Hit::Print(Option_t *opt) const { }

void TMode3Hit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  board_id = -1;
  charge   = -1;
  led      = -1;
  cfd      = -1;
  //if(fOwnWave && wave) {
  //  delete wave;
  //}
  ClearWave();
  //raw_data.clear();
}

void TMode3Hit::ClearWave(Option_t *opt) {
  for(int i=0;i<wavesize;i++) {
    if(i>MAXTRACE)
      break;
    //std::cout << "Clear wave  " << i << std::endl;
    wavebuffer[i] = 0;
  }
  wave = NULL;
  wavesize =  0;
}
