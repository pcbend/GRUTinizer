#include "TFastScintHit.h"
#include "TFastScint.h"
#include "TRandom.h"


TFastScintHit::TFastScintHit() {
  Clear();
}


TFastScintHit::TFastScintHit(const TDetectorHit &hit) {
  Clear();
  hit.Copy(*this);
}

void TFastScintHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TFastScintHit& hit = (TFastScintHit&)obj;
  hit.fTimes  = fTimes;
  //hit.fCharge  = fCharge;
  //  hit.fChannel = fChannel;
  //hit.fTime    = fTime;
  //hit.fEnergy  = fEnergy;
}

void TFastScintHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);
  fTimes.clear();
  //  fChannel = -1;
  //fCharge  = -1;
  //fTime    = -1;
  //fEnergy  = -1;
}

void TFastScintHit::Print(Option_t *opt) const {
  printf("LaBrHit[%i]:   chg[%04i] -> %.02f  |  time[%04i] -> %.02f \n",
	 GetChannel(),Charge(),GetEnergy(),Time(),GetTime());
}


TVector3 &TFastScintHit::GetPosition() const { return TFastScint::GetPosition(GetChannel()); }

