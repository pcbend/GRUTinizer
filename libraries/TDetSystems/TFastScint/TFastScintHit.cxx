#include "TFastScintHit.h"
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
  case 0:
    //fEnergy = 8.3684*charge - 446.29; //825 V different base
    //fEnergy = 5.0476*charge - 830.9;  //1000 V different base
    //fEnergy = 4.0253*charge - 1012.7; //1100 V different base
    //fEnergy = 3.2121*charge + 174.03; //run279
    //fEnergy = 1.0095*charge - 758.72; //run290
    fEnergy = charge;
    break;
  case 1:
    //fEnergy = 2.3212*charge + 119.19; //run279
    //fEnergy = 0.626*charge + 39.343; //run290
    //fEnergy = 1.2091*charge + 48.513; //run340
    fEnergy = charge;
    break;
  case 2:
    //fEnergy = 2.5645*charge + 97.185; //run279
    //fEnergy = 0.6868*charge + 20.851; //run290
    //fEnergy = 1.4521*charge + 22.973; //run340
    fEnergy = charge;
    break;
  case 3:
    //fEnergy = 5.129*charge + 275.42; //run279
    //fEnergy = 0.7114*charge + 39.013; //run290
    fEnergy = charge;
    break;
  case 4:
    //fEnergy = 2.8393*charge + 81.295; //run279
    //fEnergy = 0.77*charge + 5.3317; //run290
    fEnergy = charge;
    break;
  case 5:
    //fEnergy = 2.3643*charge + 61.182; //run279
    //Energy = 0.6424*charge + 9.8909; //run290
    fEnergy = charge;
    break;
  case 6:
    //fEnergy = 3.4011*charge + 69.353; //run279
    //fEnergy = 0.9164*charge + 27.009; //run290
    fEnergy = charge;
    break;
  case 7:
    //fEnergy = 3.3474*charge + 96.821; //run279
    //fEnergy = 0.8933*charge + 15.337; //run290
    fEnergy = charge;
    break;
  case -10:
    fEnergy = -1;
    break;
  }

  //Old Calibration Settings
  //switch(fChannel){ 
  //case 3:
    //fEnergy = 3.5333*charge + 176.6;
    //break;
  //case 5:
    //fEnergy = 3.4565*charge + 56.543;
    //break;
  //case 11:
    //fEnergy = 4.9688*charge + 174.28;
    //break;
  //case 13:
    //fEnergy = 1.8068*charge + 78.068;
    //break;
  //case -10:
    //fEnergy = -1;
    //break;
  //}

  if(fCharge<0){
     fEnergy = -1;
  }

}
