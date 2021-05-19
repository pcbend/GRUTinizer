
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
      obj.FillHistogram("sim","emitted_z",
			1000,-50., 50.,
			hit.GetZ());
    }
  }

  if(!gretina)
    return;
    
  for(int x=0; x<gretina->Size(); x++){

    TGretinaHit hit = gretina->GetGretinaHit(x);
    Double_t mE = measuredE(hit.GetCoreEnergy());

    //                directory, histogram
    obj.FillHistogram("energy", "overview",
		      energyNChannels, energyLlim, energyUlim, mE,
		      250, 0, 250, hit.GetCrystalId());

    obj.FillHistogram("energy", "energy",
		      energyNChannels, energyLlim, energyUlim, mE);

    if(gretSim && gretSim->Size()>0)
      if(gretSim->GetGretinaSimHit(0).IsFEP())
        obj.FillHistogram("energy", "photopeak",
	  		  energyNChannels, energyLlim, energyUlim, mE);
    
    obj.FillHistogram("energy", "fold_vs_energy",
		      energyNChannels/8, energyLlim, energyUlim, mE,
		      20, 0, 20, hit.NumberOfInteractions());

    // Symmetrized gamma-gamma matrix
    for(int y=x+1; y<gretina->Size(); y++){
      TGretinaHit hit2 = gretina->GetGretinaHit(y);
      Double_t mE2 = measuredE(hit2.GetCoreEnergy());
      obj.FillHistogram("energy", "gamma_gamma",
			energyNChannels/4, energyLlim, energyUlim, mE,
			energyNChannels/4, energyLlim, energyUlim, mE2);
      obj.FillHistogram("energy", "gamma_gamma",
			energyNChannels/4, energyLlim, energyUlim, mE2,
			energyNChannels/4, energyLlim, energyUlim, mE);
    }

    // crmat/crystal Transformations turned off for FDS, so local positions
    // are in world coordinates.
    TVector3 hitpos = hit.GetLocalPosition(0);

    // Peter wrote these to give Dirk his Phi range (0-360).
    obj.FillHistogram("position", "theta_vs_phi",
		      //		      360, 0., 360.,
		      360, -180., 180.,
		      hitpos.Phi()*TMath::RadToDeg(),
		      180, 0., 180.,
		      hitpos.Theta()*TMath::RadToDeg());
    //		      hit.GetPhi()*TMath::RadToDeg(),
    //		      180, 0., 180.,
    //		      hit.GetTheta()*TMath::RadToDeg());

    if(hit.GetSegmentId(0)==1)
      obj.FillHistogram("position", "theta_vs_phi_R",
			360, -180., 180.,
			hitpos.Phi()*TMath::RadToDeg(),
			180, 0., 180.,
			hitpos.Theta()*TMath::RadToDeg());
    if(hit.GetSegmentId(0)==2){
      obj.FillHistogram("position", "theta_vs_phi_M",
			360, -180., 180.,
			hitpos.Phi()*TMath::RadToDeg(),
			180, 0., 180.,
			hitpos.Theta()*TMath::RadToDeg());
      if(hit.GetCrystalId()%4 == 0 || hit.GetCrystalId()%4 == 3)
	obj.FillHistogram("position", "theta_vs_phi_MR",
			  360, -180., 180.,
			  hitpos.Phi()*TMath::RadToDeg(),
			  180, 0., 180.,
			  hitpos.Theta()*TMath::RadToDeg());
      if(hit.GetCrystalId()%4 == 1 || hit.GetCrystalId()%4 == 2)
	obj.FillHistogram("position", "theta_vs_phi_ML",
			  360, -180., 180.,
			  hitpos.Phi()*TMath::RadToDeg(),
			  180, 0., 180.,
			  hitpos.Theta()*TMath::RadToDeg());
    }
    if(hit.GetSegmentId(0)==3){
      obj.FillHistogram("position", "theta_vs_phi_L",
			360, -180., 180.,
			hitpos.Phi()*TMath::RadToDeg(),
			180, 0., 180.,
			hitpos.Theta()*TMath::RadToDeg());
    }

    // Position spectra in crystal coordinates
    if(hit.NumberOfInteractions()){
    
      obj.FillHistogram("position", "r", 500, 0, 500, hitpos.Mag());
      obj.FillHistogram("position", "x", 1000, -500, 500, hitpos.X());
      obj.FillHistogram("position", "y", 1000, -500, 500, hitpos.Y());
      obj.FillHistogram("position", "z", 1000, -500, 500, hitpos.Z());

      obj.FillHistogram("position",
			Form("crys_%d_x", hit.GetCrystalId()),
			1000, -500, 500, hit.GetLocalPosition(0).X());

      obj.FillHistogram("position",
			Form("crys_%d_y", hit.GetCrystalId()),
			1000, -500, 500, hit.GetLocalPosition(0).Y());

      obj.FillHistogram("position",
			Form("crys_%d_z", hit.GetCrystalId()),
			1000, -500, 500, hit.GetLocalPosition(0).Z());

      obj.FillHistogram("position",
			Form("crys_%d_r", hit.GetCrystalId()),
			500, 0, 500, hit.GetLocalPosition(0).Mag());
      
      obj.FillHistogram("position",
			Form("crys_%d_xy", hit.GetCrystalId()),
			100, -500, 500, hit.GetLocalPosition(0).X(),
			100, -500, 500, hit.GetLocalPosition(0).Y());

      obj.FillHistogram("position",
			Form("crys_%d_xz", hit.GetCrystalId()),
			100, -500, 500,  hit.GetLocalPosition(0).X(),
			100, -500, 500, hit.GetLocalPosition(0).Z());

      obj.FillHistogram("position",
			Form("crys_%d_yz", hit.GetCrystalId()),
			100, -500, 500,  hit.GetLocalPosition(0).Y(),
			100, -500, 500, hit.GetLocalPosition(0).Z());
    }

  }    

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

  while(hits.size() > 0){
    TGretinaHit currentHit = hits.back();
    hits.pop_back();
        std::vector<TGretinaHit> cluster;
    cluster.push_back(currentHit);
    int lastClusterSize = 0;
    while(lastClusterSize < cluster.size()){
      for(int i = 0; i < cluster.size(); i++){
	for(int j = 0; j < hits.size(); j++){
	  // Same clover
	  if(cluster[i].GetHoleNumber() == hits[j].GetHoleNumber()){
	    cluster.push_back(hits.back());
	    hits.pop_back();
	  }
	}
      }
      lastClusterSize = cluster.size();
    }
    
    // Calculate the total energy deposited in the cluster,
    Int_t neighbors = 0;
    Double_t addbackEnergy = 0.;
    for(int i = 0; i < cluster.size(); i++)
      addbackEnergy += measuredE(cluster[i].GetCoreEnergy());

    // Fill addback histograms.

    obj.FillHistogram("energy", "addback", 
		      energyNChannels, energyLlim, energyUlim,
		      addbackEnergy);
  }
  
  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();
  
  if(numobj!=list->GetSize())
    list->Sort();
}
