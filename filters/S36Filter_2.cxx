// To make a new filter, copy this file under a new name in the "filter" directory.
// The "FilterCondition" function should return a boolean value.
// The boolean indicates whether the event should be kept or not.

#include "TRuntimeObjects.h"
#include "TFSU.h"

#include "TList.h"
#include "GCutG.h"

GCutG *pp_time=0;
GCutG *proton=0;


extern "C"
bool FilterCondition(TRuntimeObjects& obj) {

  TFSU  *fsu = obj.GetDetector<TFSU>();
  TList *gates = &(obj.GetGates());
  if(!fsu || !fsu->Size()) {
    return false;
  } 

  if(!pp_time) {
    TIter it(gates);
    while(GCutG *gate =(GCutG*)it.Next()) {
      std::cout << gate->GetName() << std::endl;
      std::cout << gate->GetTag() << std::endl;

      if(strncmp(gate->GetTag(),"timing",6)==0) {
        printf("loading %s\n",gate->GetName()); fflush(stdout);
        pp_time = gate;
       //break;
      } else if(strncmp(gate->GetTag(),"pid",3)==0) {
        printf("loading %s\n",gate->GetName()); fflush(stdout);
        proton = gate;
      }
    }
  }


  if(fsu->GetDeltaE().Charge()<10) 
    return false;

  if(!proton->IsInside(fsu->GetE().Charge(),fsu->GetDeltaE().Charge())) 
    return false;

  double dtime = fsu->GetDeltaE().Timestamp() - fsu->GetE().Timestamp(); 
  
  if(!pp_time->IsInside(dtime,fsu->GetDeltaE().Charge())) 
    return false;





  return true;
}
