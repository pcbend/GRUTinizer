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

TFile *cut_file  = 0;


const int total_det_in_prev_rings[N_RINGS] = {0,10,24,48,72,96,120,144,168,182};
const double START_ANGLE = 3.2;
const double FINAL_ANGLE = 3.2;
const double ANGLE_STEPS = 0.1;
const int ENERGY_THRESHOLD = 300;
const int TOTAL_ANGLES = (FINAL_ANGLE-START_ANGLE)/ANGLE_STEPS + 1;

std::vector<double> angles;


bool init_called = false;

void Init() {
  double temp_angle = START_ANGLE;
  for (int i = 0; i < TOTAL_ANGLES; i++){
    angles.push_back(temp_angle);
    temp_angle += ANGLE_STEPS;
  }
  init_called = true;
}



// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TCaesar  *caesar  = obj.GetDetector<TCaesar>();
  TS800    *s800    = obj.GetDetector<TS800>();
  double    beta = GValue::Value("BETA");
  double    z_shift = GValue::Value("TARGET_SHIFT_Z");

  static TCutG *timingcut = 0;
  if(!timingcut) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/timingcut.root");
    timingcut = (TCutG*)fcut.Get("tcut");
  }
  static TCutG *InBeam_Mid = 0;
  if(!InBeam_Mid) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/newmid.root");
    InBeam_Mid = (TCutG*)fcut.Get("InBeam_Mid");
  }
  static TCutG *InBeam_Top = 0;
  if(!InBeam_Top) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/newtop.root");
    InBeam_Top = (TCutG*)fcut.Get("Inbeam_top");
  }
  static TCutG *InBeam_btwnTopMid = 0;
  if(!InBeam_btwnTopMid) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/newbetween.root");
    InBeam_btwnTopMid = (TCutG*)fcut.Get("between");
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
  static TCutG *newmg22blob = 0;
  if(!newmg22blob) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/newmg22blob.root");
    newmg22blob = (TCutG*)fcut.Get("newmg22blob");
  }

  // Stuff for interactive gates!
  TList *gates = &(obj.GetGates());
  bool haspids = gates->GetSize();


  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();
  if(caesar) {
    if(!init_called)
      Init();
    int valid_counter=0;
    int ng_newal23blob=0;
    int ng_newsi24blob=0;
    int ng_newmg22blob=0;

    for(unsigned int k=0;k<caesar->Size();k++) {
      TCaesarHit hit_m = caesar->GetCaesarHit(k);
        if(!hit_m.IsValid())
          continue;

        if(!s800)
          continue;

        double ic_sum_m = s800->GetIonChamber().GetAve();
	double objtac_corr_m = s800->GetCorrTOF_OBJTAC();
	double objtac_m = s800->GetTof().GetTacOBJ();
	double xfptac_m = s800->GetTof().GetTacXFP();
        TVector3 track_m = s800->Track();
        double corr_time_m = caesar->GetCorrTime(hit_m,s800);

        if(newal23blob->IsInside(objtac_corr_m,ic_sum_m) && InBeam_btwnTopMid->IsInside(objtac_m,xfptac_m) && 		       		   timingcut->IsInside(corr_time_m,hit_m.GetDoppler(beta,z_shift,&track_m)))
          ng_newal23blob++;

        if(newsi24blob->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Top->IsInside(objtac_m,xfptac_m) &&                         	          timingcut->IsInside(corr_time_m,hit_m.GetDoppler(beta,z_shift,&track_m)))
          ng_newsi24blob++;

        if(newmg22blob->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Mid->IsInside(objtac_m,xfptac_m) &&                         	      	   timingcut->IsInside(corr_time_m,hit_m.GetDoppler(beta,z_shift,&track_m)))
          ng_newmg22blob++;

    }//end for loop to get multiplicity of caesar event

    for(unsigned int i=0;i<caesar->Size();i++) {
      TCaesarHit hit = caesar->GetCaesarHit(i);
      if(!hit.IsValid())
        continue;
      valid_counter++;

      int ring = hit.GetRingNumber();

      obj.FillHistogram("Caesar","CAESAR_raw_time", 4096,0,4096,hit.Time());
      obj.FillHistogram("Caesar","GetTime_vs_GetDoppler",2000,0,2000,hit.GetTime(),
                                                         2048,0,8192,hit.GetDoppler(beta,z_shift));
      if(hit.GetDoppler(beta,z_shift)>300){
        obj.FillHistogram("Caesar","GetTime_vs_GetAbsoluteDetectorNumber",200,0,200,hit.GetAbsoluteDetectorNumber(),
                                                                          2000,0,2000,hit.GetTime());
      }//end if Doppler > 300

      obj.FillHistogram("Caesar","Detector_Charge_Summary",300,0,300,hit.GetDetectorNumber()+total_det_in_prev_rings[ring],
                                                  4096,0,4096,hit.Charge());

      obj.FillHistogram("Caesar","Detector_Energy_Summary",300,0,300,hit.GetDetectorNumber()+total_det_in_prev_rings[ring],
                                                  2048,0,8192,hit.GetEnergy());

      obj.FillHistogram("Caesar","Detector_Doppler_Summary",300,0,300,hit.GetDetectorNumber()+total_det_in_prev_rings[ring],
                                                  2048,0,8192,hit.GetDoppler(beta,z_shift));

      for(unsigned int j=0;j<caesar->Size();j++) {
        if(i==j)
          continue;
        TCaesarHit hit2 = caesar->GetCaesarHit(j);
      if(!hit2.IsValid())
        continue;
        obj.FillHistogram("Caesar","Energy_Coincidence_Matrix",2048,0,8192,hit.GetEnergy(),
                                                             2048,0,8192,hit2.GetEnergy());

      }//end for loop over coincidence hits

      if(!s800) {
       continue;
      }

      /**************************************\
      | Lets think about the location of this|
      | if(s800) that is inside our loop over|
      | CAESAR Hits!!!!                      |
      \**************************************/

      if(s800) {

	double ic_sum = s800->GetIonChamber().GetAve();
	double objtac_corr = s800->GetCorrTOF_OBJTAC();
	double objtac = s800->GetTof().GetTacOBJ();
	double xfptac = s800->GetTof().GetTacXFP();
	double afp = s800->GetAFP();
	double xfp_focalplane = s800->GetXFP(0);
        TVector3 track = s800->Track();
        double corr_time = caesar->GetCorrTime(hit,s800);
        obj.FillHistogram("Caesar","GetCorrTime_vs_GetDoppler",4000,-2000,2000,corr_time,
                                                               2048,0,8192,hit.GetDoppler(beta,z_shift,&track));



        if(timingcut->IsInside(corr_time,hit.GetDoppler(beta,z_shift,&track))){

          std::string dirname  = "PID";

          std::string histname = "IncomingPID";
	  obj.FillHistogram(dirname,histname,
			    1000,-1000,5000,objtac,
			    1000,-1000,5000,xfptac);

	  histname = "AFP_vs_OBJTOF";
	  obj.FillHistogram(dirname,histname,
	  		    1000,-500,2500,objtac_corr,
			    1000,-1,1,afp); // check units of AFP

	  histname = "XFP_vs_OBJTOF";
	  obj.FillHistogram(dirname,histname,
			    1000,-500,2500,objtac_corr,
			    1000,-300,300,xfp_focalplane);

	  histname = "IC_vs_OBJTOF_PID";
	  obj.FillHistogram(dirname,histname,
			    1000,-500,2500,objtac_corr,
			    1000,-100,4000,ic_sum);

	  if(InBeam_Mid->IsInside(objtac,xfptac)){
	    histname = "IC_vs_OBJTOF_PID_InBeam_Mid";
	    obj.FillHistogram(dirname,histname,
			      1000,-500,2500,objtac_corr,
			      1000,-100,4000,ic_sum);
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Mid",4000,-2000,2000,corr_time,
                                                               2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if in mid cut

          if(InBeam_Top->IsInside(objtac,xfptac)){
	    histname = "IC_vs_OBJTOF_PID_InBeam_Top";
	    obj.FillHistogram(dirname,histname,
			      1000,-500,2500,objtac_corr,
			      1000,-100,4000,ic_sum);
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Top",4000,-2000,2000,corr_time,
                                                               2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if in top cut

          if(InBeam_btwnTopMid->IsInside(objtac,xfptac)){
	    histname = "IC_vs_OBJTOF_PID_InBeam_btwnTopMid";
	    obj.FillHistogram(dirname,histname,
			      1000,-500,2500,objtac_corr,
			      1000,-100,4000,ic_sum);
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Btwn",4000,-2000,2000,corr_time,
                                                            2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if in between cut


          if(al23blob->IsInside(objtac_corr,ic_sum) && InBeam_btwnTopMid->IsInside(objtac,xfptac)){

	    histname = "Gamma_Gated_al23blob";
	    obj.FillHistogram("GATED",histname,
			      1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
            obj.FillHistogram("s800","CRDC1Y_Gated_al23blob",
			      10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    obj.FillHistogram("s800","CRDC2Y_Gated_al23blob",
			      10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());

            obj.FillHistogram("s800","CRDC1X_Gated_al23blob",
			      800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    obj.FillHistogram("s800","CRDC2X_Gated_al23blob",
			      800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            dirname = "InverseMap_GATED";

            histname = "S800_YTA_Gated_al23blob";
            obj.FillHistogram(dirname,histname,
			      1000,-50,50,s800->GetYta());

            histname = "S800_DTA_Gated_al23blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetDta());

            histname = "ATA_vs_BTA_Gated_al23blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetAta(),
			      1000,-0.2,0.2,s800->GetBta());

            histname = "FindBeta_al23blob";
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
	      }//for loop over beta_i
	  }//end if inside al23blob and inside between incomingPID

          if(si24blob->IsInside(objtac_corr,ic_sum) && InBeam_Top->IsInside(objtac,xfptac)){
	    histname = "Gamma_Gated_si24blob";
	    obj.FillHistogram("GATED",histname,
			      1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
            obj.FillHistogram("s800","CRDC1Y_Gated_si24blob",10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    obj.FillHistogram("s800","CRDC2Y_Gated_si24blob",10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
            obj.FillHistogram("s800","CRDC1X_Gated_si24blob",800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    obj.FillHistogram("s800","CRDC2X_Gated_si24blob",800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            dirname = "InverseMap_GATED";

            histname = "S800_YTA_Gated_si24blob";
            obj.FillHistogram(dirname,histname,
			      1000,-50,50,s800->GetYta());

            histname = "S800_DTA_Gated_si24blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetDta());

            histname = "ATA_vs_BTA_Gated_si24blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetAta(),
			      1000,-0.2,0.2,s800->GetBta());

            histname = "FindBeta_si24blob";
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
	      }//for loop over beta_i

	  }//end if inside si24blob and inside top incomingPID

          if(newal23blob->IsInside(objtac_corr,ic_sum) && InBeam_btwnTopMid->IsInside(objtac,xfptac)){

	    histname = "Gamma_Gated_newal23blob";
	    obj.FillHistogram("GATED",histname,
			      1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
            obj.FillHistogram("s800","CRDC1Y_Gated_newal23blob",
			      10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    obj.FillHistogram("s800","CRDC2Y_Gated_newal23blob",
			      10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());

            obj.FillHistogram("s800","CRDC1X_Gated_newal23blob",
			      800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    obj.FillHistogram("s800","CRDC2X_Gated_newal23blob",
			      800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            dirname = "InverseMap_GATED";

            histname = "S800_YTA_Gated_newal23blob";
            obj.FillHistogram(dirname,histname,
			      1000,-50,50,s800->GetYta());

            histname = "S800_DTA_Gated_newal23blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetDta());

            histname = "ATA_vs_BTA_Gated_newal23blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetAta(),
			      1000,-0.2,0.2,s800->GetBta());

            histname = "FindBeta_newal23blob";
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));

                if(ng_newal23blob==1){
                   obj.FillHistogram("GATED","FindBeta_newal23blob_Mult1",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_newal23blob==2){
                   obj.FillHistogram("GATED","FindBeta_newal23blob_Mult2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_newal23blob>2){
                   obj.FillHistogram("GATED","FindBeta_newal23blob_Mult>2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2
	      }//for loop over beta_i

            if(hit.GetDoppler(beta,z_shift,&track)>300){
              obj.FillHistogram("Caesar_GATED","GetTime_vs_GetAbsoluteDetectorNumber_Gated_newal23blob",
                                200,0,200,hit.GetAbsoluteDetectorNumber(),
                                2000,0,2000,hit.GetTime());
            }
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Btwn_newal23blob",
                              4000,-2000,2000,corr_time,
                              2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if inside newal23blob and inside between incomingPID

          if(newsi24blob->IsInside(objtac_corr,ic_sum) && InBeam_Top->IsInside(objtac,xfptac)){
	    histname = "Gamma_Gated_newsi24blob";
	    obj.FillHistogram("GATED",histname,
			      1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
            obj.FillHistogram("s800","CRDC1Y_Gated_newsi24blob",10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    obj.FillHistogram("s800","CRDC2Y_Gated_newsi24blob",10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
            obj.FillHistogram("s800","CRDC1X_Gated_newsi24blob",800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    obj.FillHistogram("s800","CRDC2X_Gated_newsi24blob",800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            dirname = "InverseMap_GATED";

            histname = "S800_YTA_Gated_newsi24blob";
            obj.FillHistogram(dirname,histname,
			      1000,-50,50,s800->GetYta());

            histname = "S800_DTA_Gated_newsi24blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetDta());

            histname = "ATA_vs_BTA_Gated_newsi24blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetAta(),
			      1000,-0.2,0.2,s800->GetBta());

            histname = "FindBeta_newsi24blob";
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));

                if(ng_newsi24blob==1){
                   obj.FillHistogram("GATED","FindBeta_newsi24blob_Mult1",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_newsi24blob==2){
                   obj.FillHistogram("GATED","FindBeta_newsi24blob_Mult2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_newsi24blob>2){
                   obj.FillHistogram("GATED","FindBeta_newsi24blob_Mult>2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2
	      }//for loop over beta_i

            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Top_newsi24blob",
                              4000,-2000,2000,corr_time,
                              2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if inside newsi24blob and inside top incomingPID

          if(newmg22blob->IsInside(objtac_corr,ic_sum) && InBeam_Mid->IsInside(objtac,xfptac)){
	    histname = "Gamma_Gated_newmg22blob";
	    obj.FillHistogram("GATED",histname,
			      1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
            obj.FillHistogram("s800","CRDC1Y_Gated_newmg22blob",10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    obj.FillHistogram("s800","CRDC2Y_Gated_newmg22blob",10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
            obj.FillHistogram("s800","CRDC1X_Gated_newmg22blob",800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    obj.FillHistogram("s800","CRDC2X_Gated_newmg22blob",800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            dirname = "InverseMap_GATED";

            histname = "S800_YTA_Gated_newmg22blob";
            obj.FillHistogram(dirname,histname,
			      1000,-50,50,s800->GetYta());

            histname = "S800_DTA_Gated_newmg22blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetDta());

            histname = "ATA_vs_BTA_Gated_newmg22blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetAta(),
			      1000,-0.2,0.2,s800->GetBta());

            histname = "FindBeta_newmg22blob";
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));

                if(ng_newmg22blob==1){
                   obj.FillHistogram("GATED","FindBeta_newmg22blob_Mult1",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_newmg22blob==2){
                   obj.FillHistogram("GATED","FindBeta_newmg22blob_Mult2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_newmg22blob>2){
                   obj.FillHistogram("GATED","FindBeta_newmg22blob_Mult>2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2
	      }//for loop over beta_i

            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Mid_newmg22blob",4000,-2000,2000,corr_time,
                                                            2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if inside newmg22blob and inside mid incomingPID

	  obj.FillHistogram("s800","CRDC1Y",10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	  obj.FillHistogram("s800","CRDC2Y",10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
          obj.FillHistogram("s800","CRDC1X",800,-400,400,s800->GetCrdc(0).GetDispersiveX());
          obj.FillHistogram("s800","CRDC2X",800,-400,400,s800->GetCrdc(1).GetDispersiveX());

	  obj.FillHistogram("s800","TrigBit",5,0,5,s800->GetTrigger().GetRegistr());

          dirname = "InverseMap";

          histname = "S800_YTA";
          obj.FillHistogram(dirname,histname,
			    1000,-50,50,s800->GetYta());

          histname = "S800_DTA";
          obj.FillHistogram(dirname,histname,
			    1000,-0.2,0.2,s800->GetDta());

          histname = "ATA_vs_BTA";
          obj.FillHistogram(dirname,histname,
			    1000,-0.2,0.2,s800->GetAta(),
			    1000,-0.2,0.2,s800->GetBta());


	  if(haspids) {
	    dirname = "GATED";
	    TIter it(gates);
	    while(TObject *itobj = it.Next()) {
	      if(!itobj->InheritsFrom(TCutG::Class()))
	        continue;
	      TCutG *mygate = (TCutG*)itobj;

	      if(mygate->IsInside(objtac_corr,ic_sum)){

	        histname = Form("FindBeta_%s",mygate->GetName());
	        for(int beta_i=0;beta_i<300;beta_i++){
		  double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		  obj.FillHistogram(dirname,histname,
				    300,0.2,0.5,beta_use,
				    1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
	        }

	        histname = Form("Gamma_%s",mygate->GetName());
	        obj.FillHistogram(dirname,histname,
				  1024,0,8192,hit.GetDoppler(beta,z_shift,&track));


                histname = Form("CRDC1Y_Gated_%s",mygate->GetName());
                obj.FillHistogram("s800",histname,
                                  10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());

                histname = Form("CRDC2Y_Gated_%s",mygate->GetName());
	        obj.FillHistogram("s800",histname,
                                  10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());


	      }// end IsInside(ResiduePID)
	    }// end iterating over gates
	  }//end haspids

        }// end of if inside timingcut
      }// end of if(s800)


    }// end of caesar for hit loop
    obj.FillHistogram("Caesar","Multiplicity",300,0,300,valid_counter);
    obj.FillHistogram("Caesar","Multiplicity_ng_al23blob",300,0,300,ng_newal23blob);
    obj.FillHistogram("Caesar","Multiplicity_ng_si24blob",300,0,300,ng_newsi24blob);
    obj.FillHistogram("Caesar","Multiplicity_ng_mg22blob",300,0,300,ng_newmg22blob);
  } // I have a valid caesar event

  if(numobj!=list->GetSize())
    list->Sort();

}//end MakeHistograms
