#include "TRuntimeObjects.h"

#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TObject.h"
#include "TRandom.h"

#include "GValue.h"
#include "TJanusDDAS.h"
#include "TNSCLScalers.h"
#include "TSega.h"


void MakeSegaHistograms(TRuntimeObjects& obj, TSega& sega);
void MakeJanusHistograms(TRuntimeObjects& obj, TJanusDDAS& janus);
void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus);
void MakeScalerHistograms(TRuntimeObjects& obj, TNSCLScalers& scalers);

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TSega* sega = obj.GetDetector<TSega>();
  TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();
  TNSCLScalers* scalers = obj.GetDetector<TNSCLScalers>();

  int hitpat = bool(sega) + bool(janus)*2;
  obj.FillHistogram("sega_janus_coinc",
                    4, 0, 4, hitpat);

  if(janus){
    MakeJanusHistograms(obj, *janus);
  }
  if(sega){
    MakeSegaHistograms(obj, *sega);
  }
  if(sega && janus){
    MakeCoincidenceHistograms(obj, *sega, *janus);
  }
  if(scalers){
    MakeScalerHistograms(obj, *scalers);
  }
}

void MakeJanusHistograms(TRuntimeObjects& obj, TJanusDDAS& janus) {

  obj.FillHistogram("janus", "num_channels",
                    150, -10, 140, janus.GetAllChannels().size());
  obj.FillHistogram("janus", "num_hits",
                    150, -10, 140, janus.GetAllHits().size());

  for(auto& chan : janus.GetAllChannels()) {
    obj.FillHistogram("janus", "charge_summary",
                      128, 0, 128, chan.GetFrontChannel(),
                      4200, -100, 4100, chan.Charge());
    obj.FillHistogram("janus", "energy_summary",
                      128, 0, 128, chan.GetFrontChannel(),
                      1200, -20, 100, chan.GetEnergy());


    obj.FillHistogram("janus", "channel_detnum",
                      15,-5,10, chan.GetDetnum());
  }

  for(auto& hit : janus.GetAllHits()) {
    obj.FillHistogram("janus", "charge_summary_hit",
                      128, 0, 128, hit.GetFrontChannel(),
                      4200, -100, 4100, hit.Charge());
    obj.FillHistogram("janus", "charge_summary_hit",
                      128, 0, 128, hit.GetBackChannel(),
                      4200, -100, 4100, hit.GetBackHit().Charge());
    obj.FillHistogram("janus", "energy_summary_hit",
                      128, 0, 128, hit.GetFrontChannel(),
                      1200, -20, 100, hit.GetEnergy());
    obj.FillHistogram("janus", "energy_summary_hit",
                      128, 0, 128, hit.GetBackChannel(),
                      1200, -20, 100, hit.GetBackHit().GetEnergy());

    obj.FillHistogram("janus", "charge_frontback",
                      4200, -100, 4100, hit.Charge(),
                      4200, -100, 4100, hit.GetBackHit().Charge());
    obj.FillHistogram("janus", "energy_frontback",
                      1200, -20, 100, hit.GetEnergy(),
                      1200, -20, 100, hit.GetBackHit().GetEnergy());

    obj.FillHistogram("janus", Form("det%d_ringnum_sectornum", hit.GetDetnum()),
                      30, 0, 30, hit.GetRing(),
                      40, 0, 40, hit.GetSector());

    obj.FillHistogram("janus", Form("det%d_ringnum", hit.GetDetnum()),
                      30, 0, 30, hit.GetRing());

    obj.FillHistogram("janus", "frontback_tdiff",
                      2000, -1000, 1000, hit.Timestamp() - hit.GetBackHit().Timestamp());


    obj.FillHistogram("janus",Form("det%d_xy", hit.GetDetnum()),
                      100,-4,4,hit.GetPosition().X(),
                      100,-4,4,hit.GetPosition().Y());
  }
}


void MakeSegaHistograms(TRuntimeObjects& obj, TSega& sega) {
  obj.FillHistogram("sega","num_hits",
                    256,0,256,sega.Size());

  long cc_timestamp = -1;
  long segment_timestamp = -1;
  for(unsigned int i=0; i<sega.Size(); i++){
    TSegaHit& hit = sega.GetSegaHit(i);
    int hit_detnum = hit.GetDetnum();
    double energy = hit.GetEnergy();
    obj.FillHistogram("sega","energy",
                      8000, 0, 4000, energy);
    obj.FillHistogram("sega","charge_summary",
                      18, 1, 19, hit_detnum,
                      32768, 0, 32768, hit.Charge());
    obj.FillHistogram("sega","energy_summary",
                      18, 1, 19, hit_detnum,
                      8000, 0, 4000, energy);

    obj.FillHistogram("sega","numsegments",
                      16, 1, 17, hit_detnum,
                      33, 0, 33, hit.GetNumSegments());

    // obj.FillHistogram("sega", Form("det%d_numsegments_ts", hit_detnum),
    //                   36000, 0, 3600e9, hit.Timestamp(),
    //                   33, 0, 33, hit.GetNumSegments());

    // obj.FillHistogram("sega", Form("energy_det%02d_ts", hit_detnum),
    //                   214100/60, 0, 2.141e14, hit.Timestamp(),
    //                   1000, 0, 2000, energy);

    // obj.FillHistogram("sega", Form("charge_det%02d_ts", hit_detnum),
    //                   214100/60, 0, 2.141e14, hit.Timestamp(),
    //                   22000, 0, 22000, hit.Charge());


    for(unsigned int segi=0; segi<hit.GetNumSegments(); segi++){
      TSegaSegmentHit& seg = hit.GetSegment(segi);
      int segnum = seg.GetSegnum();
      obj.FillHistogram("sega", Form("det%02d_charge_segsummary", hit_detnum),
                        32, 1, 33, segnum,
                        32768, 0, 32768, seg.Charge());
      obj.FillHistogram("sega", Form("det%02d_energy_segsummary", hit_detnum),
                        32, 1, 33, segnum,
                        32768, 0, 32768, seg.GetEnergy());
      obj.FillHistogram("sega", "allseg_summary",
                        32*16, 1, 32*16 + 1, 32*(hit_detnum-1) + segnum,
                        32768, 0, 32768, seg.Charge());
    }

    if(hit.GetCrate()==1){
      cc_timestamp = hit.Timestamp();
    } else if (hit.GetCrate()==3) {
      segment_timestamp = hit.Timestamp();
    }
  }


  if(cc_timestamp>0 && segment_timestamp>0){
    obj.FillHistogram("sega","segment_core_tdiff",
                      1000, -5000, 5000, cc_timestamp - segment_timestamp);
  }
}


void MakeScalerHistograms(TRuntimeObjects& obj, TNSCLScalers& scalers) {
  if(scalers.GetSourceID() == 4) {
    for(unsigned int i=0; i<scalers.Size(); i++) {
      int value = scalers.GetScaler(i);

      obj.FillHistogram("scalers", "cumulative",
                        128, 0, 128, i, value);

      obj.FillHistogram("scalers", Form("chan%02d_timedep", i),
                        1800, 0, 3600, scalers.GetIntervalStart(), value);
    }
  }
}

void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus) {
}
