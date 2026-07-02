
#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>

#include "TGretina.h"
#include "TBank88.h"

#include "TChannel.h"
#include "GValue.h"
#include "TCutG.h"
#include "TFile.h"
#include "TCutG.h"
#include "TCanvas.h"
#include "TH1D.h"

#define BETA .046
//#define beta .0415

class FMA {
  public:
    FMA() { Clear(); }
    ~FMA() {} 

    double dE() { return ic1; }
    double dE2() { return ic2+ic3; }
    double E()  { return ic1+ic2+ic3; }
    double X()  { return (mcpl/(mcpl+mcpr))*1000.;}
    double Y()  { return (mcpr/(mcpl+mcpr))*1000.;}
    double Time()  { return timestamp;}

    void Set(TBank88 *bank) {
      for(size_t i=0;i<bank->Size();i++) {
        int id = bank->GetMode3Hit(i).GetChannel();
        double value = bank->GetMode3Hit(i).Charge();
        double time = bank->GetMode3Hit(i).GetLed();
        switch(id) {
          case 0:
            mcpl = value;
            break;
          case 1:
            mcpr = value;
            break;
          case 2:
            wobbler = value;
            break;
          case 5:
            ic1 = value;
            break;
          case 6:
            ic2 = value;
            break;
          case 7:
            rfTof = value;
            break;
          case 8:
            ic3 = value;
            break;
          case 9:
            particleTrigger = value;
            timestamp = time;
            break;
        }
      }
    }

    void Clear() {
      mcpl = -1;
      mcpr = -1;
      wobbler = -1;
      ic1 = -1;
      ic2 = -1;
      ic3 = -1;
      rfTof = -1;
      particleTrigger = -1;
      timestamp = -1;
    }

    double mcpl;
    double mcpr;
    double wobbler;
    double ic1;
    double ic2;
    double ic3;
    double rfTof;
    double particleTrigger;
    double timestamp;
};

#define INTEGRATION 128.0

//initialize gates
TCutG *Neon22;
TCutG *Florine22;
TCutG *Ne22;
TCutG *F22;
TCutG *timeFootPrompt;
TCutG *timeFootRandomB;
TCutG *timeFootRandomF;



bool cutset = false;

TList *PID = new TList;
//TLisi *NAq = new TList;
//TList *Aoq = new TList;


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank88  *bank88  = obj.GetDetector<TBank88>();

  FMA fma;

  if(bank88) {
    fma.Set(bank88);
  }

  obj.FillHistogram("dEE", 8000,0,8000,fma.E(),
      2000,0,2000,fma.dE());


  if(!gretina)
    return;

  TGretinaHit::SetCalibrationCore(3);
  gretina->BuildClusters();

  std::vector<TGretinaHit> hits;

  for(const auto &hit : gretina->GetHits()) {
    obj.FillHistogram("hits","summary",4000,0,8000,hit.GetEnergy(),
                                   200,0,200,hit.GetId());
    obj.FillHistogram("hits","summaryDoppler",4000,0,8000,hit.GetDoppler(BETA),
                                   200,0,200,hit.GetId());
    obj.FillHistogram("hits","position",360,0,180,hit.GetThetaDeg(),
                                 720,0,360,hit.GetPhiDeg());

  }
  
  for(const auto &hit : gretina->GetClusters()) {
    obj.FillHistogram("clusters","summary",4000,0,8000,hit.GetEnergy(),
                                   200,0,200,hit.GetId());
    obj.FillHistogram("clusters","summaryDoppler",4000,0,8000,hit.GetDoppler(BETA),
                                   200,0,200,hit.GetId());
    obj.FillHistogram("cluster","position",360,0,180,hit.GetThetaDeg(),
                                 720,0,360,hit.GetPhiDeg());

  }


} //end MakeHistogram
