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

