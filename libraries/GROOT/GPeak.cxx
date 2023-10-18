#include <GPeak.h>
#include <TGraph.h>
#include <TVirtualFitter.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TH1.h>

#include "Globals.h"
#include "GRootFunctions.h"
#include "GCanvas.h"

ClassImp(GPeak)

GPeak::GPeak(Double_t cent,Double_t xlow,Double_t xhigh,Option_t *opt)
      : TF1("photopeakbg",GRootFunctions::PhotoPeakBG,xlow,xhigh,7),
        fBGFit("background",GRootFunctions::StepBG,xlow,xhigh,6)  {
  Clear("");
  if(cent>xhigh || cent<xlow) {
    //out of range...
    if(xlow>cent) std::swap(xlow,cent);
    if(xlow>xhigh) std::swap(xlow,xhigh);
    if(cent>xhigh) std::swap(cent,xhigh);
  }

  TF1::SetRange(xlow,xhigh);
  fBGFit.SetNpx(1000);
  fBGFit.SetLineStyle(2);
  fBGFit.SetLineColor(kBlack);

  SetName(Form("Chan%d_%d_to_%d",(Int_t)(cent),(Int_t)(xlow),(Int_t)(xhigh)));
  InitNames();
  TF1::SetParameter("centroid",cent);
  SetParent(0);
  DetachBackground();
}

GPeak::GPeak(Double_t cent,Double_t xlow,Double_t xhigh,TF1 *bg,Option_t *opt)
      : TF1("photopeakbg",GRootFunctions::PhotoPeakBG,xlow,xhigh,7) {
  Clear("");
  if(cent>xhigh || cent<xlow) {
    //out of range...
    if(xlow>cent) std::swap(xlow,cent);
    if(xlow>xhigh) std::swap(xlow,xhigh);
    if(cent>xhigh) std::swap(cent,xhigh);
  }
  TF1::SetRange(xlow,xhigh);
  SetName(Form("Chan%d_%d_to_%d",(Int_t)(cent),(Int_t)(xlow),(Int_t)(xhigh)));
  InitNames();
  TF1::SetParameter("centroid",cent);

  if(bg) {
    fBGFit.Clear();
    fBGFit.Copy(*bg);
  } else {
    fBGFit = TF1("BGFit",GRootFunctions::StepBG,xlow,xhigh,10);
  }

  fBGFit.SetNpx(1000);
  fBGFit.SetLineStyle(2);
  fBGFit.SetLineColor(kBlack);

  SetParent(0);
  DetachBackground();
}


GPeak::GPeak()
      : TF1("photopeakbg",GRootFunctions::PhotoPeakBG,0,1000,10),
        fBGFit("background",GRootFunctions::StepBG,0,1000,10) {

  Clear();
  InitNames();
  fBGFit.SetNpx(1000);
  fBGFit.SetLineStyle(2);
  fBGFit.SetLineColor(kBlack);

  SetParent(0);
  DetachBackground();
}

GPeak::GPeak(const GPeak &peak)
  : TF1(peak) {

  SetParent(0);
  DetachBackground();
  peak.Copy(*this);
}

GPeak::~GPeak() {
}

void GPeak::InitNames(){
  TF1::SetParName(0,"Height");
  TF1::SetParName(1,"centroid");
  TF1::SetParName(2,"sigma");
  TF1::SetParName(3,"R");
  TF1::SetParName(4,"beta");
  TF1::SetParName(5,"step");
  TF1::SetParName(6,"bg_offset");
}

void GPeak::Copy(TObject &obj) const {
  TF1::Copy(obj);
  ((GPeak&)obj).init_flag = init_flag;
  ((GPeak&)obj).fArea     = fArea;
  ((GPeak&)obj).fDArea    = fDArea;
  ((GPeak&)obj).fSum      = fSum;
  ((GPeak&)obj).fDSum     = fDSum;
  ((GPeak&)obj).fChi2     = fChi2;
  ((GPeak&)obj).fNdf      = fNdf;

  fBGFit.Copy((((GPeak&)obj).fBGFit));
  ((GPeak&)obj).DetachBackground();
}

bool GPeak::InitParams(TH1 *fithist){
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

  if(lowy > highy) std::swap(lowy,highy);

  double largestx=0.0;
  double largesty=0.0;

  for(int i = binlow; i <= binhigh;i++) {
    if(fithist->GetBinContent(i) > largesty) {
      largesty = fithist->GetBinContent(i);
      largestx = fithist->GetXaxis()->GetBinCenter(i);
    }
  }

  double step = (highy-lowy)/largesty*50;
  double offset = lowy;

  // - par[0]: height of peak
  // - par[1]: cent of peak
  // - par[2]: sigma
  // - par[3]: R:    relative height of skewed gaus to gaus
  // - par[4]: beta: "skewedness" of the skewed gaussin
  // - par[5]: step: size of stepfunction step.
  // - par[6]: base bg height.

  //limits.
  TF1::SetParLimits(0, 0, largesty*2);
  TF1::SetParLimits(1, xlow, xhigh);
  TF1::SetParLimits(2, 0.1, xhigh - xlow);
  TF1::SetParLimits(3, 0.0, 40);
  TF1::SetParLimits(4, 0.01, 5);
  TF1::SetParLimits(5, 0.0, step + step);
  TF1::SetParLimits(6,offset-0.5*offset,offset+offset);

  //Make initial guesses
  TF1::SetParameter(0, largesty);
  TF1::SetParameter(1, largestx);
  TF1::SetParameter(2, (largestx*.01)/2.35);
  TF1::SetParameter(3, 5.);
  TF1::SetParameter(4, 1.);
  TF1::SetParameter(5, step);
  TF1::SetParameter(6, offset);

  TF1::SetParError(0, 0.10 * largesty);
  TF1::SetParError(1, 0.25);
  TF1::SetParError(2, 0.10 *((largestx*.01)/2.35));
  TF1::SetParError(3, 5);
  TF1::SetParError(4, 0.5);
  TF1::SetParError(5, 0.10 * step);
  TF1::SetParError(6, 0.10 * offset);

  SetInitialized();
  return true;
}


Bool_t GPeak::Fit(TH1 *fithist,Option_t *opt) {
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

  TFitResultPtr fitres = fithist->Fit(this,Form("%sLRSME",options.Data()));

  printf("chi^2/NDF = %.02f\n",this->GetChisquare()/(double)this->GetNDF());

  if(!fitres.Get()->IsValid()) {
    printf(RED  "fit has failed, trying refit... " RESET_COLOR);
    fithist->GetListOfFunctions()->Last()->Delete();
    fitres = fithist->Fit(this,Form("%sLRSME",options.Data()));
    if( fitres.Get()->IsValid() ) {
      printf(DGREEN " refit passed!" RESET_COLOR "\n");
    } else {
      printf(DRED " refit also failed :( " RESET_COLOR "\n");
    }
  }

  Double_t xlow,xhigh;
  TF1::GetRange(xlow,xhigh);

  //Make a function that does not include the background
  //Intgrate the background.
  GPeak *tmppeak = new GPeak;
  this->Copy(*tmppeak);

  tmppeak->SetParameter("bg_offset",0.0);
  tmppeak->SetRange(xlow, xhigh); //This will help get the true area of the gaussian 200 ~ infinity in a gaus
  tmppeak->SetName("tmppeak");

  TMatrixDSym CovMat = fitres->GetCovarianceMatrix();
  fDArea = (tmppeak->IntegralError(xlow, xhigh, tmppeak->GetParameters(), CovMat.GetMatrixArray()))/fithist->GetBinWidth(1);

  double bgpars[5];
  bgpars[0] = TF1::GetParameters()[0];
  bgpars[1] = TF1::GetParameters()[1];
  bgpars[2] = TF1::GetParameters()[2];
  bgpars[3] = TF1::GetParameters()[5];
  bgpars[4] = TF1::GetParameters()[6];
  fBGFit.SetParameters(bgpars);

  fChi2 = this->GetChisquare();
  fNdf = this->GetNDF();

  fArea = this->Integral(xlow,xhigh) / fithist->GetBinWidth(1);
  double bgArea = fBGFit.Integral(xlow,xhigh) / fithist->GetBinWidth(1);
  fArea -= bgArea;


  if(xlow>xhigh) std::swap(xlow,xhigh);
  fSum = fithist->Integral(fithist->GetXaxis()->FindBin(xlow), fithist->GetXaxis()->FindBin(xhigh));
  printf("sum between markers: %02f\n",fSum);
  fDSum = TMath::Sqrt(fSum);
  fSum -= bgArea;
  printf("sum after subtraction: %02f\n",fSum);

  if(!verbose) {
    printf("hist: %s\n",fithist->GetName());
    Print();
  }

  Copy(*fithist->GetListOfFunctions()->FindObject(GetName()));
  fithist->GetListOfFunctions()->Add(fBGFit.Clone()); //use to be a clone.

  SetParent(0);;
  DetachBackground();

  delete tmppeak;
  return true;
}


Bool_t GPeak::FitExclude(TH1 *fithist,double xlow,double xhigh,Option_t *opt) {
  //ok, we are going to assume we have a funny shaped peak here,
  //freeze all parameters except offset and step and see if we can get the
  //bg right so we can at least try to use the sum...  - this is bad, i am going to create and
  //return a new one.....
  TString options = opt;
  bool verbose = !options.Contains("Q");

  TF1 ff("photopeakbg_exclude",GRootFunctions::PhotoPeakBGExcludeRegion,xlow,xhigh,9);
  ff.SetParName(0,"Height");
  ff.SetParName(1,"centroid");
  ff.SetParName(2,"sigma");
  ff.SetParName(3,"R");
  ff.SetParName(4,"beta");
  ff.SetParName(5,"step");
  ff.SetParName(6,"bg_offset");
  ff.SetParName(7,"exclude_low");
  ff.SetParName(8,"exclude_high");

  ff.FixParameter(0,TF1::GetParameter(0));         //fithist->GetBinContent(bin));
  ff.FixParameter(1,TF1::GetParameter(1));
  ff.FixParameter(2,TF1::GetParameter(2));
  ff.FixParameter(3,TF1::GetParameter(3));
  ff.FixParameter(4,TF1::GetParameter(4));
  ff.SetParameter(5,TF1::GetParameter(5));
  ff.SetParameter(6,TF1::GetParameter(6));
  ff.FixParameter(7,xlow+10);
  ff.FixParameter(8,xhigh-10);

  fithist->Fit(&ff,"QRN+");

  TF1::SetParameter(5,ff.GetParameter(5));
  TF1::SetParameter(6,ff.GetParameter(6));

  double bgpars[5];
  bgpars[0] = TF1::GetParameters()[0];
  bgpars[1] = TF1::GetParameters()[1];
  bgpars[2] = TF1::GetParameters()[2];
  bgpars[3] = TF1::GetParameters()[5];
  bgpars[4] = TF1::GetParameters()[6];
  //bgpars[5] = TF1::GetParameters()[7];
  fBGFit.SetParameters(bgpars);

  fChi2 = this->GetChisquare();
  fNdf = this->GetNDF();

  fArea = this->Integral(xlow,xhigh) /  fithist->GetBinWidth(1);
  double bgArea = fBGFit.Integral(xlow,xhigh) / fithist->GetBinWidth(1);
  fArea -= bgArea;

  if(xlow>xhigh)std::swap(xlow,xhigh);
  fSum = fithist->Integral(fithist->GetXaxis()->FindBin(xlow), fithist->GetXaxis()->FindBin(xhigh));
  printf("sum between markers: %02f\n",fSum);
  fDSum = TMath::Sqrt(fSum);
  fSum -= bgArea;
  printf("sum after subtraction: %02f\n",fSum);

  if(!verbose) {
    printf("exclude on hist: %s\n",fithist->GetName());
    Print();
  }

  fithist->GetListOfFunctions()->Clear();
  fithist->GetListOfFunctions()->Add(this);

  fithist->GetListOfFunctions()->Add(fBGFit.Clone()); //use to be a clone.
  DetachBackground();
  return true;
}

void GPeak::Clear(Option_t *opt){
  TString options = opt;
  //Clear the GPeak including functions and histogram
  if(options.Contains("all"))
    TF1::Clear();
  init_flag = false;
  fArea  = 0.0;
  fDArea = 0.0;
  fSum   = 0.0;
  fDSum  = 0.0;
  fChi2  = 0.0;
  fNdf   = 0.0;
}

void GPeak::Print(Option_t *opt) const {
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


void GPeak::DrawResiduals(TH1 *hist) const{
  if(hist){
    return;
  }
  if(fChi2 < 0.000000001){
    printf("No fit performed\n");
    return;
  }
  Double_t xlow,xhigh;
  GetRange(xlow, xhigh);
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

void GPeak::DetachBackground() {
  fBGFit.SetParent(0);
  fBGFit.SetBit(TObject::kCanDelete,false);
}
