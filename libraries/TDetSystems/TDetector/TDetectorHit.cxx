#include "TDetectorHit.h"

#include <iostream>

#include "TRandom.h"

ClassImp(TDetectorHit)

const TVector3 TDetectorHit::BeamUnitVec(0,0,1);

TDetectorHit::TDetectorHit() {
  Clear();
}

TDetectorHit::~TDetectorHit() { }

void TDetectorHit::Clear(Option_t *opt) {
  TObject::Clear(opt);
  fAddress = -1;
  fCharge = -1;
}

void TDetectorHit::Print(Option_t *opt) const { }

void TDetectorHit::Copy(TObject& obj) const {
  TObject::Copy(obj);

  TDetectorHit& hit = (TDetectorHit&)obj;
  hit.fAddress = fAddress;
  hit.fCharge = fCharge;
}

double TDetectorHit::GetEnergy() const {
  return fCharge + gRandom->Uniform();
}

Int_t TDetectorHit::Compare(const TObject *obj) const{
  const TDetectorHit* other = (const TDetectorHit*)obj;
  if(GetEnergy() < other->GetEnergy()) {
    return -1;
  } else if (GetEnergy() > other->GetEnergy()) {
    return +1;
  } else {
    return 0;
  }
}
