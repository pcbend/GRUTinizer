// To make a new filter, copy this file under a new name in the "filter" directory.
// The "FilterCondition" function should return a boolean value.
// The boolean indicates whether the event should be kept or not.

#include "TRuntimeObjects.h"

#include <TGretina.h>
#include <TS800.h>


extern "C"
bool FilterCondition(TRuntimeObjects& obj) {


  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800 *s800       = obj.GetDetector<TS800>();

  if(!gretina)
    return false;

  if(!s800)
    return false;

  return true;
}



