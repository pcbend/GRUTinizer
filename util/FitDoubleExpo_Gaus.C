#include <iostream>
#include "TF1.h"
#include "TMath.h"
#include "TH1.h"

Double_t gBgConstant, gBgSlope, gContent, gMean, gContent_1, gMean_1, gContent_2, gMean_2, gSigma, gSigma_1, gSigma_2, gBinW, gChi2pNDF;

int temp_glob = 0;

class histHolder{
public:
  histHolder(TH1* hist){
    m_hist = hist;
  }
  double histValue(double x){
    int binNum = m_hist->GetXaxis()->FindBin(x); //gHist->GetBin() does not respect rebinning.

    int nBins = m_hist->GetNbinsX();
    int kevPerBin = m_hist->GetXaxis()->GetXmax()/nBins;
    int curBinX = m_hist->GetBinCenter(binNum);
    int nextBinX = m_hist->GetBinCenter(binNum+1);
    int prevBinX = m_hist->GetBinCenter(binNum-1);

    if (x > prevBinX && x <= curBinX){
      double leftDiff = x - prevBinX;
      double rightDiff = curBinX - x;

      leftDiff = 1.0 - leftDiff/(double)kevPerBin;   //These numbers are now less than 1
      rightDiff = 1.0 - rightDiff/(double)kevPerBin; //and a measure of how close it is to that bin
      double binContentLeft = m_hist->GetBinContent(binNum-1);
      double binContentRight = m_hist->GetBinContent(binNum);
      return (leftDiff*binContentLeft+rightDiff*binContentRight);
    }
    
    else if (x > curBinX && x < nextBinX){
      double leftDiff = x - curBinX;
      double rightDiff = nextBinX - x;

      leftDiff = 1.0 - leftDiff/(double)kevPerBin;
      rightDiff = 1.0 - rightDiff/(double)kevPerBin;
      double binContentLeft = m_hist->GetBinContent(binNum);
      double binContentRight = m_hist->GetBinContent(binNum+1);
      return (leftDiff*binContentLeft+rightDiff*binContentRight);
    }

    std::cout << "FAILED IN HISTVALUE!" << std::endl;

    return m_hist->GetBinContent(binNum);
  }
  virtual double operator() (double* x, double* par){
    return histValue(x[0])*par[0];
  }
public:
  TH1* m_hist;
};

class ExpHist : histHolder{
  /*
    par[0]  histogram scaling factor
    par[1]  initial exponential value
    par[2]  exponential decay constant
   */
public:
  ExpHist(TH1* hist) : histHolder(hist) {}
  virtual double operator() (double* x, double* par){
    return (par[0]*histValue(x[0]) +
	    par[1]*TMath::Exp(par[2]*x[0]));
  }
};

class DoubleExpHist : histHolder{
  /*
    par[0]  histogram scaling factor
    par[1]  double exponential scaling factor
    par[2]  initial exponential value
    par[3]  exponential decay constant
    par[4]  2nd initial exponential value
    par[5]  2nd exponential decay constant
   */
public:
  DoubleExpHist(TH1* hist) : histHolder(hist) {}
  virtual double operator() (double* x, double* par){
    return (par[0]*histValue(x[0]) + par[1]*(
	    par[2]*TMath::Exp(par[3]*x[0]) +
            par[4]*TMath::Exp(par[5]*x[0]))
            );
  }
};

class DoubleExpHistNoScale : histHolder{
  /*
    par[0]  histogram scaling factor
    par[1]  double exponential scaling factor
    par[2]  initial exponential value
    par[3]  exponential decay constant
    par[4]  2nd initial exponential value
    par[5]  2nd exponential decay constant
   */
public:
  DoubleExpHistNoScale(TH1* hist) : histHolder(hist) {}
  virtual double operator() (double* x, double* par){
    return (par[0]*histValue(x[0]) + 
	    par[1]*TMath::Exp(par[2]*x[0]) +
            par[3]*TMath::Exp(par[4]*x[0])
            );
  }
};

TF1* FitExpHist(TH1F *hist, TH1F* fitting, Double_t gLowX, Double_t gUpX,Double_t* init=NULL)
{
  if(gLowX > gUpX){
    std::cout << "Your range is illogical" << std::endl;
    return NULL;
  }

  hist->GetXaxis()->SetRangeUser(gLowX,gUpX);
  hist->Draw();
  gBinW = hist->GetBinWidth(1);

  ExpHist* eh = new ExpHist(fitting);
  TF1* fitfunc = new TF1("exp_hist",eh, 0, 1, 3,"ExpHist");


  if (init!=NULL){
    fitfunc->SetParameters(init);
  }

  fitfunc->SetRange(gLowX, gUpX);
  fitfunc->SetLineColor(3);
  fitfunc->SetLineWidth(3);

  fitfunc->SetParName(0, "Hist.Const     ");
  fitfunc->SetParName(1, "Exp.Intercept  ");
  fitfunc->SetParName(2, "Exp.Decay.Const");

  hist->Fit(fitfunc, "LR0", "SAME");
  return fitfunc;
}

TF1* FitDoubleExpHist(TH1F *hist, TH1F* fitting, Double_t gLowX, Double_t gUpX, Double_t* init=NULL)
{
  if(gLowX > gUpX){
    std::cout << "Your range is illogical" << std::endl;
    return NULL;
  }

  hist->GetXaxis()->SetRangeUser(gLowX,gUpX);
//  hist->Draw();
  gBinW = hist->GetBinWidth(1);

  DoubleExpHist* deh = new DoubleExpHist(fitting);
//  DoubleExpHistNoScale* deh = new DoubleExpHistNoScale(fitting);
  TF1* fitfunc = new TF1("double_exp_hist",deh, 0, 1, 6,"DoubleExpHist");
//  TF1* fitfunc = new TF1("double_exp_hist",deh, 0, 1, 5,"DoubleExpHist");


  if (init!=NULL){
    fitfunc->SetParameters(init);
  }

  for (int i = 2; i < 6; i++){
    fitfunc->FixParameter(i, init[i]);
  }

  fitfunc->SetRange(gLowX, gUpX);
  fitfunc->SetLineColor(4);
  fitfunc->SetLineWidth(3);
  hist->SetLineColor(3);
  fitting->SetLineColor(2);

  fitfunc->SetParName(0, "Hist.Const          ");
  fitfunc->SetParName(1, "Double Exp. Scaling ");
  fitfunc->SetParName(2, "Exp.Intercept       ");
  fitfunc->SetParName(3, "Exp.Decay.Const     ");
  fitfunc->SetParName(4, "2nd Exp. Intercept   ");
  fitfunc->SetParName(5, "2nd Exp. Decay Const ");
//
//fitfunc->SetParName(1, "Exp.Intercept       ");
//fitfunc->SetParName(2, "Exp.Decay.Const     ");
//fitfunc->SetParName(3, "2nd Exp.Intercept   ");
//fitfunc->SetParName(4, "2nd Exp.Decay.Const ");

  hist->Fit(fitfunc, "PRMEQ", "");
  return fitfunc;
}
