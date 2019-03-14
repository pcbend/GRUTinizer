#ifndef GROOTFUNCTIONS__H
#define GROOTFUNCTIONS__H

#include "TMath.h"

namespace GRootFunctions {

  Double_t LinFit(Double_t *dim, Double_t *par);
  Double_t QuadFit(Double_t *dim, Double_t *par);
  
  Double_t PolyBg(Double_t *dim, Double_t *par,Int_t order);
  
  Double_t StepBG(Double_t *dim, Double_t *par);
  Double_t StepFunction(Double_t *dim, Double_t *par);
  Double_t PhotoPeak(Double_t *dim, Double_t *par);
  Double_t PhotoPeakBG(Double_t *dim, Double_t *par);
  Double_t PhotoPeakBGExcludeRegion(Double_t *dim,Double_t *par);
  Double_t Gaus(Double_t *dim, Double_t *par);
  Double_t DoubleGaus(Double_t *dim, Double_t *par);
  Double_t SkewedGaus(Double_t *dim, Double_t *par);
  Double_t Efficiency(Double_t *dim, Double_t *par);



  Double_t GausExpo(Double_t *dim,Double_t *par);
  
  Double_t LanGaus(Double_t *dim,Double_t *par);
  Double_t LanGausHighRes(Double_t *dim,Double_t *par);

  Double_t GammaEff(Double_t *dim,Double_t *par);
  Double_t AlignedAD(Double_t *x,Double_t *par);
  Double_t AlignedAD_Norm(Double_t *x,Double_t *par);
  Double_t AlignedADPol_Norm(Double_t *x,Double_t *par);


  Double_t ComptonEnergy(Double_t *x,Double_t *par);
  Double_t ComptonAngle(Double_t *x,Double_t *par);
  Double_t ComptonRatio(Double_t *x,Double_t *par);
  Double_t AnalyzingPower(Double_t *x,Double_t *par);
  Double_t Polarization(Double_t *x,Double_t *par);
  Double_t PolarizationAsymmetry(Double_t *x,Double_t *par);
  Double_t KN_unpol(Double_t *x,Double_t *par);
  Double_t KN_unpol_theta(Double_t *x,Double_t *par);
  Double_t KN_unpol_theta_norm(Double_t *x,Double_t *par);
  Double_t KN_unpol_norm(Double_t *x,Double_t *par);
  Double_t KN_pol(Double_t *x,Double_t *par);

  Double_t W_pol(Double_t *x, Double_t *par);
  Double_t W_pol_dirk(Double_t *x, Double_t *par);
  Double_t W_pol_dirk2(Double_t *x, Double_t *par);
  Double_t W_pol_norm(Double_t *x, Double_t *par);
  Double_t W_pol2(Double_t *x, Double_t *par);
  Double_t W_pol3(Double_t *x, Double_t *par);
  Double_t W_pol4(Double_t *x, Double_t *par);
  Double_t W_pol5(Double_t *x, Double_t *par);
  Double_t W_pol6(Double_t *x, Double_t *par);
  //Double_t W_pol7(Double_t *x, Double_t *par);

  Double_t W_pol_test(Double_t *x, Double_t *par);
  Double_t W_pol_diff(Double_t *x, Double_t *par);


  Double_t Q_pol(Double_t *x, Double_t *par);

}

#endif
