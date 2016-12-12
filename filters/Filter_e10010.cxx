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

  if(!s800)
    return false;

  //if((s800->GetTrigger().GetRegistr()&0x0002)>>1)
  // return true;
  if(s800->GetTrigger().GetRegistr()==3) {
    //printf("i am returning true\n"); fflush(stdout);
    return true;
  }
  
  return false;


}



