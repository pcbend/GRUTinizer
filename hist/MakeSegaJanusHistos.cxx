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

void MakeJanusHistograms(TJanus* janus, TRuntimeObjects& obj);
void MakeSegaHistograms(TSega* sega, TRuntimeObjects& obj);

extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TSega* sega = obj.GetDetector<TSega>();
  TJanus* janus = obj.GetDetector<TJanus>();

  obj.FillHistogram("sega_janus_size",
                    16, 0, 16, sega->Size(),
                    256, 0, 256, janus->Size());

  static long last_janus = 0;
  static long last_sega = 0;
  if(janus->Size()){
    MakeJanusHistograms(janus, obj);
    last_janus = janus->Timestamp();
    obj.FillHistogram("sega_janus_tiff",
                      10000, 0, 100000, last_janus - last_sega);
  }
  if(sega->Size()){
    MakeSegaHistograms(sega, obj);
    last_sega = sega->Timestamp();
    obj.FillHistogram("janus_sega_tiff",
                      10000, 0, 100000, last_sega - last_janus);
  }
}

void MakeJanusHistograms(TJanus* janus, TRuntimeObjects& obj) {
  for(int i=0; i<janus->Size(); i++){
    TJanusHit& hit = janus->GetJanusHit(i);

    obj.FillHistogram("channel",
                      128, 0, 128, hit.GetAnalogChannel());
    obj.FillHistogram("channel_energy",
                      128, 0, 128, hit.GetAnalogChannel(),
                      4096, 0, 4096, hit.Charge());
  }
}

void MakeSegaHistograms(TSega* sega, TRuntimeObjects& obj){
  obj.FillHistogram("sega_size",
                    2,0,2,sega->Size());
  for(int i=0; i<sega->Size(); i++){
    TSegaHit& hit = sega->GetSegaHit(i);
		obj.FillHistogram("sega_crate",
											50, 0, 50, hit.GetCrate());
		obj.FillHistogram("sega_slot",
											15, 0, 15, hit.GetSlot());
		obj.FillHistogram("sega_energy",
                      50000, 0, 50000, hit.Charge());
		obj.FillHistogram("sega_energy_crate",
											5, 0, 5, hit.GetCrate(),
											50000, 0, 50000, hit.Charge());

  }
}
