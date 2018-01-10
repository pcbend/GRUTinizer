
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

#include "TChannel.h"
#include "GValue.h"


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TFSU  *fsu = obj.GetDetector<TFSU>();
  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  if(!fsu) {
    return;
  } 

  for(size_t i=0;i<fsu->Size();i++) {
    TFSUHit hit = fsu->GetFSUHit(i);
    obj.FillHistogram("summary",8000,0,16000,hit.Charge(),
                                200,0,200,hit.Id());
  }




  if(numobj!=list->GetSize())
    list->Sort();
}
