
#include "GRootFunctions.h"


NamespaceImp(GRootFunctions);


#define PI TMATH::Pi()

Double_t GRootFunctions::PolyBg(Double_t *dim, Double_t *par, Int_t order) {
  Double_t result = 0.0;
  for(Int_t i=0;i<=order;i++)
    result += par[i]*TMath::Power(dim[0]-par[order+1],i);
  return result;
}


Double_t GRootFunctions::StepFunction(Double_t *dim, Double_t *par) {
  //  -dim[0]: channels to fit
  //  -par[0]: height of peak
  //  -par[1]: centroid of peak
  //  -par[2]: sigma of peak
  //  -par[4]: relative height of skewed gaus to gaus
  //  -par[5]: size of step in step function.

  Double_t x       = dim[0];
  Double_t height  = par[0];
  Double_t cent    = par[1];
  Double_t sigma   = par[2];
  Double_t R       = par[4];
  Double_t step    = par[5];

  //return TMath::Abs(step)*height/100.0*TMath::Erfc((x-cent)/(TMath::Sqrt(2.0)*sigma));
  return TMath::Abs(step)*TMath::Erfc((x-cent)/(TMath::Sqrt(2.0)*sigma));

}

Double_t GRootFunctions::StepBG(Double_t *dim, Double_t *par) {
  return StepFunction(dim,par) + PolyBg(dim,&par[6],2);
}

Double_t GRootFunctions::Gaus(Double_t *dim, Double_t *par) {
  // - dim[0]: channels to fit
  // - par[0]: height of peak
  // - par[1]: cent of peak
  // - par[2]: sigma
  // - par[4]: relative height of skewed gaus to gaus

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

  double scaling = R*height/100.0;
  double x_rel = (x - cent)/sigma;

  return (scaling *
          TMath::Gaus(x_rel)/std::sqrt(2*3.1415926535) *
          (1 + 1*TMath::Erf(beta*x_rel/TMath::Sqrt(2.0))) );
}

Double_t GRootFunctions::PhotoPeak(Double_t *dim,Double_t *par) {
  return Gaus(dim,par) + SkewedGaus(dim,par);
}

Double_t GRootFunctions::PhotoPeakBG(Double_t *dim,Double_t *par) {
  return Gaus(dim,par) + SkewedGaus(dim,par) + StepFunction(dim,par) + PolyBg(dim,par+6,2);
}


// For fitting Ge detector efficiencies.
Double_t GRootFunctions::Efficiency(Double_t *dim, Double_t *par){
  // - dim[0]: energy.
  // - par[0]: zeroth order
  // - par[1]: first order
  // - par[2]: second order
  // - par[3]: inverse energy squared term.
  // - Formula : 10**(0+1*Log(x)+2*Log(x)**2+3/x**2)

  Double_t x  = dim[0];
  Double_t p0 = par[0];
  Double_t p1 = par[1];
  Double_t p2 = par[2];
  Double_t p3 = par[3];

  if(x!=0)
    return pow(10.0,(p0+p1*TMath::Log10(x)+p2*std::pow(TMath::Log10(x),2.0)+p3/(std::pow(x,2.0))));
  else
    return 0;
	    
}
