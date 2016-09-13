
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
  // TS800 *s800       = obj.GetDetector<TS800>();


  if(!gretina)
    return;

  for(unsigned int x=0;x<gretina->Size();x++) {
    TGretinaHit hit = gretina->GetGretinaHit(x);
    obj.FillHistogram("energy","summary",4000,0,4000,hit.GetCoreEnergy(),
        100,0,100,hit.GetCrystalId() );

    obj.FillHistogram("energy","summary_gaus",4000,0,4000,hit.GetCoreEnergy()*gRandom->Gaus(1,(1./1000.)),
        100,0,100,hit.GetCrystalId());

    obj.FillHistogram("position","everything",360,-180,180,hit.GetPosition().Phi()*TMath::RadToDeg(),
        180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg());

    obj.FillHistogram("position","everything2",360,0,360,hit.GetPhi()*TMath::RadToDeg(),
        180,0,180,hit.GetTheta()*TMath::RadToDeg());

    int largest_layer=-1;
    for(int y=0;y<hit.NumberOfInteractions();y++) {
      int layer =  hit.GetSegmentId(y)/6;
      if(layer>largest_layer)
        largest_layer = layer;
      obj.FillHistogram("layers",Form("position_%s",LayerMap[layer].c_str()),360,0,360,hit.GetPhi()*TMath::RadToDeg(),
          180,0,180,hit.GetTheta()*TMath::RadToDeg());
    }

    //obj.FillHistogram("max_layer",12,-2,10,largest_layer);
   // if(largest_layer<5) {
    for(int z=5;z>-1;z--) {
      if(largest_layer<z) {
        obj.FillHistogram("energy",Form("summary_%s_and_below",LayerMap[z-1].c_str()),
            4000,0,4000,hit.GetCoreEnergy(),
            100,0,100,hit.GetCrystalId() );
        obj.FillHistogram("energy",Form("summary_gaus_%s_and_below",LayerMap[z-1].c_str()),
            4000,0,4000,hit.GetCoreEnergy()*gRandom->Gaus(1,(1./1000.)),
            100,0,100,hit.GetCrystalId());
      }
    }
    //if(largest_layer==0)




    /*
    }
    iif(largest_layer<4) {
      obj.FillHistogram("energy",Form("summary_%s_veto",LayerMap[4].c_str()),
          4000,0,4000,hit.GetCoreEnergy(),
          100,0,100,hit.GetCrystalId() );
      obj.FillHistogram("energy",Form("summary_gaus_%s_veto",LayerMap[4].c_str()),
          4000,0,4000,hit.GetCoreEnergy()*gRandom->Gaus(1,(1./1000.)),
          100,0,100,hit.GetCrystalId());
    }
    if(largest_layer<3) {
      obj.FillHistogram("energy",Form("summary_%s_veto",LayerMap[3].c_str()),
          4000,0,4000,hit.GetCoreEnergy(),
          100,0,100,hit.GetCrystalId() );
      obj.FillHistogram("energy",Form("summary_gaus_%s_veto",LayerMap[3].c_str()),
          4000,0,4000,hit.GetCoreEnergy()*gRandom->Gaus(1,(1./1000.)),
          100,0,100,hit.GetCrystalId());
    }
    if(largest_layer<2) {
      obj.FillHistogram("energy",Form("summary_%s_veto",LayerMap[2].c_str()),
          4000,0,4000,hit.GetCoreEnergy(),
          100,0,100,hit.GetCrystalId() );
      obj.FillHistogram("energy",Form("summary_gaus_%s_veto",LayerMap[2].c_str()),
          4000,0,4000,hit.GetCoreEnergy()*gRandom->Gaus(1,(1./1000.)),
          100,0,100,hit.GetCrystalId());
    }
    if(largest_layer<1) {
      obj.FillHistogram("energy",Form("summary_%s_veto",LayerMap[1].c_str()),
          4000,0,4000,hit.GetCoreEnergy(),
          100,0,100,hit.GetCrystalId() );
      obj.FillHistogram("energy",Form("summary_gaus_%s_veto",LayerMap[1].c_str()),
          4000,0,4000,hit.GetCoreEnergy()*gRandom->Gaus(1,(1./1000.)),
          100,0,100,hit.GetCrystalId());
    }
    */








  }

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  if(numobj!=list->GetSize())
    list->Sort();
}
