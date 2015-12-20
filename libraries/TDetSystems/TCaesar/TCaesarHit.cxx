#include "TCaesarHit.h"

#include "TCaesar.h"

void TCaesarHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TCaesarHit& hit = (TCaesarHit&)obj;

  hit.fVsn = fVsn;
  hit.fChannel = fChannel;
  hit.fCharge = fCharge;
  hit.fTime = fTime;
}

void TCaesarHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);

  fVsn = -1;;
  fChannel = -1;
  fCharge = -1;
  fTime = -1;
}

