#include <fstream>
#include <cstdio>

#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TSpectrum.h"
#include "TH1.h"
#include "TList.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TRandom.h"

#include "TRawEvent.h"

using namespace std;

TList *FitRawEu(TH1 *hist,bool use121 = true,Double_t sigma=2.0,Double_t thresh = 0.2,Double_t width=10 /*keV*/, bool draw=false, bool is_core=false){

  TList *list = 0;
  if(!hist) return list;

  list = new TList();
  list->Add(hist);
  TH1 *bg = hist->ShowBackground(100,"");
  TH1 *newhist = (TH1*)hist->Clone(Form("%s_bgsub",hist->GetName()));
  TH1 *scalex = (TH1*)hist->Clone(Form("%s_roughcal",hist->GetName()));  
  TH1 *scalex_Clone = (TH1*)scalex->Clone(Form("%s_ShowAllFits",hist->GetName()));
  
  bg->SetTitle(Form("%s_bg",hist->GetName()));
  newhist->SetTitle(Form("%s_bgsub",hist->GetName()));
  newhist->Add(bg,-1);

  list->Add(bg);
  list->Add(newhist);

  Int_t peaksForFit=0;
  
  if(use121) peaksForFit = 6;
  else       peaksForFit = 4;
    
  if(is_core){
    hist->GetXaxis()->SetRangeUser(500,hist->GetXaxis()->GetXmax());
    newhist->GetXaxis()->SetRangeUser(500,hist->GetXaxis()->GetXmax());
  } else{
    hist->GetXaxis()->SetRangeUser(20,hist->GetXaxis()->GetXmax());
    newhist->GetXaxis()->SetRangeUser(20,hist->GetXaxis()->GetXmax());
  }

  TSpectrum s;
  s.Search(newhist,sigma,"",thresh); // hist,sigma,"",threshold (percentage)
  /*TCanvas Temp1;newhist->Draw();
  Temp1.Update();
  char inchar;
  cin >>inchar;
  */
  if(s.GetNPeaks()<peaksForFit){
    cout << " *** Raw Eu fit failed.  Only " << s.GetNPeaks() << " peaks found. *** " << endl;
    return 0;
  }

  Double_t *XVal = new Double_t[s.GetNPeaks()];
  Double_t *YVal = new Double_t[s.GetNPeaks()];
  Double_t XTemp=0;
  Double_t YTemp=0;
  
  // Order the peaks:
  for(Int_t i = 0;i<s.GetNPeaks();i++){
   
    XVal[i] = s.GetPositionX()[i];
    YVal[i] = s.GetPositionY()[i];
    
    if(i!=0){
      for(Int_t j = 0; j<i;j++){
	if(XVal[i]<XVal[j]){
	  XTemp = XVal[j];
	  YTemp = YVal[j];
	  XVal[j] = XVal[i];
	  YVal[j] = XVal[i];
	  XVal[i]=XTemp;
	  YVal[i]=YTemp;
	}
      }
    }
  }

  
  // Peaks we want to identify:
  //  1 - 121 keV    Int = 28.5 %   Rel Eff to 344 ~ 1.2 
  //  2 - 244 keV    Int =  7.6 %   Rel Eff to 344 ~ 1.0 
  //  3 - 344 keV    Int = 26.6 %   Rel Eff to 344 ~ 1.0 
  //  4 - 443 keV    Int =  2.8 %   Rel Eff to 344 ~ 0.9 
  //  5 - 778 keV    Int = 12.9 %   Rel Eff to 344 ~ 0.6 
  //  6 - 867 keV    Int =  4.2 %   Rel Eff to 344 ~ 0.5 
  //  7 - 964 keV    Int = 14.5 %   Rel Eff to 344 ~ 0.5 
  //  8 - 1112 keV   Int = 13.7 %   Rel Eff to 344 ~ 0.5 
  //  9 - 1408 keV   Int = 20.9 %   Rel Eff to 344 ~ 0.4

  Double_t Low_RC;
  Double_t High_RC;
  Double_t Rough_RC;
  Double_t Peaks9[9] = {121.782,244.697,344.279,443.965,778.904,867.373,964.079,1112.069,1408.006};
  Double_t Peaks8[8] = {244.697,344.279,443.965,778.904,867.373,964.079,1112.069,1408.006};
  TF1 *gausFit = new TF1("gausFit","gaus");
  vector<TF1*> gFit;
  TCanvas *C1 = new TCanvas("C1","",1750,750);
  TLine *lowLine = new TLine(0,0,0,100);
  TLine *highLine = new TLine(0,0,0,100);
  C1->Divide(3,1);
  lowLine->SetLineColor(kRed);
  highLine->SetLineColor(kRed);
  
  if(use121){
    if(XVal[0]>0 && XVal[s.GetNPeaks()]>0){
      Low_RC = 121.782/XVal[0];
      High_RC = 1408.006/XVal[s.GetNPeaks()-1];
      Rough_RC = (Low_RC+High_RC)/2.0;
    }
    // do the rough calibration here
    scalex->GetXaxis()->Set(newhist->GetNbinsX(),0,Rough_RC*newhist->GetXaxis()->GetXmax());
    scalex_Clone->GetXaxis()->Set(newhist->GetNbinsX(),0,Rough_RC*newhist->GetXaxis()->GetXmax());

    scalex_Clone->GetXaxis()->SetRangeUser(Peaks9[0]-150,Peaks9[8]+150);
    lowLine->SetY2(scalex_Clone->GetMaximum());
    highLine->SetY2(scalex_Clone->GetMaximum());

    C1->cd(1);
    scalex_Clone->Draw();
    for(int i_i = 0;i_i<9;i_i++){

      scalex->GetXaxis()->SetRangeUser(Peaks9[i_i]-width,Peaks9[i_i]+width);
      
      C1->cd(1);
      lowLine->SetX1(Peaks9[i_i]-width);lowLine->SetX2(Peaks9[i_i]-width);
      highLine->SetX1(Peaks9[i_i]+width);highLine->SetX2(Peaks9[i_i]+width);
      lowLine->Draw("same");
      highLine->Draw("same");

      C1->cd(2);
      scalex->Draw();
      scalex->Fit(gausFit,"Q");
      gFit.push_back((TF1*)(scalex->GetFunction("gausFit")->Clone(Form("gaus_%i",i_i))));


      // Message to user:
      cout << " *** Peak " << i_i  << " Fit    :    " << Peaks9[i_i] << " keV *** " << endl;
      cout << endl;
      cout << "      Centroid : " << gausFit->GetParameter(1) << endl;
      cout << "      Chi2/NDF : " << Double_t((Double_t)(gausFit->GetChisquare())/(Double_t)(gausFit->GetNDF())) << endl;
      cout << endl;
    
      C1->Update();
    }
    C1->cd(1);
    TPad *mypad = (TPad*)C1->GetPad(1);
    TF1 *linFit = new TF1("linFit","pol1(0)");
    TH1 *hist1 = (TH1*)( mypad->GetListOfPrimitives()->At(1));
    TGraphErrors *linGraph = new TGraphErrors(9);
    
    for(int j = 0; j<gFit.size();j++) {
      hist1->GetListOfFunctions()->Add(gFit.at(j));
      linGraph->SetPoint(j,gFit[j]->GetParameter(1),Peaks9[j]);
      linGraph->SetPointError(j,TMath::Sqrt(gFit[j]->GetParameter(1)),j);
    }
    
    mypad->Modified();
    mypad->Update();
    list->Add(scalex_Clone);
    C1->cd(3);
    linGraph->Draw("AP");
    
    linFit->SetParameter(0,0);
    linFit->SetParameter(1,Rough_RC);
    linGraph->Draw("AP");
    linGraph->Fit(linFit,"Q");
    list->Add(linGraph);

    cout << " ***** Linear Fit  ***** " << endl;
    cout << endl;
    cout << "      Offset   : " << linFit->GetParameter(0) << endl;
    cout << "      Slope    : " << linFit->GetParameter(1) << endl;
    cout << "      Chi2/NDF : " << Double_t((Double_t)(linFit->GetChisquare())/(Double_t)(linFit->GetNDF())) << endl << endl;
     
  }
  else{
    if(XVal[0]>0 && XVal[s.GetNPeaks()]>0){
      Double_t Ratio_1to2 = YVal[0]/YVal[1];
      if(Ratio_1to2>0) Low_RC = 244.697/XVal[1];
      else             Low_RC = 244.697/XVal[0];
      High_RC = 1408.006/XVal[s.GetNPeaks()-1];
      Rough_RC = (Low_RC+High_RC)/2.0;

      cout << " Low RC   : " << Low_RC << endl;
      cout << " High RC  : " << High_RC << endl;
      cout << " Rough RC : " << Rough_RC << endl;

      cout << " Peaks Found : " << s.GetNPeaks() << endl;
      //for(int i = 0; i < s.GetNPeaks();i++)
    }
    // do the rough calibration here
    scalex->GetXaxis()->Set(newhist->GetNbinsX(),0,Rough_RC*newhist->GetXaxis()->GetXmax());
    scalex_Clone->GetXaxis()->Set(newhist->GetNbinsX(),0,Rough_RC*newhist->GetXaxis()->GetXmax());

    scalex_Clone->GetXaxis()->SetRangeUser(Peaks8[0]-150,Peaks8[7]+150);
    lowLine->SetY2(scalex_Clone->GetMaximum());
    highLine->SetY2(scalex_Clone->GetMaximum());

    C1->cd(1);
    scalex_Clone->Draw();
    for(int i_i = 0;i_i<8;i_i++){

      scalex->GetXaxis()->SetRangeUser(Peaks8[i_i]-width,Peaks8[i_i]+width);
      
      C1->cd(1);
      lowLine->SetX1(Peaks8[i_i]-width);lowLine->SetX2(Peaks8[i_i]-width);
      highLine->SetX1(Peaks8[i_i]+width);highLine->SetX2(Peaks8[i_i]+width);
      lowLine->Draw("same");
      highLine->Draw("same");

      C1->cd(2);
      scalex->Draw();
      scalex->Fit(gausFit,"Q");
      gFit.push_back((TF1*)(scalex->GetFunction("gausFit")->Clone(Form("gaus_%i",i_i))));


      // Message to user:
      cout << " *** Peak " << i_i  << " Fit    :    " << Peaks8[i_i] << " keV *** " << endl;
      cout << endl;
      cout << "      Centroid : " << gausFit->GetParameter(1) << endl;
      cout << "      Chi2/NDF : " << Double_t((Double_t)(gausFit->GetChisquare())/(Double_t)(gausFit->GetNDF())) << endl;
      cout << endl;
    
      C1->Update();
    }
    C1->cd(1);
    TPad *mypad = (TPad*)C1->GetPad(1);
    TF1 *linFit = new TF1("linFit","pol1(0)");
    TH1 *hist1 = (TH1*)( mypad->GetListOfPrimitives()->At(1));
    TGraphErrors *linGraph = new TGraphErrors(9);
    
    for(int j = 0; j<gFit.size();j++) {
      hist1->GetListOfFunctions()->Add(gFit.at(j));
      linGraph->SetPoint(j,gFit[j]->GetParameter(1),Peaks8[j]);
      linGraph->SetPointError(j,TMath::Sqrt(gFit[j]->GetParameter(1)),j);
    }
    
    mypad->Modified();
    mypad->Update();
    list->Add(scalex_Clone);
    C1->cd(3);
    linGraph->Draw("AP");
    
    linFit->SetParameter(0,0);
    linFit->SetParameter(1,Rough_RC);
    linGraph->Draw("AP");
    linGraph->Fit(linFit,"Q");
    list->Add(linGraph);

    cout << " ***** Linear Fit  ***** " << endl;
    cout << endl;
    cout << "      Offset   : " << linFit->GetParameter(0) << endl;
    cout << "      Slope    : " << linFit->GetParameter(1) << endl;
    cout << "      Chi2/NDF : " << Double_t((Double_t)(linFit->GetChisquare())/(Double_t)(linFit->GetNDF())) << endl << endl;        
  }
  
  return list;

}
