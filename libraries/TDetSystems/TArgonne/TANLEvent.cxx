#include "TANLEvent.h"

#include "TGEBEvent.h"
#include "TGRUTOptions.h"

ClassImp(TANLEvent)

//bool TANLEvent::fExtractWaves = true;

TANLEvent::TANLEvent(TSmartBuffer& buf) {

  bool read_waveform = TGRUTOptions::Get()->ExtractWaves();
  if (read_waveform) {
    throw std::invalid_argument(
      "void TANLEvent::BuildFrom(TSmartBuffer& buf) :: Waveforms not supported in GEBArgonne data.");
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
      "void TANLEvent::BuildFrom(TSmartBuffer& buf) :: ArgonneType::LEDv10 is not implemented.");
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
      "void TANLEvent::BuildFrom(TSmartBuffer buf) :: ArgonneType::CFDv11 is not implemented.");
    break;
  }
  case TRawEvent::ArgonneType::LEDv18: {
    // auto data = (TRawEvent::GEBArgonneLEDv11*)buf.GetData();
    // buf.Advance(sizeof(TRawEvent::GEBArgonneLEDv11));
    // // Swap big endian for little endian
    // TRawEvent::SwapArgonneLEDv11(*data);
    // // Extract data from payload
    // led_prev = data->GetPreviousLED();
    // flags = data->flags;
    // prerise_energy = data->GetPreRiseE();
    // postrise_energy = data->GetPostRiseE();
    // postrise_begin_sample = data->GetPostRiseSampleBegin();
    // prerise_begin_sample = data->GetPreRiseSampleBegin();
    // postrise_end_sample = data->GetPostRiseSampleEnd();
    // prerise_end_sample = data->GetPreRiseSampleEnd();

    // // ignore waveform data
    // size_t wave_bytes = header->GetLength()*4 - sizeof(*header) - sizeof(*data);
    // buf.Advance(wave_bytes);

    break;
  }
  case TRawEvent::ArgonneType::CFDv18:
    break;
  }

}



TANLEvent::~TANLEvent() {
  //if(fOwnWave && wave) {
  //  wave = 0;
  //  delete wave;
  //}
}



