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
#include "TSega.h"
#include "TJanus.h"

void MakeJanusHistograms(TRuntimeObjects& obj, TJanus* janus);
void MakeSegaHistograms(TRuntimeObjects& obj, TSega* sega);
void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega* sega, TJanus* janus);

extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TSega* sega = obj.GetDetector<TSega>();
  TJanus* janus = obj.GetDetector<TJanus>();

  obj.FillHistogram("hitpattern",
                    4, 0, 4, !!sega + 2*(!!janus));

  if(janus){
    MakeJanusHistograms(obj, janus);
  }
  if(sega){
    MakeSegaHistograms(obj, sega);
  }
  if(sega && janus){
    MakeCoincidenceHistograms(obj, sega, janus);
  }
}

void MakeJanusHistograms(TRuntimeObjects& obj, TJanus* janus) {
  for(int i=0; i<janus->Size(); i++){
    TJanusHit& hit = janus->GetJanusHit(i);

    obj.FillHistogram("channel",
                      128, 0, 128, hit.GetAnalogChannel());
    obj.FillHistogram("channel_energy",
                      128, 0, 128, hit.GetAnalogChannel(),
                      4096, 0, 4096, hit.Charge());
  }
}

void MakeSegaHistograms(TRuntimeObjects& obj, TSega* sega) {
  obj.FillHistogram("sega_size",
                    10, 0, 10, sega->Size());
  for(int i=0; i<sega->Size(); i++){
    TSegaHit& hit = sega->GetSegaHit(i);
    obj.FillHistogram("sega_adc",
                      40000, 0, 40000, hit.Charge());
  }
}

void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega* sega, TJanus* janus) {

}
