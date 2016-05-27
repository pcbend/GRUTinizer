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
std::vector<double> energies_singles;


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


  static TCutG *InBeam_Mid = 0;
  if(!InBeam_Mid) {
    TDirectory *cur = gDirectory;
    TFile fcut("/mnt/analysis/pecan-2015/longfellow/e10002/newmid.root");
    InBeam_Mid = (TCutG*)fcut.Get("InBeam_Mid");
    cur->cd();
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

  // Stuff for interactive gates!
  TList *gates = &(obj.GetGates());
  bool haspids = gates->GetSize();
  

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();
  if(caesar) {
    if(!init_called)
      Init();
    int valid_counter=0;

    for(int y=0;y<caesar->Size();y++) {
      TCaesarHit hit = caesar->GetCaesarHit(y);
      obj.FillHistogram("Caesar","CAESAR_raw_time", 4096,0,4096,hit.Time());
      if(!hit.IsValid())
        continue;

      for(int z=0;z<caesar->Size();z++) {
        if(y==z)
          continue;
        TCaesarHit hit2 = caesar->GetCaesarHit(z);
        if(!hit2.IsValid())
          continue;
        obj.FillHistogram("Caesar","Energy_Coincidence_Matrix",2000,0,8000,hit.GetEnergy(),
                                                             2000,0,8000,hit2.GetEnergy());

      }

      valid_counter++;
      int ring = hit.GetRingNumber();
      obj.FillHistogram("Caesar","Detector_Charge_Summary",300,0,300,hit.GetDetectorNumber()+total_det_in_prev_rings[ring],
                                                  4096,0,4096,hit.Charge());

      obj.FillHistogram("Caesar","Detector_Energy_Summary",300,0,300,hit.GetDetectorNumber()+total_det_in_prev_rings[ring],
                                                  2000,0,8000,hit.GetEnergy());

      obj.FillHistogram("Caesar","Detector_Doppler_Summary",300,0,300,hit.GetDetectorNumber()+total_det_in_prev_rings[ring],
                                                  2000,0,8000,hit.GetDoppler());


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

        double corr_time = caesar->GetCorrTime(hit,s800);
        obj.FillHistogram("Caesar","GetCorrTime_vs_GetDoppler",2000,0,2000,corr_time,
                                                               2000,0,4000,hit.GetDoppler());

	std::string dirname  = "PID";

        std::string histname = "IncomingPID";
	obj.FillHistogram(dirname,histname,
			  1000,-1000,5000,objtac,
			  1000,-1000,5000,xfptac);

	histname = "AFP_vs_OBJTOF";
	obj.FillHistogram(dirname,histname,
			  1000,-500,2500,objtac_corr,
			  1000,-1,1,afp); // check units of AFP.
    
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
	}//end if in mid cut
  
        if(InBeam_Top->IsInside(objtac,xfptac)){
	  histname = "IC_vs_OBJTOF_PID_InBeam_Top";
	  obj.FillHistogram(dirname,histname,
			    1000,-500,2500,objtac_corr,
			    1000,-100,4000,ic_sum); 
	}//end if in top cut
    
        if(InBeam_btwnTopMid->IsInside(objtac,xfptac)){
	  histname = "IC_vs_OBJTOF_PID_InBeam_btwnTopMid";
	  obj.FillHistogram(dirname,histname,
			    1000,-500,2500,objtac_corr,
			    1000,-100,4000,ic_sum); 

	}//end if in between cut 
         

        if(al23blob->IsInside(objtac_corr,ic_sum) && InBeam_btwnTopMid->IsInside(objtac,xfptac)){

	  histname = "Gamma_Gated_al23blob";
	  obj.FillHistogram("GATED",histname,
				1000,0,4000,hit.GetDoppler());
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
				  1000,0,4000,hit.GetDoppler(beta_use));		
	      }
	}//end if inside al23blob and inside between incomingPID

        if(si24blob->IsInside(objtac_corr,ic_sum) && InBeam_Top->IsInside(objtac,xfptac)){
	  histname = "Gamma_Gated_si24blob";
	  obj.FillHistogram("GATED",histname,
				1000,0,4000,hit.GetDoppler());
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
				  1000,0,4000,hit.GetDoppler(beta_use));		
	      }
	}//end if inside si24blob and inside top incomingPID

        if(newal23blob->IsInside(objtac_corr,ic_sum) && InBeam_btwnTopMid->IsInside(objtac,xfptac)){

	  histname = "Gamma_Gated_newal23blob";
	  obj.FillHistogram("GATED",histname,
				1000,0,4000,hit.GetDoppler());
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
				  1000,0,4000,hit.GetDoppler(beta_use));		
	      }
	}//end if inside newal23blob and inside between incomingPID

        if(newsi24blob->IsInside(objtac_corr,ic_sum) && InBeam_Top->IsInside(objtac,xfptac)){
	  histname = "Gamma_Gated_newsi24blob";
	  obj.FillHistogram("GATED",histname,
				1000,0,4000,hit.GetDoppler());
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
				  1000,0,4000,hit.GetDoppler(beta_use));		
	      }
	}//end if inside newsi24blob and inside top incomingPID
    
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
	    if(mygate->IsInside(objtac,xfptac)){
	      // histname = Form("AFP_vs_OBJTOF_%s",mygate->GetName());
	      // obj.FillHistogram(dirname,histname,
	      // 			1000,-500,2500,objtac_corr,
	      // 			1000,-1,1,afp); // check units of AFP.
    
	      // histname = Form("XFP_vs_OBJTOF_%s",mygate->GetName());
	      // obj.FillHistogram(dirname,histname,
	      // 			1000,-500,2500,objtac_corr,
	      // 			1000,-300,300,xfp_focalplane); 
    
	      // histname = Form("IC_vs_OBJTOF_PID_%s",mygate->GetName());
	      // obj.FillHistogram(dirname,histname,
	      // 			1000,-500,2500,objtac_corr,
	      // 			1000,-100,4000,ic_sum); 
    
	    }// end IsInside(InBeamGate)
	    if(mygate->IsInside(objtac_corr,ic_sum)){
	      //if(!InBeam_Mid) break;
	      //if(!(InBeam_Mid->IsInside(objtac,xfptac))) break;
	      
	      histname = Form("FindBeta_%s",mygate->GetName());
	      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram(dirname,histname,
				  300,0.2,0.5,beta_use,
				  1000,0,4000,hit.GetDoppler(beta_use));		
	      }

	      histname = Form("Gamma_%s",mygate->GetName());
	      obj.FillHistogram(dirname,histname,
				1000,0,4000,hit.GetDoppler());


              histname = Form("CRDC1Y_Gated_%s",mygate->GetName());  
              obj.FillHistogram("s800",histname,
                                10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
              
              histname = Form("CRDC2Y_Gated_%s",mygate->GetName()); 
	      obj.FillHistogram("s800",histname,
                                 10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());


	    }// end IsInside(ResiduePID)
	  }// end iterating over gates.
	}//end haspids
 

      }// end of if(s800)











    }// end of caesar hit loop.
    obj.FillHistogram("Caesar","Multiplicity",300,0,300,valid_counter);
  } // I have a valid caesar event.

  if(numobj!=list->GetSize())
    list->Sort();
}
