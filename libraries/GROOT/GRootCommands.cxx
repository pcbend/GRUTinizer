
#include "GRootCommands.h"
#include "Globals.h"
#include <cstdio>
//#include <string>
#include <sstream>
#include <fstream>

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
#include <TPython.h>
#include <TTimer.h>
#include <TF1.h>
#include <TROOT.h>
#include <TStyle.h>

#include <GCanvas.h>
#include <GPeak.h>
#include <GGaus.h>
#include <GDoubleGaus.h>
#include <GH2D.h>
#include <GH1D.h>
//#include <GRootObjectManager.h>
#include <TGRUTOptions.h>
//#include <TGRUTInt.h>
#include <GrutNotifier.h>

TChain *gChain = new TChain("EventTree");//NULL;

class TempThing{
public:
  TempThing() {
    gChain->SetNotify(GrutNotifier::Get());
  }
} temp_thing;



void Help()     { printf("This is helpful information.\n"); }

void Commands() { printf("this is a list of useful commands.\n");}

void Prompt() { Getlinem(EGetLineMode::kInit,((TRint*)gApplication)->GetPrompt()); }

void Version() {
  system(Form("%s/bin/grutinizer-config --version", getenv("GRUTSYS")));
}


/*
bool GetProjectionX(GH2D *hist,double low, double high, double bg_low,double bg_high){
  if(!hist) return 0;
  GCanvas *C_projections = 0;
  GCanvas *C_gammagamma  = 0;
  if(gROOT->GetListOfCanvases()->FindObject("C_projections"))
    C_projections = (GCanvas*)gROOT->GetListOfCanvases()->FindObject("C_projections");
  else{
    C_projections = new GCanvas("C_projections","Projection Canvas",0,0,1450,600);
    C_projections->Divide(2,1);
  }

  if(gROOT->GetListOfCanvases()->FindObject("C_gammagamma"))
    C_gammagamma = (GCanvas*)gROOT->GetListOfCanvases()->FindObject("C_gammagamma");
  else
    C_gammagamma = new GCanvas("C_gammagamma","Gamma-Gamma Canvas",1700,0,650,650);

  C_gammagamma->cd();
  hist->Draw();

  C_projections->cd(1);
  GH1D *Proj_y = hist->ProjectionX("Gamma_Gamma_xProjection");
  GH1D *Proj_y_Clone = (GH1D*)Proj_y->Clone();
  GH1D *Proj_gated = 0;

  if(bg_high>0 && bg_low>0){
    Proj_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] and Background [%.01f,%.01f]",low,high,bg_low,bg_high));
  }else{
    Proj_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] NO background",low,high));
  }

  Proj_y->GetXaxis()->SetTitle("Energy [keV]");
  Proj_y->GetYaxis()->SetTitle("Counts");



  double Grace = 300;
  double ZoomHigh = high+Grace;
  double ZoomLow  = low-Grace;
  if(bg_high>0 && bg_high>high)
    ZoomHigh = bg_high+Grace;
  if(bg_low>0 && bg_low<low)
    ZoomLow = bg_low-Grace;

  Proj_y->GetXaxis()->SetRangeUser(ZoomLow,ZoomHigh);
  Proj_y->Draw();
  double Projy_Max = Proj_y->GetMaximum();
  double Projy_Min = Proj_y->GetMinimum();

  TLine *CutLow  = new TLine(low,Projy_Min,low,Projy_Max);
  TLine *CutHigh = new TLine(high,Projy_Min,high,Projy_Max);
  TLine *BGLow   = new TLine(bg_low,Projy_Min,bg_low,Projy_Max);
  TLine *BGHigh  = new TLine(bg_high,Projy_Min,bg_high,Projy_Max);
  CutLow->SetLineColor(kRed);
  CutHigh->SetLineColor(kRed);
  CutLow->SetLineWidth(2);
  CutHigh->SetLineWidth(2);
  BGLow->SetLineColor(kBlue);
  BGHigh->SetLineColor(kBlue);
  BGLow->SetLineWidth(2);
  BGHigh->SetLineWidth(2);
  BGLow->SetLineStyle(kDashed);
  BGHigh->SetLineStyle(kDashed);
  CutLow->Draw("same");
  CutHigh->Draw("same");
  if(bg_low>0 && bg_high>0){
    BGHigh->Draw("same");
    BGLow->Draw("same");
    Proj_gated = Proj_y_Clone->Project(low,high,bg_low,bg_high,kRegionBackground);
  }else{
    Proj_gated = Proj_y_Clone->Project(low,high);
  }

  if(bg_high>0 && bg_low>0){
    Proj_gated->SetTitle(Form("Gate From [%.01f,%.01f] with Background [%.01f,%.01f]",low,high,bg_low,bg_high));
  }else{
    Proj_gated->SetTitle(Form("Gate From [%.01f,%.01f] NO Background",low,high));
  }
  Proj_gated->GetXaxis()->SetTitle("Energy [keV]");
  Proj_gated->GetYaxis()->SetTitle("Counts");

  C_projections->cd(2);
  Proj_gated->Draw();
  return 1;
}
*/


/*
bool GetProjectionX(GH2D *hist,GH2D *hist2,
		   double low, double high, double bg_low,double bg_high,
		   bool overlay,
		   double back_low,double back_high, double back_bg_low,
		   double back_bg_high,
		   bool back_overlay){
  //Note -> The first histogram, ie hist, is used to draw the total projection.
  //        The second histogram, ie hist2, is used to draw the background
  //        subtracted spectrum. This way you can have different bins.
  //
  //

  if(!hist || !hist2) return 0;

  gStyle->SetOptStat(0);

  GCanvas *C_projections      = 0;
  GCanvas *C_gammagamma       = 0;
  if(gROOT->GetListOfCanvases()->FindObject("C_projections")){
    C_projections = (GCanvas*)gROOT->GetListOfCanvases()->FindObject("C_projections");
    C_projections->Clear();
    if(back_low>0 && back_high>0)
      C_projections->Divide(2,2);
    else
      C_projections->Divide(1,2);

  }
  else{
    C_projections = new GCanvas("C_projections","Projection Canvas",0,0,1675,900);
    if(back_low>0 && back_high>0)
      C_projections->Divide(2,2);
    else
      C_projections->Divide(1,2);
  }

  if(gROOT->GetListOfCanvases()->FindObject("C_gammagamma"))
    C_gammagamma = (GCanvas*)gROOT->GetListOfCanvases()->FindObject("C_gammagamma");
  else
    C_gammagamma = new GCanvas("C_gammagamma","Gamma-Gamma Canvas",1700,0,650,650);

  C_gammagamma->cd();
  hist->Draw();

  C_projections->cd(1);
  GH1D *Proj_y  = hist->ProjectionX("Gamma_Gamma_xProjection");
  GH1D *Proj_y2 = hist2->ProjectionX("Gamma_Gamma_xProjection2");

  GH2D *hist_Clone  = (GH2D*)hist->Clone();
  GH2D *hist2_Clone = (GH2D*)hist2->Clone();

  GH1D *Proj_back_y  = hist_Clone->ProjectionX("Gamma_Gamma_xProjection_back");
  GH1D *Proj_back_y2 = hist2_Clone->ProjectionX("Gamma_Gamma_xProjection2_back");


  //  GH1D *Proj_x_Clone = (GH1D*)Proj_x->Clone();
  GH1D *Proj_y2_Clone       = (GH1D*)Proj_y2->Clone();
  GH1D *Proj_y2_Clone2      = (GH1D*)Proj_y2->Clone();
  GH1D *Proj_back_y2_Clone  = (GH1D*)Proj_back_y2->Clone();
  GH1D *Proj_back_y2_Clone2 = (GH1D*)Proj_back_y2->Clone();
  GH1D *Proj_gated          = 0;
  GH1D *Proj_gated2         = 0;
  GH1D *Proj_gated_back     = 0;
  GH1D *Proj_gated_back2    = 0;

  if(bg_high>0 && bg_low>0){
    Proj_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] and Background [%.01f,%.01f]",low,high,bg_low,bg_high));
  }else{
    Proj_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] NO background",low,high));
  }

  if(back_bg_high>0 && back_bg_low>0 && back_low>0 && back_high>0){
    Proj_back_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] and Background [%.01f,%.01f]",back_low,back_high,back_bg_low,back_bg_high));
  }else if(back_low>0 && back_high>0){
    Proj_back_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] NO background",back_low,back_high));
  }


  double binsize = (Proj_y->GetXaxis()->GetXmax()-Proj_y->GetXaxis()->GetXmin())/Proj_y->GetXaxis()->GetNbins();
  Proj_y->GetXaxis()->SetTitle("Energy [keV]");
  Proj_y->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize));
  Proj_back_y->GetXaxis()->SetTitle("Energy [keV]");
  Proj_back_y->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize));



  double Grace = 300;
  double ZoomHigh = high+Grace;
  double ZoomLow  = low-Grace;
  if(bg_high>0 && bg_high>high)
    ZoomHigh = bg_high+Grace;
  if(bg_low>0 && bg_low<low)
    ZoomLow = bg_low-Grace;

  Proj_y->GetXaxis()->SetRangeUser(ZoomLow,ZoomHigh);
  Proj_y->Draw();
  double Projy_Max = Proj_y->GetMaximum();
  double Projy_Min = Proj_y->GetMinimum();

  TLine *CutLow  = new TLine(low,Projy_Min,low,Projy_Max);
  TLine *CutHigh = new TLine(high,Projy_Min,high,Projy_Max);
  TLine *BGLow   = new TLine(bg_low,Projy_Min,bg_low,Projy_Max);
  TLine *BGHigh  = new TLine(bg_high,Projy_Min,bg_high,Projy_Max);
  CutLow->SetLineColor(kRed);
  CutHigh->SetLineColor(kRed);
  CutLow->SetLineWidth(2);
  CutHigh->SetLineWidth(2);
  BGLow->SetLineColor(kBlue);
  BGHigh->SetLineColor(kBlue);
  BGLow->SetLineWidth(2);
  BGHigh->SetLineWidth(2);
  BGLow->SetLineStyle(kDashed);
  BGHigh->SetLineStyle(kDashed);
  CutLow->Draw("same");
  CutHigh->Draw("same");
  if(bg_low>0 && bg_high>0){
    BGHigh->Draw("same");
    BGLow->Draw("same");
    Proj_gated = Proj_y2_Clone->Project_Background(low,high,
						  bg_low,bg_high,
						  kRegionBackground);
  }else{
    Proj_gated = Proj_y2_Clone->Project(low,high);
  }
  if(overlay){
    Proj_gated2 = Proj_y2_Clone2->Project(low,high);
    Proj_gated2->SetLineColor(2);
  }

  if(bg_high>0 && bg_low>0){
    Proj_gated->SetTitle(Form("Gate From [%.01f,%.01f] with Background [%.01f,%.01f]",low,high,bg_low,bg_high));
    if(overlay)
      Proj_gated2->SetTitle(Form("Gate From [%.01f,%.01f] with Background [%.01f,%.01f]",low,high,bg_low,bg_high));
  }else{
    Proj_gated->SetTitle(Form("Gate From [%.01f,%.01f] NO Background",low,high));
    if(overlay)
      Proj_gated2->SetTitle(Form("Gate From [%.01f,%.01f] NO Background",low,high));
  }
  double binsize_gated = (Proj_gated->GetXaxis()->GetXmax()-Proj_gated->GetXaxis()->GetXmin())/Proj_gated->GetXaxis()->GetNbins();
  Proj_gated->GetXaxis()->SetTitle("Energy [keV]");
  Proj_gated->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize_gated));


  C_projections->cd(2);
  if(overlay){
    Proj_gated2->GetXaxis()->SetTitle("Energy [keV]");
    Proj_gated2->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize_gated));

    Proj_gated2->Draw();
    Proj_gated->Draw("same");
  } else
    Proj_gated->Draw();
  if(back_low<0 && back_high<0)
    return 1;


  //---------------------------------------------------------------------------

  C_projections->cd(3);
  gPad->Clear();
  C_projections->cd(4);
  gPad->Clear();

  if(back_low>0 && back_high>0){
    C_projections->cd(3);
    double back_ZoomHigh = back_high+Grace;
    double back_ZoomLow  = back_low-Grace;
    if(back_bg_high>0 && back_bg_high>high)
      back_ZoomHigh = back_bg_high+Grace;
    if(back_bg_low>0 && back_bg_low<low)
      back_ZoomLow = back_bg_low-Grace;


    Proj_back_y->GetXaxis()->SetRangeUser(back_ZoomLow,back_ZoomHigh);
    Proj_back_y->Draw();
    double Proj_back_y_Max = Proj_back_y->GetMaximum();
    double Proj_back_y_Min = Proj_back_y->GetMinimum();

    TLine *back_CutLow  = new TLine(back_low,Proj_back_y_Min,back_low,Proj_back_y_Max);
    TLine *back_CutHigh = new TLine(back_high,Proj_back_y_Min,back_high,Proj_back_y_Max);
    TLine *back_BGLow   = new TLine(back_bg_low,Proj_back_y_Min,back_bg_low,Proj_back_y_Max);
    TLine *back_BGHigh  = new TLine(back_bg_high,Proj_back_y_Min,back_bg_high,Proj_back_y_Max);
    back_CutLow->SetLineColor(kRed);
    back_CutHigh->SetLineColor(kRed);
    back_CutLow->SetLineWidth(2);
    back_CutHigh->SetLineWidth(2);
    back_BGLow->SetLineColor(kBlue);
    back_BGHigh->SetLineColor(kBlue);
    back_BGLow->SetLineWidth(2);
    back_BGHigh->SetLineWidth(2);
    back_BGLow->SetLineStyle(kDashed);
    back_BGHigh->SetLineStyle(kDashed);
    back_CutLow->Draw("same");
    back_CutHigh->Draw("same");
    if(back_bg_low>0 && back_bg_high>0){
      back_BGHigh->Draw("same");
      back_BGLow->Draw("same");
      Proj_gated_back = Proj_back_y2_Clone->Project_Background(back_low,back_high,
							       back_bg_low,back_bg_high,
							       kRegionBackground);
    }else{
      Proj_gated_back = Proj_back_y2_Clone->Project(back_low,back_high);
    }

    if(back_overlay){
      Proj_gated_back2 = Proj_back_y2_Clone2->Project(back_low,back_high);
      Proj_gated_back2->SetLineColor(2);
    }

    if(back_bg_high>0 && back_bg_low>0){
      Proj_gated_back->SetTitle(Form("Gate From [%.01f,%.01f] with Background [%.01f,%.01f]",
				     back_low,back_high,back_bg_low,back_bg_high));
      if(back_overlay){
      Proj_gated_back2->SetTitle(Form("Gate From [%.01f,%.01f] with Background [%.01f,%.01f]",
				     back_low,back_high,back_bg_low,back_bg_high));
      }
    }else{
      Proj_gated_back->SetTitle(Form("Gate From [%.01f,%.01f] NO Background",back_low,back_high));
      if(back_overlay){
	Proj_gated_back2->SetTitle(Form("Gate From [%.01f,%.01f] NO Background",back_low,back_high));
      }

    }
    double binsize_back_gated = (Proj_gated_back->GetXaxis()->GetXmax()-Proj_gated_back->GetXaxis()->GetXmin())/Proj_gated_back->GetXaxis()->GetNbins();
    Proj_gated_back->GetXaxis()->SetTitle("Energy [keV]");
    Proj_gated_back->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize_back_gated));

    C_projections->cd(4);
    if(back_overlay){
      Proj_gated_back2->GetXaxis()->SetTitle("Energy [keV]");
      Proj_gated_back2->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize_back_gated));

      Proj_gated_back2->Draw();
      Proj_gated_back->Draw("same");
    } else{
      Proj_gated_back->Draw();
    }
  }





  return 1;
}
*/
/*
bool GetProjectionY(GH2D *hist,double low, double high, double bg_low,double bg_high){
  if(!hist) return 0;
  GCanvas *C_projections = 0;
  GCanvas *C_gammagamma  = 0;
  if(gROOT->GetListOfCanvases()->FindObject("C_projections"))
    C_projections = (GCanvas*)gROOT->GetListOfCanvases()->FindObject("C_projections");
  else{
    C_projections = new GCanvas("C_projections","Projection Canvas",0,0,1450,600);
    C_projections->Divide(2,1);
  }

  if(gROOT->GetListOfCanvases()->FindObject("C_gammagamma"))
    C_gammagamma = (GCanvas*)gROOT->GetListOfCanvases()->FindObject("C_gammagamma");
  else
    C_gammagamma = new GCanvas("C_gammagamma","Gamma-Gamma Canvas",1700,0,650,650);

  C_gammagamma->cd();
  hist->Draw();

  C_projections->cd(1);
  GH1D *Proj_y = hist->ProjectionY("Gamma_Gamma_yProjection");
  GH1D *Proj_y_Clone = (GH1D*)Proj_y->Clone();
  GH1D *Proj_gated = 0;

  if(bg_high>0 && bg_low>0){
    Proj_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] and Background [%.01f,%.01f]",low,high,bg_low,bg_high));
  }else{
    Proj_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] NO background",low,high));
  }

  Proj_y->GetXaxis()->SetTitle("Energy [keV]");
  Proj_y->GetYaxis()->SetTitle("Counts");

  double Grace = 300;
  double ZoomHigh = high+Grace;
  double ZoomLow  = low-Grace;
  if(bg_high>0 && bg_high>high)
    ZoomHigh = bg_high+Grace;
  if(bg_low>0 && bg_low<low)
    ZoomLow = bg_low-Grace;

  Proj_y->GetXaxis()->SetRangeUser(ZoomLow,ZoomHigh);
  Proj_y->Draw();
  double Projy_Max = Proj_y->GetMaximum();
  double Projy_Min = Proj_y->GetMinimum();

  TLine *CutLow  = new TLine(low,Projy_Min,low,Projy_Max);
  TLine *CutHigh = new TLine(high,Projy_Min,high,Projy_Max);
  TLine *BGLow   = new TLine(bg_low,Projy_Min,bg_low,Projy_Max);
  TLine *BGHigh  = new TLine(bg_high,Projy_Min,bg_high,Projy_Max);
  CutLow->SetLineColor(kRed);
  CutHigh->SetLineColor(kRed);
  CutLow->SetLineWidth(2);
  CutHigh->SetLineWidth(2);
  BGLow->SetLineColor(kBlue);
  BGHigh->SetLineColor(kBlue);
  BGLow->SetLineWidth(2);
  BGHigh->SetLineWidth(2);
  BGLow->SetLineStyle(kDashed);
  BGHigh->SetLineStyle(kDashed);
  CutLow->Draw("same");
  CutHigh->Draw("same");
  if(bg_low>0 && bg_high>0){
    BGHigh->Draw("same");
    BGLow->Draw("same");
    Proj_gated = Proj_y_Clone->Project_Background(low,high,
						  bg_low,bg_high,
						  kRegionBackground);
  }else{
    Proj_gated = Proj_y_Clone->Project(low,high);
  }

  if(bg_high>0 && bg_low>0){
    Proj_gated->SetTitle(Form("Gate From [%.01f,%.01f] with Background [%.01f,%.01f]",low,high,bg_low,bg_high));
  }else{
    Proj_gated->SetTitle(Form("Gate From [%.01f,%.01f] NO Background",low,high));
  }
  Proj_gated->GetXaxis()->SetTitle("Energy [keV]");
  Proj_gated->GetYaxis()->SetTitle("Counts");

  C_projections->cd(2);
  Proj_gated->Draw();
  return 1;
}

bool GetProjectionY(GH2D *hist,GH2D *hist2,

		   double low, double high, double bg_low,double bg_high,
		   bool overlay,
		   double back_low,double back_high, double back_bg_low,
		   double back_bg_high,
		   bool back_overlay){
  // Note -> The first histogram, ie hist, is used to draw the total projection.
  //         The second histogram, ie hist2, is used to draw the background
  //         subtracted spectrum. This way you can have different bins.
  //
  //

  if(!hist || !hist2) return 0;

  gStyle->SetOptStat(0);

  GCanvas *C_projections      = 0;
  GCanvas *C_gammagamma       = 0;
  if(gROOT->GetListOfCanvases()->FindObject("C_projections")){
    C_projections = (GCanvas*)gROOT->GetListOfCanvases()->FindObject("C_projections");
    C_projections->Clear();
    if(back_low>0 && back_high>0)
      C_projections->Divide(2,2);
    else
      C_projections->Divide(1,2);

  }
  else{
    C_projections = new GCanvas("C_projections","Projection Canvas",0,0,1675,900);
    if(back_low>0 && back_high>0)
      C_projections->Divide(2,2);
    else
      C_projections->Divide(1,2);
  }

  if(gROOT->GetListOfCanvases()->FindObject("C_gammagamma"))
    C_gammagamma = (GCanvas*)gROOT->GetListOfCanvases()->FindObject("C_gammagamma");
  else
    C_gammagamma = new GCanvas("C_gammagamma","Gamma-Gamma Canvas",1700,0,650,650);

  C_gammagamma->cd();
  hist->Draw();

  C_projections->cd(1);
  GH1D *Proj_y  = hist->ProjectionY("Gamma_Gamma_yProjection");
  GH1D *Proj_y2 = hist2->ProjectionY("Gamma_Gamma_yProjection2");

  GH2D *hist_Clone  = (GH2D*)hist->Clone();
  GH2D *hist2_Clone = (GH2D*)hist2->Clone();

  GH1D *Proj_back_y  = hist_Clone->ProjectionY("Gamma_Gamma_yProjection_back");
  GH1D *Proj_back_y2 = hist2_Clone->ProjectionY("Gamma_Gamma_yProjection2_back");


  //  GH1D *Proj_x_Clone = (GH1D*)Proj_x->Clone();
  GH1D *Proj_y2_Clone       = (GH1D*)Proj_y2->Clone();
  GH1D *Proj_y2_Clone2      = (GH1D*)Proj_y2->Clone();
  GH1D *Proj_back_y2_Clone  = (GH1D*)Proj_back_y2->Clone();
  GH1D *Proj_back_y2_Clone2 = (GH1D*)Proj_back_y2->Clone();
  GH1D *Proj_gated          = 0;
  GH1D *Proj_gated2         = 0;
  GH1D *Proj_gated_back     = 0;
  GH1D *Proj_gated_back2    = 0;

  if(bg_high>0 && bg_low>0){
    Proj_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] and Background [%.01f,%.01f]",low,high,bg_low,bg_high));
  }else{
    Proj_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] NO background",low,high));
  }

  if(back_bg_high>0 && back_bg_low>0 && back_low>0 && back_high>0){
    Proj_back_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] and Background [%.01f,%.01f]",back_low,back_high,back_bg_low,back_bg_high));
  }else if(back_low>0 && back_high>0){
    Proj_back_y->SetTitle(Form("Projection with Gate From [%.01f,%.01f] NO background",back_low,back_high));
  }


  double binsize = (Proj_y->GetXaxis()->GetXmax()-Proj_y->GetXaxis()->GetXmin())/Proj_y->GetXaxis()->GetNbins();
  Proj_y->GetXaxis()->SetTitle("Energy [keV]");
  Proj_y->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize));
  Proj_back_y->GetXaxis()->SetTitle("Energy [keV]");
  Proj_back_y->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize));



  double Grace = 300;
  double ZoomHigh = high+Grace;
  double ZoomLow  = low-Grace;
  if(bg_high>0 && bg_high>high)
    ZoomHigh = bg_high+Grace;
  if(bg_low>0 && bg_low<low)
    ZoomLow = bg_low-Grace;

  Proj_y->GetXaxis()->SetRangeUser(ZoomLow,ZoomHigh);
  Proj_y->Draw();
  double Projy_Max = Proj_y->GetMaximum();
  double Projy_Min = Proj_y->GetMinimum();

  TLine *CutLow  = new TLine(low,Projy_Min,low,Projy_Max);
  TLine *CutHigh = new TLine(high,Projy_Min,high,Projy_Max);
  TLine *BGLow   = new TLine(bg_low,Projy_Min,bg_low,Projy_Max);
  TLine *BGHigh  = new TLine(bg_high,Projy_Min,bg_high,Projy_Max);
  CutLow->SetLineColor(kRed);
  CutHigh->SetLineColor(kRed);
  CutLow->SetLineWidth(2);
  CutHigh->SetLineWidth(2);
  BGLow->SetLineColor(kBlue);
  BGHigh->SetLineColor(kBlue);
  BGLow->SetLineWidth(2);
  BGHigh->SetLineWidth(2);
  BGLow->SetLineStyle(kDashed);
  BGHigh->SetLineStyle(kDashed);
  CutLow->Draw("same");
  CutHigh->Draw("same");
  if(bg_low>0 && bg_high>0){
    BGHigh->Draw("same");
    BGLow->Draw("same");
    Proj_gated = Proj_y2_Clone->Project_Background(low,high,
						  bg_low,bg_high,
						  kRegionBackground);
  }else{
    Proj_gated = Proj_y2_Clone->Project(low,high);
  }
  if(overlay){
    Proj_gated2 = Proj_y2_Clone2->Project(low,high);
    Proj_gated2->SetLineColor(2);
  }

  if(bg_high>0 && bg_low>0){
    Proj_gated->SetTitle(Form("Gate From [%.01f,%.01f] with Background [%.01f,%.01f]",low,high,bg_low,bg_high));
    if(overlay)
      Proj_gated2->SetTitle(Form("Gate From [%.01f,%.01f] with Background [%.01f,%.01f]",low,high,bg_low,bg_high));
  }else{
    Proj_gated->SetTitle(Form("Gate From [%.01f,%.01f] NO Background",low,high));
    if(overlay)
      Proj_gated2->SetTitle(Form("Gate From [%.01f,%.01f] NO Background",low,high));
  }
  double binsize_gated = (Proj_gated->GetXaxis()->GetXmax()-Proj_gated->GetXaxis()->GetXmin())/Proj_gated->GetXaxis()->GetNbins();
  Proj_gated->GetXaxis()->SetTitle("Energy [keV]");
  Proj_gated->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize_gated));


  C_projections->cd(2);
  if(overlay){
    Proj_gated2->GetXaxis()->SetTitle("Energy [keV]");
    Proj_gated2->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize_gated));

    Proj_gated2->Draw();
    Proj_gated->Draw("same");
  } else
    Proj_gated->Draw();
  if(back_low<0 && back_high<0)
    return 1;


  //---------------------------------------------------------------------------

  C_projections->cd(3);
  gPad->Clear();
  C_projections->cd(4);
  gPad->Clear();

  if(back_low>0 && back_high>0){
    C_projections->cd(3);
    double back_ZoomHigh = back_high+Grace;
    double back_ZoomLow  = back_low-Grace;
    if(back_bg_high>0 && back_bg_high>high)
      back_ZoomHigh = back_bg_high+Grace;
    if(back_bg_low>0 && back_bg_low<low)
      back_ZoomLow = back_bg_low-Grace;


    Proj_back_y->GetXaxis()->SetRangeUser(back_ZoomLow,back_ZoomHigh);
    Proj_back_y->Draw();
    double Proj_back_y_Max = Proj_back_y->GetMaximum();
    double Proj_back_y_Min = Proj_back_y->GetMinimum();

    TLine *back_CutLow  = new TLine(back_low,Proj_back_y_Min,back_low,Proj_back_y_Max);
    TLine *back_CutHigh = new TLine(back_high,Proj_back_y_Min,back_high,Proj_back_y_Max);
    TLine *back_BGLow   = new TLine(back_bg_low,Proj_back_y_Min,back_bg_low,Proj_back_y_Max);
    TLine *back_BGHigh  = new TLine(back_bg_high,Proj_back_y_Min,back_bg_high,Proj_back_y_Max);
    back_CutLow->SetLineColor(kRed);
    back_CutHigh->SetLineColor(kRed);
    back_CutLow->SetLineWidth(2);
    back_CutHigh->SetLineWidth(2);
    back_BGLow->SetLineColor(kBlue);
    back_BGHigh->SetLineColor(kBlue);
    back_BGLow->SetLineWidth(2);
    back_BGHigh->SetLineWidth(2);
    back_BGLow->SetLineStyle(kDashed);
    back_BGHigh->SetLineStyle(kDashed);
    back_CutLow->Draw("same");
    back_CutHigh->Draw("same");
    if(back_bg_low>0 && back_bg_high>0){
      back_BGHigh->Draw("same");
      back_BGLow->Draw("same");
      Proj_gated_back = Proj_back_y2_Clone->Project_Background(back_low,back_high,
							       back_bg_low,back_bg_high,
							       kRegionBackground);
    }else{
      Proj_gated_back = Proj_back_y2_Clone->Project(back_low,back_high);
    }

    if(back_overlay){
      Proj_gated_back2 = Proj_back_y2_Clone2->Project(back_low,back_high);
      Proj_gated_back2->SetLineColor(2);
    }

    if(back_bg_high>0 && back_bg_low>0){
      Proj_gated_back->SetTitle(Form("Gate From [%.01f,%.01f] with Background [%.01f,%.01f]",
				     back_low,back_high,back_bg_low,back_bg_high));
      if(back_overlay){
      Proj_gated_back2->SetTitle(Form("Gate From [%.01f,%.01f] with Background [%.01f,%.01f]",
				     back_low,back_high,back_bg_low,back_bg_high));
      }
    }else{
      Proj_gated_back->SetTitle(Form("Gate From [%.01f,%.01f] NO Background",back_low,back_high));
      if(back_overlay){
	Proj_gated_back2->SetTitle(Form("Gate From [%.01f,%.01f] NO Background",back_low,back_high));
      }

    }
    double binsize_back_gated = (Proj_gated_back->GetXaxis()->GetXmax()-Proj_gated_back->GetXaxis()->GetXmin())/Proj_gated_back->GetXaxis()->GetNbins();
    Proj_gated_back->GetXaxis()->SetTitle("Energy [keV]");
    Proj_gated_back->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize_back_gated));

    C_projections->cd(4);
    if(back_overlay){
      Proj_gated_back2->GetXaxis()->SetTitle("Energy [keV]");
      Proj_gated_back2->GetYaxis()->SetTitle(Form("Counts / %.0f keV",binsize_back_gated));

      Proj_gated_back2->Draw();
      Proj_gated_back->Draw("same");
    } else{
      Proj_gated_back->Draw();
    }
  }





  return 1;
}
*/


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
  //  double *y = pm->GetY();
  for(int i=0;i<n;i++) {
    //y[i] += y[i]*0.15;
    double y = 0;
    for(int i_x = x[i]-3;i_x<x[i]+3;i_x++){
      if((hist->GetBinContent(hist->GetXaxis()->FindBin(i_x)))>y){
	y = hist->GetBinContent(hist->GetXaxis()->FindBin(i_x));
      }
    }
    y+=y*0.1;
    text = new TText(x[i],y,Form("%.1f",x[i]));
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



bool ShowPeaks(TH1 **hists,unsigned int nhists,double sigma,double thresh) {
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

GGaus *GausFit(TH1 *hist,double xlow, double xhigh,Option_t *opt) {
  //bool edit = false;
  if(!hist)
    return 0;
  if(xlow>xhigh)
    std::swap(xlow,xhigh);

  //std::cout << "here." << std::endl;

  GGaus *mypeak= new GGaus(xlow,xhigh);
  std::string options = opt;
  options.append("Q+");
  mypeak->Fit(hist,options.c_str());
  //mypeak->Background()->Draw("SAME");
  TF1 *bg = new TF1(*mypeak->Background());
  hist->GetListOfFunctions()->Add(bg);
  //edit = true;

  return mypeak;
}


GDoubleGaus *DoubleGausFit(TH1 *hist,double cent1,double cent2,double xlow, double xhigh,Option_t *opt) {
  if(!hist)
    return 0;
  if(xlow>xhigh)
    std::swap(xlow,xhigh);

  //std::cout << "here." << std::endl;

  GDoubleGaus *mypeak= new GDoubleGaus(cent1,cent2,xlow,xhigh);
  std::string options = opt;
  options.append("Q+");
  mypeak->Fit(hist,cent1,cent2,options.c_str());
  //mypeak->Background()->Draw("SAME");
  TF1 *bg = new TF1(*mypeak->Background());
  hist->GetListOfFunctions()->Add(bg);
  //edit = true;

  return mypeak;
}









/*



=======



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

  TIter it(hist->GetListOfFunctions());
  while(TObject *obj=it.Next()) {
    if(!hist->InheritsFrom(TF1::Class()))
      continue;
    ((TF1*)obj)->Draw("same");
  }


  return edit;

}
*/


GPeak *PhotoPeakFit(TH1 *hist,double xlow, double xhigh,Option_t *opt) {
  //bool edit = 0;
  if(!hist)
    return 0;
  if(xlow>xhigh)
    std::swap(xlow,xhigh);

  //std::cout << "here." << std::endl;

  GPeak *mypeak= new GPeak((xlow+xhigh)/2.0,xlow,xhigh);
  std::string options = opt;
  options.append("Q+");
  mypeak->Fit(hist,options.c_str());
  //mypeak->Background()->Draw("SAME");
  TF1 *bg = new TF1(*mypeak->Background());
  hist->GetListOfFunctions()->Add(bg);
  //edit = true;

  return mypeak;
}

GPeak *PhotoPeakFitNormBG(TH1 *hist,double xlow, double xhigh,Option_t *opt) {
  //bool edit = 0;
  if(!hist)
    return 0;
  if(xlow>xhigh)
    std::swap(xlow,xhigh);

  //std::cout << "here." << std::endl;

  GPeak *mypeak= new GPeak((xlow+xhigh)/2.0,xlow,xhigh);
  std::string options = opt;
  options.append("Q+");
  mypeak->Fit(hist,options.c_str());
  mypeak->FitExclude(hist,xlow,xhigh);
  //mypeak->Background()->Draw("SAME");
  //TF1 *bg = new TF1(*mypeak->Background());
  //hist->GetListOfFunctions()->Add(bg);
  //edit = true;

  return mypeak;
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


TH1 *GrabHist(int i)  {
  //return the histogram from the current canvas, pad i.
  TH1 *hist = 0;
  if(!gPad)
    return hist;
  TIter iter(gPad->GetListOfPrimitives());
  int j=0;
  while(TObject *obj = iter.Next()) {
    if(obj->InheritsFrom(TH1::Class())) {
      if(j==i) {
        hist = (TH1*)obj;
        break;
      }
      j++;
    }
  }
  return hist;
}

TF1 *GrabFit(int i)  {
  //return the histogram from the current canvas, pad i.
  TH1 *hist = 0;
  TF1 *fit = 0;
  if(!gPad)
    return fit;
  TIter iter(gPad->GetListOfPrimitives());
  int j=0;
  while(TObject *obj = iter.Next()) {
    if(obj->InheritsFrom(TH1::Class())) {
      hist = (TH1*)obj;
      TIter iter2(hist->GetListOfFunctions());
      while(TObject *obj2 = iter2.Next()){
	if(obj2->InheritsFrom(TF1::Class())){
	  if(j==i) {
	    fit=(TF1*)obj2;
	    return fit;
	  }
	  j++;
	}
      }
    }
  }
  return fit;
}


namespace {
  bool gui_is_running = false;
}

void StartGUI() {
  std::string   script_filename = Form("%s/pygui/grut-view.py",getenv("GRUTSYS"));
  std::ifstream script(script_filename);
  std::string   script_text((std::istreambuf_iterator<char>(script)),
                             std::istreambuf_iterator<char>());
  TPython::Exec(script_text.c_str());

  // TTimer* gui_timer = new TTimer("TPython::Exec(\"update()\");", 10, true);
  // gui_timer->TurnOn();
  TTimer* gui_timer = new TTimer();
  DummyGuiCaller* dummy_gui_caller = new DummyGuiCaller;
  gui_timer->Connect("Timeout()", "DummyGuiCaller",
                     dummy_gui_caller, "CallUpdate()");
  gui_timer->Start(10, false);

  gui_is_running = true;
  for(int i=0;i<gROOT->GetListOfFiles()->GetSize();i++) {
    TPython::Bind((TFile*)gROOT->GetListOfFiles()->At(i),"tdir");
    gROOT->ProcessLine("TPython::Exec(\"window.AddDirectory(tdir)\");");
  }
}

bool GUIIsRunning() {
  return gui_is_running;
}

void DummyGuiCaller::CallUpdate() {
  TPython::Exec("update()");
}




/*

TH2 *AddOffset(TH2 *mat,double offset,EAxis axis) {
 TH2 *toreturn = 0;
 if(!mat)
    return toreturn;
 //int dim = mat->GetDimension();
 int xmax = mat->GetXaxis()->GetNbins()+1;
 int ymax = mat->GetYaxis()->GetNbins()+1;
 toreturn = (TH2*)mat->Clone(Form("%s_offset",mat->GetName()));
 toreturn->Reset();

  for(int x=0;x<xmax;x++) {
    for(int y=0;y<ymax;y++) {
      double newx = mat->GetXaxis()->GetBinCenter(x);
      double newy = mat->GetYaxis()->GetBinCenter(y);;
      double bcont = mat->GetBinContent(x,y);
      if(axis&kXAxis)
        newx += offset;
      if(axis&kYAxis)
        newy += offset;
      toreturn->Fill(newx,newy,bcont);
     }
   }
  return toreturn;
}
*/
