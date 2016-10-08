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
TCutG *pid   = 0;
TCutG *tcut  = 0;
TCutG *in    = 0;
int omitted_det = -1;

#define NUM_DETS 192

//void initializeKr88Cuts(TFile * &cut_file, TCutG* &pid_kr88, TCutG* &tcut_kr88, 
//                        TCutG* &pid_rb, TCutG* &pid_br, TCutG* &in_kr88,
//                        TCutG* &pid_rb_left, TCutG* &pid_br_left,
//                        TCutG* &pid_rb_right, TCutG* &pid_br_right) {
//    cut_file = new TFile("/mnt/analysis/pecan-gade/elman/Kr88/cut_files/kr88_cuts.root","Read");
//    pid_kr88 = (TCutG*)cut_file->Get("large2_pid_kr88");
////    pid_kr88 = (TCutG*)cut_file->Get("pid_kr88_large");
//    pid_rb = (TCutG*)cut_file->Get("pid_rb_large");
//    pid_br = (TCutG*)cut_file->Get("pid_br_large");
////    tcut_kr88 = (TCutG*)cut_file->Get("tcut_widest_03_17_2016");
//    tcut_kr88 = (TCutG*)cut_file->Get("full_tcut");
//    in_kr88 = (TCutG*)cut_file->Get("in_kr88_large");
//
//    pid_rb_left = (TCutG*)cut_file->Get("pid_rb_left");
//    pid_br_left = (TCutG*)cut_file->Get("pid_br_left");
//    pid_rb_right = (TCutG*)cut_file->Get("pid_rb_right");
//    pid_br_right = (TCutG*)cut_file->Get("pid_br_right");
//}

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
}
void initializeKr90Cuts(TFile* &cut_file, TCutG* &pid, TCutG* &in, TCutG *&tcut){
  TPreserveGDirectory a;//needed to stop root from switching directories to cut_file
  cut_file = new TFile("/mnt/analysis/pecan-gade/elman/Kr90/cut_files/kr90_cuts.root","Read");
  if (!cut_file){
    std::cout << "Failed to open cut file" << std::endl;
    exit(5);
  }
  pid = (TCutG*)cut_file->Get("kr90_mid");
  //in = (TCutG*)cut_file->Get("prelim_in_cut");
  in = (TCutG*)cut_file->Get("in_kr90");
//  tcut = (TCutG*)cut_file->Get("tcut");
  tcut = (TCutG*)cut_file->Get("tcut_tighter");
  omitted_det = 176;
}
void initializeKr88Cuts(TFile* &cut_file, TCutG* &pid, TCutG* &in, TCutG *&tcut){
  TPreserveGDirectory a;//needed to stop root from switching directories to cut_file
  cut_file = new TFile("/mnt/analysis/pecan-gade/elman/Kr88/cut_files/kr88_cuts.root","Read");
  if (!cut_file){
    std::cout << "Failed to open cut file" << std::endl;
    exit(5);
  }
  pid = (TCutG*)cut_file->Get("large2_pid_kr88");
  in = (TCutG*)cut_file->Get("in_kr88_large");
  tcut  = (TCutG*)cut_file->Get("full_tcut");
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

  std::string histname;
  std::string dirname;
  const int ENERGY_LOW_X = 0;
  const int ENERGY_HIGH_X = 8192;
  const int N_BINS_X = 8192;

  if(s800) {
    double ic_sum = s800->GetIonChamber().GetAve();
    double objtac_corr = s800->GetCorrTOF_OBJTAC();
    double objtac = s800->GetTof().GetTacOBJ();
    double xfptac = s800->GetTof().GetTacXFP();
    double afp = s800->GetAFP();
    double xfp = s800->GetCrdc(0).GetDispersiveX();//for timing correction
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
    histname ="raw_tacobj_vs_xfp";
    obj.FillHistogram(dirname, histname,N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, objtac,
                                        600,-300,300, xfp);
    histname ="raw_tacobj_vs_afp";
    obj.FillHistogram(dirname, histname,N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, objtac,
                                        1000,-0.1,0.1, afp);
    histname ="corr_tacobj_vs_xfp";
    obj.FillHistogram(dirname, histname,N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, objtac_corr,
                                        600,-300,300, xfp);
    histname ="corr_tacobj_vs_afp";
    obj.FillHistogram(dirname, histname,N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, objtac_corr,
                                        1000,-0.1,0.1, afp);
    
    
    histname = "TrigBit";
    int trig_bit = 6;
    int freg = s800->GetTrigger().GetRegistr();
    if (freg != 1 && freg != 2 && freg != 3){
      trig_bit = 9; //Just a random channel to fill for nonsense input
      obj.FillHistogram(dirname, histname,10,0,10 ,trig_bit);
    }
    else{
      if (freg&1){
        trig_bit = 0;
        obj.FillHistogram(dirname, histname,10,0,10 ,trig_bit);
      }
      if (freg&2){
        trig_bit = 1;
        obj.FillHistogram(dirname, histname,10,0,10 ,trig_bit);
      }
    }
    if(caesar) {
      for(unsigned int y=0;y<caesar->Size();y++) {
        if (cut_file == 0){
          //        initializeSe86Cuts(cut_file,pid,in,tcut);
          initializeKr88Cuts(cut_file,pid,in,tcut);
          //        initializeKr90Cuts(cut_file,pid,in,tcut);
        }
        TCaesarHit &hit = caesar->GetCaesarHit(y);
        if (hit.IsValid()&&!hit.IsOverflow()){
          std::string histname;
          int det = hit.GetDetectorNumber();
          int ring = hit.GetRingNumber();

          dirname = "GeneralCaesar";
          int abs_det_num = hit.GetAbsoluteDetectorNumber();
          double raw_time = hit.GetTime();
          double corr_time = caesar->GetCorrTime(hit,s800);
          double energy = hit.GetEnergy();
          double energy_dc = hit.GetDoppler();

          //note crdc_1_x is xfp
          double crdc_1_y = s800->GetCrdc(0).GetNonDispersiveY();
          double crdc_2_y = s800->GetCrdc(1).GetNonDispersiveY();
          double crdc_2_x = s800->GetCrdc(1).GetDispersiveX();
          if (energy_dc > 300){//need energy cut to not gate on noise
            histname = "Raw_Time_Summary_Ungated";
            obj.FillHistogram(dirname, histname,200,0, 200, abs_det_num,
                2048,0,2048, raw_time);
          }
          histname = "CRDC1_Y";
          obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_1_y);
          histname = "CRDC2_Y";
          obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_2_y);
          histname = "CRDC2_X";
          obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_2_x);
          histname = "CRDC1_X";
          obj.FillHistogram(dirname, histname, 2000,-200,200, xfp);

          histname = "EnergyDC_CorrTime_ungated";
          obj.FillHistogram(dirname, histname, 
              4000,-2000,2000, corr_time, 
              N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);

          if (in->IsInside(xfptac,objtac)){
            histname ="PID_TAC_INBEAM";
            obj.FillHistogram(dirname, histname,N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, objtac_corr,
                N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, ic_sum);
          }

          if (pid->IsInside(objtac_corr, ic_sum)){
            dirname = "PID";
            histname = "EnergyDC_CorrTime_PID";
            obj.FillHistogram(dirname, histname, 
                4000,-2000,2000, corr_time, 
                N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
            histname = "tacxfp_tacobj_PID";
            obj.FillHistogram(dirname, histname, 
                N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, xfptac, 
                N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, objtac);
            if (energy_dc > 300){//need energy cut to not gate on noise
              histname = "Raw_Time_Summary_PID";
              obj.FillHistogram(dirname, histname,200,0, 200, abs_det_num,
                  2048,0,2048, raw_time);
            }
            histname = "CRDC1_Y_PID";
            obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_1_y);
            histname = "CRDC2_Y_PID";
            obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_2_y);
            histname = "CRDC2_X_PID";
            obj.FillHistogram(dirname, histname, 2000,-200,200, crdc_2_x);
            histname = "CRDC1_X_PID";
            obj.FillHistogram(dirname, histname, 2000,-200,200, xfp);


            if (tcut->IsInside(corr_time, energy_dc)){
              if (in->IsInside(xfptac,objtac)){
                if (omitted_det != -1 && det+total_det_in_prev_rings[ring] != omitted_det){
                  histname = "EnergyDC_PID_incut_no176";
                  obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
                  histname = "EnergyDC_CorrTime_PID_no176";
                  obj.FillHistogram(dirname, histname, 
                      4000,-2000,2000, corr_time, 
                      N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
                }
                histname = "EnergyDC_PID_incut";
                obj.FillHistogram(dirname, histname, N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);
                histname = "EnergyDC_CorrTime_PID";
                obj.FillHistogram(dirname, histname, 
                    4000,-2000,2000, corr_time, 
                    N_BINS_X,ENERGY_LOW_X,ENERGY_HIGH_X, energy_dc);

                histname = "Detector_Energy_Summary_PID_IN_TCUT";
                obj.FillHistogram(dirname, histname, 
                    NUM_DETS+1, 0, NUM_DETS+1,             det+total_det_in_prev_rings[ring],
                    N_BINS_X, ENERGY_LOW_X, ENERGY_HIGH_X, energy);

                histname = "Detector_DCEnergy_Summary_PID_IN_TCUT";
                obj.FillHistogram(dirname, histname, 
                    NUM_DETS+1, 0, NUM_DETS+1,             det+total_det_in_prev_rings[ring],
                    N_BINS_X, ENERGY_LOW_X, ENERGY_HIGH_X, energy_dc);
              }//inside in-beam cut
            }//inside time
          }//inside pid
        }//hit is valid
      }//loop over hits 
    }//caesar exists
  }//if s800

  if(numobj!=list->GetSize())
    list->Sort();
}
