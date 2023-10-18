
#include <GGaus.h>
#include <TGraph.h>
#include <TVirtualFitter.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TH1.h>

#include "Globals.h"
#include "GRootFunctions.h"
#include "GCanvas.h"

ClassImp(GGaus)

GGaus::GGaus(Double_t xlow,Double_t xhigh,Option_t *opt)
  : TF1("gausbg","gaus(0)+pol1(3)",xlow,xhigh,TF1::EAddToList::kNo),
    fBGFit("background","pol1",xlow,xhigh,TF1::EAddToList::kNo)  {
  Clear("");
  if(xlow>xhigh) std::swap(xlow,xhigh);

  TF1::SetRange(xlow,xhigh);

  fBGFit.SetNpx(1000);
  fBGFit.SetLineStyle(2);
  fBGFit.SetLineColor(kBlack);
  InitNames();
}

GGaus::GGaus(Double_t xlow,Double_t xhigh,TF1 *bg,Option_t *opt)
      : TF1("gausbg","gaus(0)+pol1(3)",xlow,xhigh) {
  Clear("");
  if(xlow>xhigh) std::swap(xlow,xhigh);
  TF1::SetRange(xlow,xhigh);
  InitNames();

  if(bg) {
    fBGFit.Clear();
    fBGFit.Copy(*bg);
  } else {
    fBGFit = TF1("BGFit","pol1",xlow,xhigh);
  }

  fBGFit.SetNpx(1000);
  fBGFit.SetLineStyle(2);
  fBGFit.SetLineColor(kBlack);
}


GGaus::GGaus()
      : TF1("gausbg","gaus(0)+pol1(3)",0,1000),
        fBGFit("background","pol1",0,1000) {

  Clear();
  InitNames();
  fBGFit.SetNpx(1000);
  fBGFit.SetLineStyle(2);
  fBGFit.SetLineColor(kBlack);
}

GGaus::GGaus(const GGaus &peak)
  : TF1(peak) {
  peak.Copy(*this);
}

GGaus::~GGaus() {
}

void GGaus::InitNames(){
  TF1::SetParName(0,"Height");
  TF1::SetParName(1,"centroid");
  TF1::SetParName(2,"sigma");
  TF1::SetParName(3,"bg_offset");
  TF1::SetParName(4,"bg_slope");
}

void GGaus::Copy(TObject &obj) const {
  TF1::Copy(obj);
  ((GGaus&)obj).init_flag = init_flag;
  ((GGaus&)obj).fArea     = fArea;
  ((GGaus&)obj).fDArea    = fDArea;
  ((GGaus&)obj).fSum     = fSum;
  ((GGaus&)obj).fDSum    = fDSum;
  ((GGaus&)obj).fChi2     = fChi2;
  ((GGaus&)obj).fNdf      = fNdf;
  fBGFit.Copy((((GGaus&)obj).fBGFit));
}

bool GGaus::InitParams(TH1 *fithist){
  if(!fithist){
    printf("No histogram is associated yet, no initial guesses made\n");
    return false;
  }

  //Makes initial guesses at parameters for the fit. Uses the histogram to
  Double_t xlow,xhigh;
  GetRange(xlow,xhigh);

  Int_t binlow = fithist->GetXaxis()->FindBin(xlow);
  Int_t binhigh = fithist->GetXaxis()->FindBin(xhigh);

  Double_t highy  = fithist->GetBinContent(binlow);
  Double_t lowy   = fithist->GetBinContent(binhigh);
  for(int x = 1; x < 5; x++) {
    highy += fithist->GetBinContent(binlow-x);
    lowy  += fithist->GetBinContent(binhigh+x);
  }
  highy = highy/5.0;
  lowy = lowy/5.0;

  if(lowy>highy) std::swap(lowy,highy);

  double largestx = 0.0;
  double largesty = 0.0;
  for(int i = binlow; i <= binhigh; i++) {
    if(fithist->GetBinContent(i) > largesty) {
      largesty = fithist->GetBinContent(i);
      largestx = fithist->GetXaxis()->GetBinCenter(i);
    }
  }

  // - par[0]: height of peak
  // - par[1]: cent of peak
  // - par[2]: sigma
  // - par[3]: bg constent
  // - par[4]: bg slope

  //limits.
  TF1::SetParLimits(0,0,largesty*2);
  TF1::SetParLimits(1,xlow,xhigh);
  TF1::SetParLimits(2,0,xhigh-xlow);
  //TF1::SetParLimits(3,0.0,40);
  //TF1::SetParLimits(4,0.01,5);

  //Make initial guesses
  TF1::SetParameter(0,largesty);
  TF1::SetParameter(1,largestx);
  TF1::SetParameter(2,(largestx*.01)/2.35);
  //TF1::SetParameter(3,5.);
  //TF1::SetParameter(4,1.);

  TF1::SetParError(0,0.10 * largesty);
  TF1::SetParError(1,0.25);
  TF1::SetParError(2,0.10 *((largestx*.01)/2.35));
  //TF1::SetParError(3,5);
  //TF1::SetParError(4,0.5);

  SetInitialized();
  return true;
}

Bool_t GGaus::Fit(TH1 *fithist,Option_t *opt) {
  if(!fithist) return false;
  TString options = opt;
  if(!IsInitialized()) InitParams(fithist);
  TVirtualFitter::SetMaxIterations(100000);

  bool verbose = !options.Contains("Q");
  bool noprint =  options.Contains("no-print");
  if(noprint) {
    options.ReplaceAll("no-print","");
  }


  if(fithist->GetSumw2()->fN!=fithist->GetNbinsX()+2) fithist->Sumw2();
  TFitResultPtr fitres = fithist->Fit(this,Form("%sRSME",options.Data()));

  if(!fitres.Get()->IsValid()) {
    if(!verbose) printf(RED  "fit has failed, trying refit... " RESET_COLOR);
    fithist->GetListOfFunctions()->Last()->Delete();
    fitres = fithist->Fit(this,Form("%sRSME",options.Data()));
    if( fitres.Get()->IsValid() ) {
      if(!verbose && !noprint)
        printf(DGREEN " refit passed!" RESET_COLOR "\n");
    } else {
      if(!verbose && !noprint)
        printf(DRED " refit also failed :( " RESET_COLOR "\n");
    }
  }

  Double_t xlow,xhigh;
  TF1::GetRange(xlow,xhigh);

  //Make a function that does not include the background
  //Intgrate the background.
  GGaus *tmppeak = new GGaus;
  this->Copy(*tmppeak);

  tmppeak->SetParameter("bg_offset",0.0);
  tmppeak->SetRange(xlow,xhigh); //This will help get the true area of the gaussian 200 ~ infinity in a gaus
  tmppeak->SetName("tmppeak");

  TMatrixDSym CovMat = fitres->GetCovarianceMatrix();
  fDArea = (tmppeak->IntegralError(xlow, xhigh, tmppeak->GetParameters(), CovMat.GetMatrixArray()))/fithist->GetBinWidth(1);

  double bgpars[2];
  bgpars[0] = TF1::GetParameters()[3];
  bgpars[1] = TF1::GetParameters()[4];

  fBGFit.SetParameters(bgpars);

  fArea = this->Integral(xlow,xhigh) / fithist->GetBinWidth(1);
  double bgArea = fBGFit.Integral(xlow,xhigh) / fithist->GetBinWidth(1);;
  fArea -= bgArea;

  if(xlow>xhigh) std::swap(xlow,xhigh);
  fSum = fithist->Integral(fithist->GetXaxis()->FindBin(xlow), fithist->GetXaxis()->FindBin(xhigh));
  printf("sum between markers: %02f\n",fSum);
  fDSum = TMath::Sqrt(fSum);
  fSum -= bgArea;
  printf("sum after subtraction: %02f\n",fSum);

  if(!verbose && !noprint) {
    printf("hist: %s\n",fithist->GetName());
    Print();
  }

  Copy(*fithist->GetListOfFunctions()->FindObject(GetName()));
  fithist->GetListOfFunctions()->Add(fBGFit.Clone());

  delete tmppeak;
  return true;
}


void GGaus::Clear(Option_t *opt){
  TString options = opt;
  //Clear the GGaus including functions and histogram
  if(options.Contains("all")) TF1::Clear();
  init_flag = false;
  fArea  = 0.0;
  fDArea = 0.0;
  fSum   = 0.0;
  fDSum  = 0.0;
  fChi2  = 0.0;
  fNdf   = 0.0;
}

void GGaus::Print(Option_t *opt) const {
  TString options = opt;
  printf(GREEN );
  printf("Name: %s \n", this->GetName());
  printf("Centroid:  %1f +/- %1f \n", this->GetParameter("centroid"),this->GetParError(GetParNumber("centroid")));
  printf("Area:      %1f +/- %1f \n", fArea, fDArea);
  printf("Sum:       %1f +/- %1f \n", fSum, fDSum);
  printf("FWHM:      %1f +/- %1f \n",this->GetFWHM(),this->GetFWHMErr());
  printf("Reso:      %1f%%  \n",this->GetFWHM()/this->GetParameter("centroid")*100.);
  printf("Chi^2/NDF: %1f\n",fChi2/fNdf);
  if(options.Contains("all")){
    TF1::Print(opt);
  }
  printf(RESET_COLOR);
  printf("\n");
}


void GGaus::DrawResiduals(TH1 *hist) const{
  if(hist){
    return;
  }
  if(fChi2 < 0.000000001){
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
    if(hist->GetBinCenter(i) <= xlow || hist->GetBinCenter(i) >= xhigh) continue;
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
