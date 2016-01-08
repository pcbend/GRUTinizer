
#include "GRootCommands.h"
#include "Globals.h"
#include <cstdio>
//#include <string>
#include <sstream>


#include <TRint.h>
#include <TTree.h>
#include <Getline.h>
#include <TAxis.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TPolyMarker.h>
#include <TSpectrum.h>
#include <TText.h>
#include <TExec.h>
#include <TKey.h>
#include <TObject.h>
#include <TObjArray.h>
#include <TH1.h>

#include <GPeak.h>
//#include <GRootObjectManager.h>


void Help()     { printf("This is helpful information.\n"); }

void Commands() { printf("this is a list of useful commands.\n");}

void Prompt() { Getlinem(EGetLineMode::kInit,((TRint*)gApplication)->GetPrompt()); }

int LabelPeaks(TH1 *hist,double sigma,double thresh,Option_t *opt) {
  TSpectrum::StaticSearch(hist,sigma,"Qnodraw",thresh);
  TPolyMarker *pm = (TPolyMarker*)hist->GetListOfFunctions()->FindObject("TPolyMarker");
  if(!pm) {
    //something has gone wrong....
    return 0;
  }
  TObjArray *array = (TObjArray*)hist->GetListOfFunctions()->FindObject("PeakLabels");
  if(array) {
    hist->GetListOfFunctions()->Remove((TObject*)array);
    array->Delete();
  }
  array = new TObjArray();
  array->SetName("PeakLabels");
  int n = pm->GetN();
  if(n==0)
    return n;
  TText *text;
  double *x = pm->GetX();
  double *y = pm->GetY();
  for(int i=0;i<n;i++) {
    text = new TText(x[i],y[i],Form("%.1f",x[i]));
    text->SetTextSize(0.025);
    text->SetTextAngle(90);
    text->SetTextAlign(12);
    text->SetTextFont(42);
    text->SetTextColor(hist->GetLineColor());
    array->Add(text);
  }
  hist->GetListOfFunctions()->Remove(pm);
  pm->Delete();
  hist->GetListOfFunctions()->Add(array);
  return n;
}


bool ShowPeaks(TH1 **hists,unsigned int nhists) {
  double sigma  = 2.0;
  double thresh = 0.02;
  int num_found = 0;
  for(unsigned int i=0;i<nhists;i++) {
    if(TObject *obj = hists[i]->GetListOfFunctions()->FindObject("PeakLabels")) {
      hists[i]->GetListOfFunctions()->Remove(obj);
      ((TObjArray*)obj)->Delete();
    }
    num_found += LabelPeaks(hists[i],sigma,thresh,"");
  }
  if(num_found)
    return true;
  return false;
}

bool RemovePeaks(TH1 **hists,unsigned int nhists) {
  bool flag = false;
  for(unsigned int i=0;i<nhists;i++) {
    if(TObject *obj=hists[i]->GetListOfFunctions()->FindObject("PeakLabels")) {
      hists[i]->GetListOfFunctions()->Remove(obj);
      ((TObjArray*)obj)->Delete();
      flag = true;
    }
  }
  return flag;
}

//bool PeakFit(TH1 *hist,Double_t xlow, Double_t xhigh,Option_t *opt) {
//  if(!hist)
//   return;
//  TString option = opt;
//}

bool GausFit(TH1 *hist,double xlow, double xhigh,Option_t *opt) {
  bool edit = false;
  if(!hist)
    return edit;
  int binx[2];
  double y[2];
  if(xlow>xhigh)
    std::swap(xlow,xhigh);
  binx[0] = hist->GetXaxis()->FindBin(xlow);
  binx[1] = hist->GetXaxis()->FindBin(xhigh);
  y[0] = hist->GetBinContent(binx[0]);
  y[1] = hist->GetBinContent(binx[1]);

  TF1 *myGaus= new TF1(Form("gaus_%i_%i",int(xlow),int(xhigh)),"gaus(0)+pol1(3)",xlow,xhigh);
  TF1 *bg = new TF1("bg","pol1",xlow,xhigh);

  myGaus->SetParameter(0,hist->GetBinContent((binx[1]+binx[0])/2));
  myGaus->SetParameter(2,(xhigh-xlow)/2.0);
  myGaus->SetParameter(1,(xhigh-xlow)/2.0+xlow);
  myGaus->SetParameter(4,(y[1]-y[0])/(xhigh-xlow));
  myGaus->SetParameter(3,((y[1]-y[0])/(xhigh-xlow))*xlow);

  myGaus->SetParName(0,"height");
  myGaus->SetParName(1,"centroid");
  myGaus->SetParName(2,"sigma");
  myGaus->SetParName(4,"bg_slope");
  myGaus->SetParName(3,"bg_offset");


  hist->Fit(myGaus,"RQ+");

  bg->SetParameters(myGaus->GetParameter(3),myGaus->GetParameter(4));
  bg->SetLineColor(kBlue);
  hist->GetListOfFunctions()->Add(bg);

  double param[5];
  double error[5];

  myGaus->GetParameters(param);
  error[0] = myGaus->GetParError(0);
  error[1] = myGaus->GetParError(1);
  error[2] = myGaus->GetParError(2);
  error[3] = myGaus->GetParError(3);
  error[4] = myGaus->GetParError(4);

  printf(GREEN "Integral from % 4.01f to % 4.01f: %f" RESET_COLOR "\n",
               xlow,xhigh,myGaus->Integral(xlow,xhigh)/hist->GetBinWidth(1)-
                          bg->Integral(xlow,xhigh)/hist->GetBinWidth(1));
  printf(GREEN "Centroid  : % 4.02f +/- %.02f" RESET_COLOR "\n",param[1],error[1]);
  printf(GREEN "FWHM      : % 4.02f +/- %.02f" RESET_COLOR "\n",TMath::Abs(param[2]*2.35),TMath::Abs(error[2]*2.35));
  printf(GREEN "Resolution: %.02f %%" RESET_COLOR "\n",TMath::Abs(param[2]*2.35)/param[1]*100.0);






  edit = true;

  return edit;

}

bool PhotoPeakFit(TH1 *hist,double xlow, double xhigh,Option_t *opt) {
  bool edit = false;
  if(!hist)
    return edit;
  if(xlow>xhigh)
    std::swap(xlow,xhigh);

  GPeak *mypeak= new GPeak((xlow+xhigh)/2.0,xlow,xhigh);
  mypeak->Fit(hist,"Q+");
  //mypeak->Background()->Draw("SAME");
  TF1 *bg = new TF1(*mypeak->Background());
  hist->GetListOfFunctions()->Add(bg);
  edit = true;

  return edit;
}

std::string MergeStrings(const std::vector<std::string>& strings, char split) {
  std::stringstream ss;
  for(auto it = strings.begin(); it != strings.end(); it++) {
    ss << *it;

    auto next = it;
    next++;
    if(next != strings.end()){
      ss << split;
    }
  }
  return ss.str();
}
