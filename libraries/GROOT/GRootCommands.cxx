
#include "GRootCommands.h"

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
  double thresh = 0.05;
  int num_found = 0;
  for(int i=0;i<nhists;i++) {
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
  for(int i=0;i<nhists;i++) {
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

bool PhotoPeakFit(TH1 *hist,double xlow, double xhigh,Option_t *opt) {
  bool edit = false; 
  if(!hist)
    return edit;
  int binx[2];
  double y[2];
  if(xlow>xhigh)
    std::swap(xlow,xhigh);
  binx[0] = hist->GetXaxis()->FindBin(xlow);
  binx[1] = hist->GetXaxis()->FindBin(xhigh);
  y[0] = hist->GetBinContent(xlow);
  y[1] = hist->GetBinContent(xhigh);

  GPeak *mypeak= new GPeak((xlow+xhigh)/2.0,xlow,xhigh);
  mypeak->Fit(hist,"Q+");
  //mypeak->Background()->Draw("SAME");
  TF1 *bg = new TF1(*mypeak->Background());
  hist->GetListOfFunctions()->Add(bg);
  edit = true;

  return edit;
}




