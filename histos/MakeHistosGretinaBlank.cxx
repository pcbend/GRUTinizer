#include "TRuntimeObjects.h"

#include <map>
#include <iostream>
#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>
#include <TLine.h>

#include "TGretina.h"
#include "TBank88.h"

#include "TChannel.h"
#include "GValue.h"
#include "GCutG.h"


//quads as of June 2019.
#define Q1 15
#define Q2 7
#define Q3 11
#define Q4 16
#define Q5 8
#define Q6 14
#define Q7 12
#define Q8 17
#define Q9 19
#define Q10 6
#define Q11 9
#define Q12 20

//#define BETA .37

std::map<int,int> HoleQMap;
std::map<int,std::string> LayerMap;

bool map_inited=false;

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
  HoleQMap[Q11] = 11;
  HoleQMap[Q12] = 12;

  LayerMap[0] = "alpha";
  LayerMap[1] = "beta";
  LayerMap[2] = "gamma";
  LayerMap[3] = "delta";
  LayerMap[4] = "epsilon";
  LayerMap[5] = "phi";
}


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank88  *bank88  = obj.GetDetector<TBank88>();

  TList    *list    = &(obj.GetObjects());
  int numobj        = list->GetSize();

  std::string histname = "";
  std::string dirname  = "";


  //   if(bank88) {
  //     for(unsigned int x=0; x<bank88->Size(); x++) {
  //       TMode3Hit &hit = (TMode3Hit&)bank88->GetHit(x);
  //       std::string histname = Form("bank88_%i",hit.GetChannel());
  //       obj.FillHistogram(histname,16000,0,64000,hit.Charge());
  //     }
  //   }

  if(!gretina)
    return;


  for(unsigned int x=0; x<gretina->Size(); x++) {
    TGretinaHit xhit = gretina->GetGretinaHit(x);

    //  1D hitogram 
    //  obj.FillHistogram(dirname,histname,bins,xlow,xhigh,xvalue);
    for(unsigned int y=0; y<gretina->Size(); y++) {
      TGretinaHit yhit = gretina->GetGretinaHit(y);
     
      //  2D hitogram 
      //  obj.FillHistogram(dirname,histname,xbins,xlow,xhigh,xvalue,
      //                                     ybins,ylow,yhigh,yvalue);
  
    }

  }

  if(numobj!=list->GetSize())
    list->Sort();
  
}

