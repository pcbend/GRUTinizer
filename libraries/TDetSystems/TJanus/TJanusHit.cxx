#include "TJanusHit.h"

void TJanusHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TJanusHit& hit = (TJanusHit&)obj;
  hit.fEntryType = fEntryType;
  hit.fChannel = fChannel;
  hit.fOverflowBit = fOverflowBit;
  hit.fUnderflowBit = fUnderflowBit;
}

void TJanusHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);

  fEntryType = -1;
  fChannel = -1;
  fOverflowBit = -1;
  fUnderflowBit = -1;
}
