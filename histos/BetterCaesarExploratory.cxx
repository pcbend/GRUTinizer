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


const int total_det_in_prev_rings[N_RINGS] = {0,10,24,48,72,96,120,144,168,182};
const double START_ANGLE = 3.2;
const double FINAL_ANGLE = 3.2;
const double ANGLE_STEPS = 0.1;
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
  double    beta = 0.407;
  double    beta_si25 = 0.431;
  double    beta_p26 = 0.441;
  double    z_shift = 0.65;

  static TCutG *timingcut_si25 = 0;
  if(!timingcut_si25) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/timeenergy_si25tighter.root");
    timingcut_si25 = (TCutG*)fcut.Get("si25timegatetighter");
  }
  static TCutG *timingcut_p26 = 0;
  if(!timingcut_p26) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/timeenergy_p26.root");
    timingcut_p26 = (TCutG*)fcut.Get("p26timegate");
  }
  static TCutG *InBeam_Mid = 0;
  if(!InBeam_Mid) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/newmid.root");
    InBeam_Mid = (TCutG*)fcut.Get("InBeam_Mid");
  }
  static TCutG *InBeam_Top = 0;
  if(!InBeam_Top) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/newtop.root");
    InBeam_Top = (TCutG*)fcut.Get("Inbeam_top");
  }
  static TCutG *InBeam_btwnTopMid = 0;
  if(!InBeam_btwnTopMid) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/newbetween.root");
    InBeam_btwnTopMid = (TCutG*)fcut.Get("between");
  }
  static TCutG *InBeam_Above = 0;
  if(!InBeam_Above) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/newabove.root");
    InBeam_Above = (TCutG*)fcut.Get("Inbeam_above");
  }
  static TCutG *ne17blobtop = 0;
  if(!ne17blobtop) {
     TPreserveGDirectory Preserve;
     TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/ne17blobtop.root");
     ne17blobtop = (TCutG*)fcut.Get("ne17top");
  }
  static TCutG *ne17blobbetween = 0;
  if(!ne17blobbetween) {
     TPreserveGDirectory Preserve;
     TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/ne17blobbetween.root");
     ne17blobbetween = (TCutG*)fcut.Get("ne17between");
  }
  static TCutG *mg20blobtop = 0;
  if(!mg20blobtop) {
     TPreserveGDirectory Preserve;
     TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/mg20blobtop.root");
     mg20blobtop = (TCutG*)fcut.Get("mg20top");
  }
  static TCutG *mg21blobtop = 0;
  if(!mg21blobtop) {
     TPreserveGDirectory Preserve;
     TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/mg21blobtop.root");
     mg21blobtop = (TCutG*)fcut.Get("mg21top");
  }
  static TCutG *mg21blobbtwn = 0;
  if(!mg21blobbtwn) {
     TPreserveGDirectory Preserve;
     TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/mg21blobbtwn.root");
     mg21blobbtwn = (TCutG*)fcut.Get("mg21btwn");
  }
  static TCutG *al24blobbetween = 0;
  if(!al24blobbetween) {
     TPreserveGDirectory Preserve;
     TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/al24blobbetween.root");
     al24blobbetween = (TCutG*)fcut.Get("al24between");
  }
  static TCutG *p26blobtop = 0;
  if(!p26blobtop) {
     TPreserveGDirectory Preserve;
     TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/p26blobtop.root");
     p26blobtop = (TCutG*)fcut.Get("p26top");
  }
  static TCutG *p26blobabove = 0;
  if(!p26blobabove) {
     TPreserveGDirectory Preserve;
     TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/p26blobabove.root");
     p26blobabove = (TCutG*)fcut.Get("p26above");
  }
  static TCutG *si25blobtop = 0;
  if(!si25blobtop) {
     TPreserveGDirectory Preserve;
     TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/Old_Cuts/si25blobtop2.root");
     si25blobtop = (TCutG*)fcut.Get("si25top");
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
    int ng_ne17blobtop=0;
    int ng_ne17blobbetween=0;
    int ng_mg20blobtop=0;
    int ng_mg21blobtop=0;
    int ng_mg21blobbtwn=0;
    int ng_al24blobbetween=0;
    int ng_p26blobtop=0;
    int ng_p26blobabove=0;
    int ng_si25blobtop=0;

    for(unsigned int k=0;k<caesar->Size();k++) {
      TCaesarHit hit_m = caesar->GetCaesarHit(k);
        if(!hit_m.IsValid())
          continue;

        if(hit_m.IsOverflow())
          continue;

        if(!s800)
          continue;

        double ic_sum_m = s800->GetIonChamber().GetAve();
	double objtac_corr_m = s800->GetCorrTOF_OBJTAC();
	double objtac_m = s800->GetTof().GetTacOBJ();
	double xfptac_m = s800->GetTof().GetTacXFP();
        TVector3 track_m = s800->Track();
        double corr_time_m = caesar->GetCorrTime(hit_m,s800);

        //if(ne17blobtop->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Top->IsInside(objtac_m,xfptac_m) &&                         	          timingcut->IsInside(corr_time_m,hit_m.GetDoppler(beta,z_shift,&track_m)))
          //ng_ne17blobtop++;

        //if(ne17blobbetween->IsInside(objtac_corr_m,ic_sum_m) && InBeam_btwnTopMid->IsInside(objtac_m,xfptac_m) && 		    timingcut->IsInside(corr_time_m,hit_m.GetDoppler(beta,z_shift,&track_m)))
          //ng_ne17blobbetween++;
        
        //if(mg20blobtop->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Top->IsInside(objtac_m,xfptac_m) &&                         	          timingcut->IsInside(corr_time_m,hit_m.GetDoppler(beta,z_shift,&track_m)))
          //ng_mg20blobtop++;

        //if(al22blobtop->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Top->IsInside(objtac_m,xfptac_m) &&                         	          timingcut->IsInside(corr_time_m,hit_m.GetDoppler(beta,z_shift,&track_m)))
          //ng_al22blobtop++;

        //if(p26blobtop->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Top->IsInside(objtac_m,xfptac_m) &&                         	          timingcut->IsInside(corr_time_m,hit_m.GetDoppler(beta,z_shift,&track_m)))
          //ng_p26blobtop++;

        if(ne17blobtop->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Top->IsInside(objtac_m,xfptac_m))
          ng_ne17blobtop++;

        if(ne17blobbetween->IsInside(objtac_corr_m,ic_sum_m) && InBeam_btwnTopMid->IsInside(objtac_m,xfptac_m))
          ng_ne17blobbetween++;
        
        if(mg20blobtop->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Top->IsInside(objtac_m,xfptac_m))
          ng_mg20blobtop++;

        if(mg21blobtop->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Top->IsInside(objtac_m,xfptac_m))
          ng_mg21blobtop++;

        if(mg21blobbtwn->IsInside(objtac_corr_m,ic_sum_m) && InBeam_btwnTopMid->IsInside(objtac_m,xfptac_m))
          ng_mg21blobbtwn++;

        if(al24blobbetween->IsInside(objtac_corr_m,ic_sum_m) && InBeam_btwnTopMid->IsInside(objtac_m,xfptac_m))
          ng_al24blobbetween++;

        if(p26blobtop->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Top->IsInside(objtac_m,xfptac_m))
          ng_p26blobtop++;
 
        if(p26blobabove->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Above->IsInside(objtac_m,xfptac_m) && timingcut_p26->IsInside(corr_time_m,hit_m.GetEnergy()))
          ng_p26blobabove++;
       
        if(si25blobtop->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Top->IsInside(objtac_m,xfptac_m) && timingcut_si25->IsInside(corr_time_m,hit_m.GetEnergy()))
          ng_si25blobtop++;

    }//end for loop to get multiplicity of caesar event

    for(unsigned int i=0;i<caesar->Size();i++) {
      TCaesarHit hit = caesar->GetCaesarHit(i);
      if(!hit.IsValid())
        continue;
      if(hit.IsOverflow())
        continue;
      valid_counter++;

      //int ring = hit.GetRingNumber();

      //obj.FillHistogram("Caesar","CAESAR_raw_time", 4096,0,4096,hit.Time());
      obj.FillHistogram("Caesar","GetTime_vs_GetEnergy",2000,0,2000,hit.GetTime(),
                                                         1024,0,8192,hit.GetEnergy());
      //if(hit.GetDoppler(beta,z_shift)>300){
        //obj.FillHistogram("Caesar","GetTime_vs_GetAbsoluteDetectorNumber",200,0,200,hit.GetAbsoluteDetectorNumber(),
                                                                          //2000,0,2000,hit.GetTime());
      //}//end if Doppler > 300

      obj.FillHistogram("Caesar","Detector_Charge_Summary",200,0,200,hit.GetAbsoluteDetectorNumber(),
                                                  4096,0,4096,hit.Charge());

      obj.FillHistogram("Caesar","Detector_Energy_Summary",200,0,200,hit.GetAbsoluteDetectorNumber(),
                                                  2048,0,8192,hit.GetEnergy());

      //obj.FillHistogram("Caesar","Detector_Doppler_Summary",300,0,300,hit.GetDetectorNumber()+total_det_in_prev_rings[ring],
                                                  //2048,0,8192,hit.GetDoppler(beta,z_shift));

      //for(unsigned int j=0;j<caesar->Size();j++) {
        //if(i==j)
          //continue;
        //TCaesarHit hit2 = caesar->GetCaesarHit(j);
      //if(!hit2.IsValid())
        //continue;
      //if(hit2.IsOverflow())
        //continue;
        //obj.FillHistogram("Caesar","Energy_Coincidence_Matrix",2048,0,8192,hit.GetEnergy(),
                                                             //2048,0,8192,hit2.GetEnergy());

      //}//end for loop over coincidence hits

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
        obj.FillHistogram("Caesar","GetCorrTime_vs_GetDoppler",2000,-2000,2000,corr_time,
                                                               1024,0,8192,hit.GetDoppler(beta,z_shift));

        obj.FillHistogram("Caesar","GetCorrTime_vs_GetEnergy",2000,-2000,2000,corr_time,
                                                               1024,0,8192,hit.GetEnergy());
        
        //obj.FillHistogram("E1UpDown","E1 Up Times",10000,-5000,5000,s800->GetScint().GetTimeUp());       
        //obj.FillHistogram("E1UpDown","E1 Down Times",10000,-5000,5000,s800->GetScint().GetTimeDown());

        //if(timingcut->IsInside(corr_time,hit.GetDoppler(beta,z_shift,&track))){

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
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Mid",2000,-2000,2000,corr_time,
                                                               2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if in mid cut

          if(InBeam_Top->IsInside(objtac,xfptac)){
	    histname = "IC_vs_OBJTOF_PID_InBeam_Top";
	    obj.FillHistogram(dirname,histname,
			      1000,-500,2500,objtac_corr,
			      1000,-100,4000,ic_sum);
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Top",2000,-2000,2000,corr_time,
                                                               2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if in top cut

          if(InBeam_btwnTopMid->IsInside(objtac,xfptac)){
	    histname = "IC_vs_OBJTOF_PID_InBeam_btwnTopMid";
	    obj.FillHistogram(dirname,histname,
			      1000,-500,2500,objtac_corr,
			      1000,-100,4000,ic_sum);
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Btwn",2000,-2000,2000,corr_time,
                                                            2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if in between cut

          if(InBeam_Above->IsInside(objtac,xfptac)){
	    histname = "IC_vs_OBJTOF_PID_InBeam_Above";
	    obj.FillHistogram(dirname,histname,
			      1000,-500,2500,objtac_corr,
			      1000,-100,4000,ic_sum);
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Above",2000,-2000,2000,corr_time,
                                                            2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if in between cut

          if(ne17blobbetween->IsInside(objtac_corr,ic_sum) && InBeam_btwnTopMid->IsInside(objtac,xfptac)){

	    //histname = "Gamma_Gated_ne17blobbetween";
	    //obj.FillHistogram("GATED",histname,
			      //1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
            //obj.FillHistogram("s800","CRDC1Y_Gated_al23blob",
			      //10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    //obj.FillHistogram("s800","CRDC2Y_Gated_al23blob",
			      //10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());

            //obj.FillHistogram("s800","CRDC1X_Gated_al23blob",
			      //800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    //obj.FillHistogram("s800","CRDC2X_Gated_al23blob",
			      //800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            //dirname = "InverseMap_GATED";

            //histname = "S800_YTA_Gated_al23blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-50,50,s800->GetYta());

            //histname = "S800_DTA_Gated_al23blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetDta());

            //histname = "ATA_vs_BTA_Gated_al23blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetAta(),
			      //1000,-0.2,0.2,s800->GetBta());

            histname = "FindBeta_ne17blobbetween";
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));

                if(ng_ne17blobbetween==1){
                   obj.FillHistogram("GATED","FindBeta_ne17blobbetween_Mult1",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_ne17blobbetween==2){
                   obj.FillHistogram("GATED","FindBeta_ne17blobbetween_Mult2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_ne17blobbetween>2){
                   obj.FillHistogram("GATED","FindBeta_ne17blobbetween_Mult>2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2
	      }//for loop over beta_i

	  }//end if inside al23blob and inside between incomingPID

          if(ne17blobtop->IsInside(objtac_corr,ic_sum) && InBeam_Top->IsInside(objtac,xfptac)){
	    //histname = "Gamma_Gated_ne17blobtop";
	    //obj.FillHistogram("GATED",histname,
			      //1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
            //obj.FillHistogram("s800","CRDC1Y_Gated_si24blob",10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    //obj.FillHistogram("s800","CRDC2Y_Gated_si24blob",10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
            //obj.FillHistogram("s800","CRDC1X_Gated_si24blob",800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    //obj.FillHistogram("s800","CRDC2X_Gated_si24blob",800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            //dirname = "InverseMap_GATED";

            //histname = "S800_YTA_Gated_si24blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-50,50,s800->GetYta());

            //histname = "S800_DTA_Gated_si24blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetDta());

            //histname = "ATA_vs_BTA_Gated_si24blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetAta(),
			      //1000,-0.2,0.2,s800->GetBta());

            histname = "FindBeta_ne17blobtop";
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));

                if(ng_ne17blobtop==1){
                   obj.FillHistogram("GATED","FindBeta_ne17blobtop_Mult1",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_ne17blobtop==2){
                   obj.FillHistogram("GATED","FindBeta_ne17blobtop_Mult2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_ne17blobtop>2){
                   obj.FillHistogram("GATED","FindBeta_ne17blobtop_Mult>2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2
	      }//for loop over beta_i

	  }//end if inside si24blob and inside top incomingPID

          if(mg20blobtop->IsInside(objtac_corr,ic_sum) && InBeam_Top->IsInside(objtac,xfptac)){

	    //histname = "Gamma_Gated_mg20blobtop";
	    //obj.FillHistogram("GATED",histname,
			      //1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
            //obj.FillHistogram("s800","CRDC1Y_Gated_newal23blob",
			      //10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    //obj.FillHistogram("s800","CRDC2Y_Gated_newal23blob",
			      //10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());

            //obj.FillHistogram("s800","CRDC1X_Gated_newal23blob",
			      //800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    //obj.FillHistogram("s800","CRDC2X_Gated_newal23blob",
			      //800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            //dirname = "InverseMap_GATED";

            //histname = "S800_YTA_Gated_newal23blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-50,50,s800->GetYta());

            //histname = "S800_DTA_Gated_newal23blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetDta());

            //histname = "ATA_vs_BTA_Gated_newal23blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetAta(),
			      //1000,-0.2,0.2,s800->GetBta());

            histname = "FindBeta_mg20blobtop";
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));

                if(ng_mg20blobtop==1){
                   obj.FillHistogram("GATED","FindBeta_mg20blobtop_Mult1",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_mg20blobtop==2){
                   obj.FillHistogram("GATED","FindBeta_mg20blobtop_Mult2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_mg20blobtop>2){
                   obj.FillHistogram("GATED","FindBeta_mg20blobtop_Mult>2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2
	      }//for loop over beta_i

            //if(hit.GetDoppler(beta,z_shift,&track)>300){
              //obj.FillHistogram("Caesar_GATED","GetTime_vs_GetAbsoluteDetectorNumber_Gated_newal23blob",
                                //200,0,200,hit.GetAbsoluteDetectorNumber(),
                                //2000,0,2000,hit.GetTime());
            //}
            //obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Btwn_newal23blob",
                              //4000,-2000,2000,corr_time,
                              //2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if inside newal23blob and inside between incomingPID

          if(mg21blobtop->IsInside(objtac_corr,ic_sum) && InBeam_Top->IsInside(objtac,xfptac)){
            histname = "FindBeta_mg21blobtop";
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));

                if(ng_mg21blobtop==1){
                   obj.FillHistogram("GATED","FindBeta_mg21blobtop_Mult1",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_mg21blobtop==2){
                   obj.FillHistogram("GATED","FindBeta_mg21blobtop_Mult2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_mg21blobtop>2){
                   obj.FillHistogram("GATED","FindBeta_mg21blobtop_Mult>2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2
	      }//for loop over beta_i

	  }//end if inside newal23blob and inside between incomingPID

          if(mg21blobbtwn->IsInside(objtac_corr,ic_sum) && InBeam_btwnTopMid->IsInside(objtac,xfptac)){
            histname = "FindBeta_mg21blobbtwn";
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));

                if(ng_mg21blobbtwn==1){
                   obj.FillHistogram("GATED","FindBeta_mg21blobbtwn_Mult1",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_mg21blobbtwn==2){
                   obj.FillHistogram("GATED","FindBeta_mg21blobbtwn_Mult2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_mg21blobbtwn>2){
                   obj.FillHistogram("GATED","FindBeta_mg21blobbtwn_Mult>2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2
	      }//for loop over beta_i

	  }//end if inside newal23blob and inside between incomingPID

          if(al24blobbetween->IsInside(objtac_corr,ic_sum) && InBeam_btwnTopMid->IsInside(objtac,xfptac)){
	    //histname = "Gamma_Gated_al22blobtop";
	    //obj.FillHistogram("GATED",histname,
			      //1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
            //obj.FillHistogram("s800","CRDC1Y_Gated_newsi24blob",10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    //obj.FillHistogram("s800","CRDC2Y_Gated_newsi24blob",10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
            //obj.FillHistogram("s800","CRDC1X_Gated_newsi24blob",800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    //obj.FillHistogram("s800","CRDC2X_Gated_newsi24blob",800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            //dirname = "InverseMap_GATED";

            //histname = "S800_YTA_Gated_newsi24blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-50,50,s800->GetYta());

            //histname = "S800_DTA_Gated_newsi24blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetDta());

            //histname = "ATA_vs_BTA_Gated_newsi24blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetAta(),
			      //1000,-0.2,0.2,s800->GetBta());

            histname = "FindBeta_al24blobbetween";
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));

                if(ng_al24blobbetween==1){
                   obj.FillHistogram("GATED","FindBeta_al24blobbetween_Mult1",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_al24blobbetween==2){
                   obj.FillHistogram("GATED","FindBeta_al24blobbetween_Mult2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_al24blobbetween>2){
                   obj.FillHistogram("GATED","FindBeta_al24blobbetween_Mult>2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2
	      }//for loop over beta_i

            //obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Top_newsi24blob",
                              //4000,-2000,2000,corr_time,
                              //2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if inside newsi24blob and inside top incomingPID

          if(p26blobtop->IsInside(objtac_corr,ic_sum) && InBeam_Top->IsInside(objtac,xfptac) && timingcut_p26->IsInside(corr_time,hit.GetEnergy())){
	    //histname = "Gamma_Gated_newmg22blob";
	    //obj.FillHistogram("GATED",histname,
			      //1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
            //obj.FillHistogram("s800","CRDC1Y_Gated_newmg22blob",10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    //obj.FillHistogram("s800","CRDC2Y_Gated_newmg22blob",10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
            //obj.FillHistogram("s800","CRDC1X_Gated_newmg22blob",800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    //obj.FillHistogram("s800","CRDC2X_Gated_newmg22blob",800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            //dirname = "InverseMap_GATED";

            //histname = "S800_YTA_Gated_newmg22blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-50,50,s800->GetYta());

            //histname = "S800_DTA_Gated_newmg22blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetDta());

            //histname = "ATA_vs_BTA_Gated_newmg22blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetAta(),
			      //1000,-0.2,0.2,s800->GetBta());

            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_p26blobtop",2000,-2000,2000,corr_time,
                                                                   1024,0,8192,hit.GetDoppler(beta,z_shift));

            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetEnergy_p26blobtop",2000,-2000,2000,corr_time,
                                                                  1024,0,8192,hit.GetEnergy());

            histname = "Gamma_Gated_p26blobtop";
            obj.FillHistogram("GATED",histname,
                              200,0,200,hit.GetAbsoluteDetectorNumber(),
			      1024,0,8192,hit.GetDoppler(beta_p26,z_shift,&track));

            //histname = "Gamma_Gated_p26blobtop_0Beta";
            //obj.FillHistogram("GATED",histname,
                              //200,0,200,hit.GetAbsoluteDetectorNumber(),
                              //1024,0,8192,hit.GetEnergy());

            if(ng_p26blobtop==1){
                histname = "Gamma_Gated_p26blobtop_Mult1";
                obj.FillHistogram("GATED",histname,
                                   200,0,200,hit.GetAbsoluteDetectorNumber(),
		  	           1024,0,8192,hit.GetDoppler(beta_p26,z_shift,&track));
                //histname = "Gamma_Gated_p26blobtop_0Beta_Mult1";
                //obj.FillHistogram("GATED",histname,
                                   //200,0,200,hit.GetAbsoluteDetectorNumber(),
                                   //1024,0,8192,hit.GetEnergy());
            }

            if(ng_p26blobtop==2){
                histname = "Gamma_Gated_p26blobtop_Mult2";
                obj.FillHistogram("GATED",histname,
                                   200,0,200,hit.GetAbsoluteDetectorNumber(),
		  	           1024,0,8192,hit.GetDoppler(beta_p26,z_shift,&track));
                //histname = "Gamma_Gated_p26blobtop_0Beta_Mult2";
                //obj.FillHistogram("GATED",histname,
                                   //200,0,200,hit.GetAbsoluteDetectorNumber(),
                                   //1024,0,8192,hit.GetEnergy());
            }

            histname = "FindBeta_p26blobtop";
	      for(int beta_i=0;beta_i<500;beta_i++){
		double beta_use = 0.001*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                if(ng_p26blobtop==1){
                   obj.FillHistogram("GATED","FindBeta_p26blobtop_Mult1",
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_p26blobtop==2){
                   obj.FillHistogram("GATED","FindBeta_p26blobtop_Mult2",
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_p26blobtop>2){
                   obj.FillHistogram("GATED","FindBeta_p26blobtop_Mult>2",
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2
	      }//for loop over beta_i

            //obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Mid_newmg22blob",4000,-2000,2000,corr_time,
                                                            //2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if inside newmg22blob and inside mid incomingPID

        if(p26blobabove->IsInside(objtac_corr,ic_sum) && InBeam_Above->IsInside(objtac,xfptac)){

          obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_p26blobabove",2000,-2000,2000,corr_time,
                                                                   1024,0,8192,hit.GetDoppler(beta,z_shift));

          obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetEnergy_p26blobabove",2000,-2000,2000,corr_time,
                                                                  1024,0,8192,hit.GetEnergy());

          histname = "Gamma_Gated_p26blobabove";
          obj.FillHistogram("GATED",histname,
                            200,0,200,hit.GetAbsoluteDetectorNumber(),
			    1024,0,8192,hit.GetDoppler(beta_p26,z_shift,&track));

          //histname = "Gamma_Gated_p26blobabove_0Beta";
          //obj.FillHistogram("GATED",histname,
                            //200,0,200,hit.GetAbsoluteDetectorNumber(),
                            //1024,0,8192,hit.GetEnergy());

          if(ng_p26blobabove==1){
              histname = "Gamma_Gated_p26blobabove_Mult1";
              obj.FillHistogram("GATED",histname,
                                 200,0,200,hit.GetAbsoluteDetectorNumber(),
			         1024,0,8192,hit.GetDoppler(beta_p26,z_shift,&track));
              //histname = "Gamma_Gated_p26blobabove_0Beta_Mult1";
              //obj.FillHistogram("GATED",histname,
                                 //200,0,200,hit.GetAbsoluteDetectorNumber(),
                                 //1024,0,8192,hit.GetEnergy());
          }

          if(ng_p26blobabove==2){
              histname = "Gamma_Gated_p26blobabove_Mult2";
              obj.FillHistogram("GATED",histname,
                                 200,0,200,hit.GetAbsoluteDetectorNumber(),
			         1024,0,8192,hit.GetDoppler(beta_p26,z_shift,&track));
              //histname = "Gamma_Gated_p26blobabove_0Beta_Mult2";
              //obj.FillHistogram("GATED",histname,
                                 //200,0,200,hit.GetAbsoluteDetectorNumber(),
                                 //1024,0,8192,hit.GetEnergy());
          }   

          histname = "FindBeta_p26blobabove";
	      for(int beta_i=0;beta_i<500;beta_i++){
		double beta_use = 0.001*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));             	
                if(ng_p26blobabove==1){
                   obj.FillHistogram("GATED","FindBeta_p26blobabove_Mult1",
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_p26blobabove==2){
                   obj.FillHistogram("GATED","FindBeta_p26blobabove_Mult2",
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_p26blobabove>2){
                   obj.FillHistogram("GATED","FindBeta_p26blobabove_Mult>2",
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2
	      }//for loop over beta_i

            //obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Mid_newmg22blob",4000,-2000,2000,corr_time,
                                                            //2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if inside newmg22blob and inside mid incomingPID

          if(si25blobtop->IsInside(objtac_corr,ic_sum) && InBeam_Top->IsInside(objtac,xfptac) && timingcut_si25->IsInside(corr_time,hit.GetEnergy())){
	    //histname = "Gamma_Gated_newmg23blob";
	    //obj.FillHistogram("GATED",histname,
			      //1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
            //obj.FillHistogram("s800","CRDC1Y_Gated_newmg22blob",10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    //obj.FillHistogram("s800","CRDC2Y_Gated_newmg22blob",10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
            //obj.FillHistogram("s800","CRDC1X_Gated_newmg22blob",800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    //obj.FillHistogram("s800","CRDC2X_Gated_newmg22blob",800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            //dirname = "InverseMap_GATED";
     
            //histname = "S800_YTA_Gated_newmg23blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-50,50,s800->GetYta());
      
            //histname = "S800_DTA_Gated_newmg23blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetDta());
      
            //histname = "ATA_vs_BTA_Gated_newmg23blob";
            //obj.FillHistogram(dirname,histname,
			      //1000,-0.2,0.2,s800->GetAta(),
			      //1000,-0.2,0.2,s800->GetBta());

            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_si25blobtop",2000,-2000,2000,corr_time,
                                                                   1024,0,8192,hit.GetDoppler(beta,z_shift));

            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetEnergy_si25blobtop",2000,-2000,2000,corr_time,
                                                                  1024,0,8192,hit.GetEnergy());

            histname = "Gamma_Gated_si25blobtop";
            obj.FillHistogram("GATED",histname,
                              200,0,200,hit.GetAbsoluteDetectorNumber(),
			      1024,0,8192,hit.GetDoppler(beta_si25,z_shift,&track));

            //histname = "Gamma_Gated_si25blobtop_0Beta";
            //obj.FillHistogram("GATED",histname,
                              //200,0,200,hit.GetAbsoluteDetectorNumber(),
                              //1024,0,8192,hit.GetEnergy());

            if(ng_si25blobtop==1){
                histname = "Gamma_Gated_si25blobtop_Mult1";
                obj.FillHistogram("GATED",histname,
                                   200,0,200,hit.GetAbsoluteDetectorNumber(),
		       	           1024,0,8192,hit.GetDoppler(beta_si25,z_shift,&track));
                //histname = "Gamma_Gated_si25blobtop_0Beta_Mult1";
                //obj.FillHistogram("GATED",histname,
                                   //200,0,200,hit.GetAbsoluteDetectorNumber(),
                                   //1024,0,8192,hit.GetEnergy());
            }  
     
            if(ng_si25blobtop==2){
                histname = "Gamma_Gated_si25blobtop_Mult2";
                obj.FillHistogram("GATED",histname,
                                   200,0,200,hit.GetAbsoluteDetectorNumber(),
		       	           1024,0,8192,hit.GetDoppler(beta_si25,z_shift,&track));
                //histname = "Gamma_Gated_si25blobtop_0Beta_Mult2";
                //obj.FillHistogram("GATED",histname,
                                   //200,0,200,hit.GetAbsoluteDetectorNumber(),
                                   //1024,0,8192,hit.GetEnergy());
            }  

            histname = "FindBeta_si25blobtop";
	      for(int beta_i=0;beta_i<500;beta_i++){
		double beta_use = 0.001*double(beta_i);
		obj.FillHistogram("GATED",histname,
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));	
                if(ng_si25blobtop==1){
                   obj.FillHistogram("GATED","FindBeta_si25blobtop_Mult1",
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));	
                }//if multiplicity 1
                if(ng_si25blobtop==2){
                   obj.FillHistogram("GATED","FindBeta_si25blobtop_Mult2",
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_si25blobtop>2){
                   obj.FillHistogram("GATED","FindBeta_si25blobtop_Mult>2",
				  500,0,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2	
	      }//for loop over beta_i

            //obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Mid_newmg23blob",4000,-2000,2000,corr_time,
                                                            //2048,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if inside newmg23blob and inside mid incomingPID
    
	  //obj.FillHistogram("s800","CRDC1Y",10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	  //obj.FillHistogram("s800","CRDC2Y",10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
          //obj.FillHistogram("s800","CRDC1X",800,-400,400,s800->GetCrdc(0).GetDispersiveX());
          //obj.FillHistogram("s800","CRDC2X",800,-400,400,s800->GetCrdc(1).GetDispersiveX());

	  //obj.FillHistogram("s800","TrigBit",30,0,30,s800->GetTrigger().GetRegistr());

          //dirname = "InverseMap";

          //histname = "S800_YTA";
          //obj.FillHistogram(dirname,histname,
			    //1000,-50,50,s800->GetYta());

          //histname = "S800_DTA";
          //obj.FillHistogram(dirname,histname,
			    //1000,-0.2,0.2,s800->GetDta());

          //histname = "ATA_vs_BTA";
          //obj.FillHistogram(dirname,histname,
			    //1000,-0.2,0.2,s800->GetAta(),
			    //1000,-0.2,0.2,s800->GetBta());

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
                                  5000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());

                histname = Form("CRDC2Y_Gated_%s",mygate->GetName());
	        obj.FillHistogram("s800",histname,
                                  5000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());


	      }// end IsInside(ResiduePID)
	    }// end iterating over gates
	  }//end haspids

        //}// end of if inside timingcut
      }// end of if(s800)

    }// end of caesar for hit loop
    //obj.FillHistogram("Caesar","Multiplicity",300,0,300,valid_counter);
    obj.FillHistogram("Caesar","Multiplicity_ng_ne17blobtop",200,0,200,ng_ne17blobtop);
    obj.FillHistogram("Caesar","Multiplicity_ng_ne17blobbetween",200,0,200,ng_ne17blobbetween);
    obj.FillHistogram("Caesar","Multiplicity_ng_mg20blobtop",200,0,200,ng_mg20blobtop);
    obj.FillHistogram("Caesar","Multiplicity_ng_mg21blobtop",200,0,200,ng_mg21blobtop);
    obj.FillHistogram("Caesar","Multiplicity_ng_mg21blobbtwn",200,0,200,ng_mg21blobbtwn);
    obj.FillHistogram("Caesar","Multiplicity_ng_al24blobbetween",200,0,200,ng_al24blobbetween);
    obj.FillHistogram("Caesar","Multiplicity_ng_p26blobtop",200,0,200,ng_p26blobtop);
  
  } // I have a valid caesar event

  if(numobj!=list->GetSize())
    list->Sort();

}//end MakeHistograms
