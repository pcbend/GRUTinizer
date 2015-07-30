#include "TJanusHit.h"

void TJanusHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TJanusHit& hit = (TJanusHit&)obj;
  hit.fEntryType = fEntryType;
  hit.fChannel = fChannel;
  hit.fOverflowBit = fOverflowBit;
  hit.fUnderflowBit = fUnderflowBit;
}
