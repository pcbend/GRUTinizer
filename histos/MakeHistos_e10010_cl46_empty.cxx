
#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>
#include <TLine.h>

#include "TGretina.h"
#include "TS800.h"
#include "TBank29.h"
#include "TS800.h"
#include "GCutG.h"

#include "TChannel.h"
#include "GValue.h"

#define Q1 15
#define Q2 7
#define Q3 11
#define Q4 1
#define Q5 22
#define Q6 14
#define Q7 12
#define Q8 6
#define Q9 21


std::map<int,int> HoleQMap;
std::map<int,std::string> LayerMap;

void InitMap() {
  HoleQMap[Q1] = 1;
  HoleQMap[Q2] = 2;
  HoleQMap[Q3] = 3;
  HoleQMap[Q4] = 4;
  HoleQMap[Q5] = 5;
  HoleQMap[Q6] = 6;
  HoleQMap[Q7] = 7;
  HoleQMap[Q8] = 8;
  HoleQMap[Q9] = 9;

  LayerMap[0] = "alpha";
  LayerMap[1] = "beta";
  LayerMap[2] = "gamma";
  LayerMap[3] = "delta";
  LayerMap[4] = "epsilon";
  LayerMap[5] = "phi";

}

#define INTEGRATION 128.0

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.

GCutG *incoming_cl46=0;

GCutG *outgoing_cl46=0;

GCutG *gt_time=0;

//GCutG *strange=0; // high energy events (4-8 MeV), slightly early in time

bool OutgoingS800(TRuntimeObjects &obj, TS800 *s800, GCutG *outgoing) {

  if(!s800)
    return false;
   
  std::string dirname = "S800";
  std::string histname;
  if(outgoing) {
    if(!outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
			   s800->GetIonChamber().GetSum()) )
      return false;
    //			         s800->GetIonChamber().Charge()) )
    histname = Form("incoming_%s", outgoing->GetName());
  } else {
    histname = "incoming";
  }
  obj.FillHistogram(dirname, histname,
		    1000, -4000., 0.,
		    s800->GetOBJ_E1Raw_MESY(),
		    1000, 0., 4000.,
		    s800->GetXF_E1Raw_MESY());

  return true;

}

bool IncomingS800(TRuntimeObjects &obj,TS800 *s800,GCutG *incoming) {
 
   if(!s800)
     return false;
   
   std::string dirname = "S800";
   std::string histname, ehistname;
   if(incoming) {
     if( !incoming->IsInside(s800->GetOBJ_E1Raw_MESY(),
			     s800->GetXF_E1Raw_MESY()) )
       return false;
     histname = Form("outgoing_%s", incoming->GetName());
     ehistname = Form("ICcharge_%s", incoming->GetName());
   } else {
     histname = "outgoing";
     ehistname = "ICcharge";
   }
   obj.FillHistogram(dirname, histname,
		     1000, -4000., 0.,
		     s800->GetCorrTOF_OBJ_MESY(),
		     1000, 0., 100000.,
   		     s800->GetIonChamber().GetSum());
     //		     s800->GetIonChamber().Charge());

   obj.FillHistogram(dirname, ehistname,
		     1000, 20000., 45000.,
		     s800->GetIonChamber().GetSum());
   //		     s800->GetIonChamber().Charge());

   // std::cout << "s800->GetCorrTOF_OBJ_MESY() = "
   // 	     << s800->GetCorrTOF_OBJ_MESY()
   // 	     << "s800->GetIonChamber().Charge() = "
   // 	     << s800->GetIonChamber().Charge()
   // 	     << std::endl;
   
   //   TMTof &mtof = s800->GetMTof();
   //histname = "RefSize";
   //obj.FillHistogram(dirname,histname,20,0,0,mtof.RefSize(),
   //                                   20,0,0,mtof.E1UpSize());
   
   //for(int i=0;i<mtof.E1UpSize();i++) {
   //  if(i<mtof.RefSize()) {
   //  //for(int j=0;j<mtof.E1UpSize();j++) {
   //    histname = "E1up";
   //    obj.FillHistogram(dirname,histname,2000,0,0,mtof.fE1Up.at(i)-mtof.fRef.at(i));
   //  }
   //  if(i<mtof.RfSize()) {
   //  //for(int j=0;j<mtof.RfSize();j++) {
   //    histname = "Rf";
   //    obj.FillHistogram(dirname,histname,5000,0,0,mtof.fE1Up.at(i)-mtof.fRf.at(i));
   //  }
   //  if(i<mtof.ObjSize()) {
   //  //for(int j=0;j<mtof.ObjSize();j++) {
   //    histname = "Obj";
   //    obj.FillHistogram(dirname,histname,5000,0,5000,mtof.fE1Up.at(i)-mtof.fObj.at(i));
   //  }
   //}

// Time corrections
//  histname = "time_x";
//  obj.FillHistogram(dirname,histname,2000,0,0,s800->GetCorrTOF_OBJ_MESY(),
//                                     2000,0,0,s800->GetCrdc(0).GetDispersiveX());
//
//  histname = "time_afp";
//  obj.FillHistogram(dirname,histname,2000,0,0,s800->GetCorrTOF_OBJ_MESY(),
//                                     2000,0,0,s800->GetAFP());
//
//  histname = "time_charge";
//  obj.FillHistogram(dirname,histname,2000,0,0,s800->GetCorrTOF_OBJ_MESY(),
//                                     2000,0,0,s800->GetIonChamber().Charge());

  return true;

}

bool DTA(TRuntimeObjects &obj, GCutG *incoming, GCutG *outgoing){

  TS800       *s800 = obj.GetDetector<TS800>();
  
  if(!s800)
    return false;
  
  if(!incoming || !outgoing)
    return false;

  if(!incoming->IsInside(s800->GetOBJ_E1Raw_MESY(),
			 s800->GetXF_E1Raw_MESY()) )
     return false;

  if(!outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
			 s800->GetIonChamber().GetSum()) )
    return false;
  //			 s800->GetIonChamber().Charge()) )

  std::string dirname = "S800";
  std::string histname = Form("dta_%s_%s",
			      incoming->GetName(),
			      outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    200, -0.1, 0.1,
		    s800->GetDta());

  histname = Form("ata_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    200, -0.1, 0.1,
		    s800->GetAta());
  histname = Form("bta_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    200, -0.1, 0.1,
		    s800->GetBta());
  histname = Form("ata_bta_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    200, -0.1, 0.1,
		    s800->GetBta(),
		    200, -0.1, 0.1,
		    s800->GetAta());
  histname = Form("yta_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    200, -20, 20,
		    s800->GetYta());

  return true;
}

bool TriggerRegister(TRuntimeObjects &obj, GCutG *incoming, GCutG *outgoing){

  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800       *s800 = obj.GetDetector<TS800>();

  if(!gretina || !s800)
    return false;
  
  std::string dirname = "S800";
  std::string histname;

  histname = "trigger_bit";
  unsigned short bits = s800->GetTrigger().GetRegistr();
  for(int j=0;j<16;j++) {
    if(((bits>>j)&0x0001))
      obj.FillHistogram(dirname, histname, 20, 0, 20, j);
  }
  histname = "trigger_raw";
  obj.FillHistogram(dirname, histname,
		    20, 0, 20,
		    s800->GetTrigger().GetRegistr());

  if(!incoming || !outgoing)
    return false;

  if(!incoming->IsInside(s800->GetOBJ_E1Raw_MESY(),
			 s800->GetXF_E1Raw_MESY()) )
     return false;

  if(!outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
			 s800->GetIonChamber().GetSum()) )
    return false;
  //			 s800->GetIonChamber().Charge()) )


  histname = "trigger_bit_PID";
  for(int j=0;j<16;j++) {
    if(((bits>>j)&0x0001))
      obj.FillHistogram(dirname, histname, 20, 0, 20, j);
  }
  histname = "trigger_raw_PID";
  obj.FillHistogram(dirname, histname,
		    20, 0, 20,
		    s800->GetTrigger().GetRegistr());
  return true;
  
}

bool HandleAddback(TRuntimeObjects &obj,GCutG *incoming,
		   GCutG *outgoing, GCutG *gt_time) {

  Double_t neighborLimit = 100.;
  
  Int_t    energyNChannels = 2000;
  Double_t energyLlim = 0.;
  Double_t energyUlim = 8000.;

  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800       *s800 = obj.GetDetector<TS800>();

  if(!gretina || !s800)
    return false;

  if(!incoming || !outgoing || !gt_time)
    return false;

  if(!incoming->IsInside(s800->GetOBJ_E1Raw_MESY(),
			 s800->GetXF_E1Raw_MESY()) )
     return false;

  if(!outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
			 s800->GetIonChamber().GetSum()) )
    return false;
    //                   s800->GetIonChamber().Charge()) )

  std::string dirname = Form("addback_%s_%s",
			     incoming->GetName(),
			     outgoing->GetName());
  
  double beta = GValue::Value("BETA");
  if(std::isnan(beta))
    beta=0.00;
  double xoffset = GValue::Value("GRETINA_X_OFFSET");
  if(std::isnan(xoffset))
    xoffset=0.00;
  double yoffset = GValue::Value("GRETINA_Y_OFFSET");
  if(std::isnan(yoffset))
    yoffset=0.00;
  double zoffset = GValue::Value("GRETINA_Z_OFFSET");
  if(std::isnan(zoffset))
    zoffset=0.00;
  TVector3 targetOffset(xoffset, yoffset, zoffset);

  std::vector<TGretinaHit> hits;
  
  // Preprocessing
  for(int x=0; x<gretina->Size(); x++){

    TGretinaHit hit = gretina->GetGretinaHit(x);

    if( hit.HasInteractions() &&
	hit.GetCoreEnergy() > energyLlim &&
	hit.GetCoreEnergy() < energyUlim &&
	gt_time->IsInside(s800->GetTimestamp()-hit.GetTime(),
			  hit.GetCoreEnergy()) )
	hits.push_back(hit);
  }

  // Addback
  std::vector<Double_t> dop_n0n1_energies;     // for gamma-gamma
  std::vector<Double_t> dop_n0n1n2_energies;   // for gamma-gamma
  std::vector<Double_t> dop_n0n1n2ng_energies; // for gamma-gamma
  while(hits.size() > 0){
    TGretinaHit currentHit = hits.back();
    hits.pop_back();
    
    // Find and add all hits in a cluster of adjacent crystals including
    // the current hit.
    //
    // CAUTION: This clustering includes neighbors of neighbors!
    std::vector<TGretinaHit> cluster;
    cluster.push_back(currentHit);
    int lastClusterSize = 0;
    while(lastClusterSize < cluster.size()){
      for(int i = 0; i < cluster.size(); i++){
	for(int j = 0; j < hits.size(); j++){
	  TVector3 distance = cluster[i].GetCrystalPosition()
	    - hits[j].GetCrystalPosition();
	  
	  //	  std::cout << " * distance.Mag() = " << distance.Mag()
	  //		    << std::endl;
	  
	  obj.FillHistogram(dirname,
			    "crystal_separation",
			    1000, 0., 1000.,
			    distance.Mag());
	  
	  if(distance.Mag() < neighborLimit){
	    cluster.push_back(hits.back());
	    hits.pop_back();
	  }
	}
      }
      lastClusterSize = cluster.size();
    }

    // Calculate the total energy deposited in the cluster,
    // and count the pairs of neighbors.
    Int_t neighbors = 0;
    Double_t addbackEnergy = 0.;
    TVector3 firstHitPos;
    TVector3 firstHitCrystalPos;
    Int_t firstHitHoleNum;
    Double_t firstHitEnergy = 0;
    for(int i = 0; i < cluster.size(); i++){
      addbackEnergy += cluster[i].GetCoreEnergy();

      // Find the hit with the largest energy deposit and save its position
      // for Doppler correction.

      //if(cluster[i].GetCoreEnergy() > firstHitEnergy){
      if(cluster[i].GetSegmentEng(cluster[i].GetFirstIntPoint()) >
	 firstHitEnergy){
	firstHitCrystalPos = cluster[i].GetCrystalPosition();
	firstHitHoleNum = cluster[i].GetHoleNumber();
	firstHitPos
	  = cluster[i].GetInteractionPosition(cluster[i].GetFirstIntPoint())
	    - targetOffset;
	//	firstHitEnergy = cluster[i].GetCoreEnergy();
	firstHitEnergy
	  = cluster[i].GetSegmentEng(cluster[i].GetFirstIntPoint());
      }
      
      for(int j = i+1; j < cluster.size(); j++){
	TVector3 distance =   cluster[i].GetCrystalPosition()
	  - cluster[j].GetCrystalPosition();
	if(distance.Mag() < neighborLimit) neighbors++;
      }
    }

    // Doppler correct the addback energy.
    // *** NEED TO ADD S800 TRAJECTORY ***

    Double_t dopplerABEnergy = 0.;
    double gamma = 1/(sqrt(1-pow(beta,2)));
    dopplerABEnergy =
      addbackEnergy*gamma*(1-beta*TMath::Cos(firstHitPos.Theta()));
    
    TString addbackType;
    if(neighbors == 0 && cluster.size() == 1){
      addbackType = "n0";
      dop_n0n1_energies.push_back(dopplerABEnergy);
      dop_n0n1n2_energies.push_back(dopplerABEnergy);
      dop_n0n1n2ng_energies.push_back(dopplerABEnergy);
    } else if(neighbors == 1 && cluster.size() == 2){
      addbackType = "n1";
      dop_n0n1_energies.push_back(dopplerABEnergy);
      dop_n0n1n2_energies.push_back(dopplerABEnergy);
      dop_n0n1n2ng_energies.push_back(dopplerABEnergy);
    } else if(neighbors == 3 && cluster.size() == 3){
      addbackType = "n2";
      dop_n0n1n2_energies.push_back(dopplerABEnergy);
      dop_n0n1n2ng_energies.push_back(dopplerABEnergy);
    } else {
      addbackType = "ng";
      dop_n0n1n2ng_energies.push_back(dopplerABEnergy);
    }

    // Fill addback histograms.
    obj.FillHistogram(dirname,  addbackType.Data(),
		      energyNChannels, energyLlim, energyUlim,
		      addbackEnergy);
    obj.FillHistogram(dirname,
		      Form("dop_%s_%.0f",
			   addbackType.Data(),
			   beta*10000),
		      energyNChannels, energyLlim, energyUlim,
		      dopplerABEnergy);
    //    if(firstHitHoleNum < 10){
    if( firstHitCrystalPos.Theta() < TMath::Pi()/2. ){
      obj.FillHistogram(dirname,
			Form("dop_fw_%s_%.0f",
			     addbackType.Data(),
			     beta*10000),
			energyNChannels, energyLlim, energyUlim,
			dopplerABEnergy);
    } else {
      obj.FillHistogram(dirname,
			Form("dop_bw_%s_%.0f",
			     addbackType.Data(),
			     beta*10000),
			energyNChannels, energyLlim, energyUlim,
			dopplerABEnergy);
    }
    
    if(addbackType == "n0"
       || addbackType == "n1"){
      obj.FillHistogram(dirname,  "n0n1",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy);
      obj.FillHistogram(dirname,
			Form("dop_n0n1_%.0f",
			     beta*10000),
			energyNChannels, energyLlim, energyUlim,
			dopplerABEnergy);
      //      if(firstHitHoleNum < 10){
      if( firstHitCrystalPos.Theta() < TMath::Pi()/2. ){
	obj.FillHistogram(dirname,
			  Form("dop_fw_n0n1_%.0f",
			       beta*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy);
      } else {
	obj.FillHistogram(dirname,
			  Form("dop_bw_n0n1_%.0f",
			       beta*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy);
      }
    }
      
    if(addbackType == "n0"
       || addbackType == "n1"
       || addbackType == "n2"){
      obj.FillHistogram(dirname,  "n0n1n2",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy);
      obj.FillHistogram(dirname,
			Form("dop_n0n1n2_%.0f",
			     beta*10000),
			energyNChannels, energyLlim, energyUlim,
			dopplerABEnergy);
      //      if(firstHitHoleNum < 10){
      if( firstHitCrystalPos.Theta() < TMath::Pi()/2. ){
	obj.FillHistogram(dirname,
			  Form("dop_fw_n0n1n2_%.0f",
			       beta*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy);
      } else {
	obj.FillHistogram(dirname,
			  Form("dop_bw_n0n1n2_%.0f",
			       beta*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy);
      }
    }
    
    obj.FillHistogram(dirname,  "clusterSize_vs_neighborPairs",
		      20, 0, 20, neighbors,
		      10, 0, 10, cluster.size());
  }
    
  // Symmetrized gamma-gamma
  std::string histname = Form("gam_gam_dop_n0n1_%.0f", beta*10000);
  for(int i = 0; i < dop_n0n1_energies.size(); i++){
    Double_t e1 = dop_n0n1_energies[i];
    for(int j = i+1; j < dop_n0n1_energies.size(); j++){
      Double_t e2 = dop_n0n1_energies[j];
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e1,
			energyNChannels/10, energyLlim, energyUlim/2, e2);
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e2,
			energyNChannels/10, energyLlim, energyUlim/2, e1);
    }
  }
  histname = Form("gam_gam_dop_n0n1n2_%.0f", beta*10000);
  for(int i = 0; i < dop_n0n1n2_energies.size(); i++){
    Double_t e1 = dop_n0n1n2_energies[i];
    for(int j = i+1; j < dop_n0n1n2_energies.size(); j++){
      Double_t e2 = dop_n0n1n2_energies[j];
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e1,
			energyNChannels/10, energyLlim, energyUlim/2, e2);
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e2,
			energyNChannels/10, energyLlim, energyUlim/2, e1);
    }
  }
  histname = Form("gam_gam_dop_n0n1n2ng_%.0f", beta*10000);
  for(int i = 0; i < dop_n0n1n2ng_energies.size(); i++){
    Double_t e1 = dop_n0n1n2ng_energies[i];
    for(int j = i+1; j < dop_n0n1n2ng_energies.size(); j++){
      Double_t e2 = dop_n0n1n2ng_energies[j];
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e1,
			energyNChannels/10, energyLlim, energyUlim/2, e2);
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e2,
			energyNChannels/10, energyLlim, energyUlim/2, e1);
    }
  }

  return true;
  
}

bool HandleSegmentAddback(TRuntimeObjects &obj,GCutG *incoming,
			  GCutG *outgoing, GCutG *gt_time) {

  Double_t neighborLimit = GValue::Value("SEGMENT_NEIGHBOR_LIMIT");
  if(std::isnan(neighborLimit))
    neighborLimit = 0.;
  
  Int_t    energyNChannels = 2000;
  Double_t energyLlim = 0.;
  Double_t energyUlim = 8000.;

  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800       *s800 = obj.GetDetector<TS800>();

  if(!gretina || !s800)
    return false;

  if(!incoming || !outgoing || !gt_time)
    return false;

  if(!incoming->IsInside(s800->GetOBJ_E1Raw_MESY(),
			 s800->GetXF_E1Raw_MESY()) )
     return false;

  if(!outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
			 s800->GetIonChamber().GetSum()) )
    return false;
    //                   s800->GetIonChamber().Charge()) )

  std::string dirname = Form("segAddback_%s_%s",
			     incoming->GetName(),
			     outgoing->GetName());
  
  double beta = GValue::Value("BETA");
  if(std::isnan(beta))
    beta=0.00;
  double xoffset = GValue::Value("GRETINA_X_OFFSET");
  if(std::isnan(xoffset))
    xoffset=0.00;
  double yoffset = GValue::Value("GRETINA_Y_OFFSET");
  if(std::isnan(yoffset))
    yoffset=0.00;
  double zoffset = GValue::Value("GRETINA_Z_OFFSET");
  if(std::isnan(zoffset))
    zoffset=0.00;
  TVector3 targetOffset(xoffset, yoffset, zoffset);

  std::vector<TGretinaHit> hits;
  
  // Preprocessing
  for(int x=0; x<gretina->Size(); x++){

    TGretinaHit hit = gretina->GetGretinaHit(x);

    if( hit.HasInteractions() &&
	hit.GetCoreEnergy() > energyLlim &&
	hit.GetCoreEnergy() < energyUlim &&
	gt_time->IsInside(s800->GetTimestamp()-hit.GetTime(),
			  hit.GetCoreEnergy()) )
      hits.push_back(hit);
  }

  // Addback
  std::vector<Double_t> dop_n0n1_energies;     // for gamma-gamma
  std::vector<Double_t> dop_n0n1n2_energies;   // for gamma-gamma
  std::vector<Double_t> dop_n0n1n2ng_energies; // for gamma-gamma
  while(hits.size() > 0){
    TGretinaHit currentHit = hits.back();
    hits.pop_back();
    

    // Find and add all hits in a cluster of adjacent crystals including
    // the current hit.
    //
    // CAUTION: This clustering includes neighbors of neighbors!
    std::vector<TGretinaHit> cluster;
    cluster.push_back(currentHit);
    int lastClusterSize = 0;
    while(lastClusterSize < cluster.size()){
      for(int i = 0; i < cluster.size(); i++){
	for(int j = 0; j < hits.size(); j++){
	  TVector3 distance = cluster[i].GetSegmentPosition()
	    - hits[j].GetSegmentPosition();
	  
	  obj.FillHistogram(dirname,
			    "segment_separation",
			    1000, 0., 1000.,
			    distance.Mag());
	  if(distance.Mag() < neighborLimit){
	    cluster.push_back(hits.back());
	    hits.pop_back();
	  }
	}
      }
      lastClusterSize = cluster.size();
    }

    // Calculate the total energy deposited in the cluster,
    // and count the pairs of neighbors.
    Int_t neighbors = 0;
    Double_t addbackEnergy = 0.;
    TVector3 firstHitPos;
    TVector3 firstHitSegmentPos;
    Int_t firstHitHoleNum;
    Double_t firstHitEnergy = 0;
    for(int i = 0; i < cluster.size(); i++){
      addbackEnergy += cluster[i].GetCoreEnergy();

      // Find the hit with the largest energy deposit and save its position
      // for Doppler correction.

      if(cluster[i].GetSegmentEng(cluster[i].GetFirstIntPoint()) >
	 firstHitEnergy){
	firstHitSegmentPos = cluster[i].GetCrystalPosition();
	firstHitHoleNum = cluster[i].GetHoleNumber();
	firstHitPos
	  = cluster[i].GetInteractionPosition(cluster[i].GetFirstIntPoint())
	    - targetOffset;
	firstHitEnergy
	  = cluster[i].GetSegmentEng(cluster[i].GetFirstIntPoint());
      }
      
      for(int j = i+1; j < cluster.size(); j++){
	TVector3 distance =   cluster[i].GetSegmentPosition()
	  - cluster[j].GetSegmentPosition();
	if(distance.Mag() < neighborLimit) neighbors++;
      }
    }

    // Doppler correct the addback energy.
    // *** NEED TO ADD S800 TRAJECTORY ***

    Double_t dopplerABEnergy = 0.;
    double gamma = 1/(sqrt(1-pow(beta,2)));
    dopplerABEnergy =
      addbackEnergy*gamma*(1-beta*TMath::Cos(firstHitPos.Theta()));
    
    TString addbackType;
    if(neighbors == 0 && cluster.size() == 1){
      addbackType = "n0";
      dop_n0n1_energies.push_back(dopplerABEnergy);
      dop_n0n1n2_energies.push_back(dopplerABEnergy);
      dop_n0n1n2ng_energies.push_back(dopplerABEnergy);
    } else if(neighbors == 1 && cluster.size() == 2){
      addbackType = "n1";
      dop_n0n1_energies.push_back(dopplerABEnergy);
      dop_n0n1n2_energies.push_back(dopplerABEnergy);
      dop_n0n1n2ng_energies.push_back(dopplerABEnergy);
    } else if(neighbors == 3 && cluster.size() == 3){
      addbackType = "n2";
      dop_n0n1n2_energies.push_back(dopplerABEnergy);
      dop_n0n1n2ng_energies.push_back(dopplerABEnergy);
    } else {
      addbackType = "ng";
      dop_n0n1n2ng_energies.push_back(dopplerABEnergy);
    }

    // Fill addback histograms.
    obj.FillHistogram(dirname,  addbackType.Data(),
		      energyNChannels, energyLlim, energyUlim,
		      addbackEnergy);
    obj.FillHistogram(dirname,
		      Form("dop_%s_%.0f",
			   addbackType.Data(),
			   beta*10000),
		      energyNChannels, energyLlim, energyUlim,
		      dopplerABEnergy);
    //    if(firstHitHoleNum < 10){
    //    if( firstHitCrystalPos.Theta() < TMath::Pi()/2. ){
    if( firstHitSegmentPos.Theta() < TMath::Pi()/2. ){
      obj.FillHistogram(dirname,
			Form("dop_fw_%s_%.0f",
			     addbackType.Data(),
			     beta*10000),
			energyNChannels, energyLlim, energyUlim,
			dopplerABEnergy);
    } else {
      obj.FillHistogram(dirname,
			Form("dop_bw_%s_%.0f",
			     addbackType.Data(),
			     beta*10000),
			energyNChannels, energyLlim, energyUlim,
			dopplerABEnergy);
    }
    
    if(addbackType == "n0"
       || addbackType == "n1"){
      obj.FillHistogram(dirname,  "n0n1",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy);
      obj.FillHistogram(dirname,
			Form("dop_n0n1_%.0f",
			     beta*10000),
			energyNChannels, energyLlim, energyUlim,
			dopplerABEnergy);
      //      if(firstHitHoleNum < 10){
      //      if( firstHitCrystalPos.Theta() < TMath::Pi()/2. ){
      if( firstHitSegmentPos.Theta() < TMath::Pi()/2. ){
	obj.FillHistogram(dirname,
			  Form("dop_fw_n0n1_%.0f",
			       beta*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy);
      } else {
	obj.FillHistogram(dirname,
			  Form("dop_bw_n0n1_%.0f",
			       beta*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy);
      }
    }
      
    if(addbackType == "n0"
       || addbackType == "n1"
       || addbackType == "n2"){
      obj.FillHistogram(dirname,  "n0n1n2",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy);
      obj.FillHistogram(dirname,
			Form("dop_n0n1n2_%.0f",
			     beta*10000),
			energyNChannels, energyLlim, energyUlim,
			dopplerABEnergy);
      //      if(firstHitHoleNum < 10){
      //      if( firstHitCrystalPos.Theta() < TMath::Pi()/2. ){
      if( firstHitSegmentPos.Theta() < TMath::Pi()/2. ){
	obj.FillHistogram(dirname,
			  Form("dop_fw_n0n1n2_%.0f",
			       beta*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy);
      } else {
	obj.FillHistogram(dirname,
			  Form("dop_bw_n0n1n2_%.0f",
			       beta*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy);
      }
    }
    
    obj.FillHistogram(dirname,  "clusterSize_vs_neighborPairs",
		      20, 0, 20, neighbors,
		      10, 0, 10, cluster.size());
  }
    
  // Symmetrized gamma-gamma
  std::string histname = Form("gam_gam_dop_n0n1_%.0f", beta*10000);
  for(int i = 0; i < dop_n0n1_energies.size(); i++){
    Double_t e1 = dop_n0n1_energies[i];
    for(int j = i+1; j < dop_n0n1_energies.size(); j++){
      Double_t e2 = dop_n0n1_energies[j];
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e1,
			energyNChannels/10, energyLlim, energyUlim/2, e2);
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e2,
			energyNChannels/10, energyLlim, energyUlim/2, e1);
    }
  }
  histname = Form("gam_gam_dop_n0n1n2_%.0f", beta*10000);
  for(int i = 0; i < dop_n0n1n2_energies.size(); i++){
    Double_t e1 = dop_n0n1n2_energies[i];
    for(int j = i+1; j < dop_n0n1n2_energies.size(); j++){
      Double_t e2 = dop_n0n1n2_energies[j];
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e1,
			energyNChannels/10, energyLlim, energyUlim/2, e2);
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e2,
			energyNChannels/10, energyLlim, energyUlim/2, e1);
    }
  }
  histname = Form("gam_gam_dop_n0n1n2ng_%.0f", beta*10000);
  for(int i = 0; i < dop_n0n1n2ng_energies.size(); i++){
    Double_t e1 = dop_n0n1n2ng_energies[i];
    for(int j = i+1; j < dop_n0n1n2ng_energies.size(); j++){
      Double_t e2 = dop_n0n1n2ng_energies[j];
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e1,
			energyNChannels/10, energyLlim, energyUlim/2, e2);
      obj.FillHistogram(dirname, histname,
			energyNChannels/10, energyLlim, energyUlim/2, e2,
			energyNChannels/10, energyLlim, energyUlim/2, e1);
    }
  }

  return true;
  
}

bool HandleGretina(TRuntimeObjects &obj,GCutG *incoming,
		   GCutG *outgoing, GCutG *gt_time) {

   if(!incoming || !outgoing)
     return false;

   TGretina *gretina = obj.GetDetector<TGretina>();
   TS800 *s800       = obj.GetDetector<TS800>();

   if(!gretina || !s800)
     return false;

   if(!((s800->GetTrigger().GetRegistr()&0x0002)>>1))
     return false;

   if(!incoming->IsInside(s800->GetOBJ_E1Raw_MESY(),
			  s800->GetXF_E1Raw_MESY()) )
     return false;

   if(!outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
   			  s800->GetIonChamber().GetSum()) )
     return false;
   //			  s800->GetIonChamber().Charge()) )   
   
   std::string dirname = "gretina";
   
   double beta = GValue::Value("BETA");
   if(std::isnan(beta))
     beta=0.00;
   double xoffset = GValue::Value("GRETINA_X_OFFSET");
   if(std::isnan(xoffset))
     xoffset=0.00;
   double yoffset = GValue::Value("GRETINA_Y_OFFSET");
   if(std::isnan(yoffset))
     yoffset=0.00;
   double zoffset = GValue::Value("GRETINA_Z_OFFSET");
   if(std::isnan(zoffset))
     zoffset=0.00;
   TVector3 targetOffset(xoffset, yoffset, zoffset);

   for(int x=0;x<gretina->Size();x++) {
     TGretinaHit hit = gretina->GetGretinaHit(x);

     std::string histname = Form("doppler_%s",outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       2000, 0, 8000,
		       hit.GetDoppler_2(beta, 0));

     histname = Form("doppler_s800_%s",outgoing->GetName());
     TVector3 track = s800->Track();
     obj.FillHistogram(dirname, histname,
		       2000, 0, 8000,
		       hit.GetDoppler_2(beta, &track));

     histname = Form("doppler_theta_%s",outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       100, 0, TMath::Pi(),
		       hit.GetTheta(),
		       2000, 0, 8000,
		       hit.GetDoppler_2(beta, 0));
     // if( strange->IsInside(s800->GetTimestamp()-hit.GetTimestamp(),
     // 			  hit.GetCoreEnergy()) ){
     //   histname = Form("doppler_%s_strange",outgoing->GetName());
     //   obj.FillHistogram(dirname, histname,
     // 			 2000, 0, 8000,
     // 			 hit.GetDoppler_2(beta, 0));
     //   histname = Form("doppler_theta_%s_strange",outgoing->GetName());
     //   obj.FillHistogram(dirname, histname,
     // 			 100, 0, TMath::Pi(),
     // 			 hit.GetTheta(),
     // 			 2000, 0, 8000,
     // 			 hit.GetDoppler_2(beta, 0));
     // }

     histname = Form("theta_phi_%s",outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       500, 0, TMath::Pi(),
		       hit.GetPhi(),
		       500, 0, TMath::Pi(),
		       hit.GetTheta());
     histname = Form("theta_%s",outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       500, 0, TMath::Pi(),
		       hit.GetTheta());
     histname = Form("phi_%s",outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       500, 0, TMath::Pi(),
		       hit.GetPhi());
       
     if( hit.GetCrystalPosition().Theta() < TMath::Pi()/2. ){
       histname = Form("theta_phi_fw_%s",outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetPhi(),
			 500, 0, TMath::Pi(),
			 hit.GetTheta());
       histname = Form("theta_fw_%s",outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetTheta());
       histname = Form("phi_fw_%s",outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetPhi());
     } else {
       histname = Form("theta_phi_bw_%s",outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetPhi(),
			 500, 0, TMath::Pi(),
			 hit.GetTheta());
       histname = Form("theta_bw_%s",outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetTheta());
       histname = Form("phi_bw_%s",outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetPhi());
     }

     // Time cut
     if(!gt_time)
       return false;

     if( !gt_time->IsInside(s800->GetTimestamp()-hit.GetTime(),
			    hit.GetCoreEnergy()) )
       return false;
  
     histname = Form("doppler_%s_t",outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       2000, 0, 8000,
		       hit.GetDoppler_2(beta, 0));

     histname = Form("doppler_s800_%s_t",outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       2000, 0, 8000,
		       hit.GetDoppler_2(beta, &track));

     histname = Form("doppler_theta_%s_t",outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       100, 0, TMath::Pi(),
		       hit.GetTheta(),
		       2000, 0, 8000,
		       hit.GetDoppler_2(beta, 0));

   }


  return true;
}


extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  //std::cout << "---------------------------------" <<std::endl;
  //std::cout << " At the beginning" << std::endl;
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank29  *bank29  = obj.GetDetector<TBank29>();
  TS800 *s800       = obj.GetDetector<TS800>();
  //std::cout << " Dets Gotten" << std::endl;
  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  TList *gates = &(obj.GetGates());
  if(!incoming_cl46) {
    incoming_cl46 = (GCutG*)gates->FindObject("in46Cl");
  }
  
  if(!outgoing_cl46) {
    outgoing_cl46 = (GCutG*)gates->FindObject("out46Cl");
  }

  if(!gt_time) {
    gt_time = (GCutG*)gates->FindObject("gt_time");
  }

  // if(!strange) {
  //   strange = (GCutG*)gates->FindObject("strange");
  // }

  std::string histname = "";
  std::string dirname  = "";

  if(bank29) {
    for(unsigned int x=0;x<bank29->Size();x++) {
      TMode3Hit &hit = (TMode3Hit&)bank29->GetHit(x);
      dirname = "Bank29";
      histname = Form("bank29_%i",hit.GetChannel());
      obj.FillHistogram(dirname,histname,
          16000,0,64000,hit.Charge());
    }
    if(s800) {
      histname = "S800_Bank29_time";
      dirname  = "Bank29";
      obj.FillHistogram(dirname,histname,
          200,-200,200,bank29->Timestamp()-s800->Timestamp());
    }
  }

  if(s800) {
    dirname = "S800";

    IncomingS800(obj, s800, 0);
    IncomingS800(obj, s800, incoming_cl46);

    OutgoingS800(obj, s800, 0);
    OutgoingS800(obj, s800, outgoing_cl46);

    DTA(obj, incoming_cl46, outgoing_cl46);

    if(gretina) {
      
      HandleGretina(obj, incoming_cl46, outgoing_cl46, gt_time);

      HandleAddback(obj, incoming_cl46, outgoing_cl46, gt_time);

      HandleSegmentAddback(obj, incoming_cl46, outgoing_cl46, gt_time);

      dirname = "gretina";
      for(unsigned int i=0;i<gretina->Size();i++) {
	TGretinaHit hit = gretina->GetGretinaHit(i);
	histname = "dtime_all";
	obj.FillHistogram(dirname,histname,
			  500, -250, 250,
			  s800->GetTimestamp()-hit.GetTimestamp(),
			  2000, 0, 8000,
			  hit.GetCoreEnergy());
	histname = "dtimet0_all";
	obj.FillHistogram(dirname,histname,
			  500, 0, 500,
			  s800->GetTimestamp()-hit.GetTime(),
			  2000, 0, 8000,
			  hit.GetCoreEnergy());
	unsigned short bits = s800->GetTrigger().GetRegistr();
	for(int j=0;j<5;j++) {
	  if((bits>>j)&0x0001) {
	    histname = Form("dtime_all_reg%i",j);
	    obj.FillHistogram(dirname,histname,
			      500, -250, 250,
			      s800->GetTimestamp()-hit.GetTimestamp(),
			      1000, 0, 8000,
			      hit.GetCoreEnergy());
	    histname = Form("dtimet0_reg%i",j);
	    obj.FillHistogram(dirname,histname,
			      500, 0, 500,
			      s800->GetTimestamp()-hit.GetTime(),
			      1000, 0, 8000,
			      hit.GetCoreEnergy());
	    if(incoming_cl46 && !outgoing_cl46 &&
	       incoming_cl46->IsInside(s800->GetOBJ_E1Raw_MESY(),
				      s800->GetXF_E1Raw_MESY()) &&
	       outgoing_cl46->IsInside(s800->GetCorrTOF_OBJ_MESY(),
				      s800->GetIonChamber().GetSum()) ){
	      //		      s800->GetIonChamber().Charge()) ){

	      histname = Form("dtime_all_reg%i_%s_%s",j,
			      incoming_cl46->GetName(),
			      outgoing_cl46->GetName());
	      obj.FillHistogram(dirname,histname,
				500, -250, 250,
				s800->GetTimestamp()-hit.GetTimestamp(),
				1000, 0, 8000,
				hit.GetCoreEnergy());
	      histname = Form("dtimet0_reg%i_%s_%s",j,
			      incoming_cl46->GetName(),
			      outgoing_cl46->GetName());
	      obj.FillHistogram(dirname,histname,
				500, 0, 500,
				s800->GetTimestamp()-hit.GetTime(),
				1000, 0, 8000,
				hit.GetCoreEnergy());
	    }
	  }
	}
      }
    }
    
   // histname = "TDC_vs_DispX";
   // obj.FillHistogram(dirname,histname,
   //     4000,-4000,4000,s800->GetCorrTOF_OBJ(),
   //     600,-300,300,s800->GetCrdc(0).GetDispersiveX());

   // histname ="PID_TDC";
   // obj.FillHistogram(dirname,histname,
   //     2000,0,0,s800->GetCorrTOF_OBJ(),
   //     2000,0,0,s800->GetIonChamber().Charge());

    //double delta_t = s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ();
    //if(delta_t>E1_TDC_low && delta_t<E1_TDC_high){
    //  histname ="PID_TDC_Prompt";
    //  obj.FillHistogram(dirname,histname,
    //      8000,-4000,4000,s800->GetCorrTOF_OBJ(),
    //      2000,10000,60000,s800->GetIonChamber().Charge());
    //}

    //std::cout << " In gret loop + s800 before haspids" << std::endl;
    //std::cout << " In gret loop + s800 before haspids" << std::endl;
    //histname = "E1_m_TDC";
    //obj.FillHistogram(histname,8000,-8000,8000,s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ());

    //histname = "E1Raw";
    //obj.FillHistogram(histname,2000,0,8000,s800->GetScint().GetTimeUp());

    //histname = "M_E1Raw";
    //for(int z=0;z<s800->GetMTof().E1UpSize();z++)
    //  obj.FillHistogram(histname,2000,0,8000,s800->GetMTof().fE1Up[z]);

    histname = "CRDC1_X";
    dirname  = "CRDC";
    obj.FillHistogram(dirname,histname,800,-400,400,
		      s800->GetCrdc(0).GetDispersiveX());

    histname = "CRDC2_X";
    obj.FillHistogram(dirname,histname,800,-400,400,
		      s800->GetCrdc(1).GetDispersiveX());

  }

  if(numobj!=list->GetSize())
    list->Sort();

}

