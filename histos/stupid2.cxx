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
#include "TUML.h"
//#include "TS800.h"
//#include "TFastScint.h"

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
//#define Q12 20

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
  //  HoleQMap[Q12] = 12;
  LayerMap[0] = "alpha";
  LayerMap[1] = "beta";
  LayerMap[2] = "gamma";
  LayerMap[3] = "delta";
  LayerMap[4] = "epsilon";
  LayerMap[5] = "phi";
}


static long first_timestamp = -1;

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  //InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank88  *bank88  = obj.GetDetector<TBank88>();
  TUML     *uml     = obj.GetDetector<TUML>();


  //TList    *list    = &(obj.GetObjects());
  //int numobj        = list->GetSize();

  std::string histname = "";
  std::string dirname  = "";
  if(bank88) {
    for(unsigned int i=0;i<bank88->Size();i++) {
      TMode3Hit hit = bank88->GetMode3Hit(i);
      obj.FillHistogram("bank88_summary",10000,0,10000,hit.Charge(),
                                         20,0,20,hit.GetChannel());
      obj.FillHistogram("bank88_hitpattern",20,0,20,hit.GetChannel());
      //obj.FillHistogram("bank88_hitpattern",20,0,20,hit.GetChannel(),
      //                                                              );
    }
  
  }
  
  if(uml) {

    for(unsigned int i=0;i<uml->Size();i++) {
      TUMLHit hit = uml->GetUMLHit(i);
//      std::cout << "uml" <<  hit.GetChannel() << "\t" << hit.Charge() << "\t" << std::endl;
      obj.FillHistogram("uml_summary",6400,0,64000,hit.Charge(),
                                  200,0,200,hit.GetChannel());
      obj.FillHistogram("uml_hitpattern",60,0,60,hit.GetChannel());
    }




     obj.FillHistogram("test1",6400,0,64000,uml->GetPin1().Charge());
     obj.FillHistogram("test2",6400,0,64000,uml->GetPin1().GetEnergy());

     obj.FillHistogram("xpos",16,0,16,uml->GetXPosition());

     obj.FillHistogram("xpos_mm",100,-50,50,(uml->GetXPosition()*3)-1.5-24.);

     
     obj.FillHistogram("sssd",20,0,20,uml->SizeSssd());

  }
  
  if(gretina) {
    for(unsigned int i=0;i<gretina->Size();i++) {
    TGretinaHit hit = gretina->GetGretinaHit(i);
    obj.FillHistogram("greta_summary",10000,0,10000,hit.GetCoreEnergy(),
                                200,0,200,hit.GetCrystalId());
    obj.FillHistogram("greta_hitpattern",200,0,200,hit.GetCrystalId());
    }
  }
  

}



