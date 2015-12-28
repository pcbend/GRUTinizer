#include "TTransition.h"

ClassImp(TTransition)

TTransition::TTransition(){
  Clear();
}

TTransition::~TTransition(){
  // empty
}

void TTransition::Clear(Option_t *opt){
  fEnergy         = 0;
  fEnUncertainty  = 0;
  fIntensity      = 0;
  fIntUncertainty = 0;
}

void TTransition::Print(Option_t *opt) const{

   printf("**************************\n");
   printf("TGRSITransition:\n");
   printf("Energy:    %f\t+/-%f\n",fEnergy,fEnUncertainty);
   printf("Intensity: %f\t+/-%f\n",fIntensity,fIntUncertainty);
   printf("**************************\n");
}

std::string TTransition::PrintToString(){
  std::string toString;
  toString.append(Form("%f\t",fEnergy));
  toString.append(Form("%f\t",fEnUncertainty));
  toString.append(Form("%f\t",fIntensity));
  toString.append(Form("%f\t",fIntUncertainty));

  return toString;
}

Int_t TTransition::CompareIntensity(const TObject *obj) const {
  if(this->fIntensity > ((TTransition*)obj)->fIntensity)
    return -1;
  else if(this->fIntensity < ((TTransition*)obj)->fIntensity)
    return 0;
  else
    return 1;

  printf(" *** Error : Intensity not comparable to provided intensity!!!\n");
  return -9;
}
