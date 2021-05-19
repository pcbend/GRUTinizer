
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
#include "TS800Sim.h"
#include "TGretSim.h"

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

bool DTA(TRuntimeObjects &obj){

  TS800Sim *s800Sim = obj.GetDetector<TS800Sim>();
  
  if(!s800Sim || s800Sim->Size() < 1)
    return false;

  Double_t dta_min = GValue::Value("DTA_MIN");
  if(std::isnan(dta_min))
    dta_min = -0.06;
  Double_t dta_max = GValue::Value("DTA_MAX");
  if(std::isnan(dta_max))
    dta_max = 0.06;

  TS800SimHit hit = s800Sim->GetS800SimHit(0);
  
  std::string dirname = "S800";
  std::string histname = "dta";
  obj.FillHistogram(dirname, histname,
		    200, -0.1, 0.1,
		    hit.GetDTA());
  histname = "ata";
  obj.FillHistogram(dirname, histname,
		    200, -50., 50.,
		    hit.GetATA());
  histname = "bta";
  obj.FillHistogram(dirname, histname,
		    200, -50., 50.,
		    hit.GetBTA());
  histname = "ata_bta";
  obj.FillHistogram(dirname, histname,
		    200, -50., 50.,
		    hit.GetBTA(),
		    200, -50., 50.,
		    hit.GetATA());
  histname = "yta";
  obj.FillHistogram(dirname, histname,
		    200, -20, 20,
		    hit.GetYTA());

  // Rough DTA cut
  if(hit.GetDTA() < dta_max &&
     hit.GetDTA() > dta_min){
    histname = "dta_acc";
    obj.FillHistogram(dirname, histname,
		      200, -0.1, 0.1,
		      hit.GetDTA());
    histname = "ata_acc";
    obj.FillHistogram(dirname, histname,
		      200, -50., 50.,
		      hit.GetATA());
    histname = "bta_acc";
    obj.FillHistogram(dirname, histname,
		      200, -50., 50.,
		      hit.GetBTA());
    histname = "ata_bta_acc";
    obj.FillHistogram(dirname, histname,
		      200, -50., 50.,
		      hit.GetBTA(),
		      200, -50., 50.,
		      hit.GetATA());
    histname = "yta_acc";
    obj.FillHistogram(dirname, histname,
		      200, -20, 20,
		      hit.GetYTA());
  }
  
  return true;

}

Double_t measuredE(Double_t energy, Int_t xtalID){

  Double_t threshPar1 = GValue::Value(Form("THRESH_PAR_%i_1",xtalID));
  if(std::isnan(threshPar1))
    threshPar1 = 0.;
  Double_t threshPar2 = GValue::Value(Form("THRESH_PAR_%i_2",xtalID));
  if(std::isnan(threshPar2))
    threshPar2 = 0.001;

  // std::cout << Form("THRESH_PAR_%i_1",xtalID) << " = "
  // 	    << threshPar1 << std::endl;
  // std::cout << Form("THRESH_PAR_%i_2",xtalID) << " = "
  // 	    << threshPar2 << std::endl;

  Double_t threshold = (1.0 + tanh((energy-threshPar1)/threshPar2))/2.0;

  if(gRandom->Rndm() > threshold)
    return -1.;

  Double_t resPar1 = GValue::Value("RESOLUTION_PAR_1");
  Double_t resPar2 = GValue::Value("RESOLUTION_PAR_2");
  if(std::isnan(resPar1) || std::isnan(resPar2))
    return energy;
  else
    return energy*gRandom->Gaus(1.0, exp(resPar1*log(energy)+resPar2));
}

bool HandleAddback(TRuntimeObjects &obj) {

  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800Sim *s800Sim = obj.GetDetector<TS800Sim>();

  if(!gretina || !s800Sim)
    return false;

  //Rough dta acceptance cut
  Double_t dta_min = GValue::Value("DTA_MIN");
  if(std::isnan(dta_min))
    dta_min = -0.06;
  Double_t dta_max = GValue::Value("DTA_MAX");
  if(std::isnan(dta_max))
    dta_max = 0.06;
  if(s800Sim->GetS800SimHit(0).GetDTA() < dta_min ||
     s800Sim->GetS800SimHit(0).GetDTA() > dta_max)
    return false;
  
  std::string dirname = "addback";
  
  Double_t neighborLimit = 100.;
  
  Int_t    energyNChannels = 2000;
  Double_t energyLlim = 0.;
  Double_t energyUlim = 8000.;

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
	hit.GetCoreEnergy() < energyUlim )
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
    //    Int_t firstHitHoleNum;
    Double_t firstHitEnergy = 0;
    for(int i = 0; i < cluster.size(); i++){
      addbackEnergy += measuredE(cluster[i].GetCoreEnergy(),
				 cluster[i].GetCrystalId());

      // Find the hit with the largest energy deposit and save its position
      // for Doppler correction.

      //if(cluster[i].GetCoreEnergy() > firstHitEnergy){
      if(cluster[i].GetSegmentEng(cluster[i].GetFirstIntPoint()) >
	 firstHitEnergy){
	firstHitCrystalPos = cluster[i].GetCrystalPosition();
	//	firstHitHoleNum = cluster[i].GetHoleNumber();
	firstHitPos
	  = cluster[i].GetInteractionPosition(cluster[i].GetFirstIntPoint())
	  - targetOffset;
	//	firstHitEnergy = cluster[i].GetCoreEnergy();
	firstHitEnergy
	  = measuredE(cluster[i].GetSegmentEng(cluster[i].GetFirstIntPoint()),
		      cluster[i].GetCrystalId());
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
    Double_t gamma = 1/(sqrt(1-pow(beta,2)));
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
			energyNChannels/8, energyLlim, energyUlim/2, e1,
			energyNChannels/8, energyLlim, energyUlim/2, e2);
      obj.FillHistogram(dirname, histname,
			energyNChannels/8, energyLlim, energyUlim/2, e2,
			energyNChannels/8, energyLlim, energyUlim/2, e1);
    }
  }
  histname = Form("gam_gam_dop_n0n1n2_%.0f", beta*10000);
  for(int i = 0; i < dop_n0n1n2_energies.size(); i++){
    Double_t e1 = dop_n0n1n2_energies[i];
    for(int j = i+1; j < dop_n0n1n2_energies.size(); j++){
      Double_t e2 = dop_n0n1n2_energies[j];
      obj.FillHistogram(dirname, histname,
			energyNChannels/8, energyLlim, energyUlim/2, e1,
			energyNChannels/8, energyLlim, energyUlim/2, e2);
      obj.FillHistogram(dirname, histname,
			energyNChannels/8, energyLlim, energyUlim/2, e2,
			energyNChannels/8, energyLlim, energyUlim/2, e1);
    }
  }
  histname = Form("gam_gam_dop_n0n1n2ng_%.0f", beta*10000);
  for(int i = 0; i < dop_n0n1n2ng_energies.size(); i++){
    Double_t e1 = dop_n0n1n2ng_energies[i];
    for(int j = i+1; j < dop_n0n1n2ng_energies.size(); j++){
      Double_t e2 = dop_n0n1n2ng_energies[j];
      obj.FillHistogram(dirname, histname,
			energyNChannels/8, energyLlim, energyUlim/2, e1,
			energyNChannels/8, energyLlim, energyUlim/2, e2);
      obj.FillHistogram(dirname, histname,
			energyNChannels/8, energyLlim, energyUlim/2, e2,
			energyNChannels/8, energyLlim, energyUlim/2, e1);
    }
  }
  
  return true;
  
}

bool HandleSegmentAddback(TRuntimeObjects &obj) {

  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800Sim *s800Sim = obj.GetDetector<TS800Sim>();

  if(!gretina || !s800Sim || s800Sim->Size() < 1)
    return false;

  //Rough dta acceptance cut
  Double_t dta_min = GValue::Value("DTA_MIN");
  if(std::isnan(dta_min))
    dta_min = -0.06;
  Double_t dta_max = GValue::Value("DTA_MAX");
  if(std::isnan(dta_max))
    dta_max = 0.06;
  if(s800Sim->GetS800SimHit(0).GetDTA() < dta_min ||
     s800Sim->GetS800SimHit(0).GetDTA() > dta_max)
    return false;
  
  std::string dirname = "segAddback";

  Int_t    energyNChannels = 2000;
  Double_t energyLlim = 0.;
  Double_t energyUlim = 8000.;

  Double_t neighborLimit = GValue::Value("SEGMENT_NEIGHBOR_LIMIT");
  if(std::isnan(neighborLimit))
    neighborLimit = 0.;
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
	hit.GetCoreEnergy() < energyUlim )
      hits.push_back(hit);
  }

  // Addback
  while(hits.size() > 0){
    TGretinaHit currentHit = hits.back();
    hits.pop_back();

    // Find and add all hits in a cluster of hits including the current
    // hit. 
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
    //    Int_t firstHitHoleNum;
    Double_t firstHitEnergy = 0;
    for(int i = 0; i < cluster.size(); i++){
      addbackEnergy += measuredE(cluster[i].GetCoreEnergy(),
				 cluster[i].GetCrystalId());

      // Find the hit with the largest energy deposit and save its position
      // for Doppler correction.

      if(cluster[i].GetSegmentEng(cluster[i].GetFirstIntPoint()) >
	 firstHitEnergy){
	firstHitSegmentPos = cluster[i].GetCrystalPosition();
	//	firstHitHoleNum = cluster[i].GetHoleNumber();
	firstHitPos
	  = cluster[i].GetInteractionPosition(cluster[i].GetFirstIntPoint())
	    - targetOffset;
	firstHitEnergy
	  = measuredE(cluster[i].GetSegmentEng(cluster[i].GetFirstIntPoint()),
		      cluster[i].GetCrystalId());
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
    if(neighbors == 0 && cluster.size() == 1)
      addbackType = "n0";
    else if(neighbors == 1 && cluster.size() == 2)
      addbackType = "n1";
    else if(neighbors == 3 && cluster.size() == 3)
      addbackType = "n2";
    else
      addbackType = "ng";

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
    
  return true;
  
}

bool HandleGretina(TRuntimeObjects &obj) {

  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800Sim *s800Sim = obj.GetDetector<TS800Sim>();

  if(!gretina || !s800Sim)
    return false;

  //Rough dta acceptance cut
  Double_t dta_min = GValue::Value("DTA_MIN");
  if(std::isnan(dta_min))
    dta_min = -0.06;
  Double_t dta_max = GValue::Value("DTA_MAX");
  if(std::isnan(dta_max))
    dta_max = 0.06;
  if(s800Sim->GetS800SimHit(0).GetDTA() < dta_min ||
     s800Sim->GetS800SimHit(0).GetDTA() > dta_max)
    return false;
  
  std::string dirname = "gretina";

  Int_t    energyNChannels = 2000;
  Double_t energyLlim = 0.;
  Double_t energyUlim = 8000.;
   
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

    if( !hit.HasInteractions() ||
	hit.GetCoreEnergy() < energyLlim ||
	hit.GetCoreEnergy() > energyUlim )
      continue;

    std::string histname = "energy";
    Double_t e = measuredE(hit.GetCoreEnergy(),
			   hit.GetCrystalId());
    obj.FillHistogram(dirname, histname,
		      energyNChannels*4, energyLlim, energyUlim,
		      e);
    histname = "overview";
    obj.FillHistogram(dirname, histname,
		      energyNChannels*4, energyLlim, energyUlim,
		      e,
		      120, 0, 120,
		      hit.GetCrystalId());

    histname = "doppler";
    // obj.FillHistogram(dirname, histname,
    // 		      energyNChannels, energyLlim, energyUlim,
    // 		      hit.GetDoppler_2(beta, 0));

    // Do this "by hand" to use "measured" energy.
    TVector3 firstHitPos = hit.GetInteractionPosition(hit.GetFirstIntPoint())
      - targetOffset;
    Double_t gamma = 1/(sqrt(1-pow(beta,2)));
    Double_t edop = e*gamma*(1-beta*TMath::Cos(firstHitPos.Theta()));
    
    obj.FillHistogram(dirname, histname,
		      energyNChannels, energyLlim, energyUlim,
		      edop);

    // histname = "doppler_s800";
    // TVector3 track = s800->Track();
    // obj.FillHistogram(dirname, histname,
    // 		       energyNChannels, energyLlim, energyUlim,
    // 		       hit.GetDoppler(beta, &track));

    histname = "doppler_theta";
    // obj.FillHistogram(dirname, histname,
    // 		      100, 0, TMath::Pi(),
    // 		      hit.GetTheta(),
    // 		      energyNChannels, energyLlim, energyUlim,
    // 		      hit.GetDoppler_2(beta, 0));
    obj.FillHistogram(dirname, histname,
		      100, 0, TMath::Pi(),
		      hit.GetTheta(),
		      energyNChannels, energyLlim, energyUlim,
		      edop);

    histname = "theta_phi";
    obj.FillHistogram(dirname, histname,
		      500, 0, TMath::Pi(),
		      hit.GetPhi(),
		      500, 0, TMath::Pi(),
		      hit.GetTheta());

    histname = "theta";
    obj.FillHistogram(dirname, histname,
		      500, 0, TMath::Pi(),
		      hit.GetTheta());

    histname = "phi";
    obj.FillHistogram(dirname, histname,
		      500, 0, TMath::Pi(),
		      hit.GetPhi());

    if( hit.GetCrystalPosition().Theta() < TMath::Pi()/2. ){
      histname = "theta_phi_fw";
      obj.FillHistogram(dirname, histname,
			500, 0, TMath::Pi(),
			hit.GetPhi(),
			500, 0, TMath::Pi(),
			hit.GetTheta());
      histname = "theta_fw";
      obj.FillHistogram(dirname, histname,
			500, 0, TMath::Pi(),
			hit.GetTheta());

      histname = "phi_fw";
      obj.FillHistogram(dirname, histname,
			500, 0, TMath::Pi(),
			hit.GetPhi());
    } else {
      histname = "theta_phi_bw";
      obj.FillHistogram(dirname, histname,
			500, 0, TMath::Pi(),
			hit.GetPhi(),
			500, 0, TMath::Pi(),
			hit.GetTheta());
      histname = "theta_bw";
      obj.FillHistogram(dirname, histname,
			500, 0, TMath::Pi(),
			hit.GetTheta());

      histname = "phi_bw";
      obj.FillHistogram(dirname, histname,
			500, 0, TMath::Pi(),
			hit.GetPhi());
    }

  }

  return true;
}


extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  //std::cout << "---------------------------------" <<std::endl;
  //std::cout << " At the beginning" << std::endl;
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800Sim *s800Sim = obj.GetDetector<TS800Sim>();
  TGretSim *gretSim = obj.GetDetector<TGretSim>();
  //std::cout << " Dets Gotten" << std::endl;
  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  std::string histname = "";
  std::string dirname  = "";

  if(gretSim && gretSim->Size() > 0 &&
     s800Sim && s800Sim->Size() > 0){

    dirname = "sim";

    histname = "beta";
    obj.FillHistogram(dirname, histname,
		      500, 0, 0.5,
		      gretSim->GetGretinaSimHit(0).GetBeta());
    histname = "z";
    obj.FillHistogram(dirname, histname,
			1000,-50,50.,
			gretSim->GetGretinaSimHit(0).GetZ());
    histname = "beta_z";
    obj.FillHistogram(dirname, histname,
		      1000,-50,50.,
		      gretSim->GetGretinaSimHit(0).GetZ(),
		      300, 0.2, 0.5,
		      gretSim->GetGretinaSimHit(0).GetBeta());

    //Rough dta acceptance cut
    Double_t dta_min = GValue::Value("DTA_MIN");
    if(std::isnan(dta_min))
      dta_min = -0.06;
    Double_t dta_max = GValue::Value("DTA_MAX");
    if(std::isnan(dta_max))
      dta_max = 0.06;
    if(s800Sim->GetS800SimHit(0).GetDTA() > dta_min &&
       s800Sim->GetS800SimHit(0).GetDTA() < dta_max) {
      histname = "beta_acc";
      obj.FillHistogram(dirname, histname,
			500, 0, 0.5,
			gretSim->GetGretinaSimHit(0).GetBeta());
      histname = "z_acc";
      obj.FillHistogram(dirname, histname,
			1000,-50,50.,
			gretSim->GetGretinaSimHit(0).GetZ());
      histname = "beta_z_acc";
      obj.FillHistogram(dirname, histname,
			1000,-50,50.,
			gretSim->GetGretinaSimHit(0).GetZ(),
			300, 0.2, 0.5,
			gretSim->GetGretinaSimHit(0).GetBeta());
    }
  }
  
  if(s800Sim) {
    dirname = "S800";

    DTA(obj);

    if(gretina) {
      
      HandleGretina(obj);

      HandleAddback(obj);

      HandleSegmentAddback(obj);

    }
  }

  if(numobj!=list->GetSize())
    list->Sort();

}

