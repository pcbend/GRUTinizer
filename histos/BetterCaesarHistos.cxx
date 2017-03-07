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
#include "GCutG.h"


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


std::vector<GCutG*> incoming_cuts   = {0};
std::vector<GCutG*> outgoing_cuts   = {0};
std::vector<GCutG*> timeenergy_cuts; // = {0};
int gates_loaded=0;


bool OutgoingBeam(TRuntimeObjects& obj,GCutG *incoming) {
  TS800    *s800    = obj.GetDetector<TS800>();
  if(!s800)
    return false;

  std::string histname;
  std::string dirname;
  if(incoming)
    dirname = Form("outgoing_%s",incoming->GetName());
  else
    dirname = "outgoing";


  double objtac = s800->GetTof().GetTacOBJ();
  double xfptac = s800->GetTof().GetTacXFP();
  if(incoming) {
    if(!incoming->IsInside(objtac,xfptac))
      return false;
  }

  double ic_sum         = s800->GetIonChamber().GetAve();
  double objtac_corr    = s800->GetCorrTOF_OBJTAC();
  double afp            = s800->GetAFP();
  double xfp_focalplane = s800->GetXFP(0);
  //TVector3 track        = s800->Track();
  //double corr_time      = caesar->GetCorrTime(hit,s800);


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

  obj.FillHistogram(dirname,"CRDC1Y",5000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
  obj.FillHistogram(dirname,"CRDC2Y",5000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
  obj.FillHistogram(dirname,"CRDC1X",400,-400,400,s800->GetCrdc(0).GetDispersiveX());
  obj.FillHistogram(dirname,"CRDC2X",400,-400,400,s800->GetCrdc(1).GetDispersiveX());
  obj.FillHistogram(dirname,"TrigBit",30,0,30,s800->GetTrigger().GetRegistr());
  obj.FillHistogram(dirname,"S800_YTA",1000,-50,50,s800->GetYta());
  obj.FillHistogram(dirname,"S800_DTA",1000,-0.2,0.2,s800->GetDta());
  obj.FillHistogram(dirname,"ATA_vs_BTA",1000,-0.2,0.2,s800->GetAta(),
			                 1000,-0.2,0.2,s800->GetBta());

  return true;
}


bool IncomingBeam(TRuntimeObjects& obj,GCutG *outgoing) {
  TS800    *s800    = obj.GetDetector<TS800>();
  if(!s800)
    return false;

  std::string histname;
  std::string dirname;
  if(outgoing)
    dirname = Form("incoming_%s",outgoing->GetName());
  else
    dirname = "incoming";
  
  double ic_sum         = s800->GetIonChamber().GetAve();
  double objtac_corr    = s800->GetCorrTOF_OBJTAC();
  if(outgoing) {
    if(!outgoing->IsInside(objtac_corr,ic_sum))
      return false;
  }

  double objtac = s800->GetTof().GetTacOBJ();
  double xfptac = s800->GetTof().GetTacXFP();
  histname = "IncomingPID";
  obj.FillHistogram(dirname,histname,
                     1000,-1000,5000,objtac,
                     1000,-1000,5000,xfptac);
  return true;
}


int HandleCaesar(TRuntimeObjects& obj,GCutG *incoming,GCutG *outgoing) {
   
  TS800    *s800    = obj.GetDetector<TS800>();
  TCaesar  *caesar  = obj.GetDetector<TCaesar>();
  if(!s800 || !caesar)
    return false;

  std::string histname;
  std::string dirname;
  std::string dirname2="";
  int cut=-1;

  for(unsigned int x=0;x<timeenergy_cuts.size();x++) {
    //printf("timeenergy[%i] = 0x%08x\n",x,timeenergy_cuts.at(x));
    if(strcmp(timeenergy_cuts.at(x)->GetTitle(),outgoing->GetTitle()) == 0){
      dirname2 = Form("caesar_%s_timeenergygated",outgoing->GetName());
      cut = x;
    }
  }
  
  //this enforces that the outgoing PID blob (AX_from_BY_incoming) is in the incoming gate (BY_incoming)
  const char *outgoing_name = outgoing->GetName();
  const char *toRemove = outgoing->GetTitle(); //going to remove AX from name
  size_t length = strlen(toRemove); //length of AX (can be 23Al or 26P, for example)
  outgoing_name += length+6; //remove AX_from_  
  if(strcmp(outgoing_name,incoming->GetName()) != 0)
    return false;

  dirname = Form("caesar_%s",outgoing->GetName());
  
  int good_counter=0;
  int good_counter_te=0;
 
  double beta    = GValue::Value(Form("BETA_%s",outgoing->GetTitle()));
  double z_shift = GValue::Value("TARGET_SHIFT_Z");
  TVector3 track = s800->Track(); 

  for(unsigned int j=0;j<caesar->Size();j++) {
    TCaesarHit hit_m = caesar->GetCaesarHit(j);
      if(!hit_m.IsValid())
        continue;
      if(hit_m.IsOverflow())
        continue;

      double corr_time_m = caesar->GetCorrTime(hit_m,s800);

      if(dirname2.length() && timeenergy_cuts.at(cut)->IsInside(corr_time_m,hit_m.GetDoppler(beta,z_shift,&track))){
        good_counter_te++;
      }
    
    good_counter++;

  }//end for loop to get multiplicity of caesar event (good_counter)

  for(unsigned int i=0;i<caesar->Size();i++) {
    TCaesarHit hit = caesar->GetCaesarHit(i);
    if(!hit.IsValid())
      continue;
    if(hit.IsOverflow())
      continue;

    double corr_time = caesar->GetCorrTime(hit,s800);
    
    obj.FillHistogram(dirname,"GetCorrTime_vs_GetEnergy",2000,-2000,2000,corr_time,
                                                         1024,0,8192,hit.GetEnergy());
    histname = Form("doppler_beta_%f",beta);
    obj.FillHistogram(dirname,histname,1024,0,8192,hit.GetDoppler(beta,z_shift,&track));

    histname = "doppler_vs_detnum";
    obj.FillHistogram(dirname,histname,200,0,200,hit.GetAbsoluteDetectorNumber(),
			               1024,0,8192,hit.GetDoppler(beta,z_shift,&track));

    histname = "multiplicity";
      obj.FillHistogram(dirname,histname,200,0,200,good_counter);

    histname = "doppler_vs_mult";
    obj.FillHistogram(dirname,histname,200,0,200,good_counter,
                                       1024,0,8192,hit.GetDoppler(beta,z_shift,&track));

    histname = "doppler_vs_DTA";
            obj.FillHistogram(dirname,histname,
                              500,-0.2,0.2,s800->GetDta(),
                              1024,0,8192,hit.GetDoppler(beta,z_shift,&track));

    if(good_counter==1){
      histname = "doppler_vs_DTA_mult1";
      obj.FillHistogram(dirname,histname,
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
    }//if multiplicity 1
    if(good_counter==2){
      histname = "doppler_vs_DTA_mult2";
      obj.FillHistogram(dirname,histname,
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
    }//if multiplicity 2
    if(good_counter>2){
      histname = "doppler_vs_DTA_mult>2";
      obj.FillHistogram(dirname,histname,
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
    }//if multiplicity >2	

    //for(int beta_i=0;beta_i<500;beta_i++){
      //double beta_use = (0.5/500.0)*double(beta_i);
      //histname = "find_beta";
      //obj.FillHistogram(dirname,histname,
      //                          500,0,0.499,beta_use,
      //			  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
      //if(good_counter==1){
        //histname = "find_beta_mult1";
        //obj.FillHistogram(dirname,histname,
        //                          500,0,0.499,beta_use,
	//                          1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
      
      //}//if multiplicity 1
      //if(good_counter==2){
        //histname = "find_beta_mult2";
        //obj.FillHistogram(dirname,histname,
        //                          500,0,0.499,beta_use,
	//			  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
      
      //}//if multiplicity 2
      //if(good_counter>2){
        //histname = "find_beta_mult>2";
        //obj.FillHistogram(dirname,histname,
        //                          500,0,0.499,beta_use,
	//			  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
      
      //}//if multiplicity >2

    //}//for loop over beta_i

    if(dirname2.length() && timeenergy_cuts.at(cut)->IsInside(corr_time,hit.GetDoppler(beta,z_shift,&track))) {

      obj.FillHistogram(dirname2,"GetCorrTime_vs_GetEnergy",2000,-2000,2000,corr_time,
                                                            1024,0,8192,hit.GetEnergy());
      histname = Form("doppler_beta_%f",beta);
      obj.FillHistogram(dirname2,histname,1024,0,8192,hit.GetDoppler(beta,z_shift,&track));

      histname = "doppler_vs_detnum";
      obj.FillHistogram(dirname2,histname,200,0,200,hit.GetAbsoluteDetectorNumber(),
			                 1024,0,8192,hit.GetDoppler(beta,z_shift,&track));

      histname = "multiplicity";
      obj.FillHistogram(dirname2,histname,200,0,200,good_counter_te);
      
      histname = "doppler_vs_mult";
      obj.FillHistogram(dirname2,histname,200,0,200,good_counter_te,
                                         1024,0,8192,hit.GetDoppler(beta,z_shift,&track));

      histname = "doppler_vs_DTA";
            obj.FillHistogram(dirname2,histname,
                              500,-0.2,0.2,s800->GetDta(),
                              1024,0,8192,hit.GetDoppler(beta,z_shift,&track));

      if(good_counter==1){
        histname = "doppler_vs_DTA_mult1";
        obj.FillHistogram(dirname2,histname,
                                  500,-0.2,0.2,s800->GetDta(),
                                  1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
      }//if multiplicity 1
      if(good_counter==2){
        histname = "doppler_vs_DTA_mult2";
        obj.FillHistogram(dirname2,histname,
                                  500,-0.2,0.2,s800->GetDta(),
                                  1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
      }//if multiplicity 2
      if(good_counter>2){
        histname = "doppler_vs_DTA_mult>2";
        obj.FillHistogram(dirname2,histname,
                                  500,-0.2,0.2,s800->GetDta(),
                                  1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
      }//if multiplicity >2	

      //for(int beta_i=0;beta_i<500;beta_i++){
        //double beta_use = (0.5/500.0)*double(beta_i);
        //histname = "find_beta";
        //obj.FillHistogram(dirname2,histname,
        //                          500,0,0.499,beta_use,
        //                          1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
        //if(good_counter==1){
          //histname = "find_beta_mult1";
          //obj.FillHistogram(dirname2,histname,
          //                          500,0,0.499,beta_use,
          //                          1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
        //}//if multiplicity 1
        //if(good_counter==2){
          //histname = "find_beta_mult2";
          //obj.FillHistogram(dirname2,histname,
          //                          500,0,0.499,beta_use,
          //                          1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
      
        //}//if multiplicity 2
        //if(good_counter>2){
          //histname = "find_beta_mult>2";
          //obj.FillHistogram(dirname2,histname,
          //                          500,0,0.499,beta_use,
          //                          1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
        //}//if multiplicity >2

      //}//for loop over beta_i
    
    }//if in te gate
  }//for loop over caesar 
  
  return 0;
}




// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  //TCaesar  *caesar  = obj.GetDetector<TCaesar>();
  //TS800    *s800    = obj.GetDetector<TS800>();

  TList *list  = &(obj.GetObjects());
  TList *gates = &(obj.GetGates());
  int numobj = list->GetSize();

  if(gates_loaded!=gates->GetSize()) {
    TIter iter(gates);
    while(TObject *obj = iter.Next()) {
      GCutG *gate = (GCutG*)obj;
      std::string tag = gate->GetTag();
      if(!tag.compare("incoming")) {
        incoming_cuts.push_back(gate);
      } else if(!tag.compare("outgoing")) {
        outgoing_cuts.push_back(gate);
      } else if(!tag.compare("timeenergy")){
        timeenergy_cuts.push_back(gate);
      }
      gates_loaded++;
    }
  }


  //printf("incoming.size() == %i\n",incoming_cuts.size());
  int incoming_passed=-1;
  int outgoing_passed=-1;
  for(unsigned int x=0;x<incoming_cuts.size();x++) {
    bool passed = OutgoingBeam(obj,incoming_cuts.at(x)); 
    if(x!=0 && passed) {
      incoming_passed = x;
      break;
    }
  }
  for(unsigned int x=0;x<outgoing_cuts.size();x++) {
    bool passed = IncomingBeam(obj,outgoing_cuts.at(x)); 
    if(x!=0 && passed) {
      outgoing_passed = x;
      break;
    }
  } 


  if(incoming_passed>0 && outgoing_passed>0) {
    HandleCaesar(obj,incoming_cuts.at(incoming_passed),outgoing_cuts.at(outgoing_passed));
  }



  if(numobj!=list->GetSize())
    list->Sort();

}//end MakeHistograms
  
  
  
 /* 
  
  //double    beta = GValue::Value("BETA");
  double    beta = 0.407;
  double    beta_al23 = 0.433;
  double    beta_si24 = 0.444;
  double    beta_mg22 = 0.421;
  double    beta_mg23 = 0.407;
  double    z_shift = 0.65;

  static TCutG *timingcut_al23 = 0;
  if(!timingcut_al23) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/timeenergy_al23.root");
    timingcut_al23 = (TCutG*)fcut.Get("al23timegate");
  }
  static TCutG *timingcut_si24 = 0;
  if(!timingcut_si24) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/timeenergy_si24.root");
    timingcut_si24 = (TCutG*)fcut.Get("si24timegate");
  }
  static TCutG *timingcut_mg22 = 0;
  if(!timingcut_mg22) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/timeenergy_mg22.root");
    timingcut_mg22 = (TCutG*)fcut.Get("mg22timegate");
  }
  static TCutG *timingcut_mg23 = 0;
  if(!timingcut_mg23) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/timeenergy_mg23.root");
    timingcut_mg23 = (TCutG*)fcut.Get("mg23timegate");
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
  static TCutG *InBeam_below = 0;
  if(!InBeam_below) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/InBeam_below.root");
    InBeam_below = (TCutG*)fcut.Get("below");
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
  static TCutG *newmg23blob = 0;
  if(!newmg23blob) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/newmg23blob.root");
    newmg23blob = (TCutG*)fcut.Get("newmg23blob");
  }

  static TCutG *o16blob = 0;
  if(!o16blob) {
    TPreserveGDirectory Preserve;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/o16pid.root");
    o16blob = (TCutG*)fcut.Get("o16");
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
    int ng_newmg23blob=0;

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

        if(newal23blob->IsInside(objtac_corr_m,ic_sum_m) && InBeam_btwnTopMid->IsInside(objtac_m,xfptac_m) && 		       		   timingcut_al23->IsInside(corr_time_m,hit_m.GetEnergy()))
          ng_newal23blob++;

        if(newsi24blob->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Top->IsInside(objtac_m,xfptac_m) &&                         	          timingcut_si24->IsInside(corr_time_m,hit_m.GetEnergy()))
          ng_newsi24blob++;

        if(newmg22blob->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Mid->IsInside(objtac_m,xfptac_m) &&                         	      	   timingcut_mg22->IsInside(corr_time_m,hit_m.GetEnergy()))
          ng_newmg22blob++;

        if(newmg23blob->IsInside(objtac_corr_m,ic_sum_m) && InBeam_Mid->IsInside(objtac_m,xfptac_m) &&                         	      	   timingcut_mg23->IsInside(corr_time_m,hit_m.GetEnergy()))
          ng_newmg23blob++;

    }//end for loop to get multiplicity of caesar event

    for(unsigned int i=0;i<caesar->Size();i++) {
      TCaesarHit hit = caesar->GetCaesarHit(i);
      if(!hit.IsValid())
        continue;
      if(hit.IsOverflow())
        continue;
      valid_counter++;

      int ring = hit.GetRingNumber();

      //obj.FillHistogram("Caesar","CAESAR_raw_time", 4096,0,4096,hit.Time());
      //obj.FillHistogram("Caesar","GetTime_vs_GetDoppler",2000,0,2000,hit.GetTime(),
                                                         //2048,0,8192,hit.GetDoppler(beta,z_shift));
      //if(hit.GetDoppler(beta,z_shift)>300){
        //obj.FillHistogram("Caesar","GetTime_vs_GetAbsoluteDetectorNumber",200,0,200,hit.GetAbsoluteDetectorNumber(),
                                                                          //2000,0,2000,hit.GetTime());
      //}//end if Doppler > 300

      obj.FillHistogram("Caesar","Detector_Charge_Summary",200,0,200,hit.GetDetectorNumber()+total_det_in_prev_rings[ring],
                                                  4096,0,4096,hit.Charge());

      obj.FillHistogram("Caesar","Detector_Energy_Summary",200,0,200,hit.GetDetectorNumber()+total_det_in_prev_rings[ring],
                                                  2048,0,8192,hit.GetEnergy());

      //obj.FillHistogram("Caesar","Detector_Doppler_Summary",300,0,300,hit.GetDetectorNumber()+total_det_in_prev_rings[ring],
                                                  //1024,0,8192,hit.GetDoppler(beta,z_shift));

      for(unsigned int j=0;j<caesar->Size();j++) {
        if(i==j)
          continue;
        TCaesarHit hit2 = caesar->GetCaesarHit(j);
      if(!hit2.IsValid())
        continue;
      if(hit2.IsOverflow())
        continue;
        obj.FillHistogram("Caesar","Coincidence_Timing",2000,-2000,2000,hit.GetTime()-hit2.GetTime());

      if(abs(hit.GetEnergy()-1332)<25){
        obj.FillHistogram("Caesar","Coincidence_Timing_Gated_1332",2000,-2000,2000,hit.GetTime()-hit2.GetTime());
      }
                                                             

      }//end for loop over coincidence hits

      if(!s800) {
       continue;
      }
*/
      /**************************************\
      | Lets think about the location of this|
      | if(s800) that is inside our loop over|
      | CAESAR Hits!!!!                      |
      \**************************************/
/*
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
                                                               1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
        obj.FillHistogram("Caesar","GetCorrTime_vs_GetEnergy",2000,-2000,2000,corr_time,
                                                               1024,0,8192,hit.GetEnergy());
        if(hit.GetDoppler(beta,z_shift)>300){
          obj.FillHistogram("Caesar","GetCorrTime_vs_GetAbsoluteDetectorNumber",200,0,200,hit.GetAbsoluteDetectorNumber(),
                                                                          2000,-2000,2000,corr_time);
        }//end if Doppler > 300
        
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
                                                               1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if in mid cut

          if(InBeam_Top->IsInside(objtac,xfptac)){
	    histname = "IC_vs_OBJTOF_PID_InBeam_Top";
	    obj.FillHistogram(dirname,histname,
			      1000,-500,2500,objtac_corr,
			      1000,-100,4000,ic_sum);
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Top",2000,-2000,2000,corr_time,
                                                               1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if in top cut

          if(InBeam_btwnTopMid->IsInside(objtac,xfptac)){
	    histname = "IC_vs_OBJTOF_PID_InBeam_btwnTopMid";
	    obj.FillHistogram(dirname,histname,
			      1000,-500,2500,objtac_corr,
			      1000,-100,4000,ic_sum);
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Btwn",2000,-2000,2000,corr_time,
                                                            1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if in between cut

          if(InBeam_below->IsInside(objtac,xfptac)){
	    histname = "IC_vs_OBJTOF_PID_InBeam_below";
	    obj.FillHistogram(dirname,histname,
			      1000,-500,2500,objtac_corr,
			      1000,-100,4000,ic_sum);
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Below",2000,-2000,2000,corr_time,
                                                            1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
	  }//end if in below cut

          if(o16blob->IsInside(objtac_corr,ic_sum) && InBeam_Mid->IsInside(objtac,xfptac)){
            histname = "Gamma_Gated_o16blob";
            obj.FillHistogram("GATED",histname,
                              1024,0,8192,hit.GetDoppler(beta,z_shift,&track));
          }

          if(newal23blob->IsInside(objtac_corr,ic_sum) && InBeam_btwnTopMid->IsInside(objtac,xfptac) && timingcut_al23->IsInside(corr_time,hit.GetEnergy())){

	    histname = "Gamma_Gated_newal23blob";
	    obj.FillHistogram("GATED",histname,
                              200,0,200,hit.GetAbsoluteDetectorNumber(),
			      1024,0,8192,hit.GetDoppler(beta_al23,z_shift,&track));
            obj.FillHistogram("s800","CRDC1Y_Gated_newal23blob",
			      5000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    obj.FillHistogram("s800","CRDC2Y_Gated_newal23blob",
			      5000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());

            obj.FillHistogram("s800","CRDC1X_Gated_newal23blob",
			      400,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    obj.FillHistogram("s800","CRDC2X_Gated_newal23blob",
			      400,-400,400,s800->GetCrdc(1).GetDispersiveX());

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

            histname = "DTA_vs_Doppler_newal23blob";
            obj.FillHistogram("Momentum_Distribution",histname,
                              500,-0.2,0.2,s800->GetDta(),
                              1024,0,8192,hit.GetDoppler(beta_al23,z_shift,&track));

            if(ng_newal23blob==1){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newal23blob_Mult1",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_al23,z_shift,&track));
              histname = "Gamma_Gated_newal23blob_Mult1";
	      obj.FillHistogram("GATED",histname,
                                200,0,200,hit.GetAbsoluteDetectorNumber(),
			        1024,0,8192,hit.GetDoppler(beta_al23,z_shift,&track));
            }//if multiplicity 1

            if(ng_newal23blob==2){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newal23blob_Mult2",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_al23,z_shift,&track));
            }//if multiplicity 2
     
            if(ng_newal23blob>2){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newal23blob_Mult>2",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_al23,z_shift,&track));
            }//if multiplicity > 2	


            //for(int z_i=0;z_i<20;z_i++){ 
              //double z_use = 0.1*double(z_i);
      
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
                //histname = Form("FindBeta_newal23blob_zshift_is_%f",z_use);
                histname = "FindBeta_newal23blob";
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

            //}//end for loop over z_i

            if(hit.GetDoppler(beta_al23,z_shift,&track)>300){
              obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetAbsoluteDetectorNumber_Gated_newal23blob",
                                200,0,200,hit.GetAbsoluteDetectorNumber(),
                                2000,-2000,2000,corr_time);
            }
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Btwn_newal23blob",
                              2000,-2000,2000,corr_time,
                              1024,0,8192,hit.GetDoppler(beta_al23,z_shift,&track));
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetEnergy_Btwn_newal23blob",
                              2000,-2000,2000,corr_time,
                              1024,0,8192,hit.GetEnergy());
	  }//end if inside newal23blob and inside between incomingPID

          if(newsi24blob->IsInside(objtac_corr,ic_sum) && InBeam_Top->IsInside(objtac,xfptac) && timingcut_si24->IsInside(corr_time,hit.GetEnergy())){

	    histname = "Gamma_Gated_newsi24blob";
	    obj.FillHistogram("GATED",histname,
                              200,0,200,hit.GetAbsoluteDetectorNumber(),
			      1024,0,8192,hit.GetDoppler(beta_si24,z_shift,&track));
            obj.FillHistogram("s800","CRDC1Y_Gated_newsi24blob",5000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    obj.FillHistogram("s800","CRDC2Y_Gated_newsi24blob",5000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
            obj.FillHistogram("s800","CRDC1X_Gated_newsi24blob",400,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    obj.FillHistogram("s800","CRDC2X_Gated_newsi24blob",400,-400,400,s800->GetCrdc(1).GetDispersiveX());

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

            histname = "DTA_vs_Doppler_newsi24blob";
            obj.FillHistogram("Momentum_Distribution",histname,
                              500,-0.2,0.2,s800->GetDta(),
                              1024,0,8192,hit.GetDoppler(beta_si24,z_shift,&track));

            if(ng_newsi24blob==1){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newsi24blob_Mult1",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_si24,z_shift,&track));
            }//if multiplicity 1

            if(ng_newsi24blob==2){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newsi24blob_Mult2",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_si24,z_shift,&track));
              histname = "Gamma_Gated_newsi24blob_Mult1";
	      obj.FillHistogram("GATED",histname,
                                200,0,200,hit.GetAbsoluteDetectorNumber(),
			        1024,0,8192,hit.GetDoppler(beta_si24,z_shift,&track));
            }//if multiplicity 2
     
            if(ng_newsi24blob>2){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newsi24blob_Mult>2",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_si24,z_shift,&track));
            }//if multiplicity > 2	

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
                              2000,-2000,2000,corr_time,
                              1024,0,8192,hit.GetDoppler(beta_si24,z_shift,&track));
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetEnergy_Top_newsi24blob",
                              2000,-2000,2000,corr_time,
                              1024,0,8192,hit.GetEnergy());
	  }//end if inside newsi24blob and inside top incomingPID

          if(newmg22blob->IsInside(objtac_corr,ic_sum) && InBeam_Mid->IsInside(objtac,xfptac) && timingcut_mg22->IsInside(corr_time,hit.GetEnergy())){

	    histname = "Gamma_Gated_newmg22blob";
	    obj.FillHistogram("GATED",histname,
                              200,0,200,hit.GetAbsoluteDetectorNumber(),
			      1024,0,8192,hit.GetDoppler(beta_mg22,z_shift,&track));
            obj.FillHistogram("s800","CRDC1Y_Gated_newmg22blob",5000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    obj.FillHistogram("s800","CRDC2Y_Gated_newmg22blob",5000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
            obj.FillHistogram("s800","CRDC1X_Gated_newmg22blob",400,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    obj.FillHistogram("s800","CRDC2X_Gated_newmg22blob",400,-400,400,s800->GetCrdc(1).GetDispersiveX());

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

            histname = "DTA_vs_Doppler_newmg22blob";
            obj.FillHistogram("Momentum_Distribution",histname,
                              500,-0.2,0.2,s800->GetDta(),
                              1024,0,8192,hit.GetDoppler(beta_mg22,z_shift,&track));

            if(ng_newmg22blob==1){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newmg22blob_Mult1",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_mg22,z_shift,&track));
              histname = "Gamma_Gated_newmg22blob_Mult1";
	      obj.FillHistogram("GATED",histname,
                                200,0,200,hit.GetAbsoluteDetectorNumber(),
			        1024,0,8192,hit.GetDoppler(beta_mg22,z_shift,&track));
            }//if multiplicity 1

            if(ng_newmg22blob==2){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newmg22blob_Mult2",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_mg22,z_shift,&track));
            }//if multiplicity 2
     
            if(ng_newmg22blob>2){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newmg22blob_Mult>2",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_mg22,z_shift,&track));
            }//if multiplicity > 2	

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

            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Mid_newmg22blob",
                              2000,-2000,2000,corr_time,
                              1024,0,8192,hit.GetDoppler(beta_mg22,z_shift,&track));
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetEnergy_Mid_newmg22blob",
                              2000,-2000,2000,corr_time,
                              1024,0,8192,hit.GetEnergy());
	  }//end if inside newmg22blob and inside mid incomingPID

          if(newmg23blob->IsInside(objtac_corr,ic_sum) && InBeam_Mid->IsInside(objtac,xfptac) && timingcut_mg23->IsInside(corr_time,hit.GetEnergy())){

	    histname = "Gamma_Gated_newmg23blob";
	    obj.FillHistogram("GATED",histname,
                              200,0,200,hit.GetAbsoluteDetectorNumber(),
			      1024,0,8192,hit.GetDoppler(beta_mg23,z_shift,&track));
            obj.FillHistogram("s800","CRDC1Y_Gated_newmg22blob",10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	    obj.FillHistogram("s800","CRDC2Y_Gated_newmg22blob",10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
            obj.FillHistogram("s800","CRDC1X_Gated_newmg22blob",800,-400,400,s800->GetCrdc(0).GetDispersiveX());
	    obj.FillHistogram("s800","CRDC2X_Gated_newmg22blob",800,-400,400,s800->GetCrdc(1).GetDispersiveX());

            dirname = "InverseMap_GATED";
     
            histname = "S800_YTA_Gated_newmg23blob";
            obj.FillHistogram(dirname,histname,
			      1000,-50,50,s800->GetYta());
      
            histname = "S800_DTA_Gated_newmg23blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetDta());
      
            histname = "ATA_vs_BTA_Gated_newmg23blob";
            obj.FillHistogram(dirname,histname,
			      1000,-0.2,0.2,s800->GetAta(),
			      1000,-0.2,0.2,s800->GetBta());

            histname = "DTA_vs_Doppler_newmg23blob";
            obj.FillHistogram("Momentum_Distribution",histname,
                              500,-0.2,0.2,s800->GetDta(),
                              1024,0,8192,hit.GetDoppler(beta_mg23,z_shift,&track));

            if(ng_newmg23blob==1){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newmg23blob_Mult1",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_mg23,z_shift,&track));
              histname = "Gamma_Gated_newmg23blob_Mult1";
	      obj.FillHistogram("GATED",histname,
                                200,0,200,hit.GetAbsoluteDetectorNumber(),
			        1024,0,8192,hit.GetDoppler(beta_mg23,z_shift,&track));
            }//if multiplicity 1

            if(ng_newmg23blob==2){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newmg23blob_Mult2",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_mg23,z_shift,&track));
            }//if multiplicity 2
     
            if(ng_newmg23blob>2){
              obj.FillHistogram("Momentum_Distribution","DTA_vs_Doppler_newmg23blob_Mult>2",
                                500,-0.2,0.2,s800->GetDta(),
                                1024,0,8192,hit.GetDoppler(beta_mg23,z_shift,&track));
            }//if multiplicity > 2	


            //for(int z_i=0;z_i<20;z_i++){ 
              //double z_use = 0.1*double(z_i);
            
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
                 histname = Form("FindBeta_newmg23blob");
                 obj.FillHistogram("GATED",histname,
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
		
                //histname = Form("FindBeta_newmg23blob_zshift_is_%f",z_use);
                //obj.FillHistogram("GATED",histname,
				  //300,0.2,0.499,beta_use,
				  //1024,0,8192,hit.GetDoppler(beta_use,z_use,&track));	

                if(ng_newmg23blob==1){
                   obj.FillHistogram("GATED","FindBeta_newmg23blob_Mult1",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 1
                if(ng_newmg23blob==2){
                   obj.FillHistogram("GATED","FindBeta_newmg23blob_Mult2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity 2
                if(ng_newmg23blob>2){
                   obj.FillHistogram("GATED","FindBeta_newmg23blob_Mult>2",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
                }//if multiplicity > 2	
	      }//for loop over beta_i

            //}//for loop over z_i

            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetDoppler_Mid_newmg23blob",
                              2000,-2000,2000,corr_time,
                              1024,0,8192,hit.GetDoppler(beta_mg23,z_shift,&track));
            obj.FillHistogram("Caesar_GATED","GetCorrTime_vs_GetEnergy_Mid_newmg23blob",
                              2000,-2000,2000,corr_time,
                              1024,0,8192,hit.GetEnergy());
	  }//end if inside newmg23blob and inside mid incomingPID
    
	  obj.FillHistogram("s800","CRDC1Y",5000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
	  obj.FillHistogram("s800","CRDC2Y",5000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
          obj.FillHistogram("s800","CRDC1X",400,-400,400,s800->GetCrdc(0).GetDispersiveX());
          obj.FillHistogram("s800","CRDC2X",400,-400,400,s800->GetCrdc(1).GetDispersiveX());

	  obj.FillHistogram("s800","TrigBit",30,0,30,s800->GetTrigger().GetRegistr());

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
    obj.FillHistogram("Caesar","Multiplicity",200,0,200,valid_counter);
    obj.FillHistogram("Caesar","Multiplicity_ng_al23blob",200,0,200,ng_newal23blob);
    obj.FillHistogram("Caesar","Multiplicity_ng_si24blob",200,0,200,ng_newsi24blob);
    obj.FillHistogram("Caesar","Multiplicity_ng_mg22blob",200,0,200,ng_newmg22blob);
    obj.FillHistogram("Caesar","Multiplicity_ng_mg23blob",200,0,200,ng_newmg23blob);
  
  } // I have a valid caesar event
*/
