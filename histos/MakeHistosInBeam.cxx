
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
#include "TS800Sim.h"
#include "TGretSim.h"
#include "GValue.h"


#include "TChannel.h"
#include "GValue.h"

#define Q1 15
#define Q2 7
#define Q3 8
#define Q4 16
#define Q5 9
#define Q6 14
#define Q7 17
#define Q8 6
#define Q9 19

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
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  //  TS800    *s800    = obj.GetDetector<TS800>();
  TS800Sim *s800Sim = obj.GetDetector<TS800Sim>();
  TGretSim *gretSim = obj.GetDetector<TGretSim>();

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  Int_t    energyNChannels = 8192;
  Double_t energyLlim = 0.;
  Double_t energyUlim = 8192.;

  if(gretSim){
    for(unsigned int x=0; x<gretSim->Size(); x++){
      TGretSimHit hit = gretSim->GetGretinaSimHit(x);
      obj.FillHistogram("sim","emitted_energy",
			energyNChannels, energyLlim, energyUlim,
			hit.GetEn());
      obj.FillHistogram("sim","emitted_theta",
			180, 0., 180.,
			hit.GetTheta()*TMath::RadToDeg());
      obj.FillHistogram("sim","emitted_phi",
			360, 0., 360.,
			hit.GetPhi()*TMath::RadToDeg());
      obj.FillHistogram("sim","emitted_z",
			1000,-50., 50.,
			hit.GetZ());
      obj.FillHistogram("sim","beta",
     			500, 0, 0.5,
     			hit.GetBeta());
      obj.FillHistogram("sim","beta_z",
     			1000,-5,5.,
     			hit.GetZ(),
     			300, 0.2, 0.5,
     			hit.GetBeta());
    }
  }
  
  if(!s800Sim)
    return;

  if(s800Sim->Size() > 0){
     // std::cout << "In MakeHistos:" << std::endl;
    
     // std::cout << "               time stamp = "
     //  	      << s800Sim->Timestamp() << std::endl;

     // std::cout << "                     size = "
     // 	       << s800Sim->Size()
     // 	       << std::endl;
     
     // std::cout << std::flush;

     Double_t dta = s800Sim->GetS800SimHit(0).GetDTA();

     // std::cout << "                      ATA = "
     //  	      << s800Sim->GetS800SimHit(0).GetATA() << std::endl;

     // std::cout << "                      BTA = "
     //    	      << s800Sim->GetS800SimHit(0).GetBTA() << std::endl;

     // std::cout << "                      DTA = "
     //  	      << dta << std::endl;
    

     obj.FillHistogram("s800","dta",
		       200, -0.10, 0.10,
		       dta);

     // Rough dta acceptance cut
     if(dta < -0.06 || dta > 0.06)
       return;
  
     obj.FillHistogram("s800","dta_cut",
		       200, -0.10, 0.10,
		       dta);

     obj.FillHistogram("s800","ata",
		       200, -100, 100,
		       s800Sim->GetS800SimHit(0).GetATA()*1000.);

     obj.FillHistogram("s800","bta",
		       200, -100, 100,
		       s800Sim->GetS800SimHit(0).GetBTA()*1000.);
     
     Double_t ata, bta, scatter;

     ata = s800Sim->GetS800SimHit(0).GetATA()*1000.;
     bta = s800Sim->GetS800SimHit(0).GetBTA()*1000.;
     scatter = sqrt(ata*ata + bta*bta);
     
     obj.FillHistogram("s800","scatter",
		       200, 0, 200,
		       scatter);
  }
  
  if(!gretina)
    return;

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
  TVector3 targetOffset(xoffset,yoffset,zoffset);

  // (optionally define extra beta values in gvalues file)
  const Int_t    nBetas = 1;
  Double_t betas[nBetas] = {beta};
    
  Double_t calorimeterEnergy = 0.;
  Double_t calorimeterEnergy_gaus = 0.;
  std::vector<TGretinaHit> hits;
  
  for(unsigned int x=0; x<gretina->Size(); x++){

    TGretinaHit hit = gretina->GetGretinaHit(x);

    // Addback preprocessing
    if(hit.GetCoreEnergy() > energyLlim &&
       hit.GetCoreEnergy() < energyUlim){

      calorimeterEnergy      += hit.GetCoreEnergy();
      calorimeterEnergy_gaus += hit.GetCoreEnergy()*gRandom->Gaus(1,1./1000.);

      hits.push_back(hit);

    }
    
    //                directory, histogram
    obj.FillHistogram("energy",  "overview",
		      energyNChannels, energyLlim, energyUlim,
		      hit.GetCoreEnergy(),
		      100, 0, 100, hit.GetCrystalId());

    obj.FillHistogram("energy",  "energy",
		      energyNChannels, energyLlim, energyUlim,
		      hit.GetCoreEnergy());
                        
    obj.FillHistogram("energy", "overview_gaus",
		      energyNChannels, energyLlim, energyUlim,
		      hit.GetCoreEnergy()*gRandom->Gaus(1,1./1000.),
		      100, 0, 100, hit.GetCrystalId());

    obj.FillHistogram("energy", "energy_gaus",
		      energyNChannels, energyLlim, energyUlim,
		      hit.GetCoreEnergy()*gRandom->Gaus(1,1./1000.));

    for(int i=0; i<nBetas; i++){

      // Symmetrized gamma-gamma
      Double_t e1 = hit.GetDoppler(betas[i])*gRandom->Gaus(1,1./1000.);
      for(unsigned int y=x+1; y<gretina->Size(); y++){

	TGretinaHit hit2 = gretina->GetGretinaHit(y);
	Double_t e2 = hit2.GetDoppler(betas[i])*gRandom->Gaus(1,1./1000.);
      
	obj.FillHistogram("energy",
			  Form("gamma_gamma_dop_%.0f_gaus", betas[i]*10000),
			  energyNChannels/16, energyLlim, energyUlim/2, e1,
			  energyNChannels/16, energyLlim, energyUlim/2, e2);
	obj.FillHistogram("energy",
			  Form("gamma_gamma_dop_%.0f_gaus", betas[i]*10000),
			  energyNChannels/8, energyLlim, energyUlim, e2,
			  energyNChannels/8, energyLlim, energyUlim, e1);
      }
    
      obj.FillHistogram("energy",
			Form("dop_%.0f", betas[i]*10000),
			energyNChannels, energyLlim, energyUlim,
			hit.GetDoppler(betas[i]));

      obj.FillHistogram("energy",
			Form("dop_%.0f_gaus", betas[i]*10000),
			energyNChannels, energyLlim, energyUlim,
			hit.GetDoppler(betas[i])*gRandom->Gaus(1,1./1000.));

      if(hit.GetHoleNumber() < 10){
	obj.FillHistogram("energy",
			  Form("dop_fw_%.0f_gaus", betas[i]*10000),
			  energyNChannels, energyLlim, energyUlim,
			  hit.GetDoppler(betas[i])*gRandom->Gaus(1,1./1000.));
      } else {
	obj.FillHistogram("energy",
			  Form("dop_bw_%.0f_gaus", betas[i]*10000),
			  energyNChannels, energyLlim, energyUlim,
			  hit.GetDoppler(betas[i])*gRandom->Gaus(1,1./1000.));
      }
    }
    
    obj.FillHistogram("position", "theta_vs_phi",
		      360, 0., 360.,
		      hit.GetPhi()*TMath::RadToDeg(),
		      180, 0., 180.,
		      hit.GetTheta()*TMath::RadToDeg());

    if(hit.GetHoleNumber() < 10){
      obj.FillHistogram("position", "theta_vs_phi_fw",
			360, 0., 360.,
			hit.GetPhi()*TMath::RadToDeg(),
			180, 0., 180.,
			hit.GetTheta()*TMath::RadToDeg());
    } else {
      obj.FillHistogram("position", "theta_vs_phi_bw",
			360, 0., 360.,
			hit.GetPhi()*TMath::RadToDeg(),
			180, 0., 180.,
			hit.GetTheta()*TMath::RadToDeg());
    }
    
  }
  
  // Addback
  obj.FillHistogram("addback",  "calorimeter",
		    energyNChannels, energyLlim, energyUlim,
		    calorimeterEnergy);
  obj.FillHistogram("addback",  "calorimeter_gaus",
		    energyNChannels, energyLlim, energyUlim,
		    calorimeterEnergy_gaus);

  while(hits.size() > 0){
    TGretinaHit currentHit = hits.back();
    hits.pop_back();
    
    // Find and add all hits in a cluster of adjacent crystals including
    // the current hit.
    //
    // CAUTION: This clustering includes neighbors of neighbors!
    std::vector<TGretinaHit> cluster;
    cluster.push_back(currentHit);
    unsigned int lastClusterSize = 0;
    while(lastClusterSize < cluster.size()){
      for(unsigned int i = 0; i < cluster.size(); i++){
	for(unsigned int j = 0; j < hits.size(); j++){
	  TVector3 distance = cluster[i].GetCrystalPosition()
	                       - hits[j].GetCrystalPosition();

	  obj.FillHistogram("position",  "crystal_separation",
			    1000, 0., 1000.,
			    distance.Mag());

	  if(distance.Mag() < 80.){ // Neighbors
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
    Double_t addbackEnergy_gaus = 0.;
    TVector3 firstHitPos;
    Int_t firstHitHoleNum = -1;
    Double_t firstHitEnergy = 0;
    for(unsigned int i = 0; i < cluster.size(); i++){
      addbackEnergy += cluster[i].GetCoreEnergy();
      addbackEnergy_gaus +=
	cluster[i].GetCoreEnergy()*gRandom->Gaus(1,1./1000.);

      // Find the largest IP in the cluster and save its position
      // for Doppler correction.
      //      if(cluster[i].GetSegmentEng(cluster[i].GetFirstIntPoint())
      //	 > firstHitEnergy){
      if(cluster[i].GetIntAssignEng(0) > firstHitEnergy){
	firstHitHoleNum = cluster[i].GetHoleNumber();
	firstHitPos = cluster[i].GetIntPosition(0) - targetOffset;
	firstHitEnergy = cluster[i].GetIntAssignEng(0);
      }
      
      for(unsigned int j = i+1; j < cluster.size(); j++){
	TVector3 distance =   cluster[i].GetCrystalPosition()
	                    - cluster[j].GetCrystalPosition();
	if(distance.Mag() < 80.) neighbors++;
      }
    }

    // Doppler correct the addback energy.
    // *** NEED TO ADD S800 TRAJECTORY ***

    Double_t dopplerABEnergy[nBetas] = {0.};
    Double_t dopplerABEnergy_gaus[nBetas] = {0.};
    for(int b=0; b<nBetas; b++){
      double gamma = 1/(sqrt(1-pow(betas[b],2)));
      dopplerABEnergy[b] =
	addbackEnergy*gamma*(1-betas[b]*TMath::Cos(firstHitPos.Theta()));
      dopplerABEnergy_gaus[b] =
	addbackEnergy_gaus*gamma*(1-betas[b]*TMath::Cos(firstHitPos.Theta()));
    }
    
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

    obj.FillHistogram("addback",  addbackType,
		      energyNChannels, energyLlim, energyUlim,
		      addbackEnergy);
    obj.FillHistogram("addback",  addbackType+"_gaus",
		      energyNChannels, energyLlim, energyUlim,
		      addbackEnergy_gaus);

    for(int b=0; b<nBetas; b++){
      obj.FillHistogram("addback",
			Form("dop_%s_%.0f",
			     addbackType.Data(),
			     betas[b]*10000),
			energyNChannels, energyLlim, energyUlim,
			dopplerABEnergy[b]);
      obj.FillHistogram("addback",
			Form("dop_%s_%.0f_gaus",
			     addbackType.Data(),
			     betas[b]*10000),
			energyNChannels, energyLlim, energyUlim,
			dopplerABEnergy_gaus[b]);
      if(firstHitHoleNum < 10){
	obj.FillHistogram("addback",
			  Form("dop_fw_%s_%.0f",
			       addbackType.Data(),
			       betas[b]*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy[b]);
	obj.FillHistogram("addback",
			  Form("dop_fw_%s_%.0f_gaus",
			       addbackType.Data(),
			       betas[b]*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy_gaus[b]);
      } else {
	obj.FillHistogram("addback",
			  Form("dop_bw_%s_%.0f",
			       addbackType.Data(),
			       betas[b]*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy[b]);
	obj.FillHistogram("addback",
			  Form("dop_bw_%s_%.0f_gaus",
			       addbackType.Data(),
			       betas[b]*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy_gaus[b]);
      }
    }
    
    if(addbackType == "n0"
       || addbackType == "n1"){
      obj.FillHistogram("addback",  "n0n1",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy);
      obj.FillHistogram("addback",  "n0n1_gaus",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy_gaus);
      for(int b=0; b<nBetas; b++){
	obj.FillHistogram("addback",
			  Form("dop_n0n1_%.0f",
			       betas[b]*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy[b]);
	obj.FillHistogram("addback",
			  Form("dop_n0n1_%.0f_gaus",
			       betas[b]*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy_gaus[b]);
	if(firstHitHoleNum < 10){
	  obj.FillHistogram("addback",
			    Form("dop_fw_n0n1_%.0f",
				 betas[b]*10000),
			    energyNChannels, energyLlim, energyUlim,
			    dopplerABEnergy[b]);
	  obj.FillHistogram("addback",
			    Form("dop_fw_n0n1_%.0f_gaus",
				 betas[b]*10000),
			    energyNChannels, energyLlim, energyUlim,
			    dopplerABEnergy_gaus[b]);
	} else {
	  obj.FillHistogram("addback",
			    Form("dop_bw_n0n1_%.0f",
				 betas[b]*10000),
			    energyNChannels, energyLlim, energyUlim,
			    dopplerABEnergy[b]);
	  obj.FillHistogram("addback",
			    Form("dop_bw_n0n1_%.0f_gaus",
				 betas[b]*10000),
			    energyNChannels, energyLlim, energyUlim,
			    dopplerABEnergy_gaus[b]);
	}
      }
    }
    
    if(addbackType == "n0"
       || addbackType == "n1"
       || addbackType == "n2"){
      obj.FillHistogram("addback",  "n0n1n2",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy);
      obj.FillHistogram("addback",  "n0n1n2_gaus",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy_gaus);
      for(int b=0; b<nBetas; b++){
	obj.FillHistogram("addback",
			  Form("dop_n0n1n2_%.0f",
			       betas[b]*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy[b]);
	obj.FillHistogram("addback",
			  Form("dop_n0n1n2_%.0f_gaus",
			       betas[b]*10000),
			  energyNChannels, energyLlim, energyUlim,
			  dopplerABEnergy_gaus[b]);
	if(firstHitHoleNum < 10){
	  obj.FillHistogram("addback",
			    Form("dop_fw_n0n1n2_%.0f",
				 betas[b]*10000),
			    energyNChannels, energyLlim, energyUlim,
			    dopplerABEnergy[b]);
	  obj.FillHistogram("addback",
			    Form("dop_fw_n0n1n2_%.0f_gaus",
				 betas[b]*10000),
			    energyNChannels, energyLlim, energyUlim,
			    dopplerABEnergy_gaus[b]);
	} else {
	  obj.FillHistogram("addback",
			    Form("dop_bw_n0n1n2_%.0f",
				 betas[b]*10000),
			    energyNChannels, energyLlim, energyUlim,
			    dopplerABEnergy[b]);
	  obj.FillHistogram("addback",
			    Form("dop_bw_n0n1n2_%.0f_gaus",
				 betas[b]*10000),
			    energyNChannels, energyLlim, energyUlim,
			    dopplerABEnergy_gaus[b]);
	}
      }
    }
    
    obj.FillHistogram("addback",  "clusterSize_vs_neighborPairs",
		      20, 0, 20, neighbors,
		      10, 0, 10, cluster.size());
    
  }
  
  if(numobj!=list->GetSize())
    list->Sort();

}
