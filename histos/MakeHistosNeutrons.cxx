
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
  // TBank29  *bank29  = obj.GetDetector<TBank29>();
  // TS800 *s800       = obj.GetDetector<TS800>()
  TGretSim *gretSim = obj.GetDetector<TGretSim>();

  Int_t    energyNChannels = 10000;
  Double_t energyLlim = 0.;
  Double_t energyUlim = 10000.;
  
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
      obj.FillHistogram("sim","emitted_x",
                        2000,-100., 100.,
                        hit.GetX());
      obj.FillHistogram("sim","emitted_y",
                        2000,-100., 100.,
                        hit.GetY());
      obj.FillHistogram("sim","emitted_z",
                        2000,-100., 100.,
                        hit.GetZ());
    }
  }
  
  if(!gretina)
    return;
  
  Double_t calorimeterEnergy = 0.;
  std::vector<TGretinaHit> hits;
  
  for(int x=0; x<gretina->Size(); x++){
    TGretinaHit hit = gretina->GetGretinaHit(x);

    // Addback preprocessing
    if(hit.GetCoreEnergy() > energyLlim &&
       hit.GetCoreEnergy() < energyUlim){

      calorimeterEnergy      += hit.GetCoreEnergy();

      hits.push_back(hit);

    }
    
    //                directory, histogram
    //    obj.FillHistogram("energy",  "overview",
    //		      energyNChannels, energyLlim, energyUlim,
    //		      hit.GetCoreEnergy(),
    //		      100, 0, 100, hit.GetCrystalId());

    obj.FillHistogram("energy",  "energy",
		      energyNChannels, energyLlim, energyUlim,
		      hit.GetCoreEnergy());

    obj.FillHistogram("energy",  "energy_gaus",
		      energyNChannels, energyLlim, energyUlim,
		      hit.GetCoreEnergy()*gRandom->Gaus(1,1./1000.));

    if(gretSim && gretSim->Size()>0){
      TGretSimHit simhit = gretSim->GetGretinaSimHit(0);
      obj.FillHistogram("energy",  "GT_energy_gaus_vs_neutron_energy",
			energyNChannels/5, energyLlim, energyUlim,
			simhit.GetEn(),
			energyNChannels/5, energyLlim, energyUlim,
			hit.GetCoreEnergy()*gRandom->Gaus(1,1./1000.));
      obj.FillHistogram("energy",  "neutron_energy_minus_GT_energy_gaus",
			2*energyNChannels, -energyUlim, energyUlim,
			simhit.GetEn()
			- hit.GetCoreEnergy()*gRandom->Gaus(1,1./1000.));
      if( simhit.GetEn() < hit.GetCoreEnergy() )
	obj.FillHistogram("energy",  "GT_energy_gaus_EGT_gt_En",
			  energyNChannels, energyLlim, energyUlim,
			  hit.GetCoreEnergy()*gRandom->Gaus(1,1./1000.));
    }
    
    obj.FillHistogram("energy", "fold_vs_energy",
		      energyNChannels/8, energyLlim, energyUlim,
		      hit.GetCoreEnergy(),
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
		      "segfold_vs_energy",
		      energyNChannels/8, energyLlim, energyUlim,
		      hit.GetCoreEnergy(),
		      20, 0, 20, segment_fold);
                            
    // Peter wrote these to give Dirk his Phi range (0-360).
    obj.FillHistogram("position", "theta",
		      180, 0., 180.,
		      hit.GetTheta()*TMath::RadToDeg());
    obj.FillHistogram("position", "theta_vs_phi",
		      360, 0., 360.,
		      hit.GetPhi()*TMath::RadToDeg(),
		      180, 0., 180.,
		      hit.GetTheta()*TMath::RadToDeg());

    TGretSimHit simhit = gretSim->GetGretinaSimHit(0);
    if(gretSim){
      if(gretSim->Size()){
	Double_t p = simhit.GetPhi();
	p += TMath::Pi()/2.;
	if(p>TMath::TwoPi())
	  p -= TMath::TwoPi();
	Double_t azimuthal = hit.GetPhi()-p+TMath::Pi();
	if(azimuthal>TMath::TwoPi())
	  azimuthal -= TMath::TwoPi();
	obj.FillHistogram("position", "phi_GT_phi_n",
			  360, 0., TMath::TwoPi(),
			  hit.GetPhi(),
			  360, 0., TMath::TwoPi(),
			  p);
	obj.FillHistogram("position", "azimuthal",
			  540, 0., 3.*TMath::Pi(),
			  azimuthal);

	if(azimuthal > 2.5 && azimuthal < 4.){
	  obj.FillHistogram("position", "phi_GT_phi_n_azimuthal",
			    360, 0., TMath::TwoPi(),
			    hit.GetPhi(),
			    360, 0., TMath::TwoPi(),
			    p);
	  obj.FillHistogram("energy",  "energy_gaus_azimuthal",
			    energyNChannels, energyLlim, energyUlim,
			    hit.GetCoreEnergy()*gRandom->Gaus(1,1./1000.));
	}

      }
    }
    
    int max_layer = -1;
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
	obj.FillHistogram("energy", "energy_involves_phi",
			  energyNChannels, energyLlim, energyUlim,
			  hit.GetCoreEnergy());
	obj.FillHistogram("energy", "overview_involves_phi",
			  energyNChannels, energyLlim, energyUlim,
			  hit.GetCoreEnergy(),
			  100, 0, 100, hit.GetCrystalId());
    }
    
    for(int k = 5; k > 0; k--){
      if(max_layer < k){
	obj.FillHistogram("energy",
			  Form("energy_below_%s", LayerMap[k].c_str()),
			  energyNChannels, energyLlim, energyUlim,
			  hit.GetCoreEnergy());
	if( hit.GetCrystalId()%2 )
	  obj.FillHistogram("energy",
			    Form("energy_A_below_%s", LayerMap[k].c_str()),
			    energyNChannels, energyLlim, energyUlim,
			    hit.GetCoreEnergy());
	else
	  obj.FillHistogram("energy",
			    Form("energy_B_below_%s", LayerMap[k].c_str()),
			    energyNChannels, energyLlim, energyUlim,
			    hit.GetCoreEnergy());
      }

    }
    
  }

  // Addback
  obj.FillHistogram("addback",  "calorimeter",
		    energyNChannels, energyLlim, energyUlim,
		    calorimeterEnergy);

  // For energy-gated addback spectra
  Double_t ABenergy[100];
  TString  ABtype[100];
  Int_t    Naddback = 0;
  
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
    Double_t addbackEnergy = 0.;
    for(int i = 0; i < cluster.size(); i++){
      addbackEnergy += cluster[i].GetCoreEnergy();
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

    obj.FillHistogram("addback",  addbackType,
		      energyNChannels, energyLlim, energyUlim,
		      addbackEnergy);

    if(addbackType == "addback_n0"
       || addbackType == "addback_n1"){
      obj.FillHistogram("addback",  "addback_n0n1",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy);
    }
    
    if(addbackType == "addback_n0"
       || addbackType == "addback_n1"
       || addbackType == "addback_n2"){
      obj.FillHistogram("addback",  "addback_n0n1n2",
			energyNChannels, energyLlim, energyUlim,
			addbackEnergy);
    }
    
    obj.FillHistogram("addback",  "clusterSize_vs_neighborPairs",
		      20, 0, 20, neighbors,
		      10, 0, 10, cluster.size());

    ABenergy[Naddback] = addbackEnergy;
    ABtype[Naddback]   = addbackType;
    Naddback++;

  }

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();
  
  if(numobj!=list->GetSize())
    list->Sort();
}
