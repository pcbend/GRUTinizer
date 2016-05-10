#include "TArgonneHit.h"

#include "TGEBEvent.h"
#include "TGRUTOptions.h"

ClassImp(TArgonneHit)

//bool TArgonneHit::fExtractWaves = true;

TArgonneHit::TArgonneHit(){
  //fOwnWave = false;
  //wave = NULL;
  //wavesize = 0;
  //for(int i=0;i<MAXTRACE;i++) {wavebuffer[i]=0;}
}

TArgonneHit::~TArgonneHit() {
  //if(fOwnWave && wave) {
  //  wave = 0;
  //  delete wave;
  //}
}




void TArgonneHit::BuildFrom(TSmartBuffer& buf){
  Clear();

  bool read_waveform = TGRUTOptions::Get()->ExtractWaves();
  if (read_waveform) {
    throw std::invalid_argument(
      "void TArgonneHit::BuildFrom(TSmartBuffer& buf) :: Waveforms not supported in GEBArgonne data.");
  }

  // Extract header data. Header format should stay constant pending FW updates
  auto header = (TRawEvent::GEBArgonneHead*)buf.GetData();
  buf.Advance(sizeof(TRawEvent::GEBArgonneHead));
  // Swap big endian for little endian
  TRawEvent::SwapArgonneHead(*header);
  // Extract header data
  global_addr = header->GetGA();
  board_id = header->GetBoardID();
  channel = header->GetChannel();
  led = header->GetLED();
  cfd = 0;

  // Extract payload data. Two versions LED and CFD, with small changes for different FW versions
  switch( static_cast<TRawEvent::ArgonneType>(header->GetHeaderType()) ) {
  case TRawEvent::ArgonneType::LEDv10: {
    throw std::invalid_argument(
      "void TArgonneHit::BuildFrom(TSmartBuffer& buf) :: ArgonneType::LEDv10 is not implemented.");
    break;
  }
  case TRawEvent::ArgonneType::LEDv11: {
    auto data = (TRawEvent::GEBArgonneLEDv11*)buf.GetData();
    buf.Advance(sizeof(TRawEvent::GEBArgonneLEDv11));
    // Swap big endian for little endian
    TRawEvent::SwapArgonneLEDv11(*data);
    // Extract data from payload
    led_prev = data->GetPreviousLED();
    flags = data->flags;
    prerise_energy = data->GetPreRiseE();
    postrise_energy = data->GetPostRiseE();
    postrise_begin_sample = data->GetPostRiseSampleBegin();
    prerise_begin_sample = data->GetPreRiseSampleBegin();
    postrise_end_sample = data->GetPostRiseSampleEnd();
    prerise_end_sample = data->GetPreRiseSampleEnd();

    // ignore waveform data
    size_t wave_bytes = header->GetLength()*4 - sizeof(*header) - sizeof(*data);
    buf.Advance(wave_bytes);

    break;
  }
  case TRawEvent::ArgonneType::CFDv11: {
    throw std::invalid_argument(
      "void TArgonneHit::BuildFrom(TSmartBuffer buf) :: ArgonneType::CFDv11 is not implemented.");
    break;
  }
  }

}





void TArgonneHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  // TArgonneHit& mode3 = (TArgonneHit&)obj;


  // mode3.board_id = board_id;

  // mode3.charge   = charge;
  // mode3.wavesize = wavesize;
  // mode3.led      = led;
  // mode3.cfd      = cfd;

  // if(ExtractWaves() && wavesize>0) {


  //   memcpy(mode3.wavebuffer,wavebuffer,wavesize*(sizeof(Short_t)));
  //   mode3.wave = &(mode3.wavebuffer[0]);
  // } else {
  //   mode3.wave=0;
  // }

}


void TArgonneHit::Print(Option_t *opt) const { }

void TArgonneHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  // board_id = -1;
  // charge   = -1;
  // led      = -1;
  // cfd      = -1;
  // //if(fOwnWave && wave) {
  // //  delete wave;
  // //}
  // ClearWave();
  // //raw_data.clear();
}

// void TArgonneHit::ClearWave(Option_t *opt) {
//   for(int i=0;i<wavesize;i++) {
//     if(i>MAXTRACE)
//       break;
//     //std::cout << "Clear wave  " << i << std::endl;
//     wavebuffer[i] = 0;
//   }
//   wave = NULL;
//   wavesize =  0;
// }

// double TArgonneHit::AverageWave(int samples) {
//   //printf("wavesize = %i\n",wavesize);
//   if(wavesize<1)
//     return 0.0;
//   if(samples<0 || samples >wavesize)
//     samples = wavesize;
//   double avg = 0.0;
//   for(int i=0;i<samples;i++) {
//     //printf("wavebiffer[%i] = %i\n",i,wave[i]);
//     avg += wave[i];
//   }
//   return avg/((double)samples);
// }
