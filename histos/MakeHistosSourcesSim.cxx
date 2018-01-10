
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
#include "TGretSim.h"
#include "GValue.h"


#include "TChannel.h"

#define Q1 15
#define Q2 7
#define Q3 8
#define Q4 16
#define Q5 9
#define Q6 14
#define Q7 17
#define Q8 6
#define Q9 19

//#define BETA .37

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
  TBank29  *bank29  = obj.GetDetector<TBank29>();
  TS800 *s800       = obj.GetDetector<TS800>();
  TGretSim *gretSim = obj.GetDetector<TGretSim>();
  
  Int_t    energyNChannels = 4000;
  Double_t energyLlim = 0.;
  Double_t energyUlim = 4000.;
  
  if(gretSim){
    for(int x=0; x<gretSim->Size(); x++){
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
    }
  }

  if(!gretina)
    return;
  
  Double_t res = 1./1000.; // Relative energy resolution
  
  Double_t calorimeterEnergy_gaus = 0.;
  std::vector<TGretinaHit> hits;

  // Gamma-gated crystal spectrum
  Double_t eGateLlim = 1327.5;
  Double_t eGateUlim = 1337.5;
  int iGate = -1;
  for(int i=0; i<gretina->Size(); i++){
    TGretinaHit hit = gretina->GetGretinaHit(i);
    if( hit.GetCoreEnergy() > eGateLlim &&
        hit.GetCoreEnergy() < eGateUlim )
      iGate = i;
  }
  if(iGate>=0){
    for(int i=0; i<gretina->Size(); i++){
      TGretinaHit hit = gretina->GetGretinaHit(i);
      if(i != iGate){
	obj.FillHistogram("energy",
			  Form("energy_gaus_%.0f",
			       (eGateLlim+eGateUlim)/2.0),
			  energyNChannels, energyLlim, energyUlim,
			  hit.GetCoreEnergy()*gRandom->Gaus(1,res));
	obj.FillHistogram("energy",
			  Form("fold_vs_energy_gaus_%.0f",
			       (eGateLlim+eGateUlim)/2.0),
			  energyNChannels/8, energyLlim, energyUlim,
			  hit.GetCoreEnergy()*gRandom->Gaus(1,res),
			  20, 0, 20, hit.NumberOfInteractions());

	// Count segment fold
	int segment_fold = hit.NumberOfInteractions();
	for(int y=0; y < hit.NumberOfInteractions(); y++)
	  for(int z = y+1; z < hit.NumberOfInteractions(); z++)
	    if(hit.GetSegmentId(y) == hit.GetSegmentId(z)){
	      segment_fold--;
	      break;
	    }

	obj.FillHistogram("energy",
			  Form("segfold_vs_energy_gaus_%.0f",
			       (eGateLlim+eGateUlim)/2.0),
			  energyNChannels/8, energyLlim, energyUlim,
			  hit.GetCoreEnergy()*gRandom->Gaus(1,res),
			  20, 0, 20, segment_fold);
	
      }
    }
  }

  // Addback preprocessing
  for(int x=0; x<gretina->Size(); x++){

    TGretinaHit hit = gretina->GetGretinaHit(x);
    
    if(hit.GetCoreEnergy() > energyLlim &&
       hit.GetCoreEnergy() < energyUlim){

      calorimeterEnergy_gaus += hit.GetCoreEnergy()*gRandom->Gaus(1,res);

      hits.push_back(hit);

    }
  }
  
  int max_layer = -1;

  for(int x=0; x<gretina->Size(); x++){

    TGretinaHit hit = gretina->GetGretinaHit(x);

    //                directory, histogram
    obj.FillHistogram("energy", "overview_gaus",
		      energyNChannels, energyLlim, energyUlim,
		      hit.GetCoreEnergy()*gRandom->Gaus(1,res),
		      100, 0, 100, hit.GetCrystalId());

    obj.FillHistogram("energy", "energy_gaus",
		      energyNChannels, energyLlim, energyUlim,
		      hit.GetCoreEnergy()*gRandom->Gaus(1,res));

    obj.FillHistogram("energy", "fold_vs_energy",
		      energyNChannels/8, energyLlim, energyUlim,
		      hit.GetCoreEnergy(),
		      20, 0, 20, hit.NumberOfInteractions());

    obj.FillHistogram("energy", "fold_vs_energy_gaus",
		      energyNChannels/8, energyLlim, energyUlim,
		      hit.GetCoreEnergy()*gRandom->Gaus(1,res),
		      20, 0, 20, hit.NumberOfInteractions());

    // Count segment fold
    int segment_fold = hit.NumberOfInteractions();
    for(int y=0; y < hit.NumberOfInteractions(); y++)
      for(int z = y+1; z < hit.NumberOfInteractions(); z++)
	if(hit.GetSegmentId(y) == hit.GetSegmentId(z)){
	  segment_fold--;
	  break;
	}

    obj.FillHistogram("energy",
		      "segfold_vs_energy_gaus",
		      energyNChannels/8, energyLlim, energyUlim,
		      hit.GetCoreEnergy()*gRandom->Gaus(1,res),
		      20, 0, 20, segment_fold);

    if( hit.GetCrystalId()%2 )
      obj.FillHistogram("energy",  "energy_gaus_A",
			energyNChannels, energyLlim, energyUlim,
			hit.GetCoreEnergy()*gRandom->Gaus(1,res));
    else
      obj.FillHistogram("energy",  "energy_gaus_B", 
			energyNChannels, energyLlim, energyUlim,
			hit.GetCoreEnergy()*gRandom->Gaus(1,res));
                        
    // Peter wrote these to give Dirk his Phi range (0-360).
    obj.FillHistogram("position", "theta_vs_phi",
		      360, 0., 360.,
		      hit.GetPhi()*TMath::RadToDeg(),
		      180, 0., 180.,
		      hit.GetTheta()*TMath::RadToDeg());

    // Position spectra in crystal coordinates
    if(hit.NumberOfInteractions()){
      obj.FillHistogram("position",
			Form("crys_%d_x", hit.GetCrystalId()),
			1200, -60, 60, hit.GetLocalPosition(0).X());

      obj.FillHistogram("position",
			Form("crys_%d_y", hit.GetCrystalId()),
			1200, -60, 60, hit.GetLocalPosition(0).Y());

      obj.FillHistogram("position",
			Form("crys_%d_z", hit.GetCrystalId()),
			1200, -10, 100, hit.GetLocalPosition(0).Z());

      obj.FillHistogram("position",
			Form("crys_%d_xy", hit.GetCrystalId()),
			1200, -60, 60, hit.GetLocalPosition(0).X(),
			1200, -60, 60, hit.GetLocalPosition(0).Y());

      obj.FillHistogram("position",
			Form("crys_%d_xz", hit.GetCrystalId()),
			1200, -60, 60,  hit.GetLocalPosition(0).X(),
			1200, -10, 100, hit.GetLocalPosition(0).Z());

      obj.FillHistogram("position",
			Form("crys_%d_yz", hit.GetCrystalId()),
			1200, -60, 60,  hit.GetLocalPosition(0).Y(),
			1200, -10, 100, hit.GetLocalPosition(0).Z());
    }
    
    for(int y=0; y < hit.NumberOfInteractions(); y++){

      int layer = hit.GetSegmentId(y)/6;

      if(layer > max_layer) max_layer = layer;
      
      obj.FillHistogram("layers",
			Form("theta_vs_phi_%s", LayerMap[layer].c_str()),
			360, 0., 360.,
			hit.GetPhi()*TMath::RadToDeg(),
			180, 0., 180.,
			hit.GetTheta()*TMath::RadToDeg());
    }

    obj.FillHistogram("layers", "max_layer", 12, -2, 10,
		      max_layer);

    if(max_layer == 5){
	obj.FillHistogram("energy", "energy_gaus_involves_phi",
			  energyNChannels, energyLlim, energyUlim,
			  hit.GetCoreEnergy()*gRandom->Gaus(1,res));
	obj.FillHistogram("energy", "overview_gaus_involves_phi",
			  energyNChannels, energyLlim, energyUlim,
			  hit.GetCoreEnergy()*gRandom->Gaus(1,res),
			  100, 0, 100, hit.GetCrystalId());
    }
    
    for(int k = 5; k > 0; k--){
      if(max_layer < k){
	obj.FillHistogram("energy",
			  Form("energy_gaus_below_%s", LayerMap[k].c_str()),
			  energyNChannels, energyLlim, energyUlim,
			  hit.GetCoreEnergy()*gRandom->Gaus(1,res));
	if( hit.GetCrystalId()%2 )
	  obj.FillHistogram("energy",
			    Form("energy_gaus_A_below_%s", LayerMap[k].c_str()),
			    energyNChannels, energyLlim, energyUlim,
			    hit.GetCoreEnergy()*gRandom->Gaus(1,res));
	else
	  obj.FillHistogram("energy",
			    Form("energy_gaus_B_below_%s", LayerMap[k].c_str()),
			    energyNChannels, energyLlim, energyUlim,
			    hit.GetCoreEnergy()*gRandom->Gaus(1,res));
      }

    }
    
  }
  
  // Addback
  obj.FillHistogram("addback",  "calorimeter_gaus",
		    energyNChannels, energyLlim, energyUlim,
		    calorimeterEnergy_gaus);
  for(int k = 5; k > 0; k--){
    if(max_layer < k){
      obj.FillHistogram("addback",
			Form("calorimeter_gaus_below_%s",
			     LayerMap[k].c_str()),
			energyNChannels, energyLlim, energyUlim,
			calorimeterEnergy_gaus);
    }
  }
  
  // For energy-gated addback spectra
  std::vector<Double_t> ABenergy;
  std::vector<TString> ABtype;
  Int_t    Naddback = 0;
  iGate = -1;
  
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
    Double_t addbackEnergy_gaus = 0.;
    for(int i = 0; i < cluster.size(); i++){
      addbackEnergy_gaus +=
	cluster[i].GetCoreEnergy()*gRandom->Gaus(1,res);
      for(int j = i+1; j < cluster.size(); j++){
	TVector3 distance =   cluster[i].GetCrystalPosition()
	                    - cluster[j].GetCrystalPosition();
	if(distance.Mag() < 80.) neighbors++;
      }
    }

    TString addbackType;
    if(neighbors == 0 && cluster.size() == 1)
      addbackType = "addback_n0";
    else if(neighbors == 1 && cluster.size() == 2)
      addbackType = "addback_n1";
    else if(neighbors == 3 && cluster.size() == 3)
      addbackType = "addback_n2";
    else
      addbackType = "addback_ng";

    // Fill addback histograms.

    obj.FillHistogram("addback",  addbackType+"_gaus",
		      energyNChannels, energyLlim, energyUlim,
		      addbackEnergy_gaus);

    if(addbackType == "addback_n0"
       || addbackType == "addback_n1"){
      obj.FillHistogram("addback",  "addback_n0n1_gaus",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy_gaus);
    }
    
    if(addbackType == "addback_n0"
       || addbackType == "addback_n1"
       || addbackType == "addback_n2"){
      obj.FillHistogram("addback",  "addback_n0n1n2_gaus",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy_gaus);
    }
    
    obj.FillHistogram("addback",  "clusterSize_vs_neighborPairs",
		      20, 0, 20, neighbors,
		      10, 0, 10, cluster.size());

    // For energy-gated addback spectra
    if(addbackEnergy_gaus > eGateLlim &&
       addbackEnergy_gaus < eGateUlim)
      iGate = Naddback;

    ABenergy.push_back(addbackEnergy_gaus);
    ABtype.push_back(addbackType);
    Naddback++;

  }

  // Fill energy-gated addback spectra
  if(iGate >= 0){
    for(int i = 0; i<Naddback; i++){
      if(i != iGate){
	obj.FillHistogram("addback",
			  ABtype[i]+Form("_gaus_%.0f",
					   (eGateLlim+eGateUlim)/2.0),
			  energyNChannels, energyLlim, energyUlim,
			  ABenergy[i]);

	if(ABtype[i] == "addback_n0"
	   || ABtype[i] == "addback_n1"){
	  obj.FillHistogram("addback",
			    Form("addback_n0n1_gaus_%.0f",
				 (eGateLlim+eGateUlim)/2.0),
			    energyNChannels, energyLlim, energyUlim,
			    ABenergy[i]);
	}
    
	if(ABtype[i] == "addback_n0"
	   || ABtype[i] == "addback_n1"
	   || ABtype[i] == "addback_n2"){
	  obj.FillHistogram("addback",
			    Form("addback_n0n1n2_gaus_%.0f",
				 (eGateLlim+eGateUlim)/2.0),
			    energyNChannels, energyLlim, energyUlim,
			    ABenergy[i]);
	}
	
      }
    }
  }
  
  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();
  
  if(numobj!=list->GetSize())
    list->Sort();
}
