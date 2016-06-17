#include "TFastScintHit.h"
#include "TFastScint.h"
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
  //hit.fCharge  = fCharge;
  hit.fChannel = fChannel;
  //hit.fTime    = fTime;
  //hit.fEnergy  = fEnergy;
}

void TFastScintHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);

  fChannel = -1;
  //fCharge  = -1;
  //fTime    = -1;
  //fEnergy  = -1;
}

void TFastScintHit::Print(Option_t *opt) const {
  printf("LaBrHit[%i]:   chg[%04i] -> %.02f  |  time[%04i] -> %.02f \n",
          fChannel,Charge(),GetEnergy(),Time(),GetTime());
}


TVector3 &TFastScintHit::GetPosition() const { return TFastScint::GetPosition(fChannel); }


/*

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
    //fEnergy = charge;
    //fEnergy = 1.2766*charge - 902.16; //run589
    //fEnergy = 1.3208*charge - 917.47; //run618
    fEnergy = 1.2971*charge - 893.18; //run687
    break;
  case 1:
    //fEnergy = 2.3212*charge + 119.19; //run279
    //fEnergy = 0.626*charge + 39.343; //run290
    //fEnergy = 1.2091*charge + 48.513; //run340
    //fEnergy = charge;
    //fEnergy = 0.7438*charge + 32.719; //run589
    //fEnergy = 0.7402*charge + 33.425; //run618
    fEnergy = 0.7330*charge + 39.05; //run687
    break;
  case 2:
    //fEnergy = 2.5645*charge + 97.185; //run279
    //fEnergy = 0.6868*charge + 20.851; //run290
    //fEnergy = 1.4521*charge + 22.973; //run340
    //fEnergy = charge;
    //fEnergy = 0.8068*charge + 8.3474; //run589
    //fEnergy = 0.8225*charge + 6.7178; //run618
    fEnergy = 0.8171*charge + 12.864; //run687
    break;
  case 3:
    //fEnergy = 5.129*charge + 275.42; //run279
    //fEnergy = 0.7114*charge + 39.013; //run290
    //fEnergy = charge;
    //fEnergy = 0.8314*charge + 22.602; //run618
    fEnergy = 0.8331*charge + 25.992; //run687
    break;
  case 4:
    //fEnergy = 2.8393*charge + 81.295; //run279
    //fEnergy = 0.77*charge + 5.3317; //run290
    //fEnergy = charge;
    //fEnergy = 0.7995*charge - 14.056; //run589
    //fEnergy = 0.8081*charge - 14.892; //run618
    fEnergy = 0.8031*charge - 10.783; //run687
    break;
  case 5:
    //fEnergy = 2.3643*charge + 61.182; //run279
    //Energy = 0.6424*charge + 9.8909; //run290
    //fEnergy = charge;
    //fEnergy = 0.8124*charge - 7.5045; //run589
    //fEnergy = 0.8216*charge - 7.1619; //run618
    fEnergy = 0.8195*charge - 4.4342; //run687
    break;
  case 6:
    //fEnergy = 3.4011*charge + 69.353; //run279
    //fEnergy = 0.9164*charge + 27.009; //run290
    //fEnergy = charge;
    //fEnergy = 0.8037*charge - 8.9718; //run589
    //fEnergy = 0.8006*charge - 8.4897; //run618
    fEnergy = 0.7908*charge - 6.0767; //run687
    break;
  case 7:
    //fEnergy = 3.3474*charge + 96.821; //run279
    //fEnergy = 0.8933*charge + 15.337; //run290
    //fEnergy = charge;
    //fEnergy = 0.7595*charge - 8.7219; //run618
    fEnergy = 0.7549*charge - 4.2764; //run687
    break;
  case 8:
    //fEnergy = charge;
    //fEnergy = 0.7709*charge - 20.417;  //run589
    //fEnergy = 0.7773*charge - 20.66; //run618
    fEnergy = 0.7748*charge - 17.672; //run687
    break;
  case 9:
    //fEnergy = charge;
    fEnergy = 0.7771*charge + 9.7792; //run687
    break;
  case 10:
    //fEnergy = charge;
    fEnergy = 0.7748*charge + 3.8723; //run687
    break;
  case 11:
    //fEnergy = charge;
    fEnergy = 0.7686*charge - 3.7752; //run687
    break;
  case 12:
    fEnergy = charge;
    //fEnergy = 0.7512*charge - 1.151; //run589
    break;
  case 13:
    //fEnergy = charge;
    //fEnergy = 0.8331*charge + 5.9087; //run589
    fEnergy = 0.7619*charge - 29.451; //run687
    break;
  case 14:
    //fEnergy = charge;
    fEnergy = 0.7457*charge - 12.069; //run687
    break;
  case 15:
    //fEnergy = charge;
    fEnergy = 0.8118*charge - 5.1587; //run687
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
*/
