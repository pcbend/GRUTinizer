
#include <TFSUHit.h>
#include <TChannel.h>
#include <GValue.h>


#include <TMath.h>

TFSUHit::TFSUHit() { }

TFSUHit::~TFSUHit() { }


void TFSUHit::Clear(Option_t *opt) { 
  TDetectorHit::Clear(); 

}

void TFSUHit::Print(Option_t *opt) const {

  printf("---------------------------\n");
  printf("TFSUHit @ %lu\n",Timestamp());
  printf("\tAddress:  0x%08x\n",Address());
  printf("Name:       %s\n",GetName());
  printf("\tCharge:   %i\n",Charge());
  printf("\tTime:     %i\n",Time());
  GetPosition().Print();
  printf("---------------------------\n");
 }


TVector3 TFSUHit::GetPosition() const {
  TChannel *chan = TChannel::GetChannel(Address());
  if(chan) 
    return chan->GetChannelPosition();
  return TVector3(0,0,1);
}


double TFSUHit::GetDoppler(const TVector3 *recoil_vector) const {
  //insert some checks that positions and betas are actually set.....
  double beta = GValue::Value("beta");
  double gamma = 1/(sqrt(1-pow(beta,2)));
  if(recoil_vector==0) {
     recoil_vector = &BeamUnitVec;
  }
  return GetEnergy()*gamma *(1 - beta*TMath::Cos(GetPosition().Angle(*recoil_vector)));
} 


double TFSUHit::GetDoppler(double beta) const {
  //insert some checks that positions and betas are actually set.....
  double gamma = 1/(sqrt(1-pow(beta,2)));
  return GetEnergy()*gamma *(1 - beta*TMath::Cos(GetPosition().Theta()));
} 




