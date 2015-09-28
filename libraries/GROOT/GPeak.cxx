
#include <GPeak.h>
#include <TGraph.h>
#include <TVirtualFitter.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>

#include "Globals.h"
#include <GRootFunctions.h>
#include <GCanvas.h>

ClassImp(GPeak)

GPeak::GPeak(Double_t cent,Double_t xlow,Double_t xhigh,Option_t *opt) 
      : TF1("photopeakbg",GRootFunctions::PhotoPeakBG,xlow,xhigh,10),  
        background("background",GRootFunctions::StepBG,xlow,xhigh,10)  {
  Clear("");
  if(cent>xhigh || cent<xlow) {
    //out of range...
    if(xlow>cent)
      std::swap(xlow,cent);
    if(xlow>xhigh)
      std::swap(xlow,xhigh);
    if(cent>xhigh)
      std::swap(cent,xhigh);
  }

  TF1::SetRange(xlow,xhigh);

  background.SetNpx(1000);
  background.SetLineStyle(2);
  background.SetLineColor(kBlack);

  SetName(Form("Chan%d_%d_to_%d",(Int_t)(cent),(Int_t)(xlow),(Int_t)(xhigh)));
  InitNames();
  TF1::SetParameter("centroid",cent);
      
}

GPeak::GPeak() 
      : TF1("photopeakbg",GRootFunctions::PhotoPeakBG,0,1000,10),
        background("background",GRootFunctions::StepBG,0,1000,10) {
        
  Clear();
  InitNames();
  background.SetNpx(1000);
  background.SetLineStyle(2);
  background.SetLineColor(kBlack);
}

GPeak::GPeak(const GPeak &peak) {
  peak.Copy(*this);
}

GPeak::~GPeak() {
  //if(background)
  //  delete background;
}

void GPeak::InitNames(){
  TF1::SetParName(0,"Height");
  TF1::SetParName(1,"centroid");
  TF1::SetParName(2,"sigma");
  TF1::SetParName(3,"beta");
  TF1::SetParName(4,"R");
  TF1::SetParName(5,"step");
  TF1::SetParName(6,"A");
  TF1::SetParName(7,"B");
  TF1::SetParName(8,"C");
  TF1::SetParName(9,"bg_offset");
}

void GPeak::Copy(TObject &obj) const {
  TF1::Copy(obj);
  ((GPeak&)obj).init_flag = init_flag;
  ((GPeak&)obj).farea     = farea;
  ((GPeak&)obj).fd_area   = fd_area;
  ((GPeak&)obj).fchi2     = fchi2;
  ((GPeak&)obj).fNdf      = fNdf;



  background.Copy((((GPeak&)obj).background));
}

bool GPeak::InitParams(TH1 *fithist){
  if(!fithist){
    printf("No histogram is associated yet, no initial guesses made\n");
    return false;
  }
  //Makes initial guesses at parameters for the fit. Uses the histogram to
  Double_t xlow,xhigh;
  GetRange(xlow,xhigh);
 
  Int_t bin = fithist->GetXaxis()->FindBin(GetParameter("centroid"));
  Int_t binlow = fithist->GetXaxis()->FindBin(xlow);
  Int_t binhigh = fithist->GetXaxis()->FindBin(xhigh);
  Double_t binWidth = fithist->GetBinWidth(bin);

  double largestx=0.0;
  double largesty=0.0;
  for(int i = binlow;i<=binhigh;i++) {
    //printf("bin[%i] content:  %.1f\n",i,fithist->GetBinContent(i));
    if(fithist->GetBinContent(i) > largesty) {
      largesty = fithist->GetBinContent(i);
      largestx = fithist->GetXaxis()->GetBinCenter(i);
    }
  }
  //printf("largest x = %.1f\n",largestx);
  //printf("largest y = %.1f\n",largesty);

  TF1::SetParLimits(1,xlow,xhigh);
  TF1::SetParLimits(2,0.1,xhigh-xlow); // sigma should be less than the window width - JKS
  TF1::SetParLimits(3,0.000,10);
  TF1::SetParLimits(4,0,100); // this is a percentage. no reason for it to go to 500% - JKS
  //Step size is allow to vary to anything. If it goes below 0, the code will fix it to 0
  TF1::SetParLimits(6,0.0,fithist->GetBinContent(bin)*1.4);
  //this->SetParLimits(9,xlow,xhigh);
  TF1::SetParLimits(5,0.0,1.0E2);
  
  //Make initial guesses
  TF1::SetParameter("Height",largesty);  //fithist->GetBinContent(bin));
  TF1::SetParameter("centroid",largestx);          //GetParameter("centroid"));
  //TF1::SetParameter("sigma",(xhigh-xlow)*0.5);  
  TF1::SetParameter("sigma",2.0/binWidth); // 
  TF1::SetParameter("beta",0.5);
  TF1::SetParameter("R", 1.0);
  TF1::SetParameter("step",1.0);
  TF1::SetParameter("A",fithist->GetBinContent(binhigh));
  TF1::SetParameter("B",(fithist->GetBinContent(binlow) - fithist->GetBinContent(binhigh))/(xlow-xhigh));
  TF1::SetParameter("C",-0.5);
  TF1::SetParameter("bg_offset",GetParameter("centroid"));
  //TF1::FixParameter(8,0.00);
  //TF1::FixParameter(4,0.00);
  SetInitialized();
  return true;
}

Bool_t GPeak::Fit(TH1 *fithist,Option_t *opt) {
  if(!fithist) 
    return false;
  TString options = opt;
  if(!IsInitialized())
    InitParams(fithist);
  TVirtualFitter::SetMaxIterations(100000);

  bool verbose = !options.Contains("Q");
  TF1::SetParLimits(1,GetXmin(),GetXmax());
  TF1::SetParLimits(9,GetXmin(),GetXmax());
  
  TFitResultPtr fitres = fithist->Fit(this,Form("%sRSM",options.Data()));
  
  if(fitres->ParError(2) != fitres->ParError(2)) { // checks if nan.
    if(fitres->Parameter(3)<1) {
      FixParameter(4,0);
      FixParameter(3,1);
      //printf("Beta may have broken the fit, retrying with R=0);
      fithist->GetListOfFunctions()->Last()->Delete();
      fitres = fithist->Fit(this,Form("%sRSM",options.Data()));
    }
  }

  Double_t binwidth = fithist->GetBinWidth(GetParameter("centroid"));
  Double_t width    = TF1::GetParameter("sigma");
  Double_t xlow,xhigh;
  Double_t int_low,int_high;
  TF1::GetRange(xlow,xhigh);
  int_low  = xlow - 5.*width;
  int_high = xhigh +5.*width;

  //Make a function that does not include the background
  //Intgrate the background.
  GPeak *tmppeak = new GPeak;
  this->Copy(*tmppeak);
  tmppeak->SetParameter("step",0.0);
  tmppeak->SetParameter("A",0.0);
  tmppeak->SetParameter("B",0.0);
  tmppeak->SetParameter("C",0.0);
  tmppeak->SetParameter("bg_offset",0.0);
  tmppeak->SetRange(int_low,int_high);//This will help get the true area of the gaussian 200 ~ infinity in a gaus
  tmppeak->SetName("tmppeak");

  farea = (tmppeak->Integral(int_low,int_high))/binwidth;
  TMatrixDSym CovMat = fitres->GetCovarianceMatrix();
  CovMat(6,6) = 0.0;
  CovMat(7,7) = 0.0;
  CovMat(8,8) = 0.0;
  CovMat(9,9) = 0.0;

  fd_area = (tmppeak->IntegralError(int_low,int_high,tmppeak->GetParameters(),CovMat.GetMatrixArray()))/binwidth;
  if(!verbose)
    Print();


  background.SetParameters(TF1::GetParameters());
  Copy(*fithist->GetListOfFunctions()->Last());
  delete tmppeak;
  return true;
}


void GPeak::Clear(Option_t *opt){
  TString options = opt;
  //Clear the GPeak including functions and histogram
  if(options.Contains("all"))
    TF1::Clear();
  init_flag = false;
  farea = 0.0;
  fd_area = 0.0;
  fchi2 = 0.0;
  fNdf = 0.0;
}

void GPeak::Print(Option_t *opt) const {
  TString options = opt;
  printf(GREEN "\n");
  printf("Name: %s \n", this->GetName());
  printf("Centroid:  %1f +/- %1f \n", this->GetParameter("centroid"),this->GetParError(GetParNumber("centroid")));
  printf("Area:      %1f +/- %1f \n", farea, fd_area);
  printf("FWHM:      %1f +/- %1f \n", this->GetParameter("sigma")*2,35,this->GetParError(GetParNumber("sigma")*2.35));
  printf("Chi^2/NDF: %1f\n",fchi2/fNdf);
  if(options.Contains("+")){
    TF1::Print(opt);
  }
  printf(RESET_COLOR);
}


void GPeak::DrawResiduals(TH1 *hist) const{
  if(hist){
    return;
  }
  if(fchi2<0.000000001){
    printf("No fit performed\n");
    return;
  }
  Double_t xlow,xhigh;
  GetRange(xlow,xhigh);
  Int_t nbins = hist->GetXaxis()->GetNbins();
  Double_t *res = new Double_t[nbins];
  Double_t *bin = new Double_t[nbins];
  Int_t points = 0;
  for(int i =1;i<=nbins;i++) {
    if(hist->GetBinCenter(i) <= xlow || hist->GetBinCenter(i) >= xhigh)
      continue;
    res[points] = (hist->GetBinContent(i) - this->Eval(hist->GetBinCenter(i)))+ this->GetParameter("Height")/2;
    bin[points] = hist->GetBinCenter(i);
    points++;
  }
  new GCanvas();
  TGraph *residuals = new TGraph(points,bin,res);
  residuals->Draw("*AC");
  delete[] res;
  delete[] bin;
}














