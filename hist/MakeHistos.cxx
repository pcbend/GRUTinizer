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
#include <TMath.h>
#include "TRandom.h"

#include "TObject.h"
#include "TGretina.h"
#include "TS800.h"
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
  TS800    *s800   = 0;
  TIter iter(&obj.GetDetectors());
  while(TObject *object = iter.Next()) {
    if(object->InheritsFrom(TGretina::Class()))
        gretina = (TGretina*)object;
    if(object->InheritsFrom(TS800::Class()))
        s800 = (TS800*)object;
    if(object->InheritsFrom(TBank29::Class()))
        bank29 = (TBank29*)object;
  }

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

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
    std::string histname;
    
    for(int z=y+1;z<gretina->Size();z++) {
      TGretinaHit hit2 = gretina->GetGretinaHit(z);
      histname = "Gamma_Gamma";
      TH2 *energy_mat = (TH2*)list->FindObject(histname.c_str());
      if(!energy_mat) {
        energy_mat = new TH2F(histname.c_str(),histname.c_str(),2000,0,4000,2000,0,4000);
        list->Add(energy_mat);
      }
      energy_mat->Fill(hit.GetCoreEnergy(),hit2.GetCoreEnergy());
      energy_mat->Fill(hit2.GetCoreEnergy(),hit.GetCoreEnergy());
      histname = "Gamma_Gamma_Time";
      TH2 *energy_time = (TH2*)list->FindObject(histname.c_str());
      if(!energy_time) {
        energy_time= new TH2F(histname.c_str(),"delta time vs. energy of low gamma"
                                                ,800,-400,400,2000,0,4000);
        list->Add(energy_time);
      }
      //if(hit.GetCoreEnergy()>hit2.GetCoreEnergy()) {
        energy_time->Fill(hit2.GetTime()-hit.GetTime(),hit2.GetCoreEnergy());
      //} else {
        energy_time->Fill(hit.GetTime()-hit2.GetTime(),hit.GetCoreEnergy());
      //}
    }

      
    if(bank29) {
      histname = "Gretina_Bank29_time";
      TH2 *g29time = (TH2*)list->FindObject(histname.c_str());
      if(!g29time) {
        g29time = new TH2F(histname.c_str(),histname.c_str(),600,0,600,2000,0,4000);
        list->Add(g29time);
      }
      g29time->Fill(bank29->GetTimestamp()-hit.GetTimestamp(),hit.GetCoreEnergy());
      histname = "Gretina_t0_Bank29_time";
      TH2 *gw29time = (TH2*)list->FindObject(histname.c_str());
      if(!gw29time) {
        gw29time = new TH2F(histname.c_str(),histname.c_str(),600,0,600,2000,0,4000);
        list->Add(gw29time);
      }
      gw29time->Fill(bank29->GetTimestamp()-hit.GetTime(),hit.GetCoreEnergy());
    }
    if(s800) {
      histname = "Gretina_S800_time";
      TH2 *g800time = (TH2*)list->FindObject(histname.c_str());
      if(!g800time) {
        g800time = new TH2F(histname.c_str(),histname.c_str(),600,0,600,2000,0,4000);
        list->Add(g800time);
      }
      g800time->Fill(s800->GetTimestamp()-hit.GetTimestamp(),hit.GetCoreEnergy());
      histname = "Gretina_t0_S800_time";
      TH2 *gw800time = (TH2*)list->FindObject(histname.c_str());
      if(!gw800time) {
        gw800time = new TH2F(histname.c_str(),histname.c_str(),600,0,600,2000,0,4000);
        list->Add(gw800time);
      }
      gw800time->Fill(s800->GetTimestamp()-hit.GetTime(),hit.GetCoreEnergy());
    }




    histname = Form("GretinaSummaryX%02i",hit.GetCrystalId());
    TH2 *seg_summary = (TH2*)list->FindObject(histname.c_str());
    if(!seg_summary) {
      seg_summary = new TH2F(histname.c_str(),histname.c_str(),40,0,40,2000,0,4000);
      list->Add(seg_summary);
    }

    //seg_summary->Fill(36,hit.GetCoreCharge(0)/INTEGRATION);
    //seg_summary->Fill(37,hit.GetCoreCharge(1)/INTEGRATION);
    //seg_summary->Fill(38,hit.GetCoreCharge(2)/INTEGRATION);
    //seg_summary->Fill(39,hit.GetCoreCharge(3)/INTEGRATION);
    for(int z=0;z<hit.NumberOfInteractions();z++) {
      seg_summary->Fill(hit.GetSegmentId(z),hit.GetSegmentEng(z));
      int layer  = hit.GetSegmentId(z)/6;
      histname = Form("GretinaPosition_%s",LayerMap[layer].c_str());
      TH2 *position = (TH2*)list->FindObject(histname.c_str());
      if(!position) {
        position = new TH2F(histname.c_str(),histname.c_str(),628,0,6.28,314,0,3.14);
        list->Add(position);
      }
      double theta = hit.GetInteractionPosition(z).Theta();
      double phi   = hit.GetInteractionPosition(z).Phi();
      if(phi<0)
        phi = TMath::TwoPi()+phi;
      position->Fill(phi,theta);

    }
    histname = Form("GretinaPosition");
    TH2 *total_position = (TH2*)list->FindObject(histname.c_str());
    if(!total_position) {
      total_position = new TH2F(histname.c_str(),histname.c_str(),628,0,6.28,314,0,3.14);
      list->Add(total_position);
    }
    total_position->Fill(hit.GetPhi(),hit.GetTheta());

    histname = "GretinaEnergySum";
    TH1 *hist1d = (TH1*)list->FindObject(histname.c_str());
    if(!hist1d) {
      hist1d = new TH1F(histname.c_str(),histname.c_str(),8000,0,4000);
      list->Add(hist1d);
    }
    hist1d->Fill(hit.GetCoreEnergy());
    
    histname = Form("Q%iCoreCharge",HoleQMap[hit.GetCrystalId()/4]);
    //if(HoleQMap[hit.GetHoleNumber()]==0)
    //  printf("HoleQMap[hit.GetHoleNumber() = 0 is %i\n",hit.GetHoleNumber());

    TH2 *qchg = (TH2*)list->FindObject(histname.c_str());
    if(!qchg) {
      qchg = new TH2F(histname.c_str(),histname.c_str(),4,0,4,8000,0,32000);
      list->Add(qchg);
    }
    qchg->Fill(0.,((double)hit.GetCoreCharge(0)));
    qchg->Fill(1.,((double)hit.GetCoreCharge(1)));
    qchg->Fill(2.,((double)hit.GetCoreCharge(2)));
    qchg->Fill(3.,((double)hit.GetCoreCharge(3)));

    histname = "SegmentId_hitpattern";
    TH1 *seghit = (TH1*)list->FindObject(histname.c_str());
    if(!seghit) {
      seghit = new TH1F(histname.c_str(),histname.c_str(),100,0,100);
      list->Add(seghit);
    }
    seghit->Fill(hit.GetCrystalId());



  }


  




  if(numobj!=list->GetSize())
    list->Sort();


}
