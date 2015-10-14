#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
//
// Note: from TRuntimeObjects:
//    TList& GetDetectors();
//    TList& GetObjects();
//

#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include "TRandom.h"

#include "TObject.h"
#include "TGretina.h"
#include "TBank29.h"

#define Q1 15
#define Q2 7
#define Q3 8
#define Q4 16
#define Q5 9
#define Q6 14
#define Q7 17
#define Q8 6

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

  LayerMap[0] = "alpha";
  LayerMap[1] = "beta";
  LayerMap[2] = "gamma";
  LayerMap[3] = "delta";
  LayerMap[4] = "epsilon";
  LayerMap[5] = "phi";

}

#define INTEGRATION 128.0


extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  //printf("I am Here 1\n"); fflush(stdout);
  InitMap();
  TGretina *gretina = 0;
  TBank29  *bank29  = 0;
  TIter iter(&obj.GetDetectors());
  while(TObject *object = iter.Next()) {
    if(object->InheritsFrom(TGretina::Class()))
        gretina = (TGretina*)object;
    if(object->InheritsFrom(TBank29::Class()))
        bank29 = (TBank29*)object;
  }

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  obj.FillHistogram("rand",
                    100,-10,10,gRandom->Gaus(0,1));
  obj.FillHistogram("rand2",
                    100,-10,10,gRandom->Gaus(0,3));


  if(bank29) {
    for(int x=0;x<bank29->Size();x++) {
      TMode3Hit &hit = (TMode3Hit&)bank29->GetHit(x);
      std::string histname = Form("bank29_%i",hit.GetChannel());
      TH1 *hist = (TH1*)list->FindObject(histname.c_str());
      if(!hist) {
        hist= new TH1F(histname.c_str(),histname.c_str(),16000,0,64000);
        list->Add(hist);
      }
      //printf("%i\t\t%i\n",hit.Charge(),hit.Charge()/INTEGRATION);
      //std::cout << "In makehistos "  << hit.Charge() << "\t\t" << hit.GetChannel() << std::endl;
      //hist->Fill(hit.Charge()/INTEGRATION);
      hist->Fill(hit.Charge());
    }
  }

  if(!gretina)
    return;

  for(int y=0;y<gretina->Size();y++) {
    //printf("I am Here %i\n",2+y); fflush(stdout);
    TGretinaHit hit = gretina->GetGretinaHit(y);

    std::string histname = Form("GretinaSummaryX%02i",hit.GetCrystalId());
    TH2 *hist = (TH2*)list->FindObject(histname.c_str());
    if(!hist) {
      hist = new TH2F(histname.c_str(),histname.c_str(),40,0,40,2000,0,4000);
      list->Add(hist);
    }
    hist->Fill(36,hit.GetCoreCharge(0)/INTEGRATION);
    hist->Fill(37,hit.GetCoreCharge(1)/INTEGRATION);
    hist->Fill(38,hit.GetCoreCharge(2)/INTEGRATION);
    hist->Fill(39,hit.GetCoreCharge(3)/INTEGRATION);
    for(int z=0;z<hit.NumberOfInteractions();z++) {
      int layer  = hit.GetSegmentId(z)%6;
      hist->Fill(hit.GetSegmentId(z),hit.GetSegmentEng(z));
    }
  }
  if(numobj!=list->GetSize())
    list->Sort();


}
