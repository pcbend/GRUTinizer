#include "TGretSimHit.h"

#include <cmath>
#include <set>

#include <TRandom.h>

#include "TGretSim.h"



TGretSimHit::TGretSimHit(){ Clear(); }

TGretSimHit::~TGretSimHit(){ }

void TGretSimHit::Copy(TObject &rhs) const {
  TDetectorHit::Copy(rhs);
  ((TGretSimHit&)rhs).fEnergy      = fEnergy;
  ((TGretSimHit&)rhs).fPosit       = fPosit;
  ((TGretSimHit&)rhs).fBeta        = fBeta;
  ((TGretSimHit&)rhs).fIsFull      = fIsFull;
  ((TGretSimHit&)rhs).fTotalHits   = fTotalHits;
  ((TGretSimHit&)rhs).fHitNum      = fHitNum;

}

const char *TGretSimHit::GetName() const {
  std::string name = "name";
  return name.c_str();
}

void TGretSimHit::Print(Option_t *opt) const {

  std::cout << "TGretSimHit:" <<  std::endl;
  //std::cout << "\tAddress:        \t0x" << std::hex << fAddress << std::dec << std::endl;
  std::cout << "\tGamma 1 En:      \t" << GetEn()            << std::endl;
  std::cout << "\tGamma 1 X :      \t" << GetX()                << std::endl;
  std::cout << "\tGamma 1 X :      \t" << GetY()                << std::endl;
  std::cout << "\tGamma 1 X :      \t" << GetZ()                << std::endl;
  std::cout << "\tBeta 1    :      \t" << GetBeta()             << std::endl;
  std::cout << "------------------------------"  << std::endl;

}

void TGretSimHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fEnergy=sqrt(-1);
  fPosit.SetMagThetaPhi(sqrt(-1),sqrt(-1),sqrt(-1));
  fInteraction.SetXYZ(sqrt(-1),sqrt(-1),sqrt(-1));
  fBeta=sqrt(-1);
  fIsFull=sqrt(-1);
  fTotalHits=sqrt(-1);
  fHitNum=sqrt(-1);
 
}

