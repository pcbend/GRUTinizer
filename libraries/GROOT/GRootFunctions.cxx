
#include "GRootFunctions.h"


NamespaceImp(GRootFunctions);


#define PI TMATH::Pi()

Double_t GRootFunctions::PolyBg(Double_t *dim, Double_t *par, Int_t order) {
  Double_t result = 0.0;
  for(Int_t i=0;i<order;i++)
    result += par[i]*TMath::Power(dim[0]-par[order+1],i);
  return result;
}


Double_t GRootFunctions::StepFunction(Double_t *dim, Double_t *par) {
  //  -dim[0]: channels to fit
  //  -par[0]: height of peak
  //  -par[1]: centroid of peak
  //  -par[2]: sigma of peak
  //  -par[5]: size of step in step function.

  Double_t x       = dim[0];
  Double_t height  = par[0];
  Double_t cent    = par[1];
  Double_t sigma   = par[2];
  Double_t step    = par[5];

  return TMath::Abs(step)*height/100.0*TMath::Erfc((x-cent)/(TMath::Sqrt(2.0)*sigma));

}

Double_t GRootFunctions::StepBG(Double_t *dim, Double_t *par) {
  return StepFunction(dim,par) + PolyBg(dim,&par[6],2);
}

Double_t GRootFunctions::Gaus(Double_t *dim, Double_t *par) {
  // -dim[0]: channels to fit
  // -par[0]: height of peak
  // -par[1]: cent of peak
  // -par[2]: sigma
  // -par[4]: relative height of skewed gaus to gaus
  
  Double_t x      = dim[0];
  Double_t height = par[0];
  Double_t cent   = par[1];
  Double_t sigma  = par[2];
  Double_t R      = par[4];

  return height*(1.0-R/100.0)*TMath::Gaus(x,cent,sigma);
}

Double_t GRootFunctions::SkewedGaus(Double_t *dim,Double_t *par) {

  // - dim[0]: channels to fit
  // - par[0]: height of peak
  // - par[1]: cent of peak
  // - par[2]: sigma
  // - par[3]: "skewedness" of the skewed gaussin
  // - par[4]: relative height of skewed gaus to gaus 

  Double_t x      = dim[0]; //channel number used for fitting
  Double_t height = par[0]; //height of photopeak
  Double_t cent   = par[1]; //Peak Centroid of non skew gaus
  Double_t sigma  = par[2]; //standard deviation of gaussian
  Double_t beta   = par[3]; //"skewedness" of the skewed gaussian
  Double_t R      = par[4]; //relative height of skewed gaussian
 
 return R*height/100.0*(TMath::Exp((x-cent)/beta))*(TMath::Erfc(((x-cent)/(TMath::Sqrt(2.0)*sigma)) + sigma/(TMath::Sqrt(2.0)*beta)));

}



Double_t GRootFunctions::PhotoPeak(Double_t *dim,Double_t *par) {
  return Gaus(dim,par) + SkewedGaus(dim,par);
}

Double_t GRootFunctions::PhotoPeakBG(Double_t *dim,Double_t *par) {
  return Gaus(dim,par) + SkewedGaus(dim,par) + StepFunction(dim,par) + PolyBg(dim,&par[6],2);
}








