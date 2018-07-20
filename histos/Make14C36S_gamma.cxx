
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

#include "TFSU.h"
#include "GCutG.h"

#include "TChannel.h"

GCutG *pg_prompt = 0;
GCutG *pg_random = 0;



// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TFSU  *fsu = obj.GetDetector<TFSU>();
  TList *list  = &(obj.GetObjects());
  TList *gates = &(obj.GetGates());
  int numobj = list->GetSize();

  if(!fsu || !fsu->Size()) {
    return;
  } 

  if(!pg_prompt) {
    TIter it(gates);
    while(GCutG *gate =(GCutG*)it.Next()) {
      if(strncmp(gate->GetName(),"gg_prompt",9)==0) {
        printf("loading %s\n",gate->GetName()); fflush(stdout);
        pg_prompt = gate;
      } else if(strncmp(gate->GetName(),"gg_random",9)==0) {
        printf("loading %s\n",gate->GetName()); fflush(stdout);
        pg_random = gate;
      }
    }
  }


  double dtime = fsu->GetDeltaE().Timestamp() - fsu->GetE().Timestamp(); 
  obj.FillHistogram("de_dtime",200,-100,100,dtime,
                               4000,0,4000,fsu->GetDeltaE().Charge());
  obj.FillHistogram("e_dtime",200,-100,100,dtime,
                              10000,0,10000,fsu->GetE().Charge());
  obj.FillHistogram("pid",3000,0,30000,fsu->GetE().Charge(),
                          3000,0,30000,fsu->GetDeltaE().Charge());

  fsu->OrderHits();

  bool gg_prompt1 = false;
  bool gg_prompt2 = false;
  
  bool gg_random1 = false;
  bool gg_random2 = false;

  for(int x=0;x<fsu->Size();x++) {
    TFSUHit hit1 = fsu->GetFSUHit(x);
    if(hit1.GetEnergy()<20 || hit1.Charge()>30000) continue;
  

    obj.FillHistogram("gsingles",10000,0,10000,hit1.GetEnergy());
    obj.FillHistogram("gsummary",120,0,120,hit1.GetNumber(),
                                 10000,0,10000,hit1.GetEnergy());

    double dgetime1 = fsu->GetDeltaE().Timestamp()-hit1.Timestamp();

    obj.FillHistogram("getime",200,-100,100,dgetime1,
                                    5000,0,5000,hit1.GetEnergy());

    obj.FillHistogram("getime_sum",200,-100,100,dgetime1,
                                        120,0,120,hit1.GetNumber());

    if(pg_prompt) {
      gg_prompt1 = pg_prompt->IsInside(dgetime1,hit1.GetEnergy());
      gg_random1 = pg_random->IsInside(dgetime1,hit1.GetEnergy());
    }
    
    if(gg_prompt1) {
      obj.FillHistogram("prompt","gsingles",10000,0,10000,hit1.GetEnergy());
      obj.FillHistogram("prompt","gsummary",120,0,120,hit1.GetNumber(),
                                            10000,0,10000,hit1.GetEnergy());
      obj.FillHistogram("prompt","getime",200,-100,100,dgetime1,
                                          5000,0,5000,hit1.GetEnergy());
      obj.FillHistogram("prompt","getime_sum",200,-100,100,dgetime1,
                                          120,0,120,hit1.GetNumber());
      double beta = 0.001;
      while(beta<0.03) {
        obj.FillHistogram("prompt","gsingles_betahunt",6000,0,12000,hit1.GetDoppler(beta),
                                                       100,0,0.03,beta);
        beta+=0.03/100.;

      }
    }

    if(gg_random1) {
      obj.FillHistogram("random","gsingles",10000,0,10000,hit1.GetEnergy());
      obj.FillHistogram("random","gsummary",120,0,120,hit1.GetNumber(),
                                            10000,0,10000,hit1.GetEnergy());
      obj.FillHistogram("random","getime",200,-100,100,dgetime1,
                                          5000,0,5000,hit1.GetEnergy());
      obj.FillHistogram("random","getime_sum",200,-100,100,dgetime1,
                                          120,0,120,hit1.GetNumber());
      double beta = 0.001;
      while(beta<0.03) {
        obj.FillHistogram("random","gsingles_betahunt",6000,0,12000,hit1.GetDoppler(beta),
                                                       100,0,0.03,beta);
        beta+=0.03/100.;

      }
    }

    for(int y=x+1;y<fsu->Size();y++) {
      TFSUHit hit2 = fsu->GetFSUHit(y);
      if(hit2.GetEnergy()<20 || hit2.Charge()>30000) continue;
      double dgetime2 = fsu->GetDeltaE().Timestamp()-hit2.Timestamp();
    
      if(pg_prompt) {
        gg_prompt2 = pg_prompt->IsInside(dgetime2,hit2.GetEnergy());
        gg_random2 = pg_random->IsInside(dgetime2,hit2.GetEnergy());
      }

      if(hit1.GetEnergy()>hit2.GetEnergy()) {
        obj.FillHistogram("ggtime",200,-100,100,dgetime1-dgetime2,
                                   5000,0,5000,hit2.GetEnergy());
      } else {
        obj.FillHistogram("ggtime",200,-100,100,dgetime2-dgetime1,
                                   5000,0,5000,hit1.GetEnergy());
      }
    
      obj.FillHistogramSym("ggmat",5000,0,5000,hit1.GetEnergy(),
                                   5000,0,5000,hit2.GetEnergy());
      if(gg_prompt1 && gg_prompt2) {
        obj.FillHistogramSym("ggmat_prompt",5000,0,5000,hit1.GetEnergy(),
                                             5000,0,5000,hit2.GetEnergy());
      } else {
        obj.FillHistogramSym("ggmat_rand_all",5000,0,5000,hit1.GetEnergy(),
                                              5000,0,5000,hit2.GetEnergy());
      }

      if(gg_random1 && gg_random2) {
        obj.FillHistogramSym("ggmat_random",5000,0,5000,hit1.GetEnergy(),
                                            5000,0,5000,hit2.GetEnergy());
      }

      //int card1 = (hit1.Address()&0xf0)>>4;
      //int card2 = (hit2.Address()&0xf0)>>4;
      //int card1 = hit1.GetNumber();
      //int card2 = hit2.GetNumber();
      //if(card1>card2) { std::swap(card1,card2); }
   
      //obj.FillHistogram(Form("gtime_%i_%i",card1,card2),200,-100,100,dgtime,
      //                          5000,0,5000,hit2.GetEnergy());

 
    }
  }

  gg_prompt1 = false;
  gg_prompt2 = false;
  
  gg_random1 = false;
  gg_random2 = false;



  fsu->MakeAddbackHits();
  for(int x=0;x<fsu->AddbackSize();x++) {
    TFSUHit hit1 = fsu->GetAddbackHit(x);
    if(hit1.GetEnergy()<20 || hit1.Charge()>30000) continue;
    obj.FillHistogram("asingles",10000,0,10000,hit1.GetEnergy());


      double dgetime1 = fsu->GetDeltaE().Timestamp()-hit1.Timestamp();

      obj.FillHistogram("aetime",200,-100,100,dgetime1,
                                    5000,0,5000,hit1.GetEnergy());

      obj.FillHistogram("aetime_sum",200,-100,100,dgetime1,
                                        120,0,120,hit1.GetNumber());
    if(pg_prompt) {
      gg_prompt1 = pg_prompt->IsInside(dgetime1,hit1.GetEnergy());
      gg_random1 = pg_random->IsInside(dgetime1,hit1.GetEnergy());
    }
    
    if(gg_prompt1) {
      obj.FillHistogram("prompt","asingles",10000,0,10000,hit1.GetEnergy());
      obj.FillHistogram("prompt","asummary",120,0,120,hit1.GetNumber(),
                                            10000,0,10000,hit1.GetEnergy());
      obj.FillHistogram("prompt","aetime",200,-100,100,dgetime1,
                                          5000,0,5000,hit1.GetEnergy());
      obj.FillHistogram("prompt","aetime_sum",200,-100,100,dgetime1,
                                          120,0,120,hit1.GetNumber());
      double beta = 0.001;
      while(beta<0.03) {
        obj.FillHistogram("prompt","asingles_betahunt",6000,0,12000,hit1.GetDoppler(beta),
                                                       100,0,0.03,beta);
        beta+=0.03/100.;

      }


    }

    if(gg_random1) {
      obj.FillHistogram("random","asingles",10000,0,10000,hit1.GetEnergy());
      obj.FillHistogram("random","asummary",120,0,120,hit1.GetNumber(),
                                            10000,0,10000,hit1.GetEnergy());
      obj.FillHistogram("random","aetime",200,-100,100,dgetime1,
                                          5000,0,5000,hit1.GetEnergy());
      obj.FillHistogram("random","aetime_sum",200,-100,100,dgetime1,
                                          120,0,120,hit1.GetNumber());
      double beta = 0.001;
      while(beta<0.03) {
        obj.FillHistogram("random","asingles_betahunt",6000,0,12000,hit1.GetDoppler(beta),
                                                       100,0,0.03,beta);
        beta+=0.03/100.;

      }
    }

    for(int y=x+1;y<fsu->AddbackSize();y++) {
      TFSUHit hit2 = fsu->GetAddbackHit(y);
      if(hit2.GetEnergy()<20 || hit2.Charge()>30000) continue;

      obj.FillHistogramSym("aamat",5000,0,5000,hit1.GetEnergy(),
                                   5000,0,5000,hit2.GetEnergy());
      
      double dgetime2 = fsu->GetDeltaE().Timestamp()-hit2.Timestamp();
      
      if(pg_prompt) {
        gg_prompt2 = pg_prompt->IsInside(dgetime2,hit2.GetEnergy());
        gg_random2 = pg_random->IsInside(dgetime2,hit2.GetEnergy());
      }

      double dgtime = hit1.Timestamp()-hit2.Timestamp();


      obj.FillHistogram("aatime",200,-100,100,dgtime,
                                 5000,0,5000,hit2.GetEnergy());
      if(gg_prompt1 && gg_prompt2) {
        obj.FillHistogramSym("aamat_prompt",5000,0,5000,hit1.GetEnergy(),
                                             5000,0,5000,hit2.GetEnergy());
      } else {
        obj.FillHistogramSym("aamat_rand_all",5000,0,5000,hit1.GetEnergy(),
                                              5000,0,5000,hit2.GetEnergy());
      }

      if(gg_random1 && gg_random2) {
        obj.FillHistogramSym("aamat_random",5000,0,5000,hit1.GetEnergy(),
                                            5000,0,5000,hit2.GetEnergy());
      }

           

      

    }
  }





  if(numobj!=list->GetSize())
    list->Sort();
}
