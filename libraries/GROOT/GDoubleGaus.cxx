
#include <GDoubleGaus.h>
#include <TGraph.h>
#include <TVirtualFitter.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TH1.h>

#include "Globals.h"
#include "GRootFunctions.h"
#include "GCanvas.h"

ClassImp(GDoubleGaus)

GDoubleGaus::GDoubleGaus(Double_t cent1,Double_t cent2,Double_t xlow,Double_t xhigh,Option_t *opt)
  : TF1("doublegausbg",GRootFunctions::DoubleGaus,xlow,xhigh,7,1,TF1::EAddToList::kNo),
    fBGFit("background","pol1",xlow,xhigh,TF1::EAddToList::kNo),
    fGaus1("fGaus1","gaus(0)+pol1(3)",xlow,xhigh,TF1::EAddToList::kNo),
    fGaus2("fGaus2","gaus(0)+pol1(3)",xlow,xhigh,TF1::EAddToList::kNo){
  Clear("");
  if(xlow>xhigh)
    std::swap(xlow,xhigh);

  if(cent1>cent2)
    std::swap(cent1,cent2);

  TF1::SetRange(xlow,xhigh);

  fBGFit.SetNpx(1000);
  fBGFit.SetLineStyle(2);
  fBGFit.SetLineColor(kBlack);

  // Changing the name here causes an infinite loop when starting the FitEditor
  //SetName(Form("gaus_%d_to_%d",(Int_t)(xlow),(Int_t)(xhigh)));
  InitNames();
  //TF1::SetParameter("centroid",cent);

}



GDoubleGaus::GDoubleGaus()
      : TF1("gausbg",GRootFunctions::DoubleGaus,0,1000,7,1,TF1::EAddToList::kNo),
        fBGFit("background","pol1",0,1000) {

  Clear();
  InitNames();
  fBGFit.SetNpx(1000);
  fBGFit.SetLineStyle(2);
  fBGFit.SetLineColor(kBlack);
}

GDoubleGaus::GDoubleGaus(const GDoubleGaus &peak)
  : TF1(peak) {
  peak.Copy(*this);
}

GDoubleGaus::~GDoubleGaus() {
  //if(background)
  //  delete background;
}

void GDoubleGaus::InitNames(){
  TF1::SetParName(0,"height1");
  TF1::SetParName(1,"centroid1");
  TF1::SetParName(2,"height2");
  TF1::SetParName(3,"centroid2");
  TF1::SetParName(4,"sigma");
  TF1::SetParName(5,"bg_offset");
  TF1::SetParName(6,"bg_slope");
}

void GDoubleGaus::Copy(TObject &obj) const {

  TF1::Copy(obj);
  ((GDoubleGaus&)obj).init_flag = init_flag;
  ((GDoubleGaus&)obj).fAreaTotal     = fAreaTotal;
  ((GDoubleGaus&)obj).fDAreaTotal    = fDAreaTotal;
  ((GDoubleGaus&)obj).fArea1         = fArea1;
  ((GDoubleGaus&)obj).fDArea1        = fDArea1;
  ((GDoubleGaus&)obj).fArea2         = fArea2;
  ((GDoubleGaus&)obj).fDArea2        = fDArea2;

  ((GDoubleGaus&)obj).fSumTotal     = fSumTotal;
  ((GDoubleGaus&)obj).fDSumTotal    = fDSumTotal;
  ((GDoubleGaus&)obj).fSum1         = fSum1;
  ((GDoubleGaus&)obj).fDSum1        = fDSum1;
  ((GDoubleGaus&)obj).fSum2         = fSum2;
  ((GDoubleGaus&)obj).fDSum2        = fDSum2;

  ((GDoubleGaus&)obj).fChi2     = fChi2;
  ((GDoubleGaus&)obj).fNdf      = fNdf;

  fBGFit.Copy((((GDoubleGaus&)obj).fBGFit));
}

bool GDoubleGaus::InitParams(TH1 *fithist,double cent1, double cent2){
  if(!fithist){
    printf("No histogram is associated yet, no initial guesses made\n");
    return false;
  }
  //printf("%s called.\n",__PRETTY_FUNCTION__); fflush(stdout);
  //Makes initial guesses at parameters for the fit. Uses the histogram to
  Double_t xlow,xhigh;
  GetRange(xlow,xhigh);

  Int_t binlow = fithist->GetXaxis()->FindBin(xlow);
  Int_t binhigh = fithist->GetXaxis()->FindBin(xhigh);
/*
  Double_t highy  = fithist->GetBinContent(binlow);
  Double_t lowy   = fithist->GetBinContent(binhigh);
  for(int x=1;x<5;x++) {
    highy += fithist->GetBinContent(binlow-x);
    lowy  += fithist->GetBinContent(binhigh+x);
  }
  highy = highy/5.0;
  lowy = lowy/5.0;

  if(lowy>highy)
    std::swap(lowy,highy);
*/
  double largestx=0.0;
  double largesty=0.0;
  int i = binlow;
  for(;i<=binhigh;i++) {
    if(fithist->GetBinContent(i) > largesty) {
      largesty = fithist->GetBinContent(i);
      largestx = fithist->GetXaxis()->GetBinCenter(i);
    }
  }

  // - par[0]: height of peak1
  // - par[1]: cent of peak1
  // - par[2]: height of peak2
  // - par[3]: cent of peak2
  // - par[4]: shared sigma
  // - par[5]: bg offset
  // - par[6]: bg slope

  //limits.
  TF1::SetParLimits(0,0,largesty*2);
  TF1::SetParLimits(1,xlow,cent2);
  TF1::SetParLimits(2,0,largesty*2);
  TF1::SetParLimits(3,cent1,xhigh);
  TF1::SetParLimits(4,0,xhigh-xlow);

  //TF1::SetParLimits(3,0.0,40);
  //TF1::SetParLimits(4,0.01,5);

  //Make initial guesses
  TF1::SetParameter(0,largesty);         //fithist->GetBinContent(bin));
  TF1::SetParameter(1,cent1);             //fithist->GetBinContent(bin));
  TF1::SetParameter(2,largesty);         //fithist->GetBinContent(bin));
  TF1::SetParameter(3,cent2);             //fithist->GetBinContent(bin));
  TF1::SetParameter(2,(largestx*.01)/2.35);                    //2,(xhigh-xlow));     //2.0/binWidth); //
 

  TF1::SetParError(0,0.10 * largesty);
  TF1::SetParError(1,0.25);

  TF1::SetParError(2,0.10 * largesty);
  TF1::SetParError(3,0.25);

  TF1::SetParError(4,0.10 *((largestx*.01)/2.35));
  
  SetInitialized();
  return true;
}


Bool_t GDoubleGaus::Fit(TH1 *fithist,double cent1,double cent2,Option_t *opt) {
  if(!fithist)
    return false;
  TString options = opt;
  if(!IsInitialized())
    InitParams(fithist,cent1,cent2);
  TVirtualFitter::SetMaxIterations(100000);

  bool verbose = !options.Contains("Q");
  bool noprint =  options.Contains("no-print");
  if(noprint) {
    options.ReplaceAll("no-print","");
  }


  if(fithist->GetSumw2()->fN!=fithist->GetNbinsX()+2)
    fithist->Sumw2();

  TFitResultPtr fitres = fithist->Fit(this,Form("%sRSME",options.Data()));

  //fitres.Get()->Print();
  if(!fitres.Get()->IsValid()) {
    if(!verbose)
      printf(RED  "fit has failed, trying refit... " RESET_COLOR);
    //SetParameter(3,0.1);
    //SetParameter(4,0.01);
    //SetParameter(5,0.0);
    fithist->GetListOfFunctions()->Last()->Delete();
    fitres = fithist->Fit(this,Form("%sRSME",options.Data())); //,Form("%sRSM",options.Data()))
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


  double bgpars[2];
  bgpars[0] = TF1::GetParameters()[5];
  bgpars[1] = TF1::GetParameters()[6];
  //bgpars[5] = TF1::GetParameters()[7];

  fBGFit.SetParameters(bgpars);
  //fithist->GetListOfFunctions()->Print();

  double gaus1pars[5];
  gaus1pars[0] = TF1::GetParameters()[0];
  gaus1pars[1] = TF1::GetParameters()[1];
  gaus1pars[2] = TF1::GetParameters()[4];
  gaus1pars[3] = TF1::GetParameters()[5];
  gaus1pars[4] = TF1::GetParameters()[6];
  fGaus1.SetParameters(gaus1pars);
  fGaus1.SetLineColor(kBlue);

  double gaus2pars[5];
  gaus2pars[0] = TF1::GetParameters()[2];
  gaus2pars[1] = TF1::GetParameters()[3];
  gaus2pars[2] = TF1::GetParameters()[4];
  gaus2pars[3] = TF1::GetParameters()[5];
  gaus2pars[4] = TF1::GetParameters()[6];
  fGaus2.SetParameters(gaus2pars);
  fGaus2.SetLineColor(kGreen);


  fAreaTotal = this->Integral(xlow,xhigh) / fithist->GetBinWidth(1);
  double bgArea = fBGFit.Integral(xlow,xhigh) / fithist->GetBinWidth(1);;
  fAreaTotal -= bgArea;

  if(xlow>xhigh)
    std::swap(xlow,xhigh);
  fSumTotal = fithist->Integral(fithist->GetXaxis()->FindBin(xlow),
                                 fithist->GetXaxis()->FindBin(xhigh)); //* fithist->GetBinWidth(1);
  printf("sum between markers: %02f\n",fSumTotal);
  fDSumTotal = TMath::Sqrt(fSumTotal);
  fSumTotal -= bgArea;
  printf("sum after subtraction: %02f\n",fSumTotal);

  if(!verbose && !noprint) {
    printf("hist: %s\n",fithist->GetName());
    Print();/*
    printf("BG Area:         %.02f\n",bgArea);
    printf("GetChisquared(): %.4f\n", TF1::GetChisquare());
    printf("GetNDF():        %i\n",   TF1::GetNDF());
    printf("GetProb():       %.4f\n", TF1::GetProb());*/
    //TF1::Print();
  }

  Copy(*fithist->GetListOfFunctions()->FindObject(GetName()));
  fithist->GetListOfFunctions()->Add(fBGFit.Clone());
  fithist->GetListOfFunctions()->Add(fGaus1.Clone());
  fithist->GetListOfFunctions()->Add(fGaus2.Clone());

  



  //delete tmppeak;
  return true;
}


void GDoubleGaus::Clear(Option_t *opt){
  TString options = opt;
  //Clear the GDoubleGaus including functions and histogram
  if(options.Contains("all"))
    TF1::Clear();
  init_flag = false;
  fAreaTotal  = 0.0;
  fDAreaTotal = 0.0;
  fSumTotal   = 0.0;
  fDSumTotal  = 0.0;
  fChi2       = 0.0;
  fNdf        = 0.0;
}

void GDoubleGaus::Print(Option_t *opt) const {
  TString options = opt;
  printf(GREEN );
  printf("Name: %s \n", this->GetName());
  printf("Centroid1: %1f +/- %1f \n", this->GetParameter("centroid1"),this->GetParError(GetParNumber("centroid1")));
  printf("Centroid2: %1f +/- %1f \n", this->GetParameter("centroid2"),this->GetParError(GetParNumber("centroid2")));
  printf("AreaTotal:      %1f +/- %1f \n", fAreaTotal, fDAreaTotal);
  printf("SumTotal:       %1f +/- %1f \n", fSumTotal, fDSumTotal);
  printf("FWHM:      %1f +/- %1f \n",this->GetFWHM(),this->GetFWHMErr());
  printf("Reso:      %1f%%  \n",this->GetFWHM()/this->GetParameter("centroid")*100.);
  printf("Chi^2/NDF: %1f\n",fChi2/fNdf);
  if(options.Contains("all")){
    TF1::Print(opt);
  }
  printf(RESET_COLOR);
  printf("\n");
}


void GDoubleGaus::DrawResiduals(TH1 *hist) const{
  if(hist){
    return;
  }
  if(fChi2<0.000000001){
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
