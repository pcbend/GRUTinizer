#include "TDetectorHit.h"

#include <iostream>

#include <TClass.h>
#include "TRandom.h"

ClassImp(TDetectorHit)

const TVector3 TDetectorHit::BeamUnitVec(0,0,1);

TDetectorHit::TDetectorHit() {
  Class()->CanIgnoreTObjectStreamer();
  Clear();
}

TDetectorHit::~TDetectorHit() { }

void TDetectorHit::Clear(Option_t *opt) {
  TObject::Clear(opt);
  fAddress = -1;
  fCharge = -1;
  fTime = -1;
  fTimestamp = -1;
  fFlags = 0;
}

void TDetectorHit::Print(Option_t *opt) const { }

void TDetectorHit::Copy(TObject& obj) const {
  TObject::Copy(obj);

  TDetectorHit& hit = (TDetectorHit&)obj;
  hit.fAddress = fAddress;
  hit.fCharge = fCharge;
  hit.fTime = fTime;
  hit.fTimestamp = fTimestamp;
  hit.fFlags = fFlags;
}

void TDetectorHit::SetCharge(int charge) {
  fCharge = charge + gRandom->Uniform();
  fFlags &= ~kIsEnergy;
}

void TDetectorHit::SetEnergy(double energy) {
  fCharge = energy;
  fFlags |= kIsEnergy;
}

Int_t  TDetectorHit::Charge() const {
  if(fFlags & kIsEnergy) {
    return 0;
  } else {
    return fCharge;
  }
}

double TDetectorHit::GetEnergy() const {
  if(fFlags & kIsEnergy) {
    return fCharge;
  } else {
    TChannel* chan = TChannel::GetChannel(fAddress);
    if(!chan){
      return fCharge;
    } else {
      return chan->CalEnergy(fCharge, fTimestamp);
    }
  }
}

void TDetectorHit::AddEnergy(double eng) {
  SetEnergy(eng + GetEnergy());
}




double TDetectorHit::GetTime() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(!chan){
    return Time() + gRandom->Uniform();
  }
  return chan->CalTime(Time(), fTimestamp);
}


Int_t TDetectorHit::Compare(const TObject *obj) const {
  const TDetectorHit* other = (const TDetectorHit*)obj;
  if(GetEnergy() < other->GetEnergy()) {
    return -1;
  } else if (GetEnergy() > other->GetEnergy()) {
    return +1;
  } else {
    return 0;
  }
}

const char* TDetectorHit::GetName() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetName();
  } else {
    return "";
  }
}
