// To make a new filter, copy this file under a new name in the "filter" directory.
// The "FilterCondition" function should return a boolean value.
// The boolean indicates whether the event should be kept or not.

#include "TRuntimeObjects.h"
#include "TFSU.h"

#include "GCutG.h"

GCutG* pid_cut = 0;

/*Loop over all cuts. If the cut name is equal to the name of interest, the pid_cut will be set
  to that cut. */
void set_pid_cut(TRuntimeObjects& obj)
{

  TList *gates = &(obj.GetGates());

  const char* cut_of_interest = "Alpha";

  //Loop over all gates. 
  for(int i=0; i < gates->GetEntries(); i++) 
    { 
      GCutG* cut = (GCutG*)gates->At(i);

      if(strcmp(cut->GetName(), cut_of_interest)==0) 
	{
	  pid_cut = cut;
	  std::cout << "cut of interest loaded.\n" << std::endl;
	}
    }    
}

extern "C"
bool FilterCondition(TRuntimeObjects& obj) 
{

  //loads the event and the gates
  TFSU  *fsu_event = obj.GetDetector<TFSU>();

  //check if event exists
  if(!fsu_event || !fsu_event->Size()) 
    {
      return false;
    } 

  //Declare E and dE
  TFSUHit E = fsu_event->GetE(); 
  TFSUHit dE = fsu_event->GetDeltaE();

  //Set the PID cut. If it has already been set, it will not be set again.
  if(pid_cut == 0)
    {
      set_pid_cut(obj);
    }

  /*Check if the PID cut exists, and if the E and dE of the hit are inside of the cut.
    if they are, return true.*/
  if(pid_cut && pid_cut->IsInside(E.GetEnergy(),dE.GetEnergy())) 
    {
      return true;
    }

  return false;
}
