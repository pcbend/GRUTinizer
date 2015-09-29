#include "TGEBEvent.h"

#include "TString.h"

#include "TMode3.h"

ClassImp(TGEBEvent);

TGEBEvent::TGEBEvent() { }

TGEBEvent::TGEBEvent(const TRawEvent &raw) {
  raw.Copy(*this);
}

TGEBEvent::~TGEBEvent() { }

Long_t TGEBEvent::GetTimestamp() const {
  return *((Long_t*)(TRawEvent::GetBody() + 0));
}

const char* TGEBEvent::GetPayload() const {
  return fBody.GetData() + sizeof(Long_t);
}

TSmartBuffer TGEBEvent::GetPayloadBuffer() const {
  return fBody.BufferSubset(sizeof(Long_t));
}

void TGEBEvent::Clear(Option_t *opt) {
  TRawEvent::Clear(opt);
}

void TGEBEvent::Print(Option_t *opt) const {
  TString options(opt);
  std::cout << BLUE << "Type:     \t" << DYELLOW << GetEventType() << RESET_COLOR << std::endl;
  std::cout << BLUE << "Size:     \t" << DYELLOW << GetBodySize()  << RESET_COLOR << std::endl;
  std::cout << BLUE << "Timestamp:\t" << DYELLOW << GetTimestamp() << BLUE << "  tens of ns" << RESET_COLOR << std::endl;
  if(options.Contains("all")) {
    TString pass_opt("bodyonly");
    pass_opt.Append(options);
    TRawEvent::Print(pass_opt.Data());
  }
};

ClassImp(TGEBMode3Event);

void TGEBMode3Event::BuildFragments(){
  TSmartBuffer buf = fEvent.GetPayloadBuffer();
  TGEBEvent event(fEvent);
  TRawEvent::GEBMode3Head header;
  TRawEvent::GEBMode3Data data;

  while(buf.GetSize()){
    // Read the header and body
    memcpy((char*)&header, buf.GetData(), sizeof(TRawEvent::GEBMode3Head));
    TRawEvent::SwapMode3Head(header);

    memcpy((char*)&data, buf.GetData(), sizeof(TRawEvent::GEBMode3Data));
    TRawEvent::SwapMode3Data(data);

    //header.GetLength() is number of 32-bit values, and does not include the 0xaaaaaaaa separator.
    size_t body_size = header.GetLength()*4 + 4;

    // Transfer the timestamp and body
    size_t buf_size = body_size + sizeof(Long_t);
    char* new_buf = (char*)malloc(buf_size);
    *(Long_t*)new_buf = data.GetCfd();
    memcpy(new_buf + sizeof(Long_t), buf.GetData(), body_size);
    event.SetData(TSmartBuffer(new_buf, body_size));

    // Push a copy into the list
    fragments.push_back(fEvent);
    buf.Advance(body_size);
  }
}

// TODO remove commented code
// TGEBMode3Event::TGEBMode3Event(const TGEBEvent& event)
//   : TGEBEvent(event) {
//   buf = event.GetPayloadBuffer();
// }

// TGEBMode3Event::~TGEBMode3Event() {
// }

// bool TGEBMode3Event::GetNextItem(TMode3& output, bool read_waveform){
//   if(!buf.GetSize()){
//     return false;
//   }

//   output.BuildFrom(buf, read_waveform);
//   return true;
// }

// TSmartBuffer TGEBMode3Event::GetNextItem(bool read_waveform){
//   if(!buf.GetSize()){
//     return false;
//   }

//   output.BuildFrom(buf, read_waveform);
//   return true;
// }
