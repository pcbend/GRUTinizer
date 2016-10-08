
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

#include "TMode3.h"
#include "TBank29.h"
#include "GValue.h"


#include "TChannel.h"
#include "GValue.h"


//#define Q1 15
//#define Q2 7
//#define Q3 8
//#define Q4 16
//#define Q5 9
//#define Q6 14
//#define Q7 17
//#define Q8 6
//#define Q9 19

//quads moved July 2016.
#define Q1 15
#define Q2 7
#define Q3 11
#define Q4 1 //actually at lbl
#define Q5 22
#define Q6 14
#define Q7 12
#define Q8 6
#define Q9 21



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
  //TGretina *gretina = obj.GetDetector<TGretina>();
  TMode3 *mode3     = obj.GetDetector<TMode3>();
  //TBank29  *bank29  = obj.GetDetector<TBank29>();
  TList *list = &(obj.GetObjects());

  int numobj = list->GetSize();
  if(!mode3)
    return;

  for(unsigned int x=0;x<mode3->Size();x++) {
    TMode3Hit hit = mode3->GetMode3Hit(x);
    int hole = HoleQMap[hit.GetHole()];
    std::string dirname  = Form("SummaryAll");
    std::string histname = Form("Q%i_detmap",hole); ////;channel;Charge/128.",hole);
    obj.FillHistogram(dirname,histname,
                      160,0,160,hit.GetAbsSegId(),
                      4000,0,32000,hit.Charge());  // int division done in the mode3hit class.

    if(hit.GetChannel()%9==0) {
      dirname  = Form("SummaryCore");
      histname = Form("Q%i_cores",hole); ////;channel;Charge/128.",hole);
      obj.FillHistogram(dirname,histname,
                        20,0,20,hit.GetCrystal()*4 + hit.GetVME(),
                        4000,0,32000,hit.Charge());
    }


    if(TChannel::Size()) {
      histname = Form("Q%i_cal",hole); ////;channel;Charge/128.",hole);
      obj.FillHistogram(dirname,histname,
                        160,0,160,hit.GetAbsSegId(),
                        4000,0,4000,hit.GetEnergy());
      if(hit.GetChannel()%9==0) {
        histname = Form("Q%i_cores",hole); ////;channel;Charge/128.",hole);
        obj.FillHistogram(histname,
                          20,0,20,hit.GetCrystal()*4 + hit.GetVME(),
                          4000,0,4000,hit.GetEnergy());
        //if(hit.GetVME()==2) {  //2.5 MeV channel
        //  if(hit.GetDt1()>0) {
        //    histname = Form("Q%i_2MeV_Energy_vs_d1",hole); ////;channel;Charge/128.",hole);
        //    obj.FillHistogram(dirname,histname,
        //                      2000,0,2000,hit.GetEnergy(),
        //                      2000,0,2000,hit.GetDt1());
        //  }
        //  if(hit.GetDt2()>0) {
        //    histname = Form("Q%i_2MeV_Energy_vs_d2",hole); ////;channel;Charge/128.",hole);
        //    obj.FillHistogram(dirname,histname,
        //                      2000,0,2000,hit.GetEnergy(),
        //                      2000,0,2000,hit.GetDt2());
        //  }
        //}


      }

    }
  }
  if(numobj!=list->GetSize())
    list->Sort();
}
