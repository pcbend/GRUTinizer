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

int evt_ctr = 0;
int omitted_det = -1;
TFile *cut_file  = 0;
TCutG *in   = 0;
TCutG *pid  = 0;
TCutG *tcut = 0;
//TH1 *GetHistogram(TList *list, std::string histname,int xbins,double xlow,double xhigh) {
//  //TList *list = &(obj.GetObjects());
//  TH1   *hist = (TH1*)list->FindObject(histname.c_str());
//  if(!hist) {
//    hist= new TH1I(histname.c_str(),histname.c_str(),xbins,xlow,xhigh);
//    list->Add(hist);
//  }
//  return hist;
//}
//
//TH2 *GetMatrix(TList *list, std::string histname,int xbins, double xlow,double xhigh,
//                                                 int ybins, double ylow,double yhigh) {
//  //TList *list = &(obj.GetObjects());
//  TH2   *mat  = (TH2*)list->FindObject(histname.c_str());
//  if(!mat) {
//    mat = new TH2I(histname.c_str(),histname.c_str(),xbins,xlow,xhigh,
//                                                     ybins,ylow,yhigh);
//    list->Add(mat);
//  }
//  return mat;
//}


void initializeKr88Cuts(TFile * &cut_file, TCutG* &pid, TCutG* &tcut,
                        TCutG* &in) {
    TPreserveGDirectory a;
    cut_file = new TFile("/mnt/analysis/pecan-gade/elman/Kr88/cut_files/kr88_cuts.root","Read");
    pid = (TCutG*)cut_file->Get("large2_pid_kr88");
    tcut = (TCutG*)cut_file->Get("full_tcut");
    in = (TCutG*)cut_file->Get("in_kr88_large");

}

void initializeKr90Cuts(TFile * &cut_file, TCutG* &pid, TCutG* &tcut,
                    TCutG* &in){
    TPreserveGDirectory a;
    cut_file = new TFile("/mnt/analysis/pecan-gade/elman/Kr90/cut_files/kr90_cuts.root","Read");
    pid = (TCutG*)cut_file->Get("kr90_mid");
//    tcut = (TCutG*)cut_file->Get("tcut");
    tcut = (TCutG*)cut_file->Get("tcut_tighter");
    in = (TCutG*)cut_file->Get("in_kr90");
    omitted_det = 176;
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

  if(caesar && s800) {
    if (cut_file == 0){
      initializeKr88Cuts(cut_file,pid, tcut, in);
      //initializeKr90Cuts(cut_file,pid, tcut, in);
    }
    const int SINGLES_ENERGY_THRESHOLD = 300;
    const int AB_ENERGY_THRESHOLD = 0;
    const int SCATTER_ANGLE_CUT = 180;

    std::vector<double> energies_singles;
    std::vector<double> energies_addback;
    std::vector<double> energies_addback_n0;
    std::vector<double> energies_addback_n1;
    std::vector<double> energies_addback_n2;
    std::vector<double> energies_addback_ng;
    std::vector<double> time_singles;
    std::vector<TVector3> pos_singles;
    energies_singles.clear();
    energies_addback.clear();
    std::string dirname = "";
    std::string histname = "";
    //TVector3 track_vect = s800->ExitTargetVect();
    double objtac_corr = s800->GetCorrTOF_OBJTAC();
    double objtac = s800->GetTof().GetTacOBJ();
    double xfptac = s800->GetTof().GetTacXFP();
    double ic_sum = s800->GetIonChamber().GetAve();
    for(unsigned int y=0;y<caesar->Size();y++) {
      TCaesarHit &hit = caesar->GetCaesarHit(y);

      if (omitted_det != -1 &&  omitted_det == hit.GetAbsoluteDetectorNumber()){
        continue;
      }

      if (hit.IsValid()){//only accept hits with both times and energies
        std::string histname;
        double energy_dc = hit.GetDoppler();

        //double scatter_angle = 0;
        double scatter_angle = s800->Track().Theta()*(180.0/TMath::Pi());
        double corr_time = caesar->GetCorrTime(hit,s800);
        //targ_exit_vec = (pt,theta,phi)

        if (pid->IsInside(objtac_corr, ic_sum)){
          if (tcut->IsInside(corr_time, energy_dc)){
            if (in->IsInside(xfptac,objtac)){
              if (energy_dc > SINGLES_ENERGY_THRESHOLD){
                if (scatter_angle < SCATTER_ANGLE_CUT){
                  energies_singles.push_back(energy_dc);
                  time_singles.push_back(hit.Time());
                  pos_singles.push_back(hit.GetPosition());
                }
              }//For multiplicity purposes

              dirname = "Caesar";
              histname = "energy_dc_pid_in_tcut";
              obj.FillHistogram(dirname,histname,
                                8192,0,8192,energy_dc);
            }//inside in-beam cut
          }//is inside timecut
        }//is inside pid
      }//hit has both energy and time
    }//loop over singles hits


    //Now loop over addback hits
    int num_addback_hits = caesar->AddbackSize();
    for (int y = 0; y < num_addback_hits; y++){
      TCaesarHit &hit = caesar->GetAddbackHit(y);
      
      if (omitted_det != -1 &&  omitted_det == hit.GetAbsoluteDetectorNumber()){
        continue;
      }
      if (hit.IsValid()){//only accept hits with both times and energies
        std::string histname;

        //double energy_dc = caesar->GetEnergyDC(hit);
        double energy_dc = hit.GetDoppler();
        double corr_time = caesar->GetCorrTime(hit,s800);
        double scatter_angle = s800->Track().Theta()*(180.0/TMath::Pi());


        if (pid->IsInside(objtac_corr, ic_sum)){
          if (tcut->IsInside(corr_time, energy_dc)){
            if (in->IsInside(xfptac,objtac)){
              if (energy_dc > AB_ENERGY_THRESHOLD){
                if (scatter_angle <SCATTER_ANGLE_CUT){
                  energies_addback.push_back(energy_dc);
                }
              }//For multiplicity purposes

              dirname = "CaesarAddback";
              histname = "ab_energy_dc_pid_in_tcut";
              obj.FillHistogram(dirname,histname,
                                8192,0,8192,energy_dc);
              if (hit.GetNumHitsContained() == 1 && !hit.is_garbage_addback){
                dirname = "CaesarAddback";
                histname = "ab_energy_dc_pid_in_tcut_n0";
                obj.FillHistogram(dirname,histname,
                                  8192,0,8192,energy_dc);
                energies_addback_n0.push_back(energy_dc);

              }
              else if (hit.GetNumHitsContained() == 2 && !hit.is_garbage_addback){
                dirname = "CaesarAddback";
                histname = "ab_energy_dc_pid_in_tcut_n1";
                obj.FillHistogram(dirname,histname,
                                  8192,0,8192,energy_dc);
                energies_addback_n1.push_back(energy_dc);

              }
              else if (hit.GetNumHitsContained() == 3 && !hit.is_garbage_addback){
                dirname = "CaesarAddback";
                histname = "ab_energy_dc_pid_in_tcut_n2";
                obj.FillHistogram(dirname,histname,
                                  8192,0,8192,energy_dc);
                energies_addback_n2.push_back(energy_dc);
              }
              else if(hit.is_garbage_addback){
                dirname = "CaesarAddback";
                histname = "ab_energy_dc_pid_in_tcut_ng";
                obj.FillHistogram(dirname,histname,
                                  8192,0,8192,energy_dc);
                energies_addback_ng.push_back(energy_dc);

              }
              else {
                std::cout << "Weird event not meeting any criteria for addback" << std::endl;
                std::cout << "hit.is_garbage_addback    = " << hit.is_garbage_addback << std::endl;
                std::cout << "hit.GetNumHitsContained() = " << hit.GetNumHitsContained() << std::endl;
              }
            }//inside in-beam cut
          }//is inside timecut
        }//is inside pid_
      }//hit has both energy and time
    }




    //Fill multiplicity and coincidence histograms for singles
    unsigned int num_hits_singles = energies_singles.size();
    if (num_hits_singles == 1){
      dirname = "Caesar";
      histname = "energy_dc_mult_one";
      obj.FillHistogram(dirname,histname,
                        8192,0,8192,energies_singles.at(0));
    }
    else if (num_hits_singles == 2){
      dirname = "Caesar";
      histname = "energy_dc_mult_two";
      obj.FillHistogram(dirname,histname,
                        8192,0,8192,energies_singles.at(0));
      obj.FillHistogram(dirname,histname,
                        8192,0,8192,energies_singles.at(1));
      dirname = "Caesar";
      histname = "energy_dc_coincidence_matrix_mult_two";
      obj.FillHistogram(dirname, histname,
                        8192,0,8192, energies_singles.at(0),
                        8192,0,8192, energies_singles.at(1));
      obj.FillHistogram(dirname, histname,
                        8192,0,8192, energies_singles.at(1),
                        8192,0,8192, energies_singles.at(0));

    }
    for (unsigned int i = 0; i < num_hits_singles; i++){
      for (unsigned int j = i+1; j < num_hits_singles; j++){
        dirname = "Caesar";
        histname = "energy_dc_coincidence_matrix";
        obj.FillHistogram(dirname, histname,
                          8192,0,8192, energies_singles.at(i),
                          8192,0,8192, energies_singles.at(j));
        obj.FillHistogram(dirname, histname,
                          8192,0,8192, energies_singles.at(j),
                          8192,0,8192, energies_singles.at(i));
        histname = "poss_diff";
        obj.FillHistogram(dirname, histname,
                          2000,-1000,1000,  (pos_singles.at(i)-pos_singles.at(j)).Mag());

        histname = "time_diff";
        obj.FillHistogram(dirname, histname,
                          2000,-1000,1000, time_singles.at(i)-time_singles.at(j));
      }
    }

    dirname = "Caesar";
    histname = "multiplicity";
    obj.FillHistogram(dirname, histname,
                      192, 0, 192, num_hits_singles);

    for (unsigned int i = 0; i < num_hits_singles; i++){
      histname = "multiplicity_energy_summary";
      obj.FillHistogram(dirname, histname,
                        192, 0, 192, num_hits_singles,
                        8192,0,8192, energies_singles.at(i));
    }

    //addback_mult == multiplicity in gates
    int addback_mult = energies_addback.size();
    int n0_mult = energies_addback_n0.size();
    int n1_mult = energies_addback_n1.size();
    int n2_mult = energies_addback_n2.size();
    int ng_mult = energies_addback_ng.size();
    if (addback_mult != n0_mult+n1_mult+n2_mult+ng_mult){
      std::cout << "FATAL ADDBACK ERROR! Total Addback multiplicity not sum of parts!" << std::endl;
    }
    if (addback_mult == 1){
      dirname = "CaesarAddback";
      histname = "ab_energy_dc_mult_one";
      obj.FillHistogram(dirname,histname,
                        8192,0,8192,energies_addback.at(0));
      if (n0_mult ==1){
        histname = "ab_energy_dc_mult_one_n0";
        obj.FillHistogram(dirname,histname,
                          8192,0,8192,energies_addback_n0.at(0));
      }
      else if (n1_mult ==1){
        histname = "ab_energy_dc_mult_one_n1";
        obj.FillHistogram(dirname,histname,
                          8192,0,8192,energies_addback_n1.at(0));
      }
      else if (n2_mult ==1){
        histname = "ab_energy_dc_mult_one_n2";
        obj.FillHistogram(dirname,histname,
                          8192,0,8192,energies_addback_n2.at(0));
      }
    }
    else if (addback_mult == 2){
      dirname = "CaesarAddback";
      histname = "ab_energy_dc_mult_two";
      obj.FillHistogram(dirname,histname,
                        8192,0,8192,energies_addback.at(0));
      obj.FillHistogram(dirname,histname,
                        8192,0,8192,energies_addback.at(1));
      for (int i = 0; i < addback_mult; i++){
        for (int j = i+1; j < addback_mult; j++){
          dirname = "CaesarAddback";
          histname = "ab_energy_dc_coincidence_matrix_multtwo";
          obj.FillHistogram(dirname, histname,
              8192,0,8192, energies_addback.at(i),
              8192,0,8192, energies_addback.at(j));
          obj.FillHistogram(dirname, histname,
              8192,0,8192, energies_addback.at(j),
              8192,0,8192, energies_addback.at(i));

          if (n0_mult + n1_mult + n2_mult == 2){
            histname = "ab_energy_dc_coincidence_matrix_multtwo_nogarbage";
            obj.FillHistogram(dirname,histname,
                8192,0,8192, energies_addback.at(i),
                8192,0,8192, energies_addback.at(j));
            obj.FillHistogram(dirname,histname,
                8192,0,8192, energies_addback.at(j),
                8192,0,8192, energies_addback.at(i));
          }

          if (n0_mult == 2){
            histname = "ab_energy_dc_coincidence_matrix_multtwo_n0";
            obj.FillHistogram(dirname,histname,
                8192,0,8192, energies_addback.at(i),
                8192,0,8192, energies_addback.at(j));
            obj.FillHistogram(dirname,histname,
                8192,0,8192, energies_addback.at(j),
                8192,0,8192, energies_addback.at(i));
          }
          if (n0_mult + n1_mult == 2){
            histname = "ab_energy_dc_coincidence_matrix_multtwo_n0n1";
            obj.FillHistogram(dirname,histname,
                8192,0,8192, energies_addback.at(i),
                8192,0,8192, energies_addback.at(j));
            obj.FillHistogram(dirname,histname,
                8192,0,8192, energies_addback.at(j),
                8192,0,8192, energies_addback.at(i));
          }
          if (n1_mult == 2){
            histname = "ab_energy_dc_coincidence_matrix_multtwo_n1n1";
            obj.FillHistogram(dirname,histname,
                8192,0,8192, energies_addback.at(i),
                8192,0,8192, energies_addback.at(j));
            obj.FillHistogram(dirname,histname,
                8192,0,8192, energies_addback.at(j),
                8192,0,8192, energies_addback.at(i));

          }
        }
      }
      if (n0_mult + n1_mult + n2_mult == 2){
        histname = "ab_energy_dc_multtwo_nogarbage";
        obj.FillHistogram(dirname,histname,
                          8192,0,8192,energies_addback.at(0));
        obj.FillHistogram(dirname,histname,
                          8192,0,8192,energies_addback.at(1));
        if (n2_mult ==0){
          if (n1_mult == 0){
            histname = "ab_energy_dc_multtwo_n0";
            obj.FillHistogram(dirname,histname,
                              8192,0,8192,energies_addback.at(0));
            obj.FillHistogram(dirname,histname,
                              8192,0,8192,energies_addback.at(1));
          }//pure n0
          else{
            histname = "ab_energy_dc_multtwo_n1";
            obj.FillHistogram(dirname,histname,
                              8192,0,8192,energies_addback.at(0));
            obj.FillHistogram(dirname,histname,
                              8192,0,8192,energies_addback.at(1));
          }
        }//no n2 events
        else{
            histname = "ab_energy_dc_multtwo_n2";
            obj.FillHistogram(dirname,histname,
                              8192,0,8192,energies_addback.at(0));
            obj.FillHistogram(dirname,histname,
                              8192,0,8192,energies_addback.at(1));
        }
      }
    }//addback_mult == 2
    for (int i = 0; i < addback_mult; i++){
      for (int j = i+1; j < addback_mult; j++){
        dirname = "CaesarAddback";
        histname = "ab_energy_dc_coincidence_matrix";
        obj.FillHistogram(dirname, histname,
                          8192,0,8192, energies_addback.at(i),
                          8192,0,8192, energies_addback.at(j));
        obj.FillHistogram(dirname, histname,
                          8192,0,8192, energies_addback.at(j),
                          8192,0,8192, energies_addback.at(i));
        if (ng_mult == 0){
          histname = "ab_energy_dc_coincidence_matrix_nogarbage";
          obj.FillHistogram(dirname, histname,
              8192,0,8192, energies_addback.at(i),
              8192,0,8192, energies_addback.at(j));
          obj.FillHistogram(dirname, histname,
              8192,0,8192, energies_addback.at(j),
              8192,0,8192, energies_addback.at(i));
        }
      }
    }

    dirname = "CaesarAddback";
    histname = "ab_multiplicity";
    obj.FillHistogram(dirname, histname,
                      192, 0, 192, addback_mult);
    for (int i = 0; i < addback_mult; i++){
      histname = "multiplicity_ab_energy_summary";
      obj.FillHistogram(dirname, histname,
                        192, 0, 192, addback_mult,
                        8192,0,8192, energies_addback.at(i));
    }
  }//caesar exists

  if(caesar) { caesar->Clear(); }

  if(numobj!=list->GetSize())
    list->Sort();
}
