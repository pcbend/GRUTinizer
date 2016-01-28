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

#include "TChannel.h"
#include "GValue.h"

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

  const int total_det_in_prev_rings[N_RINGS] = {0,10,24,48,72,96,120,144,168,182};
  if(caesar) {
    for(int y=0;y<caesar->Size();y++) {
      TCaesarHit hit = caesar->GetCaesarHit(y);
      if (hit.IsValid()){//only accept hits with both times and energies
        std::string histname;

        int det = hit.GetDetectorNumber();
        int ring = hit.GetRingNumber();
        double charge = hit.GetCharge();
        double energy = hit.GetEnergy();
        double energy_dc = caesar->GetEnergyDC(hit);
        double time = hit.GetTime();
        TH2 *caesar_det_charge = GetMatrix(list,"DetectorCharge",200,0,200,2500,0,2500);
        caesar_det_charge->Fill(det+total_det_in_prev_rings[ring],charge);

        TH2 *caesar_det_energy = GetMatrix(list,"DetectorEnergy",200,0,200,4096,0,4096);
        caesar_det_energy->Fill(det+total_det_in_prev_rings[ring],energy);

        TH2 *caesar_det_energy_dc = GetMatrix(list,"DetectorEnergyDC",200,0,200,4096,0,4096);
        caesar_det_energy_dc->Fill(det+total_det_in_prev_rings[ring],energy_dc);

        TH2 *caesar_det_time = GetMatrix(list,"DetectorTime",200,0,200,4000,0,4000);
        caesar_det_time->Fill(det+total_det_in_prev_rings[ring],time);
        TH2 *caesar_time_energy = GetMatrix(list,"EnergyNoDC",4000,-2000,2000,4000,0,4000);
        caesar_time_energy->Fill(time, energy);

        TH2 *caesar_time_energyDC = GetMatrix(list,"EnergyDCRawTime",4000,-2000,2000,4000,0,4000);
        caesar_time_energyDC->Fill(time, energy_dc);

        if (s800){
          double corr_time = caesar->GetCorrTime(hit,s800);
          TH2 *caesar_corrtime_energyDC = GetMatrix(list,"EnergyDCCorrTime",4000,-2000,2000,4000,0,4000);
          caesar_corrtime_energyDC->Fill(corr_time, energy_dc);
        }
      }
    }
  }

if(s800) {
  
  double ic_sum = s800->GetIonChamber().GetSum();
  double objtac_corr = s800->GetCorrTOF_OBJTAC();
  double objtac = s800->GetTof().GetTacOBJ();
  double crdc_1_x = s800->GetCrdc(0).GetDispersiveX();
  double crdc_2_x = s800->GetCrdc(1).GetDispersiveX();
  double afp = s800->GetAFP();
  double xfptac = s800->GetTof().GetTacXFP();
  double xfp = s800->GetTof().GetXFP();
  double obj = s800->GetTof().GetOBJ();
  //if (s800->GetIonChamber().Size()){
  TH2 *tac_vs_ic= GetMatrix(list,"PID_TAC",4000,0,4000,4096,0,4096);
  tac_vs_ic->Fill(objtac_corr, ic_sum);
  TH1 *ion_sum = GetHistogram(list,"Ion Chamber Sum",8000,0,64000);
  ion_sum->Fill(ic_sum);
  //}

  TH2 *tac_vs_afp= GetMatrix(list,"tac_vs_AFP",4000,0,4000,600,-0.1,0.1);
  tac_vs_afp->Fill(objtac,afp);

  TH2 *tac_vs_xfp= GetMatrix(list,"tac_vs_xfp",4000,0,4000,600,-300,300);
  tac_vs_xfp->Fill(objtac,crdc_1_x);

  TH1 *tacobj = GetHistogram(list,"tacobj",4000,0,4000);
  tacobj->Fill(objtac);
  TH1 *tacxfp = GetHistogram(list,"tacxfp",4000,0,4000);
  tacxfp->Fill(xfptac);

  TH1 *obj_hist = GetHistogram(list,"obj_hist",6000,-3000,3000);
  obj_hist->Fill(obj);
  TH1 *xfp_hist = GetHistogram(list,"xfp_hist",6000,-3000,3000);
  xfp_hist->Fill(xfp);

  TH2 *tac_corr_vs_afp= GetMatrix(list,"tac_corr_vs_AFP",4000,0,4000,600,-0.1,0.1);
  tac_corr_vs_afp->Fill(objtac_corr,afp);
  TH1 *tacobj_corr = GetHistogram(list,"tacobj_corr",4000,0,4000);
  tacobj_corr->Fill(objtac_corr);

  //if (s800->GetCrdc(0).Size()){
  TH2 *tac_corr_vs_xfp= GetMatrix(list,"tac_corr_vs_xFP",4000,0,4000,600,-300,300);
  tac_corr_vs_xfp->Fill(objtac_corr,crdc_1_x);
  TH1 *crdc1x = GetHistogram(list,"CRDC1_X",600,-300,300);
  crdc1x->Fill(crdc_1_x);
  //}


  //if (s800->GetCrdc(1).Size()){
  TH1 *crdc2x = GetHistogram(list,"CRDC2_X",600,-300,300);
  crdc2x->Fill(crdc_2_x);
 // }


  TH1 *trig_bit = GetHistogram(list, "TrigBit", 10,0,10);
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
}

  if(numobj!=list->GetSize())
    list->Sort();
}
