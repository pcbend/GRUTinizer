#include "TRuntimeObjects.h"

#include <iostream>
#include <map>

#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <sstream>
#include "TRandom.h"

#include "TObject.h"
#include "TCaesar.h"
#include "TS800.h"

#include "TChannel.h"
#include "GValue.h"
#include "TFile.h"
#include "TCutG.h"

TFile *cut_file  = 0;
TCutG *pid_kr88  = 0;
TCutG *tcut_kr88 = 0;
TCutG *pid_rb    = 0;
TCutG *pid_br    = 0;
TCutG *in_kr88    = 0;
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

void initializeKr88Cuts(TFile * &cut_file, TCutG* &pid_kr88, TCutG* &tcut_kr88, 
                        TCutG* &pid_rb, TCutG* &pid_br, TCutG* &in_kr88) {
//  pid_kr88 = new TCutG("pid_kr88",8);
//  pid_kr88->SetVarX("GetCorrTOF_OBJTAC()");
//  pid_kr88->SetVarY("GetIonChamber()->GetSum()");
//  pid_kr88->SetTitle("pid_kr88");
//  pid_kr88->SetFillColor(1);
//  pid_kr88->SetPoint(0,1774.82,1816.05);
//  pid_kr88->SetPoint(1,1788,1816.05);
//  pid_kr88->SetPoint(2,1796.15,1599.7);
//  pid_kr88->SetPoint(3,1786.11,1510.61);
//  pid_kr88->SetPoint(4,1775.45,1512.43);
//  pid_kr88->SetPoint(5,1766.04,1614.24);
//  pid_kr88->SetPoint(6,1772.94,1805.14);
//  pid_kr88->SetPoint(7,1774.82,1816.05);
//  tcut_kr88 = new TCutG("tcut_kr88",8);
//  tcut_kr88->SetVarX("EnergyDCCorrTimeKr88");
//  tcut_kr88->SetVarY("");
//  tcut_kr88->SetTitle("Graph");
//  tcut_kr88->SetFillColor(1);
//  tcut_kr88->SetLineWidth(3);
//  tcut_kr88->SetPoint(0,-1566.74,276.185);
//  tcut_kr88->SetPoint(1,-1431.18,289.001);
//  tcut_kr88->SetPoint(2,-1467.67,532.506);
//  tcut_kr88->SetPoint(3,-1467.67,4108.18);
//  tcut_kr88->SetPoint(4,-1561.52,4108.18);
//  tcut_kr88->SetPoint(5,-1558.92,417.161);
//  tcut_kr88->SetPoint(6,-1574.56,314.633);
//  tcut_kr88->SetPoint(7,-1566.74,276.185);
//  pid_rb = new TCutG("pid_rb",10);
//  pid_rb->SetVarX("PID_TAC");
//  pid_rb->SetVarY("");
//  pid_rb->SetTitle("Graph");
//  pid_rb->SetFillColor(1);
//  pid_rb->SetLineWidth(3);
//  pid_rb->SetPoint(0,1801.13,1828.56);
//  pid_rb->SetPoint(1,1805.33,1830.83);
//  pid_rb->SetPoint(2,1811.24,1767.31);
//  pid_rb->SetPoint(3,1811.43,1649.34);
//  pid_rb->SetPoint(4,1806.28,1592.63);
//  pid_rb->SetPoint(5,1795.41,1585.82);
//  pid_rb->SetPoint(6,1790.83,1692.44);
//  pid_rb->SetPoint(7,1790.45,1755.96);
//  pid_rb->SetPoint(8,1794.46,1848.97);
//  pid_rb->SetPoint(9,1801.13,1828.56);
//  pid_br = new TCutG("pid_br",11);
//  pid_br->SetVarX("PID_TAC");
//  pid_br->SetVarY("");
//  pid_br->SetTitle("Graph");
//  pid_br->SetFillColor(1);
//  pid_br->SetLineWidth(3);
//  pid_br->SetPoint(0,1760.13,1649.34);
//  pid_br->SetPoint(1,1756.13,1638);
//  pid_br->SetPoint(2,1751.93,1558.6);
//  pid_br->SetPoint(3,1752.89,1510.96);
//  pid_br->SetPoint(4,1758.61,1447.44);
//  pid_br->SetPoint(5,1768.71,1436.09);
//  pid_br->SetPoint(6,1774.44,1508.69);
//  pid_br->SetPoint(7,1766.81,1631.19);
//  pid_br->SetPoint(8,1761.09,1662.95);
//  pid_br->SetPoint(9,1758.8,1662.95);
//  pid_br->SetPoint(10,1760.13,1649.34);
//  tcut_kr88 = new TCutG("tcut_03_08_2016",8);
//  tcut_kr88->SetVarX("EnergyDC_vs_CorrTime_Kr88");
//  tcut_kr88->SetVarY("");
//  tcut_kr88->SetTitle("Graph");
//  tcut_kr88->SetFillColor(1);
//  tcut_kr88->SetLineWidth(3);
//  tcut_kr88->SetPoint(0,-1566.74,4080.78);
//  tcut_kr88->SetPoint(1,-1485.92,4080.78);
//  tcut_kr88->SetPoint(2,-1467.67,645.707);
//  tcut_kr88->SetPoint(3,-1376.43,200.017);
//  tcut_kr88->SetPoint(4,-1608.45,200.017);
//  tcut_kr88->SetPoint(5,-1577.16,504.391);
//  tcut_kr88->SetPoint(6,-1577.16,4053.61);
//  tcut_kr88->SetPoint(7,-1566.74,4080.78);
//  tcut_kr88->Draw("");
    cut_file = new TFile("/mnt/analysis/pecan-gade/elman/Kr88/kr88_cuts.root","Read");
    pid_kr88 = (TCutG*)cut_file->Get("pid_kr88_large");
    pid_rb = (TCutG*)cut_file->Get("pid_rb_large");
    pid_br = (TCutG*)cut_file->Get("pid_br_large");
    tcut_kr88 = (TCutG*)cut_file->Get("tcut_03_08_2016");
    in_kr88 = (TCutG*)cut_file->Get("in_kr88_large");
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
//TFile *cut_file  = 0;
//TCutG *pid_kr88  = 0;
//TCutG *tcut_kr88 = 0;
//TCutG *pid_rb    = 0;
//TCutG *pid_br    = 0;

  if(caesar) {
    const double START_ANGLE = 3.2;
    const double FINAL_ANGLE = 3.2;
    const double ANGLE_STEPS = 0.1;
    const int TOTAL_ANGLES = (FINAL_ANGLE-START_ANGLE)/ANGLE_STEPS + 1;
    std::vector<double> angles;
    angles.reserve(TOTAL_ANGLES);
    
    double temp_angle = START_ANGLE;
    for (int i = 0; i < TOTAL_ANGLES; i++){
      angles.push_back(temp_angle);
      temp_angle += ANGLE_STEPS;
    }

    

    for(int y=0;y<caesar->Size();y++) {
      if (cut_file == 0){
        initializeKr88Cuts(cut_file,pid_kr88,tcut_kr88,pid_rb,pid_br,in_kr88);//cuts are global
      }
      TCaesarHit hit = caesar->GetCaesarHit(y);
      int det = hit.GetDetectorNumber();
      int ring = hit.GetRingNumber();

      double time = hit.Time();
      TH1 *caesar_raw_time = GetHistogram(list,"CAESAR_raw_time", 4096,0,4096);
      caesar_raw_time->Fill(time);
      if (hit.IsValid()){//only accept hits with both times and energies
        std::string histname;

        double energy = hit.GetEnergy();
        double energy_dc = caesar->GetEnergyDC(hit);

        TH2 *caesar_det_energy = GetMatrix(list,"Detector_Energy_Summary",200,0,200,4096,0,4096);
        caesar_det_energy->Fill(det+total_det_in_prev_rings[ring],energy);

        TH2 *caesar_det_energy_dc = GetMatrix(list,"Detector_EnergyDC_Summary",200,0,200,4096,0,4096);
        caesar_det_energy_dc->Fill(det+total_det_in_prev_rings[ring],energy_dc);

        if (s800){
          double corr_time = caesar->GetCorrTime(hit,s800);
          double objtac_corr = s800->GetCorrTOF_OBJTAC();
          double ic_sum = s800->GetIonChamber().GetSum();
          //targ_exit_vec = (pt,theta,phi)
          TVector3 targ_exit_vec = s800->ExitTargetVect();
          double scatter_angle = targ_exit_vec.Y();

          double objtac = s800->GetTof().GetTacOBJ();
          double xfptac = s800->GetTof().GetTacXFP();

          TH2 *caesar_corrtime_energyDC = GetMatrix(list,"EnergyDC_vs_CorrTime",4000,-2000,2000,4096,0,4096);
          caesar_corrtime_energyDC->Fill(corr_time, energy_dc);
          
          if (pid_kr88->IsInside(objtac_corr, ic_sum)){
            TH2 *caesar_corrtime_energyDC_kr88 = GetMatrix(list,"EnergyDC_vs_CorrTime_Kr88",4000,-2000,2000,4096,0,4096);
            caesar_corrtime_energyDC_kr88->Fill(corr_time, energy_dc);

            TH2 *tacxfp_vs_tacobj_gated = GetMatrix(list,"in_beam_gated_kr88",4096,0,4096,4096,0,4096);
            tacxfp_vs_tacobj_gated->Fill(xfptac,objtac);
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              if (in_kr88->IsInside(xfptac,objtac)){
                TH1 *caesar_energydc_kr88 = GetHistogram(list,"EnergyDC_Kr88_tcut_incut", 8192,0,8192);
                caesar_energydc_kr88->Fill(energy_dc);
                bool done = false;
                int cur_angle_index = 0;
                while (!done && cur_angle_index < TOTAL_ANGLES){
                  if (scatter_angle < angles.at(cur_angle_index)){
                    for (int angle_index = cur_angle_index; angle_index < TOTAL_ANGLES; angle_index++){
                      std::stringstream ss;
                      ss << "fit_angle_" << angles.at(cur_angle_index);
                      std::string hist_name = ss.str();
                      TH1 *angle_hist = GetHistogram(list, hist_name.c_str(), 8192,0,8192); 
                      angle_hist->Fill(energy_dc); 
                    }//loop through angles above cur_angle_index to fill all histos
                    done = true;
                  }//scatter_angle < angles[cur_angle_index]
                  cur_angle_index++;
                }//!done && cur_angle_index < TOTAL_ANGLES
              }//inside in-beam cut
            }//is inside timecut
          }//is inside pid_kr88
          if (pid_rb->IsInside(objtac_corr, ic_sum)){
            TH2 *caesar_corrtime_energyDC_rb = GetMatrix(list,"EnergyDC_vs_CorrTime_Rb",4000,-2000,2000,4096,0,4096);
            caesar_corrtime_energyDC_rb->Fill(corr_time, energy_dc);
            TH2 *tacxfp_vs_tacobj_gated_rb = GetMatrix(list,"in_beam_gated_rb",4096,0,4096,4096,0,4096);
            tacxfp_vs_tacobj_gated_rb->Fill(xfptac,objtac);
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              TH1 *caesar_rb_energycal_time = GetHistogram(list,"EnergyDC_Rb_tcut", 8192,0,8192);
              caesar_rb_energycal_time->Fill(energy_dc);
            }
          }//is in pid_rb
          if (pid_br->IsInside(objtac_corr, ic_sum)){
            TH2 *caesar_corrtime_energyDC_br = GetMatrix(list,"EnergyDC_vs_CorrTime_Br",4000,-2000,2000,4096,0,4096);
            caesar_corrtime_energyDC_br->Fill(corr_time, energy_dc);
            TH2 *tacxfp_vs_tacobj_gated_br = GetMatrix(list,"in_beam_gated_br",4096,0,4096,4096,0,4096);
            tacxfp_vs_tacobj_gated_br->Fill(xfptac,objtac);
            TH2 *caesar_det_energy_dc_br = GetMatrix(list,"Detector_EnergyDC_Br",200,0,200,4096,0,4096);
            caesar_det_energy_dc_br->Fill(det+total_det_in_prev_rings[ring],energy_dc);
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              TH1 *caesar_br_energycal_time = GetHistogram(list,"EnergyDC_Br_tcut", 8192,0,8192);
              caesar_br_energycal_time->Fill(energy_dc);
            }
          }//is in pid_br
        }//s800 exists
      }//hit has both energy and time
    }//loop over hits
  }//caesar exists

  if(s800) {
  
    double ic_sum = s800->GetIonChamber().GetSum();
    double objtac_corr = s800->GetCorrTOF_OBJTAC();
    double objtac = s800->GetTof().GetTacOBJ();
    double xfptac = s800->GetTof().GetTacXFP();
    //double afp = s800->GetAFP();
    double ata = s800->GetAta();
    double bta = s800->GetBta();

    TH1 *ata_hist = GetHistogram(list, "ata", 5000,-0.5,0.5);
    ata_hist->Fill(ata);

    TH1 *bta_hist = GetHistogram(list, "bta", 5000,-0.5,0.5);
    bta_hist->Fill(bta);
    
    TH2 *tac_vs_ic= GetMatrix(list,"PID_TAC",4096,0,4096,4096,0,4096);
    tac_vs_ic->Fill(objtac_corr, ic_sum);

    TH2 *tacxfp_vs_tacobj = GetMatrix(list,"tacxfp_vs_tacobj",4096,0,4096,4096,0,4096);
    tacxfp_vs_tacobj->Fill(xfptac,objtac);
    
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
