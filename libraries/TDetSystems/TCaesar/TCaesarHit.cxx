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
  hit.fDet = fDet;
  hit.fRing = fRing;
  hit.numHitsContained = numHitsContained;
  hit.is_garbage_addback = is_garbage_addback;
}


void TCaesarHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);
  fVsn = -1;;
  fChannel = -1;
  numHitsContained = 0;
}

void TCaesarHit::Print(Option_t *opt) const {
//printf("Ring[%03i]\tDet[%03i]\tValid[%d]\tPos\tTime\tHits Contained[%d]\tGarbage[%d]\tEnergy: %1.1f\n",
//       GetRingNumber(), GetDetectorNumber(), IsValid(),GetNumHitsContained(), is_garbage_addback, GetDoppler(0.341));
  TVector3 pos = GetPosition();
  std::cout << "Ring["<<GetRingNumber()<<"]\tDet["<<GetDetectorNumber()<<"]\tPos["
            <<pos.X()<<","<<pos.Y()<<","<<pos.Z()<<"]\tTime["<<GetTime()<<"]\tHits Contained["
            <<GetNumHitsContained()<<"]\tGarbage["<<is_garbage_addback<<"]\tEnergy["
            <<GetDoppler(0.341) << "]" <<std::endl;
            
}

TVector3 TCaesarHit::GetPosition() const {
  return TCaesar::GetPosition(this);
}
TVector3 TCaesarHit::GetPosition(double z_shift) const {
  return TCaesar::GetPosition(this, z_shift);
}

void TCaesarHit::AddToSelf(const TCaesarHit& other) {

//  std::cout << "INSIDE ADD TO SELF" << std::endl;
  double energy = GetEnergy() + other.GetEnergy();
//  int temp_num_hits_contained = -1;
//std::cout << "GetNumHitsContained() = " << GetNumHitsContained()
//          <<  "\tother.GetNumHitsContained() = " << other.GetNumHitsContained() << std::endl;
//if (GetNumHitsContained() < other.GetNumHitsContained()){
//  temp_num_hits_contained = other.GetNumHitsContained() + 1;
//}
//else{
//  temp_num_hits_contained = numHitsContained+1;
//}
  if(GetEnergy()<other.GetEnergy()) {
    other.Copy(*this); 
  }
  numHitsContained++;
//std::cout  <<  "temp_num_hits_contained = " << temp_num_hits_contained << std::endl;
//numHitsContained = temp_num_hits_contained;
//std::cout  <<  "GetNumHitsContained() = " << GetNumHitsContained() << std::endl;
  TDetectorHit::SetEnergy(energy);
}





