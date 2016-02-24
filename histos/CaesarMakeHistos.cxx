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
  TCaesar  *caesar  = obj.GetDetector<TCaesar>();
  TS800    *s800    = obj.GetDetector<TS800>();

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();
  
  TCutG *pid_kr88 = new TCutG("pid_kr88",8);
  pid_kr88->SetVarX("GetCorrTOF_OBJTAC()");
  pid_kr88->SetVarY("GetIonChamber()->GetSum()");
  pid_kr88->SetTitle("pid_kr88");
  pid_kr88->SetFillColor(1);
  pid_kr88->SetPoint(0,1774.82,1816.05);
  pid_kr88->SetPoint(1,1788,1816.05);
  pid_kr88->SetPoint(2,1796.15,1599.7);
  pid_kr88->SetPoint(3,1786.11,1510.61);
  pid_kr88->SetPoint(4,1775.45,1512.43);
  pid_kr88->SetPoint(5,1766.04,1614.24);
  pid_kr88->SetPoint(6,1772.94,1805.14);
  pid_kr88->SetPoint(7,1774.82,1816.05);

  TCutG *tcut_kr88 = new TCutG("tcut_kr88",8);
  tcut_kr88->SetVarX("EnergyDCCorrTimeKr88");
  tcut_kr88->SetVarY("");
  tcut_kr88->SetTitle("Graph");
  tcut_kr88->SetFillColor(1);
  tcut_kr88->SetLineWidth(3);
  tcut_kr88->SetPoint(0,-1566.74,276.185);
  tcut_kr88->SetPoint(1,-1431.18,289.001);
  tcut_kr88->SetPoint(2,-1467.67,532.506);
  tcut_kr88->SetPoint(3,-1467.67,4108.18);
  tcut_kr88->SetPoint(4,-1561.52,4108.18);
  tcut_kr88->SetPoint(5,-1558.92,417.161);
  tcut_kr88->SetPoint(6,-1574.56,314.633);
  tcut_kr88->SetPoint(7,-1566.74,276.185);
  TCutG *pid_rb = new TCutG("pid_rb",10);
  pid_rb->SetVarX("PID_TAC");
  pid_rb->SetVarY("");
  pid_rb->SetTitle("Graph");
  pid_rb->SetFillColor(1);
  pid_rb->SetLineWidth(3);
  pid_rb->SetPoint(0,1801.13,1828.56);
  pid_rb->SetPoint(1,1805.33,1830.83);
  pid_rb->SetPoint(2,1811.24,1767.31);
  pid_rb->SetPoint(3,1811.43,1649.34);
  pid_rb->SetPoint(4,1806.28,1592.63);
  pid_rb->SetPoint(5,1795.41,1585.82);
  pid_rb->SetPoint(6,1790.83,1692.44);
  pid_rb->SetPoint(7,1790.45,1755.96);
  pid_rb->SetPoint(8,1794.46,1848.97);
  pid_rb->SetPoint(9,1801.13,1828.56);

   TCutG *pid_br = new TCutG("pid_br",11);
   pid_br->SetVarX("PID_TAC");
   pid_br->SetVarY("");
   pid_br->SetTitle("Graph");
   pid_br->SetFillColor(1);
   pid_br->SetLineWidth(3);
   pid_br->SetPoint(0,1760.13,1649.34);
   pid_br->SetPoint(1,1756.13,1638);
   pid_br->SetPoint(2,1751.93,1558.6);
   pid_br->SetPoint(3,1752.89,1510.96);
   pid_br->SetPoint(4,1758.61,1447.44);
   pid_br->SetPoint(5,1768.71,1436.09);
   pid_br->SetPoint(6,1774.44,1508.69);
   pid_br->SetPoint(7,1766.81,1631.19);
   pid_br->SetPoint(8,1761.09,1662.95);
   pid_br->SetPoint(9,1758.8,1662.95);
   pid_br->SetPoint(10,1760.13,1649.34);
  const int total_det_in_prev_rings[N_RINGS] = {0,10,24,48,72,96,120,144,168,182};
  if(caesar) {
    for(int y=0;y<caesar->Size();y++) {
      TCaesarHit hit = caesar->GetCaesarHit(y);

      int det = hit.GetDetectorNumber();
      int ring = hit.GetRingNumber();
      double charge = hit.GetCharge();
      TH2 *caesar_det_charge = GetMatrix(list,"DetectorCharge",200,0,200,2500,0,2500);
      caesar_det_charge->Fill(det+total_det_in_prev_rings[ring],charge);

      if (hit.IsValid()){//only accept hits with both times and energies
        std::string histname;

        double energy = hit.GetEnergy();
        double energy_dc = caesar->GetEnergyDC(hit);
        double time = hit.GetTime();

        TH2 *caesar_det_energy = GetMatrix(list,"DetectorEnergy",200,0,200,4096,0,4096);
        caesar_det_energy->Fill(det+total_det_in_prev_rings[ring],energy);

        TH2 *caesar_det_energy_dc = GetMatrix(list,"DetectorEnergyDC",200,0,200,4096,0,4096);
        caesar_det_energy_dc->Fill(det+total_det_in_prev_rings[ring],energy_dc);

        TH2 *caesar_det_time = GetMatrix(list,"DetectorTime",200,0,200,3000,0,3000);
        caesar_det_time->Fill(det+total_det_in_prev_rings[ring],time);

        TH2 *caesar_time_energy = GetMatrix(list,"EnergyNoDCRawTme",3000,0,3000,4096,0,4096);
        caesar_time_energy->Fill(time, energy);

        TH2 *caesar_time_energyDC = GetMatrix(list,"EnergyDCRawTime",3000,0,3000,4096,0,4096);
        caesar_time_energyDC->Fill(time, energy_dc);

        if (s800){
          double corr_time = caesar->GetCorrTime(hit,s800);
          double objtac_corr = s800->GetCorrTOF_OBJTAC();
          double ic_sum = s800->GetIonChamber().GetSum();
          TH2 *caesar_corrtime_energyDC = GetMatrix(list,"EnergyDCCorrTime",4000,-2000,2000,4096,0,4096);
          caesar_corrtime_energyDC->Fill(corr_time, energy_dc);
          
          TH2 *caesar_det_corrtime = GetMatrix(list,"DetectorCorrTime",200,0,200,3000,0,3000);
          caesar_det_corrtime->Fill(det+total_det_in_prev_rings[ring],corr_time);

          if (pid_kr88->IsInside(objtac_corr, ic_sum)){
            TH1 *caesar_raw_energy_kr88 = GetHistogram(list,"RawEnergyKr88", 8192,0,8192);
            caesar_raw_energy_kr88->Fill(charge);
            TH2 *caesar_corrtime_energyDC_kr88 = GetMatrix(list,"EnergyDCCorrTimeKr88",4000,-2000,2000,4096,0,4096);
            caesar_corrtime_energyDC_kr88->Fill(corr_time, energy_dc);
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              TH2 *caesar_det_energy_dc_kr88 = GetMatrix(list,"DetectorEnergyDCKr88",200,0,200,4096,0,4096);
              caesar_det_energy_dc_kr88->Fill(det+total_det_in_prev_rings[ring],energy_dc);
              TH1 *caesar_energydc_kr88 = GetHistogram(list,"DCEnergyKr88", 512,0,8192);
              caesar_energydc_kr88->Fill(energy_dc);
            }
          }
          if (pid_rb->IsInside(objtac_corr, ic_sum)){
            TH1 *caesar_raw_energy_rb = GetHistogram(list,"RawEnergyRb", 8192,0,8192);
            caesar_raw_energy_rb->Fill(charge);
            TH2 *caesar_corrtime_energyDC_rb = GetMatrix(list,"EnergyDCCorrTimeRb",4000,-2000,2000,4096,0,4096);
            caesar_corrtime_energyDC_rb->Fill(corr_time, energy_dc);
            TH2 *caesar_det_energy_dc_rb = GetMatrix(list,"DetectorEnergyDCRb",200,0,200,4096,0,4096);
            caesar_det_energy_dc_rb->Fill(det+total_det_in_prev_rings[ring],energy_dc);
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              TH1 *caesar_rb_energycal_time = GetHistogram(list,"CalEnergyDCRbTime", 8192,0,8192);
              caesar_rb_energycal_time->Fill(energy_dc);
              TH1 *caesar_rb_energycal_time_nodc = GetHistogram(list,"CalEnergyRbTime", 8192,0,8192);
              caesar_rb_energycal_time_nodc->Fill(energy);
              TH1 *caesar_rb_energyraw_time = GetHistogram(list,"RawEnergyRbTime", 8192,0,8192);
              caesar_rb_energyraw_time->Fill(charge);
            }
          }
          if (pid_br->IsInside(objtac_corr, ic_sum)){
            TH1 *caesar_raw_energy_br = GetHistogram(list,"RawEnergyBr", 8192,0,8192);
            caesar_raw_energy_br->Fill(charge);
            TH2 *caesar_corrtime_energyDC_br = GetMatrix(list,"EnergyDCCorrTimeBr",4000,-2000,2000,4096,0,4096);
            caesar_corrtime_energyDC_br->Fill(corr_time, energy_dc);
            TH2 *caesar_det_energy_dc_br = GetMatrix(list,"DetectorEnergyDCBr",200,0,200,4096,0,4096);
            caesar_det_energy_dc_br->Fill(det+total_det_in_prev_rings[ring],energy_dc);
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              TH1 *caesar_br_energycal_time = GetHistogram(list,"CalEnergyDCBrTime", 8192,0,8192);
              caesar_br_energycal_time->Fill(energy_dc);
              TH1 *caesar_br_energycal_time_nodc = GetHistogram(list,"CalEnergyBrTime", 8192,0,8192);
              caesar_br_energycal_time_nodc->Fill(energy);
              TH1 *caesar_br_energyraw_time = GetHistogram(list,"RawEnergyBrTime", 8192,0,8192);
              caesar_br_energyraw_time->Fill(charge);
            }
          }
        }//s800 exists
      }//hit has both energy and time
    }
  }

  if(s800) {
  
    double ic_sum = s800->GetIonChamber().GetSum();
    double objtac_corr = s800->GetCorrTOF_OBJTAC();
    double objtac = s800->GetTof().GetTacOBJ();
    double crdc_1_x = s800->GetCrdc(0).GetDispersiveX();
    double crdc_2_x = s800->GetCrdc(1).GetDispersiveX();
    double crdc_1_y = s800->GetCrdc(0).GetNonDispersiveY();
    double crdc_2_y = s800->GetCrdc(1).GetNonDispersiveY();
    double afp = s800->GetAFP();
    double xfptac = s800->GetTof().GetTacXFP();
    double xfp = s800->GetTof().GetXFP();
    double obj = s800->GetTof().GetOBJ();
    double ata = s800->GetAta_Spec();
    double bta = s800->GetBta_Spec();
    double dta = s800->GetDta_Spec();
    double yta = s800->GetYta_Spec();
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
      TH1 *crdc1data = GetHistogram(list,"CRDC1_data",4096,0,4096);
      crdc1data->Fill(crdc_1_data);
    }
    for (int i =0; i < crdc2_size; i++){
      int crdc_2_data = s800->GetCrdc(1).GetData(i);
      TH1 *crdc2data = GetHistogram(list,"CRDC2_data",4096,0,4096);
      crdc2data->Fill(crdc_2_data);
    }

    TH1 *crdc1anode = GetHistogram(list,"CRDC1_anode",4096,0,4096);
    crdc1anode->Fill(crdc_1_anode);
    TH1 *crdc2anode = GetHistogram(list,"CRDC2_anode",4096,0,4096);
    crdc2anode->Fill(crdc_2_anode);
    TH1 *crdc1tac = GetHistogram(list,"CRDC1_tac",4096,0,4096);
    crdc1tac->Fill(crdc_1_tac);
    TH1 *crdc2tac = GetHistogram(list,"CRDC2_tac",4096,0,4096);
    crdc2tac->Fill(crdc_2_tac);
    TH1 *crdc1tac_rand = GetHistogram(list,"CRDC1_tac_rand",4096,0,4096);
    crdc1tac_rand->Fill(crdc_1_tac_rand);
    TH1 *crdc2tac_rand = GetHistogram(list,"CRDC2_tac_rand",4096,0,4096);
    crdc2tac_rand->Fill(crdc_2_tac_rand);
    //if (s800->GetIonChamber().Size()){
    TH2 *tac_vs_ic= GetMatrix(list,"PID_TAC",4096,0,4096,4096,0,4096);
    tac_vs_ic->Fill(objtac_corr, ic_sum);
    TH1 *ion_sum = GetHistogram(list,"Ion Chamber Sum",4096,0,4096);
    ion_sum->Fill(ic_sum);
    //}

    TH2 *tac_vs_afp= GetMatrix(list,"tac_vs_AFP",4096,0,4096,600,-0.1,0.1);
    tac_vs_afp->Fill(objtac,afp);

    TH2 *tac_vs_xfp= GetMatrix(list,"tac_vs_xfp",4096,0,4096,600,-300,300);
    tac_vs_xfp->Fill(objtac,crdc_1_x);

    TH2 *tacxfp_vs_tacobj = GetMatrix(list,"tacxfp_vs_tacobj",4096,0,4096,4096,0,4096);
    tacxfp_vs_tacobj->Fill(xfptac,objtac);

    TH1 *tacobj = GetHistogram(list,"tacobj",4096,0,4096);
    tacobj->Fill(objtac);
    TH1 *tacxfp = GetHistogram(list,"tacxfp",4096,0,4096);
    tacxfp->Fill(xfptac);

    TH1 *obj_hist = GetHistogram(list,"obj_hist",6000,-3000,3000);
    obj_hist->Fill(obj);
    TH1 *xfp_hist = GetHistogram(list,"xfp_hist",6000,-3000,3000);
    xfp_hist->Fill(xfp);

    TH2 *tac_corr_vs_afp= GetMatrix(list,"tac_corr_vs_AFP",4096,0,4096,600,-0.1,0.1);
    tac_corr_vs_afp->Fill(objtac_corr,afp);
    TH1 *tacobj_corr = GetHistogram(list,"tacobj_corr",4096,0,4096);
    tacobj_corr->Fill(objtac_corr);

    //if (s800->GetCrdc(0).Size()){
    TH2 *tac_corr_vs_xfp= GetMatrix(list,"tac_corr_vs_xFP",4096,0,4096,600,-300,300);
    tac_corr_vs_xfp->Fill(objtac_corr,crdc_1_x);
    TH1 *crdc1x = GetHistogram(list,"CRDC1_X",600,-300,300);
    crdc1x->Fill(crdc_1_x);
    TH1 *crdc2x = GetHistogram(list,"CRDC2_X",600,-300,300);
    crdc2x->Fill(crdc_2_x);
    TH1 *crdc1y = GetHistogram(list,"CRDC1_Y",600,-300,300);
    crdc1y->Fill(crdc_1_y);
    //}

    //if (s800->GetCrdc(1).Size()){
    TH1 *crdc2y = GetHistogram(list,"CRDC2_Y",600,-300,300);
    crdc2y->Fill(crdc_2_y);
    // }

    TH1 *ata_hist = GetHistogram(list, "ata", 10000,-100,100);
    ata_hist->Fill(ata);

    TH1 *bta_hist = GetHistogram(list, "bta", 10000,-100,100);
    bta_hist->Fill(bta);

    TH1 *dta_hist = GetHistogram(list, "dta", 10000,-100,100);
    dta_hist->Fill(dta);

    TH1 *yta_hist = GetHistogram(list, "yta", 10000,-100,100);
    yta_hist->Fill(yta);

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
