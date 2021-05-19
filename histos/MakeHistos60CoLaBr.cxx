
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

Double_t measuredE(Double_t energy){
  Double_t resPar1 = GValue::Value("RESOLUTION_PAR_1");
  if(std::isnan(resPar1))
    resPar1 = 1.2;
  Double_t resPar2 = GValue::Value("RESOLUTION_PAR_2");
  if(std::isnan(resPar2))
    resPar2 = 0.0005;
  
  return energy + gRandom->Gaus(0.0, resPar1*sqrt(1 + resPar2*energy));
}

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
  Double_t cosTheta  = -1;
  Double_t mCosTheta = -1;
  Double_t x1 = -1;
  Double_t y1 = -1;
  Double_t z1 = -1;
  Double_t x2 = -1;
  Double_t y2 = -1;
  Double_t z2 = -1;
  
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
      obj.FillHistogram("sim","emitted_z",
			1000,-50., 50.,
			hit.GetZ());
      if(hit.GetEn() > 1331 && hit.GetEn() < 1334){
	x1 = sin(hit.GetTheta())*cos(hit.GetPhi());
	y1 = sin(hit.GetTheta())*sin(hit.GetPhi());
	z1 = cos(hit.GetTheta());
      }
      if(hit.GetEn() > 1171 && hit.GetEn() < 1175){
	x2 = sin(hit.GetTheta())*cos(hit.GetPhi());
	y2 = sin(hit.GetTheta())*sin(hit.GetPhi());
	z2 = cos(hit.GetTheta());
      }
    }
    cosTheta
      = (x1*x2+y1*y2+z1*z2)/(x1*x1+y1*y1+z1*z1)/(x2*x2+y2*y2+z2*z2);
    obj.FillHistogram("sim","emitted_delta",
		      100, -1, 1.,
		      cosTheta);
  }

  if(!gretina)
    return;
  
  Double_t calorimeterEnergy = 0.;
  std::vector<TGretinaHit> hits;

  int max_layer = -1;

  // LaBr-gate
  Bool_t LaBr = false;
  Double_t eGateLlim = 1327.5;
  Double_t eGateUlim = 1337.5;
  for(int x=0; x<gretina->Size(); x++){
    TGretinaHit hit = gretina->GetGretinaHit(x);
    if(hit.GetCrystalId() == 136){
      obj.FillHistogram("energy", "LaBr",
			energyNChannels, energyLlim, energyUlim,
			hit.GetCoreEnergy());
      if(hit.GetCoreEnergy() > eGateLlim &&
	 hit.GetCoreEnergy() < eGateUlim )
	LaBr = true;
    }
  }
  
  for(int x=0; x<gretina->Size(); x++){

    TGretinaHit hit = gretina->GetGretinaHit(x);
    if(hit.GetCrystalId() == 136) // Ignore LaBr
      continue;

    Double_t mE = measuredE(hit.GetCoreEnergy());

    //                directory, histogram
    obj.FillHistogram("energy", "overview",
		      energyNChannels, energyLlim, energyUlim, mE,
		      100, 0, 100, hit.GetCrystalId());
    if(LaBr){
      obj.FillHistogram("energy", "LaBr_gate",
			energyNChannels, energyLlim, energyUlim, mE);
      obj.FillHistogram("energy", "overview_LaBr_gate",
			energyNChannels, energyLlim, energyUlim, mE,
			100, 0, 100, hit.GetCrystalId());
      obj.FillHistogram("position", "theta_LaBr",
			1024, 0., TMath::Pi(),
			hit.GetTheta());
      if(mE > 1168 && mE < 1178){
	obj.FillHistogram("position", "theta_LaBr_1173",
			  1024, 0., TMath::Pi(),
			  hit.GetTheta());
      }

    }

    obj.FillHistogram("energy", "energy",
		      energyNChannels, energyLlim, energyUlim, mE);
      
    obj.FillHistogram("energy", "fold_vs_energy",
		      energyNChannels/8, energyLlim, energyUlim, mE,
		      20, 0, 20, hit.NumberOfInteractions());

    // Symmetrized gamma-gamma matrix and angular correlation
    for(int y=x+1; y<gretina->Size(); y++){
      TGretinaHit hit2 = gretina->GetGretinaHit(y);
      if(hit2.GetCrystalId() == 136) // Ignore LaBr
	continue;

      Double_t mE2 = measuredE(hit2.GetCoreEnergy());
      obj.FillHistogram("energy", "gamma_gamma",
			energyNChannels/4, energyLlim, energyUlim, mE,
			energyNChannels/4, energyLlim, energyUlim, mE2);
      obj.FillHistogram("energy", "gamma_gamma",
			energyNChannels/4, energyLlim, energyUlim, mE2,
			energyNChannels/4, energyLlim, energyUlim, mE);
    }
    
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
		      energyNChannels/8, energyLlim, energyUlim, mE,
		      20, 0, 20, segment_fold);

    if( hit.GetCrystalId()%2 )
      obj.FillHistogram("energy",  "energy_A",
			energyNChannels, energyLlim, energyUlim, mE);
    else
      obj.FillHistogram("energy",  "energy_B", 
			energyNChannels, energyLlim, energyUlim, mE);
                        
    // Peter wrote these to give Dirk his Phi range (0-360).
    obj.FillHistogram("position", "theta_vs_phi",
		      360, 0., 360.,
		      hit.GetPhi()*TMath::RadToDeg(),
		      180, 0., 180.,
		      hit.GetTheta()*TMath::RadToDeg());

    obj.FillHistogram("position", "theta",
		      180, 0., 180.,
		      hit.GetTheta()*TMath::RadToDeg());
    
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
			  energyNChannels, energyLlim, energyUlim, mE);
	obj.FillHistogram("energy", "overview_involves_phi",
			  energyNChannels, energyLlim, energyUlim, mE,
			  100, 0, 100, hit.GetCrystalId());
    }
    
    for(int k = 5; k > 0; k--){
      if(max_layer < k){
	obj.FillHistogram("energy",
			  Form("energy_below_%s", LayerMap[k].c_str()),
			  energyNChannels, energyLlim, energyUlim, mE);
	if( hit.GetCrystalId()%2 )
	  obj.FillHistogram("energy",
			    Form("energy_A_below_%s", LayerMap[k].c_str()),
			    energyNChannels, energyLlim, energyUlim, mE);
	else
	  obj.FillHistogram("energy",
			    Form("energy_B_below_%s", LayerMap[k].c_str()),
			    energyNChannels, energyLlim, energyUlim, mE);
      }

    }
    
  }
      
  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();
  
  if(numobj!=list->GetSize())
    list->Sort();
}
