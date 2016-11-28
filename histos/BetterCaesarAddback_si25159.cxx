#include "TRuntimeObjects.h"

#include <iostream>
#include <map>

#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <sstream>
#include "TRandom.h"

#include "TPreserveGDirectory.h"
#include "TObject.h"
#include "TCaesar.h"
#include "TS800.h"

#include "TChannel.h"
#include "GValue.h"
#include "TFile.h"
#include "TCutG.h"


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TCaesar  *caesar  = obj.GetDetector<TCaesar>();
  TCaesar  *caesar_ab = new TCaesar();//will be used to do addback correction 
                                      //only for the hits inside the time cut
  TS800    *s800    = obj.GetDetector<TS800>();
  //double    beta = GValue::Value("BETA");
  double    beta = 0.431;
  double    z_shift = 0.65;

  static TCutG *timingcut_si25 = 0;
  if(!timingcut_si25) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/timeenergy_si25tighter.root");
    timingcut_si25 = (TCutG*)fcut.Get("si25timegatetighter");
  }
  static TCutG *InBeam_Mid = 0;
  if(!InBeam_Mid) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/newmid140.root");
    InBeam_Mid = (TCutG*)fcut.Get("InBeam_Mid140");
  }
  static TCutG *InBeam_Top = 0;
  if(!InBeam_Top) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/newtop140.root");
    InBeam_Top = (TCutG*)fcut.Get("Inbeam_top140");
  }
  static TCutG *InBeam_btwnTopMid = 0;
  if(!InBeam_btwnTopMid) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/newbetween140.root");
    InBeam_btwnTopMid = (TCutG*)fcut.Get("between140");
  }
  static TCutG *al23blob = 0;
  if(!al23blob) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/al23blob.root");
    al23blob = (TCutG*)fcut.Get("al23blob");
  }
  static TCutG *si24blob = 0;
  if(!si24blob) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/si24blob.root");
    si24blob = (TCutG*)fcut.Get("si24blob");
  }
  static TCutG *newal23blob = 0;
  if(!newal23blob) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/newal23blob.root");
    newal23blob = (TCutG*)fcut.Get("newal23blob");
  }
  static TCutG *newsi24blob = 0;
  if(!newsi24blob) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/newsi24blob.root");
    newsi24blob = (TCutG*)fcut.Get("newsi24blob");
  }
  static TCutG *si25blobtop = 0;
  if(!si25blobtop) {
     TPreserveGDirectory Preserve;
     TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/si25blobtop159.root");
     si25blobtop = (TCutG*)fcut.Get("si25top159");
  }

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  if(caesar && s800) {
    
    const int SINGLES_ENERGY_THRESHOLD = 150;
    const int AB_ENERGY_THRESHOLD = 0;

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
    double objtac_corr = s800->GetCorrTOF_OBJTAC();
    double objtac = s800->GetTof().GetTacOBJ();
    double xfptac = s800->GetTof().GetTacXFP();
    double ic_sum = s800->GetIonChamber().GetAve();
    TVector3 track = s800->Track();
    
    for(unsigned int y=0;y<caesar->Size();y++) {
      TCaesarHit &hit = caesar->GetCaesarHit(y);

      if(hit.IsOverflow())
        continue;

      if (hit.IsValid()){//only accept hits with both times and energies
        std::string histname;
        double energy_dc = hit.GetDoppler(beta,z_shift,&track);        
        double corr_time = caesar->GetCorrTime(hit,s800);

       if (si25blobtop->IsInside(objtac_corr,ic_sum)){
          if (timingcut_si25->IsInside(corr_time,hit.GetDoppler(beta,z_shift,&track))){
            if (InBeam_Top->IsInside(objtac,xfptac)){
              caesar_ab->InsertHit(hit);
              if (energy_dc > SINGLES_ENERGY_THRESHOLD){
              
                 energies_singles.push_back(energy_dc);
                 time_singles.push_back(hit.Time());
                 pos_singles.push_back(hit.GetPosition());
                
              }//For multiplicity purposes

              dirname = "Caesar";
              histname = "energy_dc_pid_in_tcut";
              obj.FillHistogram(dirname,histname,
                                2048,0,8192,energy_dc);
            }//inside in-beam cut
          }//is inside timingcut
        }//is inside pid
      }//hit has both energy and time
    }//loop over singles hits


    //Now loop over addback hits
    int num_addback_hits = caesar_ab->AddbackSize();
    for (int y = 0; y < num_addback_hits; y++){
      TCaesarHit &hit = caesar_ab->GetAddbackHit(y);

      if(hit.IsOverflow())
        continue;

      if (hit.IsValid()){//only accept hits with both times and energies
        std::string histname;

        //double energy_dc = caesar->GetEnergyDC(hit);
        double energy_dc = hit.GetDoppler(beta,z_shift,&track);
        double corr_time = caesar_ab->GetCorrTime(hit,s800);

        if (si25blobtop->IsInside(objtac_corr,ic_sum)){
          if (timingcut_si25->IsInside(corr_time, energy_dc)){
            if (InBeam_Top->IsInside(objtac,xfptac)){
              if (energy_dc > AB_ENERGY_THRESHOLD){    
        
                 energies_addback.push_back(energy_dc);   
            
              }//For multiplicity purposes

              dirname = "CaesarAddback";
              histname = "ab_energy_dc_pid_in_tcut";
              obj.FillHistogram(dirname,histname,
                                2048,0,8192,energy_dc);
              
              obj.FillHistogram("S800","DTA_ab_energy_dc_pid_in_tcut",
			        500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,energy_dc);
              if(s800->GetDta()>=0.025){
                obj.FillHistogram("S800","DTA>=0.025_ab_energy_dc_pid_in_tcut",
			        1000,-2000,2000,corr_time,
                                1024,0,8192,energy_dc);
              }
              else{
                obj.FillHistogram("S800","DTA<0.025_ab_energy_dc_pid_in_tcut",
			        1000,-2000,2000,corr_time,
                                1024,0,8192,energy_dc);
              }

              if (hit.GetNumHitsContained() == 1 && !hit.is_garbage_addback){
                dirname = "CaesarAddback";
                histname = "ab_energy_dc_pid_in_tcut_n0";
                obj.FillHistogram(dirname,histname,
                                  2048,0,8192,energy_dc);
                energies_addback_n0.push_back(energy_dc);

              }
              else if (hit.GetNumHitsContained() == 2 && !hit.is_garbage_addback){
                dirname = "CaesarAddback";
                histname = "ab_energy_dc_pid_in_tcut_n1";
                obj.FillHistogram(dirname,histname,
                                  2048,0,8192,energy_dc);
                energies_addback_n1.push_back(energy_dc);

              }
              else if (hit.GetNumHitsContained() == 3 && !hit.is_garbage_addback){
                dirname = "CaesarAddback";
                histname = "ab_energy_dc_pid_in_tcut_n2";
                obj.FillHistogram(dirname,histname,
                                  2048,0,8192,energy_dc);
                energies_addback_n2.push_back(energy_dc);
              }
              else if(hit.is_garbage_addback){
                dirname = "CaesarAddback";
                histname = "ab_energy_dc_pid_in_tcut_ng";
                obj.FillHistogram(dirname,histname,
                                  2048,0,8192,energy_dc);
                energies_addback_ng.push_back(energy_dc);

              }
              else {
                std::cout << "Weird event not meeting any criteria for addback" << std::endl;
                std::cout << "hit.is_garbage_addback    = " << hit.is_garbage_addback << std::endl;
                std::cout << "hit.GetNumHitsContained() = " << hit.GetNumHitsContained() << std::endl;
              }
            }//inside in-beam cut
          }//is inside timingcut
        }//is inside pid_
      }//hit has both energy and time
    }




    //Fill multiplicity and coincidence histograms for singles
    unsigned int num_hits_singles = energies_singles.size();
    if (num_hits_singles == 1){
      dirname = "Caesar";
      histname = "energy_dc_mult_one";
      obj.FillHistogram(dirname,histname,
                        2048,0,8192,energies_singles.at(0));
    }
    else if (num_hits_singles == 2){
      dirname = "Caesar";
      histname = "energy_dc_mult_two";
      obj.FillHistogram(dirname,histname,
                        2048,0,8192,energies_singles.at(0));
      obj.FillHistogram(dirname,histname,
                        2048,0,8192,energies_singles.at(1));
    }
    for (unsigned int i = 0; i < num_hits_singles; i++){
      for (unsigned int j = i+1; j < num_hits_singles; j++){
        dirname = "Caesar";
        histname = "energy_dc_coincidence_matrix";
        obj.FillHistogram(dirname, histname,
                          1024,0,4096, energies_singles.at(i),
                          1024,0,4096, energies_singles.at(j));
        obj.FillHistogram(dirname, histname,
                          1024,0,4096, energies_singles.at(j),
                          1024,0,4096, energies_singles.at(i));
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


    //addback_mult == multiplicity in gates
    int addback_mult = energies_addback.size();
    int n0_mult = energies_addback_n0.size();
    int n1_mult = energies_addback_n1.size();
    int n2_mult = energies_addback_n2.size();
    int ng_mult = energies_addback_ng.size();
    if (addback_mult == 1){
      dirname = "CaesarAddback";
      histname = "ab_energy_dc_mult_one";
      obj.FillHistogram(dirname,histname,
                        2048,0,8192,energies_addback.at(0));
      obj.FillHistogram("S800","DTA_ab_energy_dc_mult_one",
			        500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,energies_addback.at(0));
      if (n0_mult ==1){
        histname = "ab_energy_dc_mult_one_n0";
        obj.FillHistogram(dirname,histname,
                          2048,0,8192,energies_addback_n0.at(0));
      }
      else if (n1_mult ==1){
        histname = "ab_energy_dc_mult_one_n1";
        obj.FillHistogram(dirname,histname,
                          2048,0,8192,energies_addback_n1.at(0));
      }
      else if (n2_mult ==1){
        histname = "ab_energy_dc_mult_one_n2";
        obj.FillHistogram(dirname,histname,
                          2048,0,8192,energies_addback_n2.at(0));
      }
    }
    else if (addback_mult == 2){
      dirname = "CaesarAddback";
      histname = "ab_energy_dc_mult_two";
      obj.FillHistogram(dirname,histname,
                        2048,0,8192,energies_addback.at(0));
      obj.FillHistogram(dirname,histname,
                        2048,0,8192,energies_addback.at(1));
      obj.FillHistogram("S800","DTA_ab_energy_dc_mult_two",
			        500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,energies_addback.at(0));
      obj.FillHistogram("S800","DTA_ab_energy_dc_mult_two",
			        500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,energies_addback.at(1));
      for (int i = 0; i < addback_mult; i++){
        for (int j = i+1; j < addback_mult; j++){
          dirname = "CaesarAddback";
          histname = "ab_energy_dc_coincidence_matrix_multtwo";
          obj.FillHistogram(dirname, histname,
              1024,0,4096, energies_addback.at(i),
              1024,0,4096, energies_addback.at(j));
          obj.FillHistogram(dirname, histname,
              1024,0,4096, energies_addback.at(j),
              1024,0,4096, energies_addback.at(i));

          if (n0_mult + n1_mult + n2_mult == 2){
            histname = "ab_energy_dc_coincidence_matrix_multtwo_nogarbage";
            obj.FillHistogram(dirname,histname,
                1024,0,4096, energies_addback.at(i),
                1024,0,4096, energies_addback.at(j));
            obj.FillHistogram(dirname,histname,
                1024,0,4096, energies_addback.at(j),
                1024,0,4096, energies_addback.at(i));
          }

          if (n0_mult == 2){
            histname = "ab_energy_dc_coincidence_matrix_multtwo_n0";
            obj.FillHistogram(dirname,histname,
                1024,0,4096, energies_addback.at(i),
                1024,0,4096, energies_addback.at(j));
            obj.FillHistogram(dirname,histname,
                1024,0,4096, energies_addback.at(j),
                1024,0,4096, energies_addback.at(i));
          }
          if (n0_mult + n1_mult == 2){
            histname = "ab_energy_dc_coincidence_matrix_multtwo_n0n1";
            obj.FillHistogram(dirname,histname,
                1024,0,4096, energies_addback.at(i),
                1024,0,4096, energies_addback.at(j));
            obj.FillHistogram(dirname,histname,
                1024,0,4096, energies_addback.at(j),
                1024,0,4096, energies_addback.at(i));
          }
        }
      }
      if (n0_mult + n1_mult + n2_mult == 2){
        histname = "ab_energy_dc_multtwo_nogarbage";
        obj.FillHistogram(dirname,histname,
                          2048,0,8192,energies_addback.at(0));
        obj.FillHistogram(dirname,histname,
                          2048,0,8192,energies_addback.at(1));
        if (n2_mult ==0){
          if (n1_mult == 0){
            histname = "ab_energy_dc_multtwo_n0";
            obj.FillHistogram(dirname,histname,
                              2048,0,8192,energies_addback.at(0));
            obj.FillHistogram(dirname,histname,
                              2048,0,8192,energies_addback.at(1));
          }//pure n0
          else{
            histname = "ab_energy_dc_multtwo_n1";
            obj.FillHistogram(dirname,histname,
                              2048,0,8192,energies_addback.at(0));
            obj.FillHistogram(dirname,histname,
                              2048,0,8192,energies_addback.at(1));
          }
        }//no n2 events
        else{
            histname = "ab_energy_dc_multtwo_n2";
            obj.FillHistogram(dirname,histname,
                              2048,0,8192,energies_addback.at(0));
            obj.FillHistogram(dirname,histname,
                              2048,0,8192,energies_addback.at(1));
        }
      }
    }//addback_mult == 2
    for (int i = 0; i < addback_mult; i++){
      for (int j = i+1; j < addback_mult; j++){
        dirname = "CaesarAddback";
        histname = "ab_energy_dc_coincidence_matrix";
        obj.FillHistogram(dirname, histname,
                          1024,0,4096, energies_addback.at(i),
                          1024,0,4096, energies_addback.at(j));
        obj.FillHistogram(dirname, histname,
                          1024,0,4096, energies_addback.at(j),
                          1024,0,4096, energies_addback.at(i));
        if (ng_mult == 0){
          histname = "ab_energy_dc_coincidence_matrix_nogarbage";
          obj.FillHistogram(dirname, histname,
              1024,0,4096, energies_addback.at(i),
              1024,0,4096, energies_addback.at(j));
          obj.FillHistogram(dirname, histname,
              1024,0,4096, energies_addback.at(j),
              1024,0,4096, energies_addback.at(i));
        }
      }
    }

    dirname = "CaesarAddback";
    histname = "ab_multiplicity";
    obj.FillHistogram(dirname, histname,
                      192, 0, 192, addback_mult);
  }//caesar exists

  if(caesar) { caesar->Clear(); }
  if(caesar_ab) { caesar_ab->Clear(); }

  if(numobj!=list->GetSize())
    list->Sort();
}
