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
TCutG *pid_rb_left    = 0;
TCutG *pid_br_left    = 0;
TCutG *pid_rb_right    = 0;
TCutG *pid_br_right    = 0;
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
                        TCutG* &pid_rb, TCutG* &pid_br, TCutG* &in_kr88,
                        TCutG* &pid_rb_left, TCutG* &pid_br_left,
                        TCutG* &pid_rb_right, TCutG* &pid_br_right) {
    cut_file = new TFile("/mnt/analysis/pecan-gade/elman/Kr88/cut_files/kr88_cuts.root","Read");
    pid_kr88 = (TCutG*)cut_file->Get("pid_kr88_large");
    pid_rb = (TCutG*)cut_file->Get("pid_rb_large");
    pid_br = (TCutG*)cut_file->Get("pid_br_large");
    tcut_kr88 = (TCutG*)cut_file->Get("tcut_widest_03_17_2016");
    in_kr88 = (TCutG*)cut_file->Get("in_kr88_large");

    pid_rb_left = (TCutG*)cut_file->Get("pid_rb_left");
    pid_br_left = (TCutG*)cut_file->Get("pid_br_left");
    pid_rb_right = (TCutG*)cut_file->Get("pid_rb_right");
    pid_br_right = (TCutG*)cut_file->Get("pid_br_right");
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
    const int ENERGY_THRESHOLD = 300;
    const int TOTAL_ANGLES = (FINAL_ANGLE-START_ANGLE)/ANGLE_STEPS + 1;
    std::vector<double> angles;
    angles.reserve(TOTAL_ANGLES);
    
    double temp_angle = START_ANGLE;
    for (int i = 0; i < TOTAL_ANGLES; i++){
      angles.push_back(temp_angle);
      temp_angle += ANGLE_STEPS;
    }

    

    std::vector<double> energies_in_tcut_kr88;
    std::vector<double> energies_in_tcut_rb;
    std::vector<double> energies_in_tcut_rb_left;
    std::vector<double> energies_in_tcut_rb_right;

    for(int y=0;y<caesar->Size();y++) {
      if (cut_file == 0){
        initializeKr88Cuts(cut_file,pid_kr88,tcut_kr88,pid_rb,pid_br,in_kr88,
                           pid_rb_left, pid_br_left, pid_rb_right, pid_br_right);//cuts are global
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
        //double energy_dc = caesar->GetEnergyDC(hit);
        double energy_dc = hit.GetDoppler();

        TH2 *caesar_det_energy = GetMatrix(list,"Detector_Energy_Summary",200,0,200,4096,0,4096);
        caesar_det_energy->Fill(det+total_det_in_prev_rings[ring],energy);

        TH2 *caesar_det_energy_dc = GetMatrix(list,"Detector_EnergyDC_Summary",200,0,200,4096,0,4096);
        caesar_det_energy_dc->Fill(det+total_det_in_prev_rings[ring],energy_dc);


        if (s800){
          double corr_time = caesar->GetCorrTime(hit,s800);
          double objtac_corr = s800->GetCorrTOF_OBJTAC();
          double ic_sum = s800->GetIonChamber().GetAve();
          //targ_exit_vec = (pt,theta,phi)
          TVector3 targ_exit_vec = s800->ExitTargetVect();
          double scatter_angle = targ_exit_vec.Theta()*(180.0/TMath::Pi());

          TH1 *scatter_angle_hist = GetHistogram(list, "scatter_angle", 18000,0,180);
          scatter_angle_hist->Fill(fabs(scatter_angle));

          double objtac = s800->GetTof().GetTacOBJ();
          double xfptac = s800->GetTof().GetTacXFP();


          TH2 *caesar_corrtime_energyDC = GetMatrix(list,"EnergyDC_vs_CorrTime",4000,-2000,2000,4096,0,4096);
          caesar_corrtime_energyDC->Fill(corr_time, energy_dc);
          
          TH2 *caesar_coincidence_spec = GetMatrix(list, "ungated_coincidence", 8192,0,8192,8192,0,8192);
          for (int hit_num = y+1; hit_num < caesar->Size(); hit_num++){
            //caesar_coincidence_spec->Fill(energy_dc, caesar->GetEnergyDC(caesar->GetCaesarHit(hit_num)));
            caesar_coincidence_spec->Fill(energy_dc, caesar->GetCaesarHit(hit_num).GetDoppler());
          }
          if (pid_kr88->IsInside(objtac_corr, ic_sum)){
            //make coincidence matrix

            int maxpad1 = s800->GetCrdc(0).GetMaxPad();
            int maxpad1_sum = s800->GetCrdc(0).GetMaxPadSum();
            int maxpad2 = s800->GetCrdc(1).GetMaxPad();
            int maxpad2_sum = s800->GetCrdc(1).GetMaxPadSum();

            TH2 *crdc1_cal_spec = GetMatrix(list,"CRDC1_MAXPAD_SPEC", 300,0,300,4000,0,4000);
            crdc1_cal_spec->Fill(maxpad1,maxpad1_sum);
            TH2 *crdc2_cal_spec = GetMatrix(list,"CRDC2_MAXPAD_SPEC", 300,0,300,4000,0,4000);
            crdc2_cal_spec->Fill(maxpad2,maxpad2_sum);
            TH2 *caesar_corrtime_energyDC_kr88 = GetMatrix(list,"EnergyDC_vs_CorrTime_Kr88",4000,-2000,2000,4096,0,4096);
            caesar_corrtime_energyDC_kr88->Fill(corr_time, energy_dc);

            TH2 *tacxfp_vs_tacobj_gated = GetMatrix(list,"in_beam_gated_kr88",4096,0,4096,4096,0,4096);
            tacxfp_vs_tacobj_gated->Fill(xfptac,objtac);

            TH1 *scatter_angle_kr88_hist = GetHistogram(list, "scatter_angle_kr88", 18000,0,180);
            scatter_angle_kr88_hist->Fill(fabs(scatter_angle));
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              if (in_kr88->IsInside(xfptac,objtac)){
                if (energy_dc > ENERGY_THRESHOLD){
                  energies_in_tcut_kr88.push_back(energy_dc);
                }
                TH1 *caesar_energydc_kr88 = GetHistogram(list,"EnergyDC_Kr88_tcut_incut", 8192,0,8192);
                caesar_energydc_kr88->Fill(energy_dc);
                bool done = false;
                int cur_angle_index = 0;
                while (!done && cur_angle_index < TOTAL_ANGLES){
                  if (fabs(scatter_angle) < angles.at(cur_angle_index)){
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
            if (energy_dc > ENERGY_THRESHOLD){
              energies_in_tcut_rb.push_back(energy_dc);
            }
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
          if (pid_br_left->IsInside(objtac_corr, ic_sum)){
            TH2 *caesar_corrtime_energyDC_br_left = GetMatrix(list,"EnergyDC_vs_CorrTime_Br_left",4000,-2000,2000,4096,0,4096);
            caesar_corrtime_energyDC_br_left->Fill(corr_time, energy_dc);
            TH2 *tacxfp_vs_tacobj_gated_br_left = GetMatrix(list,"in_beam_gated_br_left",4096,0,4096,4096,0,4096);
            tacxfp_vs_tacobj_gated_br_left->Fill(xfptac,objtac);
            TH2 *caesar_det_energy_dc_br_left = GetMatrix(list,"Detector_EnergyDC_Br_left",200,0,200,4096,0,4096);
            caesar_det_energy_dc_br_left->Fill(det+total_det_in_prev_rings[ring],energy_dc);
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              TH1 *caesar_br_left_energycal_time = GetHistogram(list,"EnergyDC_Br_tcut_left", 8192,0,8192);
              caesar_br_left_energycal_time->Fill(energy_dc);
            }
          }//inside pid_br_lrft
          if (pid_br_right->IsInside(objtac_corr, ic_sum)){
            TH2 *caesar_corrtime_energyDC_br_right = GetMatrix(list,"EnergyDC_vs_CorrTime_Br_right",4000,-2000,2000,4096,0,4096);
            caesar_corrtime_energyDC_br_right->Fill(corr_time, energy_dc);
            TH2 *tacxfp_vs_tacobj_gated_br_right = GetMatrix(list,"in_beam_gated_br_right",4096,0,4096,4096,0,4096);
            tacxfp_vs_tacobj_gated_br_right->Fill(xfptac,objtac);
            TH2 *caesar_det_energy_dc_br_right = GetMatrix(list,"Detector_EnergyDC_Br_right",200,0,200,4096,0,4096);
            caesar_det_energy_dc_br_right->Fill(det+total_det_in_prev_rings[ring],energy_dc);
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              TH1 *caesar_br_right_energycal_time = GetHistogram(list,"EnergyDC_Br_tcut_right", 8192,0,8192);
              caesar_br_right_energycal_time->Fill(energy_dc);
            }
          }//inside pid_br_right
          if (pid_rb_right->IsInside(objtac_corr, ic_sum)){
            if (energy_dc > ENERGY_THRESHOLD){
              energies_in_tcut_rb_right.push_back(energy_dc);
            }
            TH2 *caesar_corrtime_energyDC_rb_right = GetMatrix(list,"EnergyDC_vs_CorrTime_rb_right",4000,-2000,2000,4096,0,4096);
            caesar_corrtime_energyDC_rb_right->Fill(corr_time, energy_dc);
            TH2 *tacxfp_vs_tacobj_gated_rb_right = GetMatrix(list,"in_beam_gated_rb_right",4096,0,4096,4096,0,4096);
            tacxfp_vs_tacobj_gated_rb_right->Fill(xfptac,objtac);
            TH2 *caesar_det_energy_dc_rb_right = GetMatrix(list,"Detector_EnergyDC_rb_right",200,0,200,4096,0,4096);
            caesar_det_energy_dc_rb_right->Fill(det+total_det_in_prev_rings[ring],energy_dc);
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              TH1 *caesar_rb_right_energycal_time = GetHistogram(list,"EnergyDC_rb_tcut_right", 8192,0,8192);
              caesar_rb_right_energycal_time->Fill(energy_dc);
            }
          }//inside pid_rb_right
          if (pid_rb_left->IsInside(objtac_corr, ic_sum)){
            if (energy_dc > ENERGY_THRESHOLD){
              energies_in_tcut_rb_left.push_back(energy_dc);
            }
            TH2 *caesar_corrtime_energyDC_rb_left = GetMatrix(list,"EnergyDC_vs_CorrTime_rb_left",4000,-2000,2000,4096,0,4096);
            caesar_corrtime_energyDC_rb_left->Fill(corr_time, energy_dc);
            TH2 *tacxfp_vs_tacobj_gated_rb_left = GetMatrix(list,"in_beam_gated_rb_left",4096,0,4096,4096,0,4096);
            tacxfp_vs_tacobj_gated_rb_left->Fill(xfptac,objtac);
            TH2 *caesar_det_energy_dc_rb_left = GetMatrix(list,"Detector_EnergyDC_rb_left",200,0,200,4096,0,4096);
            caesar_det_energy_dc_rb_left->Fill(det+total_det_in_prev_rings[ring],energy_dc);
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              TH1 *caesar_rb_left_energycal_time = GetHistogram(list,"EnergyDC_rb_tcut_left", 8192,0,8192);
              caesar_rb_left_energycal_time->Fill(energy_dc);
            }
          }//inside pid_rb_left


        }//s800 exists
      }//hit has both energy and time
    }//loop over hits
    if (energies_in_tcut_kr88.size() == 1){
      TH1 *caesar_mult_one_kr88 = GetHistogram(list,"kr88_mult_one", 8192,0,8192);
      caesar_mult_one_kr88->Fill(energies_in_tcut_kr88.at(0));
    }
    else if (energies_in_tcut_kr88.size() == 2){
      TH1 *caesar_mult_two_kr88 = GetHistogram(list,"kr88_mult_two", 8192,0,8192);
      caesar_mult_two_kr88->Fill(energies_in_tcut_kr88.at(0));
      caesar_mult_two_kr88->Fill(energies_in_tcut_kr88.at(1));
    }
    if (energies_in_tcut_rb.size() == 1){
      TH1 *caesar_mult_one_rb = GetHistogram(list,"rb_mult_one", 8192,0,8192);
      caesar_mult_one_rb->Fill(energies_in_tcut_rb.at(0));
    }
    else if (energies_in_tcut_rb.size() == 2){
      TH1 *caesar_mult_two_rb = GetHistogram(list,"rb_mult_two", 8192,0,8192);
      caesar_mult_two_rb->Fill(energies_in_tcut_rb.at(0));
      caesar_mult_two_rb->Fill(energies_in_tcut_rb.at(1));
    }
    if (energies_in_tcut_rb_left.size() == 1){
      TH1 *caesar_mult_one_rb_left = GetHistogram(list,"rb_left_mult_one", 8192,0,8192);
      caesar_mult_one_rb_left->Fill(energies_in_tcut_rb_left.at(0));
    }
    else if (energies_in_tcut_rb_left.size() == 2){
      TH1 *caesar_mult_two_rb_left = GetHistogram(list,"rb_left_mult_two", 8192,0,8192);
      caesar_mult_two_rb_left->Fill(energies_in_tcut_rb_left.at(0));
      caesar_mult_two_rb_left->Fill(energies_in_tcut_rb_left.at(1));
    }
    if (energies_in_tcut_rb_right.size() == 1){
      TH1 *caesar_mult_one_rb_right = GetHistogram(list,"rb_right_mult_one", 8192,0,8192);
      caesar_mult_one_rb_right->Fill(energies_in_tcut_rb_right.at(0));
    }
    else if (energies_in_tcut_rb_right.size() == 2){
      TH1 *caesar_mult_two_rb_right = GetHistogram(list,"rb_right_mult_two", 8192,0,8192);
      caesar_mult_two_rb_right->Fill(energies_in_tcut_rb_right.at(0));
      caesar_mult_two_rb_right->Fill(energies_in_tcut_rb_right.at(1));
    }

    for (unsigned int i = 0; i < energies_in_tcut_kr88.size(); i++){
      for (unsigned int j = i+1; j < energies_in_tcut_kr88.size(); j++){
        TH2 *kr88_coincidence_spec = GetMatrix(list, "kr88_coincidence", 8192,0,8192,8192,0,8192);
        kr88_coincidence_spec->Fill(energies_in_tcut_kr88.at(i), energies_in_tcut_kr88.at(j));
        kr88_coincidence_spec->Fill(energies_in_tcut_kr88.at(j), energies_in_tcut_kr88.at(i));
      }
    }
    for (unsigned int i = 0; i < energies_in_tcut_rb.size(); i++){
      for (unsigned int j = i+1; j < energies_in_tcut_rb.size(); j++){
        TH2 *rb_coincidence_spec = GetMatrix(list, "rb_coincidence", 8192,0,8192,8192,0,8192);
        rb_coincidence_spec->Fill(energies_in_tcut_rb.at(i), energies_in_tcut_rb.at(j));
        rb_coincidence_spec->Fill(energies_in_tcut_rb.at(j), energies_in_tcut_rb.at(i));
      }
    }
    for (unsigned int i = 0; i < energies_in_tcut_rb_left.size(); i++){
      for (unsigned int j = i+1; j < energies_in_tcut_rb_left.size(); j++){
        TH2 *rb_left_coincidence_spec = GetMatrix(list, "rb_left_coincidence", 8192,0,8192,8192,0,8192);
        rb_left_coincidence_spec->Fill(energies_in_tcut_rb_left.at(i), energies_in_tcut_rb_left.at(j));
        rb_left_coincidence_spec->Fill(energies_in_tcut_rb_left.at(j), energies_in_tcut_rb_left.at(i));
      }
    }
    for (unsigned int i = 0; i < energies_in_tcut_rb_right.size(); i++){
      for (unsigned int j = i+1; j < energies_in_tcut_rb_right.size(); j++){
        TH2 *rb_right_coincidence_spec = GetMatrix(list, "rb_right_coincidence", 8192,0,8192,8192,0,8192);
        rb_right_coincidence_spec->Fill(energies_in_tcut_rb_right.at(i), energies_in_tcut_rb_right.at(j));
        rb_right_coincidence_spec->Fill(energies_in_tcut_rb_right.at(j), energies_in_tcut_rb_right.at(i));
      }
    }
  }//caesar exists

  if(s800) {
    double ic_sum = s800->GetIonChamber().GetAve();
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
