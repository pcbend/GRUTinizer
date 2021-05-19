
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

// Not actual mapping
#define Q1  6
#define Q2  7
#define Q3  8
#define Q4  9
#define Q5  14
#define Q6  15
#define Q7  21
#define Q8  22
#define Q9  24
#define Q10 25
  
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
  HoleQMap[Q10] = 10;

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
    resPar1 = 0.;
  Double_t resPar2 = GValue::Value("RESOLUTION_PAR_2");
  if(std::isnan(resPar2))
    resPar2 = 0.;

  if(!(resPar1 + resPar2) > 0)
    return energy;
  
  return energy*gRandom->Gaus(1.0, exp(resPar1*log(energy)+resPar2));
}

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  //  TBank29  *bank29  = obj.GetDetector<TBank29>();
  //  TS800    *s800    = obj.GetDetector<TS800>();
  TGretSim *gretSim = obj.GetDetector<TGretSim>();
  
  Int_t    energyNChannels = 8000;
  Double_t energyLlim = 0.;
  Double_t energyUlim = 8000.;

  if(!gretSim)
    return;
  
  Double_t eE = 0.;
  Bool_t fE = false;
  if(gretSim){
    for(int x=0; x<gretSim->Size(); x++){
      TGretSimHit hit = gretSim->GetGretinaSimHit(x);
      eE = hit.GetEn();
      fE = hit.IsFEP();
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
      obj.FillHistogram("sim","full_energy",
			3, -1, 2,
			hit.IsFEP());
      obj.FillHistogram("sim","beta",
			1000, 0., 1.0,
			hit.GetBeta());
    }
  }

  if(!gretina)
    return;
  
  //  Double_t res = 1./1000.; // Relative energy resolution
  
  Double_t calorimeterEnergy = 0.;
  std::vector<TGretinaHit> hits;

  // Addback preprocessing
  for(int x=0; x<gretina->Size(); x++){

    TGretinaHit hit = gretina->GetGretinaHit(x);
    
    if(hit.GetCoreEnergy() > energyLlim &&
       hit.GetCoreEnergy() < energyUlim){

      calorimeterEnergy += measuredE(hit.GetCoreEnergy());
      hits.push_back(hit);

    }
  }
  
  int max_layer = -1;

  for(int x=0; x<gretina->Size(); x++){

    TGretinaHit hit = gretina->GetGretinaHit(x);
    Double_t mE = measuredE(hit.GetCoreEnergy());

    //                directory, histogram
    obj.FillHistogram("energy", "energy",
		      energyNChannels, energyLlim, energyUlim, mE);

    if(fE)
      obj.FillHistogram("energy", "full_energy",
			energyNChannels, energyLlim, energyUlim, mE);

    obj.FillHistogram("energy", "emitted_vs_detected",
		      energyNChannels/10, energyLlim, energyUlim, mE,
		      energyNChannels/10, energyLlim, energyUlim, eE);

    // Peter wrote these to give Dirk his Phi range (0-360).
    obj.FillHistogram("position", "theta_vs_phi",
		      360, 0., 360.,
		      hit.GetPhi()*TMath::RadToDeg(),
		      180, 0., 180.,
		      hit.GetTheta()*TMath::RadToDeg());
  
  }
  
  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();
  
  if(numobj!=list->GetSize())
    list->Sort();
}
