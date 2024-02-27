#include "TPhosWallHit.h"

#include <iostream>

#include "TCutG.h"
#include "TPhosWall.h"



ClassImp(TPhosWallHit)

TPhosWallHit::TPhosWallHit() {
  Clear();
}

TPhosWallHit::TPhosWallHit(TRawEvent::PWHit *hit) {
  Clear();
  fACharge  = hit->data_a;
  fBCharge  = hit->data_b;
  fCCharge  = hit->data_c;
  fPixel    = hit->pix_id;
  fTime     = hit->time;
  fPosition = TPhosWall::GetWallPosition(hit->pix_id);
}

TPhosWallHit::~TPhosWallHit() {  }


void TPhosWallHit::Copy(TObject &rhs) const {
  TDetectorHit::Copy(rhs);

  TPhosWallHit hit = (TPhosWallHit&)rhs;
  hit.fACharge  = fACharge;
  hit.fBCharge  = fBCharge;
  hit.fCCharge  = fCCharge;
  hit.fPixel    = fPixel;
  hit.fTime     = fTime;
  hit.fPosition = fPosition;

}

void TPhosWallHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fACharge  = -1;
  fBCharge  = -1;
  fCCharge  = -1;
  fPixel    = -1;
  fTime     = -1;
  fPosition.SetXYZ(0,0,1);
}


int TPhosWallHit::IsInside(Option_t *opt) const {
  TIter iter(TPhosWall::GetGates());  //&gates);
  int counter =1;
  while(TCutG* cut = (TCutG*)iter.Next()) {
    if(cut->IsInside(B(),C()))
      return counter;
    counter++;
  }
  return 0;
}




void TPhosWallHit::Print(Option_t *opt) const {
  std::cout << "PhosWall Hit[" << fPixel   << "] "
                               << fACharge << "  "
                               << fBCharge << "  "
                               << fCCharge << "  "
                               << fTime    << std::endl;
  if(!strcmp(opt,"all")) {
  std::cout << "\tfPosition: " << "R: "    << fPosition.Mag()
                               << "T: "    << fPosition.Theta()
                               << "P: "    << fPosition.Phi()   << std::endl;
  }
  std::cout << "---------------------" << std::endl;
}
