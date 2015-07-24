#include "TDetectorHit.h"

#include <iostream>

ClassImp(TDetectorHit)

TVector3 TDetectorHit::fBeamUnitVec(0,0,1);

TDetectorHit::TDetectorHit() {
  Clear();
}

TDetectorHit::~TDetectorHit() { }

void TDetectorHit::Copy(const TObject &rhs) {
  if(!rhs.InheritsFrom("TDetectorHit")){
    std::cout << "Attempted to copy " << rhs.ClassName() << " into TDetectorHit" << std::endl;
    return;
  }

  TDetectorHit& hit = (TDetectorHit&)rhs;

  TObject::Copy((TObject&)rhs);

  fAddress = hit.fAddress;
  fCharge = hit.fCharge;
  fTime = hit.fTime;
}


void TDetectorHit::Clear(Option_t *opt) {
  TObject::Clear(opt);
  fAddress = -1;
  fCharge = -1;
  fTime = -1;
}

void TDetectorHit::Print(Option_t *opt) { }
