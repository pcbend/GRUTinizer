#include "TRuntimeObjects.h"

#include <iostream>
#include <map>

#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include "TRandom.h"

#include "TObject.h"
#include "TS800.h"

#include "TChannel.h"
#include "GValue.h"
#include "TCutG.h"

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
  TS800    *s800    = obj.GetDetector<TS800>();

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();
  
  if(s800) {
  
    double ic_sum = s800->GetIonChamber().GetSum();
    double objtac_corr = s800->GetCorrTOF_OBJTAC();
    double objtac = s800->GetTof().GetTacOBJ();
    double crdc_1_x = s800->GetCrdc(0).GetDispersiveX();
    double crdc_2_x = s800->GetCrdc(1).GetDispersiveX();
    double crdc_1_y = s800->GetCrdc(0).GetNonDispersiveY();
    double crdc_2_y = s800->GetCrdc(1).GetNonDispersiveY();
    double xfptac = s800->GetTof().GetTacXFP();
    double xfp = s800->GetTof().GetXFP();
    double obj = s800->GetTof().GetOBJ();
    double ata = s800->GetAta();
    double bta = s800->GetBta();
    double dta = s800->GetDta();
    double yta = s800->GetYta();
    double crdc_1_anode = s800->GetCrdc(0).GetAnode();
    double crdc_2_anode = s800->GetCrdc(1).GetAnode();
    double crdc_1_tac = s800->GetCrdc(0).GetTime();
    double crdc_2_tac = s800->GetCrdc(1).GetTime();
    double crdc_1_tac_rand = s800->GetCrdc(0).GetTimeRand();
    double crdc_2_tac_rand = s800->GetCrdc(1).GetTimeRand();
    int crdc1_size = s800->GetCrdc(0).Size();
    int crdc2_size = s800->GetCrdc(1).Size();
    
    for (int i =0; i < crdc1_size; i++){
      int crdc_1_data = s800->GetCrdc(0).GetData(i);
      TH1 *crdc1data = GetHistogram(list,"crdc1_data",4096,0,4096);
      crdc1data->Fill(crdc_1_data);
    }
    for (int i =0; i < crdc2_size; i++){
      int crdc_2_data = s800->GetCrdc(1).GetData(i);
      TH1 *crdc2data = GetHistogram(list,"crdc2_data",4096,0,4096);
      crdc2data->Fill(crdc_2_data);
    }

    TH1 *crdc1anode = GetHistogram(list,"crdc1_anode",4096,0,4096);
    crdc1anode->Fill(crdc_1_anode);
    TH1 *crdc2anode = GetHistogram(list,"crdc2_anode",4096,0,4096);
    crdc2anode->Fill(crdc_2_anode);
    TH1 *crdc1tac = GetHistogram(list,"crdc1_tac",4096,0,4096);
    crdc1tac->Fill(crdc_1_tac);
    TH1 *crdc2tac = GetHistogram(list,"crdc2_tac",4096,0,4096);
    crdc2tac->Fill(crdc_2_tac);
    TH1 *crdc1tac_rand = GetHistogram(list,"crdc1_tac_rand",4096,0,4096);
    crdc1tac_rand->Fill(crdc_1_tac_rand);
    TH1 *crdc2tac_rand = GetHistogram(list,"crdc2_tac_rand",4096,0,4096);
    crdc2tac_rand->Fill(crdc_2_tac_rand);

    TH2 *tac_vs_ic= GetMatrix(list,"PID_TAC",4096,0,4096,4096,0,4096);
    tac_vs_ic->Fill(objtac_corr, ic_sum);

    TH1 *ion_sum = GetHistogram(list,"ic_sum",4096,0,4096);
    ion_sum->Fill(ic_sum);


    TH1 *tacobj = GetHistogram(list,"grut_tacobj",4096,0,4096);
    tacobj->Fill(objtac);
    TH1 *tacxfp = GetHistogram(list,"grut_tacxfp",4096,0,4096);
    tacxfp->Fill(xfptac);

    TH1 *obj_hist = GetHistogram(list,"grut_obj",6000,-3000,3000);
    obj_hist->Fill(obj);
    TH1 *xfp_hist = GetHistogram(list,"grut_xfp",6000,-3000,3000);
    xfp_hist->Fill(xfp);
    TH1 *crdc1x = GetHistogram(list,"grut_crdc1_x",600,-300,300);
    crdc1x->Fill(crdc_1_x);
    TH1 *crdc2x = GetHistogram(list,"grut_crdc2_x",600,-300,300);
    crdc2x->Fill(crdc_2_x);
    TH1 *crdc1y = GetHistogram(list,"grut_crdc1_y",600,-300,300);
    crdc1y->Fill(crdc_1_y);
    TH1 *crdc2y = GetHistogram(list,"grut_crdc2_y",600,-300,300);
    crdc2y->Fill(crdc_2_y);

    TH1 *tacobj_corr = GetHistogram(list,"grut_tacobj_corr",4096,0,4096);
    tacobj_corr->Fill(objtac_corr);




    if (crdc1_size && crdc2_size){
      TH1 *ata_hist = GetHistogram(list, "grut_ata", 9999,-0.1,0.1);
      ata_hist->Fill(ata);

      TH1 *bta_hist = GetHistogram(list, "grut_bta", 9999,-0.1,0.1);
      bta_hist->Fill(bta);

      TH1 *dta_hist = GetHistogram(list, "grut_dta", 9999,-0.06,0.06);
      dta_hist->Fill(dta);

      TH1 *yta_hist = GetHistogram(list, "grut_yta", 9999,-0.1,0.1);
      yta_hist->Fill(yta);
   }

    TH1 *trig_bit = GetHistogram(list, "grut_TrigBit", 10,0,10);
    int freg = s800->GetTrigger().GetRegistr();
    if (freg != 1 && freg != 2 && freg != 3){
      trig_bit->Fill(9); //Just a random channel to fill for nonsense input
    }
    else{
      if (freg&1){
        trig_bit->Fill(0);
      }
      if (freg&2){
        trig_bit->Fill(1);
      }
    }
  }//s800 exists

  if(numobj!=list->GetSize())
    list->Sort();
}
