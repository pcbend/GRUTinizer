#include <GEfficiency.h>
#include <TGraph.h>
#include <TVirtualFitter.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>

#include "Globals.h"
#include "GRootFunctions.h"
#include "GCanvas.h"

ClassImp(GEfficiency)

GEfficiency::GEfficiency(Double_t zeroth,Double_t first,Double_t second, Double_t inverse)
: TF1("eff_fit",GRootFunctions::Efficiency,0.1,4000,4){

  Clear("");
  TF1::SetRange(0.1,4000);

  SetName("Efficiency Fit");
  InitNames();
  InitParams(); // If we adjust this in the future, it might not be happy here.
  
}

GEfficiency::GEfficiency()
  : TF1("eff_fit",GRootFunctions::Efficiency,0.1,4000,4){
  Clear("");
  TF1::SetRange(0.1,4000);

  SetName("Efficiency Fit");
  InitNames();
  InitParams();

}

GEfficiency::GEfficiency(const GEfficiency &Eff){
  Eff.Copy(*this);
}

GEfficiency::~GEfficiency(){
  // nothing
}

void GEfficiency::InitNames(){
  TF1::SetParName(0,"zeroth");
  TF1::SetParName(1,"first");
  TF1::SetParName(2,"second");
  TF1::SetParName(3,"inverse");
}

bool GEfficiency::InitParams(){

  // Maybe should make this smarter.
  TF1::SetParameter("zeroth",-1);
  TF1::SetParameter("first",0.5);
  TF1::SetParameter("second",-0.2);
  TF1::SetParameter("inverse",-50);
    
  SetInitialized();
  return true;
  
}

bool GEfficiency::Fit(TH1* fit_hist,Option_t *opt){
  // dont need you.  
}

void GEfficiency::Copy(TObject &obj) const{
  TF1::Copy(obj);
  ((GEfficiency&)obj).init_flag = init_flag;
  ((GEfficiency&)obj).fchi2     = fchi2;
  ((GEfficiency&)obj).fNdf      = fNdf;
}

void GEfficiency::Print(Option_t *opt) const{
  TString options = opt;
  printf(GREEN "\n");
  printf("Name: %s \n",this->GetName());
  printf("Zeroth:    %1f +/- %1f \n", this->GetParameter("zeroth"),this->GetParError(GetParNumber("zeroth")));
  printf("First:     %1f +/- %1f \n", this->GetParameter("first"),this->GetParError(GetParNumber("first")));
  printf("Second:    %1f +/- %1f \n", this->GetParameter("second"),this->GetParError(GetParNumber("second")));
  printf("Inverse:   %1f +/- %1f \n", this->GetParameter("inverse"),this->GetParError(GetParNumber("inverse")));

  if(options.Contains("+")){
    TF1::Print(opt);
  }
  printf(RESET_COLOR);
  
}

void GEfficiency::Clear(Option_t *opt){
  TString options = opt;
  // Clear the GEfficiency including functions and histograms.
  if(options.Contains("all"))
    TF1::Clear();
  init_flag = false;
  fchi2     = 0;
  fNdf      = 0;

}

void GEfficiency::DrawResids(TH1 *hist) const{
  if(!hist) return;

  if(fchi2<0.000000001){
    std::cout << "No fit performed" << std::endl;
    return;
  }

  Double_t xlow,xhigh;
  GetRange(xlow,xhigh);
  Int_t nbins = hist->GetXaxis()->GetNbins();
  Double_t *res = new Double_t[nbins];
  Double_t *bin = new Double_t[nbins];
  Int_t points = 0;
  for(int i = 0; i<= nbins; i++){
    if(hist->GetBinCenter(i) <= xlow || hist->GetBinCenter(i) >= xhigh)
      continue;

    res[points] = (hist->GetBinContent(i) - this->Eval(hist->GetBinCenter(i)));
    bin[points] = hist->GetBinCenter(i);
    points++;
  }
  new GCanvas();
  TGraph *residuals = new TGraph(points,bin,res);
  residuals->SetMarkerStyle(20);
  residuals->SetMarkerSize(0.9);
  residuals->Draw("AP");
  delete[] res;
  delete[] bin;
  
  
}
