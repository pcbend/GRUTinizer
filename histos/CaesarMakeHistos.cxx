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

        TH2 *caesar_det_charge = GetMatrix(list,"DetectorCharge",200,0,200,2500,0,2500);
        caesar_det_charge->Fill(hit.GetDetectorNumber()+total_det_in_prev_rings[hit.GetRingNumber()],
                                hit.GetCharge());

        TH2 *caesar_det_energy = GetMatrix(list,"DetectorEnergy",200,0,200,4096,0,4096);
        caesar_det_energy->Fill(hit.GetDetectorNumber()+total_det_in_prev_rings[hit.GetRingNumber()],
                                hit.GetEnergy());

        TH2 *caesar_det_energy_dc = GetMatrix(list,"DetectorEnergyDC",200,0,200,4096,0,4096);
        caesar_det_energy_dc->Fill(hit.GetDetectorNumber()+total_det_in_prev_rings[hit.GetRingNumber()],
                                caesar->GetEnergyDC(hit));

        TH2 *caesar_det_time = GetMatrix(list,"DetectorTime",200,0,200,4000,0,4000);
        caesar_det_time->Fill(hit.GetDetectorNumber()+total_det_in_prev_rings[hit.GetRingNumber()],
                                hit.GetTime());
        TH2 *caesar_time_energy = GetMatrix(list,"EnergyRawTime",4000,-2000,2000,4000,0,4000);
        caesar_time_energy->Fill(hit.GetTime(), hit.GetEnergy());

        TH2 *caesar_time_energyDC = GetMatrix(list,"EnergyDCRawTime",4000,-2000,2000,4000,0,4000);
        caesar_time_energyDC->Fill(hit.GetTime(), caesar->GetEnergyDC(hit));

        if (s800){
          TH2 *caesar_corrtime_energyDC = GetMatrix(list,"EnergyDCCorrTime",4000,-2000,2000,4000,0,4000);
          caesar_corrtime_energyDC->Fill(caesar->GetCorrTime(hit, s800), caesar->GetEnergyDC(hit));
        }
      }
    }
  }

if(s800) {
  
  if (s800->GetIonChamber().Size()){
    TH2 *tac_vs_ic= GetMatrix(list,"PID_TAC",4000,0,4000,4096,0,4096);
    tac_vs_ic->Fill(s800->GetCorrTOF_OBJTAC(),s800->GetIonChamber().GetSum());
    TH1 *ion_sum = GetHistogram(list,"Ion Chamber Sum",8000,0,64000);
    ion_sum->Fill(s800->GetIonChamber().GetSum());
  }

  TH2 *tac_vs_afp= GetMatrix(list,"tac_vs_AFP",4000,0,4000,600,-0.1,0.1);
  tac_vs_afp->Fill(s800->GetTof().GetTacOBJ(),s800->GetAFP());

  TH2 *tac_vs_xfp= GetMatrix(list,"tac_vs_xfp",4000,0,4000,600,-300,300);
  tac_vs_xfp->Fill(s800->GetTof().GetTacOBJ(),s800->GetCrdc(0).GetDispersiveX());

  TH1 *tacobj = GetHistogram(list,"tacobj",4000,0,4000);
  tacobj->Fill(s800->GetTof().GetTacOBJ());
  TH1 *tacxfp = GetHistogram(list,"tacxfp",4000,0,4000);
  tacxfp->Fill(s800->GetTof().GetTacXFP());

  TH1 *obj = GetHistogram(list,"obj",6000,-3000,3000);
  obj->Fill(s800->GetTof().GetOBJ());
  TH1 *xfp = GetHistogram(list,"xfp",6000,-3000,3000);
  xfp->Fill(s800->GetTof().GetXFP());

  TH2 *tac_corr_vs_afp= GetMatrix(list,"tac_corr_vs_AFP",4000,0,4000,600,-0.1,0.1);
  tac_corr_vs_afp->Fill(s800->GetCorrTOF_OBJTAC(),s800->GetAFP());
  TH1 *tacobj_corr = GetHistogram(list,"tacobj_corr",4000,0,4000);
  tacobj_corr->Fill(s800->GetCorrTOF_OBJTAC());

  if (s800->GetCrdc(0).Size()){
    TH2 *tac_corr_vs_xfp= GetMatrix(list,"tac_corr_vs_xFP",4000,0,4000,600,-300,300);
    tac_corr_vs_xfp->Fill(s800->GetCorrTOF_OBJTAC(),s800->GetCrdc(0).GetDispersiveX());
    TH1 *crdc1x = GetHistogram(list,"CRDC1_X",600,-300,300);
    crdc1x->Fill(s800->GetCrdc(0).GetDispersiveX());
  }


  if (s800->GetCrdc(1).Size()){
    TH1 *crdc2x = GetHistogram(list,"CRDC2_X",600,-300,300);
    crdc2x->Fill(s800->GetCrdc(1).GetDispersiveX());
  }


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
