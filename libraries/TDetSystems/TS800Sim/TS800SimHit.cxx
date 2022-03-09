#include "TS800SimHit.h"

#include <cmath>
#include <set>

#include <TRandom.h>

#include "TS800Sim.h"



TS800SimHit::TS800SimHit(){ Clear(); }

TS800SimHit::~TS800SimHit(){ }

void TS800SimHit::Copy(TObject &rhs) const {
  TDetectorHit::Copy(rhs);
  ((TS800SimHit&)rhs).fATA      = fATA;
  ((TS800SimHit&)rhs).fBTA      = fBTA;
  ((TS800SimHit&)rhs).fDTA      = fDTA;
  ((TS800SimHit&)rhs).fYTA      = fYTA;


}

const char *TS800SimHit::GetName() const {
  std::string name = "name";
  return name.c_str();
}

void TS800SimHit::Print(Option_t *opt) const {

  std::cout << "TS800SimHit:" <<  std::endl;
  //std::cout << "\tAddress:        \t0x" << std::hex << fAddress << std::dec << std::endl;
  std::cout << "\tATA:      \t" << GetATA() << std::endl;
  std::cout << "\tBTA:      \t" << GetBTA() << std::endl;
  std::cout << "\tDTA:      \t" << GetDTA() << std::endl;
  std::cout << "\tYTA:      \t" << GetYTA() << std::endl;
  std::cout << "------------------------------"  << std::endl;

}

void TS800SimHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fATA=sqrt(-1);
  fBTA=sqrt(-1);
  fDTA=sqrt(-1);
  fYTA=sqrt(-1);

 
}

