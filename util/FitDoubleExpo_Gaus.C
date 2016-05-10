#include <iostream>
#include "TF1.h"
#include "TMath.h"
#include "TH1.h"

Double_t gBgConstant, gBgSlope, gContent, gMean, gContent_1, gMean_1, gContent_2, gMean_2, gSigma, gSigma_1, gSigma_2, gBinW, gChi2pNDF;

int temp_glob = 0;

class twoHistHolder{
  public:
    twoHistHolder(TH1* hist1, TH1*hist2){
      this->hist1 = hist1;
      this->hist2 = hist2;
    }

    TH1* hist1;
    TH1* hist2;

    double histValue(TH1* m_hist, double x){
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

    virtual double operator() (double *x, double *par){
      return histValue(hist1, x[0])*par[0]+histValue(hist2,x[0])*par[1];
    }
};
class threeHistHolder{
  public:
    threeHistHolder(TH1* hist1, TH1* hist2, TH1 *hist3){
      this->hist1 = hist1;
      this->hist2 = hist2;
      this->hist3 = hist3;
    }

    TH1* hist1;
    TH1* hist2;
    TH1* hist3;

    double histValue(TH1* m_hist, double x){
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

    virtual double operator() (double *x, double *par){
      //return histValue(hist1, x[0])*par[0]+histValue(hist2,x[0])*par[1] + histValue(hist3,x[0])*par[2];
      //Testing varying compton and FEP More consistently 
      return par[0]*(histValue(hist1, x[0])+histValue(hist2,x[0])*par[1]) + histValue(hist3,x[0])*par[2];
    }
};
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

class DoubleExpTwoHist : twoHistHolder{
  /*
    par[0]  histogram 1 scaling factor
    par[1]  histogram 2 scaling factor
    par[2]  double exponential scaling factor
    par[3]  initial exponential value
    par[4]  exponential decay constant
    par[5]  2nd initial exponential value
    par[6]  2nd exponential decay constant
   */
  public:
    DoubleExpTwoHist(TH1 *hist1, TH1 *hist2): twoHistHolder(hist1,hist2){}
    virtual double operator()(double *x, double *par){
      return (par[0]*histValue(hist1, x[0])+par[1]*histValue(hist2,x[0]) +
              par[2]*(par[3]*TMath::Exp(par[4]*x[0])+par[5]*TMath::Exp(par[6]*x[0]))
             );
    }
  
};
class DoubleExpThreeHist : threeHistHolder{
  /*
    par[0]  histogram 1 scaling factor
    par[1]  histogram 2 scaling factor
    par[2]  histogram 3 scaling factor
    par[3]  double exponential scaling factor
    par[4]  initial exponential value
    par[5]  exponential decay constant
    par[6]  2nd initial exponential value
    par[7]  2nd exponential decay constant
   */
  public:
    DoubleExpThreeHist(TH1 *hist1, TH1 *hist2, TH1 *hist3): threeHistHolder(hist1,hist2,hist3){}
    virtual double operator()(double *x, double *par){
    //return (par[0]*histValue(hist1, x[0])+par[1]*histValue(hist2,x[0]) + 
    //        par[2]*histValue(hist3,x[0]) + par[3]*(par[4]*TMath::Exp(par[5]*x[0])+
    //        par[6]*TMath::Exp(par[7]*x[0])));
      return (par[0]*(histValue(hist1, x[0])+par[1]*histValue(hist2,x[0])) + 
              par[2]*histValue(hist3,x[0]) + par[3]*(par[4]*TMath::Exp(par[5]*x[0])+
              par[6]*TMath::Exp(par[7]*x[0])));
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
  gBinW = hist->GetBinWidth(1);

  DoubleExpHist* deh = new DoubleExpHist(fitting);
  TF1* fitfunc = new TF1("double_exp_hist",deh, 0, 1, 6,"DoubleExpHist");


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

  hist->Fit(fitfunc, "PRMEQ", "");
  return fitfunc;
}

TF1 *FitDoubleExpTwoHist(TH1F *hist_to_fit, TH1F *geant_hist1, TH1F *geant_hist2, 
                         Double_t gLowX, Double_t gUpX, Double_t *init=NULL){
  if(gLowX > gUpX){
    std::cout << "Your range is illogical" << std::endl;
    return NULL;
  }

  hist_to_fit->GetXaxis()->SetRangeUser(gLowX,gUpX);
  gBinW = hist_to_fit->GetBinWidth(1);

  DoubleExpTwoHist* deh2 = new DoubleExpTwoHist(geant_hist1,geant_hist2);
  TF1* fitfunc = new TF1("double_exp_two_hist",deh2, 0, 1, 7,"DoubleExpTwoHist");


  if (init==NULL){
    std::cout << "Need initial parameters. Exiting." << std::endl;
    return NULL;
  }

  for (int i = 3; i < 7; i++){
    fitfunc->FixParameter(i, init[i]);
  }

//std::cout << "Setting par limit 0" << std::endl;
//fitfunc->SetParLimits(0, 1e-05, 1.0);
//std::cout << "Setting par limit 1" << std::endl;
//fitfunc->SetParLimits(1, 1.5e-04, 2.5e-04);
//std::cout << "Setting par limit 2" << std::endl;
//fitfunc->SetParLimits(2, 0.2,2);

  fitfunc->SetRange(gLowX, gUpX);
  fitfunc->SetLineColor(4);
  fitfunc->SetLineWidth(3);
  hist_to_fit->SetLineColor(3);
  geant_hist1->SetLineColor(2);
  geant_hist2->SetLineColor(5);

  fitfunc->SetParName(0, "Hist.Const 1        ");
  fitfunc->SetParName(1, "Hist.Const 2        ");
  fitfunc->SetParName(2, "Double Exp. Scaling ");
  fitfunc->SetParName(3, "Exp.Intercept       ");
  fitfunc->SetParName(4, "Exp.Decay.Const     ");
  fitfunc->SetParName(5, "2nd Exp. Intercept   ");
  fitfunc->SetParName(6, "2nd Exp. Decay Const ");

  hist_to_fit->Fit(fitfunc, "PRMEQ", "");
  return fitfunc;
}

//Geant_hist1 should be the full energy peak for the main peak you want to fit
//Geant_hist2 should be hte compton background for the main peak
//Geant_hist3 should be a second two-plus state or some contaminant peak
TF1 *FitDoubleExpThreeHist(TH1F *hist_to_fit, TH1F *geant_fep, TH1F *geant_compton, 
                           TH1F *geant_2ndpeak, Double_t gLowX, Double_t gUpX, Double_t *init=NULL){
  if(gLowX > gUpX){
    std::cout << "Your range is illogical" << std::endl;
    return NULL;
  }

  hist_to_fit->GetXaxis()->SetRangeUser(gLowX,gUpX);
  gBinW = hist_to_fit->GetBinWidth(1);

  DoubleExpThreeHist* deh3 = new DoubleExpThreeHist(geant_fep,geant_compton, geant_2ndpeak);
  TF1* fitfunc = new TF1("double_exp_three_hist",deh3, 0, 1, 8,"DoubleExpThreeHist");
  TF1* fitfunc_to_draw = new TF1("double_exp_three_hist_check",deh3, 0, 1, 8,"DoubleExpThreeHist");


  if (init==NULL){
    std::cout << "Need initial parameters. Exiting." << std::endl;
    return NULL;
  }

  fitfunc->FixParameter(1,init[1]);
  for (int i = 4; i < 8; i++){
    fitfunc->FixParameter(i, init[i]);
  }

//fitfunc->SetParLimits(0, 9e-04, 1.2e-03);
  fitfunc->SetParLimits(2, 0, 1);
//fitfunc->SetParLimits(2, 1.0e-04, 2.5e-04);
//fitfunc->SetParLimits(3, 0.5, 1.5);

  //fitfunc->SetRange(gLowX, gUpX);
  fitfunc->SetLineColor(4);
  fitfunc->SetLineWidth(3);
  hist_to_fit->SetLineColor(kBlue);
  geant_fep->SetLineColor(kGreen+4);
  geant_compton->SetLineColor(kRed);
  geant_2ndpeak->SetLineColor(kMagenta);

  fitfunc->SetParName(0, "FEP Hist. Const       ");
  fitfunc->SetParName(1, "Compton Hist. Const   ");
  fitfunc->SetParName(2, "2nd 2-Plus Scaling    ");
  fitfunc->SetParName(3, "Double Exp. Scaling   ");
  fitfunc->SetParName(4, "Exp.Intercept         ");
  fitfunc->SetParName(5, "Exp.Decay.Const       ");
  fitfunc->SetParName(6, "2nd Exp. Intercept    ");
  fitfunc->SetParName(7, "2nd Exp. Decay Const  ");

  //hist_to_fit->Fit(fitfunc, "PRMEQ");
  hist_to_fit->Fit(fitfunc, "PMENQ","",gLowX,gUpX);
  fitfunc_to_draw->SetParameters(fitfunc->GetParameters());
  fitfunc_to_draw->SetRange(0,4096);
  
  return fitfunc_to_draw;
}
