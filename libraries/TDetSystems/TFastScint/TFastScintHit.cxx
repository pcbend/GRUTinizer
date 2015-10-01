#include "TFastScintHit.h"

void TFastScintHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TFastScintHit& hit = (TFastScintHit&)obj;
  hit.fChannel = fChannel;
  hit.fTime = fTime;
}

void TFastScintHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);

  fChannel = -1;
  fTime = -1;
}
