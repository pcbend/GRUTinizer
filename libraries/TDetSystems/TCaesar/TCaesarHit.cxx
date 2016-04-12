#include "TCaesarHit.h"

#include "TCaesar.h"

TCaesarHit::TCaesarHit(const TCaesarHit &hit) {
  hit.Copy(*this);
}

void TCaesarHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TCaesarHit& hit = (TCaesarHit&)obj;

  hit.fVsn = fVsn;
  hit.fChannel = fChannel;
  hit.fDet = fDet;
  hit.fRing = fRing;
}


void TCaesarHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);
  fVsn = -1;;
  fChannel = -1;
}

void TCaesarHit::Print(Option_t *opt) const {
  printf("Det[%03i]\tCharge: %i\n",GetDetectorNumber(),Charge());
}

TVector3 TCaesarHit::GetPosition() const { return TCaesar::GetPosition(this); }


