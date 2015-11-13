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
void MakeTimestampDiffs(TRuntimeObjects& obj, TSega* sega, TJanus* janus);

extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TSega* sega = obj.GetDetector<TSega>();
  TJanus* janus = obj.GetDetector<TJanus>();

  if(janus){
    MakeJanusHistograms(obj, janus);
  }
  if(sega){
    MakeSegaHistograms(obj, sega);
  }
  if(sega && janus){
    MakeCoincidenceHistograms(obj, sega, janus);
  }

  MakeTimestampDiffs(obj, sega, janus);

}

void MakeJanusHistograms(TRuntimeObjects& obj, TJanus* janus) {
  obj.FillHistogram("janus_size",
                    256, 0, 256, janus->Size());

  for(int i=0; i<janus->Size(); i++){
    TJanusHit& hit = janus->GetJanusHit(i);

    obj.FillHistogram("channel",
                      128, 0, 128, hit.GetAnalogChannel());
    obj.FillHistogram("channel_energy",
                      128, 0, 128, hit.GetAnalogChannel(),
                      4096, 0, 4096, hit.Charge());
    if(hit.IsADCValid()){
      obj.FillHistogram("channel_energy",
                        128, 0, 128, hit.GetAnalogChannel(),
                        4096, 0, 4096, hit.Charge());
    }
    if(hit.IsTDCValid()){
      obj.FillHistogram("channel_time",
                        128, 0, 128, hit.GetAnalogChannel(),
                        4096, 0, 4096, hit.GetAnalogTDC());
    }
  }
}



void MakeSegaHistograms(TRuntimeObjects& obj, TSega* sega) {
  obj.FillHistogram("sega_size",
                    256,0,256,sega->Size());

  long cc_timestamp = -1;
  long segment_timestamp = -1;
  for(int i=0; i<sega->Size(); i++){
    TSegaHit& hit = sega->GetSegaHit(i);
    obj.FillHistogram("sega_crate",
                      50, 0, 50, hit.GetCrate());
    obj.FillHistogram("sega_slot",
                      15, 0, 15, hit.GetSlot());
    obj.FillHistogram("sega_energy",
                      50000, 0, 50000, hit.Charge());
    if(hit.GetCrate()==1){
      obj.FillHistogram("sega_core_energy",
                        32768, 0, 32768, hit.Charge());
      obj.FillHistogram("sega_core_summary",
                        16, 0, 16, hit.GetChannel(),
                        32768, 0, 32768, hit.Charge());
    }
    obj.FillHistogram("sega_energy_crate",
                      5, 0, 5, hit.GetCrate(),
                      50000, 0, 50000, hit.Charge());

    if(hit.GetCrate()==1){
      cc_timestamp = hit.GetTimestamp();
    } else if (hit.GetCrate()==3) {
      segment_timestamp = hit.GetTimestamp();
    }
  }

  obj.FillHistogram("hascore_hassegment",
                    2, 0, 2, cc_timestamp!=-1,
                    2, 0, 2, segment_timestamp!=-1);
  if(cc_timestamp>0 && segment_timestamp>0){
    obj.FillHistogram("segment_core_tdiff",
                      1000, -5000, 5000, cc_timestamp - segment_timestamp);
  }
}

void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega* sega, TJanus* janus) { }

void MakeTimestampDiffs(TRuntimeObjects& obj, TSega* sega, TJanus* janus) {
  long crate1_ts = -1;
  long crate2_ts = -1;
  long crate3_ts = -1;
  long analog_ts = -1;
  if(janus){
    analog_ts = janus->Timestamp();
  }
  if(sega){
    for(int i=0; i<sega->Size(); i++){
      TSegaHit& hit = sega->GetSegaHit(i);
      if(hit.GetCrate()==1){
        crate1_ts = hit.GetTimestamp();
      } else if(hit.GetCrate()==2){
        crate2_ts = hit.GetTimestamp();
      } else if(hit.GetCrate()==3){
        crate3_ts = hit.GetTimestamp();
      }
    }
  }
  if(crate1_ts!=-1 && crate2_ts!=-1){
    obj.FillHistogram("tdiff_crate1_crate2",
                      600, -3000, 3000, crate2_ts - crate1_ts);
  }
  if(crate1_ts!=-1 && crate3_ts!=-1){
    obj.FillHistogram("tdiff_crate1_crate3",
                      600, -3000, 3000, crate3_ts - crate1_ts);
  }
  if(crate2_ts!=-1 && crate3_ts!=-1){
    obj.FillHistogram("tdiff_crate2_crate3",
                      600, -3000, 3000, crate3_ts - crate2_ts);
  }
  if(analog_ts!=-1 && crate1_ts!=-1){
    obj.FillHistogram("tdiff_analog_crate1",
                      600, -3000, 3000, analog_ts - crate1_ts);
  }
  if(analog_ts!=-1 && crate2_ts!=-1){
    obj.FillHistogram("tdiff_analog_crate2",
                      600, -3000, 3000, analog_ts - crate2_ts);
  }
  if(analog_ts!=-1 && crate3_ts!=-1){
    obj.FillHistogram("tdiff_analog_crate3",
                      600, -3000, 3000, analog_ts - crate3_ts);
  }

  if(crate1_ts!=-1){
    obj.FillHistogram("timestamp_sourceid",
                      1000000, 0, 1000e9, crate1_ts,
                      4, 1, 5, 1);
  }
  if(crate2_ts!=-1){
    obj.FillHistogram("timestamp_sourceid",
                      1000000, 0, 1000e9, crate2_ts,
                      4, 1, 5, 2);
  }
  if(crate3_ts!=-1){
    obj.FillHistogram("timestamp_sourceid",
                      1000000, 0, 1000e9, crate3_ts,
                      4, 1, 5, 3);
  }
  if(analog_ts!=-1){
    obj.FillHistogram("timestamp_sourceid",
                      1000000, 0, 1000e9, analog_ts,
                      4, 1, 5, 4);
  }
}
