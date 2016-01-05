

#include "TRuntimeObjects.h"



#include <iostream>
#include <map>



#include <cstdio>



#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include "TRandom.h"



#include "TObject.h"
#include "TCaesar.h"
#include "TS800.h"

#define BETA .37

TH1 *GetHistogram(TList *list, std::string histname,int xbins,double xlow,double xhigh) {
  //TList *list = &(obj.GetObjects());
  TH1   *hist = (TH1*)list->FindObject(histname.c_str());
  if(!hist) {
    hist= new TH1I(histname.c_str(),histname.c_str(),xbins,xlow,xhigh);
    list->Add(hist);
  }
  return hist;
}

TH2 *GetMatrix(TList *list, std::string histname,int xbins, double xlow,double xhigh,
                                                 int ybins, double ylow,double yhigh) {
  //TList *list = &(obj.GetObjects());
  TH2   *mat  = (TH2*)list->FindObject(histname.c_str());
  if(!mat) {
    mat = new TH2I(histname.c_str(),histname.c_str(),xbins,xlow,xhigh,
                                                     ybins,ylow,yhigh);
    list->Add(mat);
  }
  return mat;
}

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TCaesar  *caesar  = obj.GetDetector<TCaesar>();
  TS800    *s800    = obj.GetDetector<TS800>();

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  //if(cut) cut->Print();
  double MAFP_COEF = obj.GetVariable("MAFP_COEF");
  double MCRDCX_COEF = obj.GetVariable("MCRDCX_COEF");
  double AFP_COEF = obj.GetVariable("AFP_COEF");
  double CRDCX_COEF = obj.GetVariable("CRDCX_COEF");
  //double BETA = obj.GetVariable(BETA);
  double E1_TDC_low = obj.GetVariable("E1_TDC_low");
  double E1_TDC_high = obj.GetVariable("E1_TDC_high");

  if(caesar) {
    for(int y=0;y<caesar->Size();y++) {
      TCaesarHit hit = caesar->GetCaesarHit(y);
      std::string histname;

      TH2 *caesar_det_charge = GetMatrix(list,"DetectorCharge",200,0,200,5000,0,5000);
      caesar_det_charge->Fill(hit.GetFullChannel(),hit.GetCharge());
     }
  }

  if(s800) {

    TH2 *mtdc_vs_dispx = GetMatrix(list,"MTDC_vs_DispX",2000,-5000,5000,600,-300,300);
    mtdc_vs_dispx->Fill(s800->GetTofE1_MTDC(MAFP_COEF,MCRDCX_COEF),s800->GetCrdc(0).GetDispersiveX());
    TH2 *tdc_vs_dispx = GetMatrix(list,"TDC_vs_DispX",2000,-2000,00,600,-300,300);
    tdc_vs_dispx->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetCrdc(0).GetDispersiveX());


    TH2 *mtdc_vs_afp= GetMatrix(list,"MTDC_vs_AFP",1000,-5000,5000,600,-0.1,0.1);
    mtdc_vs_afp->Fill(s800->GetTofE1_MTDC(MAFP_COEF,MCRDCX_COEF),s800->GetAFP());
    TH2 *tdc_vs_afp= GetMatrix(list,"TDC_vs_AFP",2000,-2000,00,600,-0.1,0.1);
    tdc_vs_afp->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetAFP());






    double delta_t = s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ();

    TH2 *mtdc_vs_ic= GetMatrix(list,"PID_MTDC",2000,-2000,0,2000,20000,40000);
    mtdc_vs_ic->Fill(s800->GetTofE1_MTDC(MAFP_COEF,MCRDCX_COEF),s800->GetIonChamber().Charge());
    TH2 *tdc_vs_ic= GetMatrix(list,"PID_TDC",2000,-2000,0,2000,10000,40000);
    tdc_vs_ic->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetIonChamber().Charge());

    if(delta_t>E1_TDC_low && delta_t<E1_TDC_high){
      TH2 *tdc_vs_ic_Prompt= GetMatrix(list,"PID_TDC_Prompt",4000,-8000,8000,2000,20000,40000);
      tdc_vs_ic_Prompt->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetIonChamber().Charge());
    }

    TH1 *E1_m_TDC = GetHistogram(list,"E1_m_TDC",8000,-8000,8000);
    E1_m_TDC->Fill(s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ());

    TH1 *hist1d = GetHistogram(list,"E1Raw",2000,0,8000);
    hist1d->Fill(s800->GetScint().GetTimeUp());

    TH1 *crdc1x = GetHistogram(list,"CRDC1_X",800,-400,400);
    crdc1x->Fill(s800->GetCrdc(0).GetDispersiveX());

    TH1 *crdc2x = GetHistogram(list,"CRDC2_X",800,-400,400);
    crdc2x->Fill(s800->GetCrdc(1).GetDispersiveX());

    TH1 *ion_sum = GetHistogram(list,"Ion Chamber Sum",8000,0,64000);
    ion_sum->Fill(s800->GetIonChamber().Charge());
  }
























  if(numobj!=list->GetSize())
    list->Sort();


}
