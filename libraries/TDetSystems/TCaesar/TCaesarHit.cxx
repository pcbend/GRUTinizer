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
}

void TCaesarHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);
  fVsn = -1;;
  fChannel = -1;
}

