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

#include "TPreserveGDirectory.h"
TFile *cut_file  = 0;
TCutG *pid  = 0;
TCutG *tcut = 0;
TCutG *in    = 0;
int omitted_det = -1;
int omitted_det_2 = -1;


void initializeKr88Cuts(TFile * &cut_file, TCutG* &pid, TCutG* &tcut, 
                        TCutG* &in) {
    TPreserveGDirectory a;//needed to stop root from switching directories to cut_file
    cut_file = new TFile("/mnt/analysis/pecan-gade/elman/Kr88/cut_files/kr88_cuts.root","Read");
    //pid = (TCutG*)cut_file->Get("pid_kr88_large");
    //pid = (TCutG*)cut_file->Get("pid_kr88_med");
    //pid = (TCutG*)cut_file->Get("pear_pid_kr88");
    pid = (TCutG*)cut_file->Get("large2_pid_kr88");
    //tcut = (TCutG*)cut_file->Get("tcut_tight");
    //pid = (TCutG*)cut_file->Get("pid_kr88_tight");
    //tcut = (TCutG*)cut_file->Get("tcut_widest_03_17_2016");
    tcut = (TCutG*)cut_file->Get("full_tcut");
    //tcut = (TCutG*)cut_file->Get("tcut_tight");
    in = (TCutG*)cut_file->Get("in_kr88_large");
}
void initializeKr90Cuts(TFile * &cut_file, TCutG* &pid, TCutG* &tcut, 
                        TCutG* &in) {
    TPreserveGDirectory a;//needed to stop root from switching directories to cut_file
    cut_file = new TFile("/mnt/analysis/pecan-gade/elman/Kr90/cut_files/kr90_cuts.root","Read");
    //pid = (TCutG*)cut_file->Get("pid_large");
    //pid = (TCutG*)cut_file->Get("pid_med");
    //tcut = (TCutG*)cut_file->Get("tcut_tight");
    pid = (TCutG*)cut_file->Get("kr90_mid");
//    pid = (TCutG*)cut_file->Get("kr90_widest");
//    pid = (TCutG*)cut_file->Get("kr90_tightest");
    tcut = (TCutG*)cut_file->Get("tcut");
//    tcut = (TCutG*)cut_file->Get("tcut_tighter");
//   in = (TCutG*)cut_file->Get("in_kr90");
//    in = (TCutG*)cut_file->Get("in_kr90_tight");
  in = (TCutG*)cut_file->Get("in_kr90_widest");
    omitted_det = 176;//referenced from 0! It's ring 8 detector 8
}

void initializeSe86Cuts(TFile* &cut_file, TCutG* &pid, TCutG* &in, TCutG *&tcut){
  TPreserveGDirectory a;//needed to stop root from switching directories to cut_file
  cut_file = new TFile("/mnt/analysis/pecan-gade/elman/Se86/cut_files/grut_se86_cuts.root","Read");
  if (!cut_file){
    std::cout << "Failed to open cut file" << std::endl;
    exit(5);
  }
  pid = (TCutG*)cut_file->Get("prelim_pid_se86");
  //in = (TCutG*)cut_file->Get("prelim_in_cut");
  in = (TCutG*)cut_file->Get("in_se86");
  tcut = (TCutG*)cut_file->Get("prelim_tcut");
  omitted_det = 176;
  omitted_det_2 = 26;
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
  std::string dirname = "";
  std::string histname = "";

  if (s800){
    if (cut_file == 0){
      //initializeKr90Cuts(cut_file,pid,tcut,in);
      //initializeSe86Cuts(cut_file,pid,in,tcut);
      initializeKr88Cuts(cut_file,pid,tcut,in);
    }

    double objtac_corr = s800->GetCorrTOF_OBJTAC();
    double ic_sum = s800->GetIonChamber().GetAve();

    int freg = s800->GetTrigger().GetRegistr();
    int trigbit = 7;
    if (freg != 1 && freg != 2 && freg != 3){
      trigbit = 9;//Just a random channel to fill for nonsense input
      dirname = "Info";
      histname = "trig_bit";
      obj.FillHistogram(dirname,histname,
          10,0,10, trigbit);
    }
    else{
      if (freg&1){
        trigbit = 0;
        dirname = "Info";
        histname = "trig_bit";
        obj.FillHistogram(dirname,histname,
            10,0,10, trigbit);
      }
      if (freg&2){
        trigbit = 1;
        dirname = "Info";
        histname = "trig_bit";
        obj.FillHistogram(dirname,histname,
            10,0,10, trigbit);
      }
    }
    if (pid->IsInside(objtac_corr,ic_sum)){
      int trigbit_gated = 7;
      if (freg != 1 && freg != 2 && freg != 3){
        trigbit_gated = 9;
        dirname = "Info";
        histname = "trig_bit_gated";
        obj.FillHistogram(dirname,histname,
            10,0,10, trigbit_gated);
      }
      else{
        if (freg&1){
          trigbit_gated = 0;
          dirname = "Info";
          histname = "trig_bit_gated";
          obj.FillHistogram(dirname,histname,
              10,0,10, trigbit_gated);
        }
        if (freg&2){
          trigbit_gated = 1;
          dirname = "Info";
          histname = "trig_bit_gated";
          obj.FillHistogram(dirname,histname,
              10,0,10, trigbit_gated);
        }
      }       

    }//inside pid
    if(caesar) {
      const double START_ANGLE = 180.0;
      const double FINAL_ANGLE = 180.0;
    //const double START_ANGLE = 1.0;
    //const double FINAL_ANGLE = 3.5;
      const double ANGLE_STEPS = 0.1;
      const int TOTAL_ANGLES = (FINAL_ANGLE-START_ANGLE)/ANGLE_STEPS + 1;
      std::vector<double> angles;
      angles.reserve(TOTAL_ANGLES);

      double temp_angle = START_ANGLE;
      for (int i = 0; i < TOTAL_ANGLES; i++){
        angles.push_back(temp_angle);
        temp_angle += ANGLE_STEPS;
      }

      for(unsigned int y=0;y<caesar->Size();y++) {

        TCaesarHit &hit = caesar->GetCaesarHit(y);
        if (omitted_det != -1 && hit.GetAbsoluteDetectorNumber() == omitted_det){
          continue;
        }
        if (omitted_det_2 != -1 && hit.GetAbsoluteDetectorNumber() == omitted_det_2){
          continue;
        }
        if (hit.IsValid()){//only accept hits with both times and energies
          std::string histname;
          //double energy_dc = caesar->GetEnergyDC(hit);
          double energy_dc = hit.GetDoppler();

          double corr_time = caesar->GetCorrTime(hit,s800);
          double scatter_angle = s800->Track().Theta()*(180.0/TMath::Pi());

          dirname = "Info";
          histname = "scatter_angle_ungated";
          obj.FillHistogram(dirname,histname,
              18000,0,180,fabs(scatter_angle));

          double objtac = s800->GetTof().GetTacOBJ();
          double xfptac = s800->GetTof().GetTacXFP();

          if (pid->IsInside(objtac_corr, ic_sum)){
            if (tcut->IsInside(corr_time, energy_dc)){
              if (in->IsInside(xfptac,objtac)){
                dirname = "Info";
                histname = "scatter_angle_pid_tcut_incut";
                obj.FillHistogram(dirname,histname,
                    18000,0,180,fabs(scatter_angle));
                bool done = false;
                int cur_angle_index = 0;
                while (!done && cur_angle_index < TOTAL_ANGLES){
                  if (fabs(scatter_angle) < angles.at(cur_angle_index)){
                    for (int angle_index = cur_angle_index; angle_index < TOTAL_ANGLES; angle_index++){
                      std::stringstream ss;
                      ss << "fit_angle_" << angles.at(angle_index);
                      dirname = "Fits";
                      histname = ss.str();
                      obj.FillHistogram(dirname, histname, 
                          8192,0,8192, energy_dc);
                    }//loop through angles above cur_angle_index to fill all histos
                    done = true;
                  }//scatter_angle < angles[cur_angle_index]
                  cur_angle_index++;
                }//!done && cur_angle_index < TOTAL_ANGLES
              }//inside in-beam cut
            }//is inside timecut
          }//is inside pid
        }//hit has both energy and time
      }//loop over hits
    }//caesar exists
  }//s800 exists
  if(numobj!=list->GetSize())
    list->Sort();
}
