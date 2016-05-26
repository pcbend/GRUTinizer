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


void initializeKr88Cuts(TFile * &cut_file, TCutG* &pid_kr88, TCutG* &tcut_kr88, 
                        TCutG* &pid_rb, TCutG* &pid_br, TCutG* &in_kr88,
                        TCutG* &pid_rb_left, TCutG* &pid_br_left,
                        TCutG* &pid_rb_right, TCutG* &pid_br_right) {
    cut_file = new TFile("/mnt/analysis/pecan-gade/elman/Kr88/cut_files/kr88_cuts.root","Read");
    pid_kr88 = (TCutG*)cut_file->Get("large2_pid_kr88");
//    pid_kr88 = (TCutG*)cut_file->Get("pid_kr88_large");
    pid_rb = (TCutG*)cut_file->Get("pid_rb_large");
    pid_br = (TCutG*)cut_file->Get("pid_br_large");
//    tcut_kr88 = (TCutG*)cut_file->Get("tcut_widest_03_17_2016");
    tcut_kr88 = (TCutG*)cut_file->Get("full_tcut");
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

  std::string histname;
  std::string dirname;
  const int ENERGY_LOW_X = 0;
  const int ENERGY_HIGH_X = 8192;
  const int N_BINS_X = 8192;
  if(caesar) {
    const double START_ANGLE = 3.2;
    const double FINAL_ANGLE = 3.2;
    const double ANGLE_STEPS = 0.1;
    const int ENERGY_THRESHOLD = 300;
    const int TOTAL_ANGLES = (FINAL_ANGLE-START_ANGLE)/ANGLE_STEPS + 1;

    const int NUM_DETS = 192;
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
      TCaesarHit &hit = caesar->GetCaesarHit(y);
      int det = hit.GetDetectorNumber();
      int ring = hit.GetRingNumber();

      if (hit.IsValid()){//only accept hits with both times and energies
        std::string histname;

        double energy = hit.GetEnergy();
        //double energy_dc = caesar->GetEnergyDC(hit);
        double energy_dc = hit.GetDoppler();



        dirname = "GeneralCaesar";
        histname = "Detector_Energy_Summary";
        obj.FillHistogram(dirname, histname, 
                          NUM_DETS+1, 0, NUM_DETS+1,             det+total_det_in_prev_rings[ring],
                          N_BINS_X, ENERGY_LOW_X, ENERGY_HIGH_X, energy);

        histname = "Detector_DCEnergy_Summary";
        obj.FillHistogram(dirname, histname, 
                          NUM_DETS+1, 0, NUM_DETS+1,             det+total_det_in_prev_rings[ring],
                          N_BINS_X, ENERGY_LOW_X, ENERGY_HIGH_X, energy_dc);


        if (s800){
          double corr_time = caesar->GetCorrTime(hit,s800);
          double objtac_corr = s800->GetCorrTOF_OBJTAC();
          double ic_sum = s800->GetIonChamber().GetAve();
          //targ_exit_vec = (pt,theta,phi)
          TVector3 targ_exit_vec = s800->ExitTargetVect();
          double scatter_angle = targ_exit_vec.Theta()*(180.0/TMath::Pi());

          double crdc_1_y = s800->GetCrdc(0).GetNonDispersiveY();
          double crdc_2_y = s800->GetCrdc(1).GetNonDispersiveY();
          double crdc_1_x = s800->GetCrdc(0).GetDispersiveX();
          double crdc_2_x = s800->GetCrdc(1).GetDispersiveX();
          histname = "ScatterAngle";
          obj.FillHistogram(dirname, histname, 18000,0,180, fabs(scatter_angle));
                                               
          histname = "CRDC1_Y";
          obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_1_y);
          histname = "CRDC2_Y";
          obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_2_y);
          histname = "CRDC2_X";
          obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_2_x);
          histname = "CRDC1_X";
          obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_1_x);


          double objtac = s800->GetTof().GetTacOBJ();
          double xfptac = s800->GetTof().GetTacXFP();

          histname = "EnergyDC_CorrTime_ungated";
          obj.FillHistogram(dirname, histname, 
                            4000,-2000,2000, corr_time, 
                            N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);

          
          histname = "ungated_coincidence";
          for (int hit_num = 0; hit_num < caesar->Size(); hit_num++){
            if (hit_num ==y){
              continue; //don't add self coincidence
            }
            obj.FillHistogram(dirname, histname, 
                            N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc,
                            N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, caesar->GetCaesarHit(hit_num).GetDoppler());
          }

          
          if (pid_kr88->IsInside(objtac_corr, ic_sum)){
            //make coincidence matrix
            dirname = "Kr88";

//          int maxpad1 = s800->GetCrdc(0).GetMaxPad();
//          int maxpad1_sum = s800->GetCrdc(0).GetMaxPadSum();
//          int maxpad2 = s800->GetCrdc(1).GetMaxPad();
//          int maxpad2_sum = s800->GetCrdc(1).GetMaxPadSum();

//          TH2 *crdc1_cal_spec = GetMatrix(list,"CRDC1_MAXPAD_SPEC", 300,0,300,4000,0,4000);
//          crdc1_cal_spec->Fill(maxpad1,maxpad1_sum);
//          TH2 *crdc2_cal_spec = GetMatrix(list,"CRDC2_MAXPAD_SPEC", 300,0,300,4000,0,4000);
//          crdc2_cal_spec->Fill(maxpad2,maxpad2_sum);
            histname = "EnergyDC_CorrTime_Kr88";
            obj.FillHistogram(dirname, histname, 
                            4000,-2000,2000, corr_time, 
                            N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
            histname = "tacxfp_tacobj_Kr88";
            obj.FillHistogram(dirname, histname, 
                            N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, xfptac, 
                            N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, objtac);
            histname = "ScatterAngleKr88";
            obj.FillHistogram(dirname, histname, 18000,0,180, fabs(scatter_angle));

            histname = "CRDC1_Y_Kr88";
            obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_1_y);
            histname = "CRDC2_Y_Kr88";
            obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_2_y);
            histname = "CRDC2_X_Kr88";
            obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_2_x);
            histname = "CRDC1_X_Kr88";
            obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_1_x);


            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              if (in_kr88->IsInside(xfptac,objtac)){
                if (energy_dc > ENERGY_THRESHOLD){
                  energies_in_tcut_kr88.push_back(energy_dc);
                }
                histname = "EnergyDC_Kr88_tcut_incut";
                obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
              }//inside in-beam cut
            }//is inside timecut
          }//is inside pid_kr88

          if (pid_rb->IsInside(objtac_corr, ic_sum)){
            if (energy_dc > ENERGY_THRESHOLD){
              energies_in_tcut_rb.push_back(energy_dc);
            }
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              dirname = "Rb";
              histname = "EnergyDC_Rb_tcut";
              obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
            }
          }//is in pid_rb
          if (pid_br->IsInside(objtac_corr, ic_sum)){
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              dirname = "Br";
              histname = "EnergyDC_Br_tcut";
              obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
            }
          }//is in pid_br
          if (pid_br_left->IsInside(objtac_corr, ic_sum)){
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              dirname = "Br";
              histname = "EnergyDC_Br_left_tcut";
              obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
            }
          }//inside pid_br_lrft
          if (pid_br_right->IsInside(objtac_corr, ic_sum)){
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              dirname = "Br";
              histname = "EnergyDC_Br_right_tcut";
              obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
            }
          }//inside pid_br_right
          if (pid_rb_right->IsInside(objtac_corr, ic_sum)){
            if (energy_dc > ENERGY_THRESHOLD){
              energies_in_tcut_rb_right.push_back(energy_dc);
            }
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              dirname = "Rb";
              histname = "EnergyDC_Rb_right_tcut";
              obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
            }
          }//inside pid_rb_right
          if (pid_rb_left->IsInside(objtac_corr, ic_sum)){
            if (energy_dc > ENERGY_THRESHOLD){
              energies_in_tcut_rb_left.push_back(energy_dc);
            }
            if (tcut_kr88->IsInside(corr_time, energy_dc)){
              dirname = "Rb";
              histname = "EnergyDC_Rb_left_tcut";
              obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
            }
          }//inside pid_rb_left
        }//s800 exists
      }//hit has both energy and time
    }//loop over hits
    if (energies_in_tcut_kr88.size() == 1){

      dirname = "Kr88";
      histname = "kr88_mult_one";
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_kr88.at(0));
    }
    else if (energies_in_tcut_kr88.size() == 2){
      dirname = "Kr88";
      histname = "kr88_mult_two";
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_kr88.at(0));
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_kr88.at(1));
    }

    else if (energies_in_tcut_rb.size() == 1){
      dirname = "Rb";
      histname = "Rb_mult_one";
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_rb.at(0));
    }
    else if (energies_in_tcut_rb.size() == 2){
      dirname = "Rb";
      histname = "Rb_mult_two";
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_rb.at(0));
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_rb.at(1));
    }
    else if (energies_in_tcut_rb_left.size() == 1){
      dirname = "Rb";
      histname = "Rb_left_mult_one";
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_rb_left.at(0));
    }
    else if (energies_in_tcut_rb_left.size() == 2){
      dirname = "Rb";
      histname = "Rb_left_mult_two";
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_rb_left.at(0));
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_rb_left.at(1));
    }
    else if (energies_in_tcut_rb_right.size() == 1){
      dirname = "Rb";
      histname = "Rb_right_mult_one";
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_rb_right.at(0));
    }
    else if (energies_in_tcut_rb_right.size() == 2){
      dirname = "Rb";
      histname = "Rb_right_mult_two";
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_rb_right.at(0));
      obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energies_in_tcut_rb_right.at(1));
    }

//  for (unsigned int i = 0; i < energies_in_tcut_kr88.size(); i++){
//    for (unsigned int j = i+1; j < energies_in_tcut_kr88.size(); j++){
//      TH2 *kr88_coincidence_spec = GetMatrix(list, "kr88_coincidence", N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X,N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X);
//      kr88_coincidence_spec->Fill(energies_in_tcut_kr88.at(i), energies_in_tcut_kr88.at(j));
//      kr88_coincidence_spec->Fill(energies_in_tcut_kr88.at(j), energies_in_tcut_kr88.at(i));
//    }
//  }
//  for (unsigned int i = 0; i < energies_in_tcut_rb.size(); i++){
//    for (unsigned int j = i+1; j < energies_in_tcut_rb.size(); j++){
//      TH2 *rb_coincidence_spec = GetMatrix(list, "rb_coincidence", N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X,N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X);
//      rb_coincidence_spec->Fill(energies_in_tcut_rb.at(i), energies_in_tcut_rb.at(j));
//      rb_coincidence_spec->Fill(energies_in_tcut_rb.at(j), energies_in_tcut_rb.at(i));
//    }
//  }
//  for (unsigned int i = 0; i < energies_in_tcut_rb_left.size(); i++){
//    for (unsigned int j = i+1; j < energies_in_tcut_rb_left.size(); j++){
//      TH2 *rb_left_coincidence_spec = GetMatrix(list, "rb_left_coincidence", N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X,N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X);
//      rb_left_coincidence_spec->Fill(energies_in_tcut_rb_left.at(i), energies_in_tcut_rb_left.at(j));
//      rb_left_coincidence_spec->Fill(energies_in_tcut_rb_left.at(j), energies_in_tcut_rb_left.at(i));
//    }
//  }
//  for (unsigned int i = 0; i < energies_in_tcut_rb_right.size(); i++){
//    for (unsigned int j = i+1; j < energies_in_tcut_rb_right.size(); j++){
//      TH2 *rb_right_coincidence_spec = GetMatrix(list, "rb_right_coincidence", N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X,N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X);
//      rb_right_coincidence_spec->Fill(energies_in_tcut_rb_right.at(i), energies_in_tcut_rb_right.at(j));
//      rb_right_coincidence_spec->Fill(energies_in_tcut_rb_right.at(j), energies_in_tcut_rb_right.at(i));
//    }
//  }
  }//caesar exists

  if(s800) {
    double ic_sum = s800->GetIonChamber().GetAve();
    double objtac_corr = s800->GetCorrTOF_OBJTAC();
    double objtac = s800->GetTof().GetTacOBJ();
    double xfptac = s800->GetTof().GetTacXFP();
    //double afp = s800->GetAFP();
    double ata = s800->GetAta();
    double bta = s800->GetBta();
    dirname = "S800";
    histname ="ATA";
    obj.FillHistogram(dirname, histname,5000,-0.5,0.5, ata);
    histname ="BTA";
    obj.FillHistogram(dirname, histname,5000,-0.5,0.5, bta);
    histname ="PID_TAC";
    obj.FillHistogram(dirname, histname,N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, objtac_corr,
                                        N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, ic_sum);
    histname ="tacxfp_tacobj_ungated";
    obj.FillHistogram(dirname, histname,N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, xfptac,
                                        N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, objtac);
    
    histname = "TrigBit";

    int trig_bit = 6;
    int freg = s800->GetTrigger().GetRegistr();
    if (freg != 1 && freg != 2 && freg != 3){
      trig_bit = 9; //Just a random channel to fill for nonsense input
    }
    else{
      if (freg&1){
        trig_bit = 0;
      }
      if (freg&2){
        trig_bit = 1;
      }
    }

    obj.FillHistogram(dirname, histname,10,0,10 ,trig_bit);
  }//if s800

  if(numobj!=list->GetSize())
    list->Sort();
}
