#include "TRCNPEvent.h"
#include "RCNPEvent.h"

#include "TString.h"


ClassImp(TRCNPEvent)

TRCNPEvent::TRCNPEvent() { }

TRCNPEvent::TRCNPEvent(const TRawEvent &raw) {
  raw.Copy(*this);
}

TRCNPEvent::~TRCNPEvent() { }

Long_t TRCNPEvent::GetTimestamp() const {
  return event->GetTimestamp();
}

const char* TRCNPEvent::GetPayload() const {
  return fBody.GetData() + sizeof(Long_t);
}

TSmartBuffer TRCNPEvent::GetPayloadBuffer() const {
  return fBody;
}

void TRCNPEvent::Clear(Option_t *opt) {
  TRawEvent::Clear(opt);
}

void TRCNPEvent::Print(Option_t *opt) const {
  TRawEvent::Print(opt);
}
