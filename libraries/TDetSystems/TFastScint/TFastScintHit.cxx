#include "TFastScintHit.h"
#include "TRandom.h"

void TFastScintHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TFastScintHit& hit = (TFastScintHit&)obj;
  hit.fCharge  = fCharge;
  hit.fChannel = fChannel;
  hit.fTime    = fTime;
  hit.fEnergy  = fEnergy;
}

void TFastScintHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);

  fChannel = -1;
  fCharge  = -1;
  fTime    = -1;
  fEnergy  = -1;
}

void TFastScintHit::SetEnergy(){
  double charge = fCharge;
  charge+=gRandom->Uniform();
  
  switch(fChannel){
  case 3:
    fEnergy = 3.5333*charge + 176.6;
    break;
  case 5:
    fEnergy = 3.4565*charge + 56.543;
    break;
  case 11:
    fEnergy = 4.9688*charge + 174.28;
    break;
  case 13:
    fEnergy = 1.8068*charge + 78.068;
    break;
  case -10:
    fEnergy = -1;
    break;
  }

  if(fCharge<0){
     fEnergy = -1;
  }

}
