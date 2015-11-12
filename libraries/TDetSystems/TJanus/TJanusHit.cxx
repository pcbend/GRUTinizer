#include "TJanusHit.h"

void TJanusHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TJanusHit& hit = (TJanusHit&)obj;
  hit.fCharge       = fCharge;
  hit.fChannel      = fChannel;

  hit.fEnergyEntryType    = fEnergyEntryType;
  hit.fEnergyOverflowBit  = fEnergyOverflowBit;
  hit.fEnergyUnderflowBit = fEnergyUnderflowBit;

  hit.fTimeEntryType    = fTimeEntryType;
  hit.fTimeOverflowBit  = fTimeOverflowBit;
  hit.fTimeUnderflowBit = fTimeUnderflowBit;
}

void TJanusHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);

  fCharge       = -1;
  fChannel      = -1;

  fEnergyEntryType    = -1;
  fEnergyOverflowBit  = -1;
  fEnergyUnderflowBit = -1;
  fTimeEntryType    = -1;
  fTimeOverflowBit  = -1;
  fTimeUnderflowBit = -1;
}
