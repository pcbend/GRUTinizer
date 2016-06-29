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
  discriminator = header->GetDisc();

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
    disc_prev = data->GetPreviousLED();
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
    throw std::invalid_argument(
      "void TANLEvent::BuildFrom(TSmartBuffer buf) :: ArgonneType::LEDv18 is not implemented.");
    break;
    // auto data = (TRawEvent::GEBArgonneLEDv11*)buf.GetData();
    // buf.Advance(sizeof(TRawEvent::GEBArgonneLEDv11));
    // // Swap big endian for little endian
    // TRawEvent::SwapArgonneLEDv11(*data);
    // // Extract data from payload
    // disc_prev = data->GetPreviousLED();
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
  case TRawEvent::ArgonneType::CFDv18: {
    auto data = (TRawEvent::GEBArgonneCFDv18*)buf.GetData();
    buf.Advance(sizeof(TRawEvent::GEBArgonneCFDv18));
    // Swap big endian for little endian
    TRawEvent::SwapArgonneCFDv18(*data);
    // Extract data from payload
    //discriminator = data->GetCFD0(); // this should be a function to interpolate the zero crossing
    disc_prev = data->GetPrevCFD(header);
    flags = data->flags;
    prerise_energy = data->GetPreRiseE();
    postrise_energy = data->GetPostRiseE();
    postrise_begin_sample = data->GetPostRiseSampleBegin();
    prerise_begin_sample = data->GetPreRiseSampleBegin();
    postrise_end_sample = data->GetPostRiseSampleEnd();
    prerise_end_sample = data->GetPreRiseSampleEnd();


    size_t wave_bytes = header->GetLength()*4 - sizeof(*header) - sizeof(*data); // labr 1.52us
    // // trace analysis here
    // for (auto i=0u; i<wave_bytes; i+=sizeof(UShort_t)) {
    //   wave_data.push_back(((UShort_t*)buf.GetData())[0] & 0x3fff);
    //   buf.Advance(sizeof(UShort_t));
    // }

    // advance the buffer to the next event (sometimes there is an extra 4bytes which must be skipped)
    buf.Advance(wave_bytes);


    // const char* temp = buf.GetData();

    // // check if the next byte is the GEB type id 0xe for ANL firmware events (DGS)
    // if (*temp != 0xe) {
    //   buf.Advance(4);
    //   temp = buf.GetData();
    // }
    // // if after advancing we are still not on a geb header, then print out the bytes for debugging
    // if (*temp != 0xe) {
    //   for (int i=0;i<64;i++){
    //     if(i%2==0)std::cout << " ";
    //     if(i%16==0)std::cout << std::endl;
    //     std::cout << std::setfill('0') << std::setw(2) <<std::right << std::hex << (int)((unsigned char*)temp)[i];
    //   } std::cout << std::endl<<std::endl;
    // }

    break;
  }
  }

}



TANLEvent::~TANLEvent() {
  //if(fOwnWave && wave) {
  //  wave = 0;
  //  delete wave;
  //}
}



