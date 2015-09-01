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
  return *(Long_t*)(GetBody() + 0);
}

const char* TGEBEvent::GetPayload() const {
  return fBody.GetData() + sizeof(Long_t);
}

TSmartBuffer TGEBEvent::GetPayloadBuffer() const {
  return fBody.BufferSubset(sizeof(Long_t));
}

bool TGEBEvent::FillCondition() { return true; }


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

TGEBMode3Event::TGEBMode3Event(const TGEBEvent& event)
  : TGEBEvent(event) {
  buf = event.GetPayloadBuffer();
}

TGEBMode3Event::~TGEBMode3Event() {
}

bool TGEBMode3Event::GetNextItem(TMode3& output, bool read_waveform){
  if(!buf.GetSize()){
    return false;
  }

  output.BuildFrom(buf, read_waveform);
  return true;
}










