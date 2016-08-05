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


  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();
  if(!caesar)
    return;
  if(!s800)
    return;
  double ic_sum = s800->GetIonChamber().GetAve();
  double objtac_corr = s800->GetCorrTOF_OBJTAC();
  double objtac = s800->GetTof().GetTacOBJ();
  double xfptac = s800->GetTof().GetTacXFP();
  //double afp = s800->GetAFP();
  //double xfp_focalplane = s800->GetXFP(0);
  TVector3 track = s800->Track();
  if(! newal23blob->IsInside(objtac_corr,ic_sum) && InBeam_btwnTopMid->IsInside(objtac,xfptac)){
    return;
  }

  int counter=0;
  int vcounter=0;

  std::string histname;

  for(unsigned int y=0;y<caesar->Size();y++) {
    TCaesarHit hit = caesar->GetCaesarHit(y);
    counter++;
    if(!hit.IsValid())
      continue;
    vcounter++;
    if(newal23blob->IsInside(objtac_corr,ic_sum) && InBeam_btwnTopMid->IsInside(objtac,xfptac)){

      histname = "Gamma_Gated_newal23blob";
      obj.FillHistogram("Caesar",histname,
          1024,0,8192,hit.GetDoppler(beta,z_shift,&track));

      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("Caesar","Gamma_Gated_newal23blob_beta_scan",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
      }

    }//end if inside newal23blob and inside between incomingPID
  }
  obj.FillHistogram("Caesar","Multiplicity_All",300,0,300,counter);
  obj.FillHistogram("Caesar","Multiplicity_Valid",300,0,300,vcounter);
  counter = 0;
  vcounter =0;

  for(int y=0;y<caesar->AddbackSize();y++) {
    TCaesarHit hit = caesar->GetAddbackHit(y);
    counter++;
    if(!hit.IsValid())
      continue;
    vcounter++;
    if(newal23blob->IsInside(objtac_corr,ic_sum) && InBeam_btwnTopMid->IsInside(objtac,xfptac)){

      histname = "a_Gamma_Gated_newal23blob";
      obj.FillHistogram("Addback",histname,
          1024,0,8192,hit.GetDoppler(beta,z_shift,&track));

      for(int beta_i=0;beta_i<300;beta_i++){
		double beta_use = 0.2+(0.3/300.0)*double(beta_i);
		obj.FillHistogram("Addback","a_Gamma_Gated_newal23blob_beta_scan",
				  300,0.2,0.499,beta_use,
				  1024,0,8192,hit.GetDoppler(beta_use,z_shift,&track));
      }

    }//end if inside newal23blob and inside between incomingPID
  }
  obj.FillHistogram("Addback","a_Multiplicity_All",300,0,300,counter);
  obj.FillHistogram("Addback","a_Multiplicity_Valid",300,0,300,vcounter);


  if(numobj!=list->GetSize())
    list->Sort();
}
