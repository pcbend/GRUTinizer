// To make a new filter, copy this file under a new name in the "filter" directory.
// The "FilterCondition" function should return a boolean value.
// The boolean indicates whether the event should be kept or not.

#include "TRuntimeObjects.h"
#include "TFSU.h"


extern "C"
bool FilterCondition(TRuntimeObjects& obj) {

  TFSU  *fsu = obj.GetDetector<TFSU>();
  if(!fsu || !fsu->Size()) {
    return false;
  } 
  if(fsu->GetDeltaE().Charge()<10) 
    return false;


  return true;
}
