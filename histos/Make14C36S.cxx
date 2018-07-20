
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

GCutG *pp_time = 0;

void HandlePID(TRuntimeObjects& obj, GCutG *cut) {
  TFSU  *fsu = obj.GetDetector<TFSU>();

  if(!fsu || !cut || !pp_time) 
    return;

  if(!cut->IsInside(fsu->GetE().Charge(),fsu->GetDeltaE().Charge())) 
    return;

  double dtime = fsu->GetDeltaE().Timestamp() - fsu->GetE().Timestamp(); 
  
  if(!pp_time->IsInside(dtime,fsu->GetDeltaE().Charge())) 
    return;



  obj.FillHistogram(cut->GetName(),"pid",3000,0,30000,fsu->GetE().Charge(),
                                         3000,0,30000,fsu->GetDeltaE().Charge());
  

  obj.FillHistogram(cut->GetName(),"de_dtime",200,-100,100,dtime,
                                              4000,0,4000,fsu->GetDeltaE().Charge());

  obj.FillHistogram(cut->GetName(),"e_dtime",200,-100,100,dtime,
                                             10000,0,10000,fsu->GetE().Charge());


 
  for(int x=0;x<fsu->Size();x++) {
    TFSUHit hit = fsu->GetFSUHit(x);
    obj.FillHistogram(cut->GetName(),"singles",10000,0,10000,hit.GetEnergy());
  }

}


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


  if(!pp_time) {
    TIter it(gates);
    while(GCutG *gate =(GCutG*)it.Next()) {
      if(strncmp(gate->GetTag(),"timing",6)==0) {
        printf("loading %s\n",gate->GetName()); fflush(stdout);
        pp_time = gate;
       break;
      }
    }
  }



  obj.FillHistogram("pid",3000,0,30000,fsu->GetE().Charge(),
                          3000,0,30000,fsu->GetDeltaE().Charge());
  
  for(int x=0;x<fsu->Size();x++) {
    TFSUHit hit = fsu->GetFSUHit(x);
    obj.FillHistogram("singles",10000,0,10000,hit.GetEnergy());
  }

  double dtime = fsu->GetDeltaE().Timestamp() - fsu->GetE().Timestamp(); 

  obj.FillHistogram("de_dtime",200,-100,100,dtime,
                               4000,0,4000,fsu->GetDeltaE().Charge());

  obj.FillHistogram("e_dtime",200,-100,100,dtime,
                              10000,0,10000,fsu->GetE().Charge());


  TIter iter(gates);
  while(GCutG *cut = (GCutG*)iter.Next()) {
    if(strncmp(cut->GetTag(),"pid",3)==0) 
      HandlePID(obj,cut);
  }


  if(numobj!=list->GetSize())
    list->Sort();
}
