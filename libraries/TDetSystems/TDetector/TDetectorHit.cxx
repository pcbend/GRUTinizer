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
}

void TDetectorHit::Print(Option_t *opt) const { }

void TDetectorHit::Copy(TObject& obj) const {
  TObject::Copy(obj);

  TDetectorHit& hit = (TDetectorHit&)obj;
  hit.fAddress = fAddress;
  hit.fCharge = fCharge;
  hit.fTime = fTime;
  hit.fTimestamp = fTimestamp;
}

double TDetectorHit::GetEnergy() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(!chan){
    return Charge() + gRandom->Uniform();
  }
  return chan->CalEnergy(Charge());
}

double TDetectorHit::GetTime() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(!chan){
    return Time() + gRandom->Uniform();
  }
  return chan->CalTime(Time());
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
