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

#define BETA 0.01

void MakeJanusHistograms(TRuntimeObjects& obj, TJanus* janus);
void MakeSegaHistograms(TRuntimeObjects& obj, TSega* sega);
void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega* sega, TJanus* janus);
void MakeTimestampDiffs(TRuntimeObjects& obj, TSega* sega, TJanus* janus);
void MakeLaBrCoincPlots(TRuntimeObjects& obj, TSega* sega, TJanus* janus);

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
    MakeLaBrCoincPlots(obj, sega, janus);
  }

  MakeTimestampDiffs(obj, sega, janus);

}

void MakeJanusHistograms(TRuntimeObjects& obj, TJanus* janus) {
  obj.FillHistogram("janus_size",
                    256, 0, 256, janus->Size());

  for(int i=0; i<janus->Size(); i++){
    TJanusHit& hit = janus->GetJanusHit(i);

    obj.FillHistogram("channel",
                      128, 0, 128, hit.GetFrontChannel());
    obj.FillHistogram("channel_charge",
                      128, 0, 128, hit.GetFrontChannel(),
                      4100, -4, 4096, hit.Charge());
    obj.FillHistogram("channel_energy",
                      128, 0, 128, hit.GetFrontChannel(),
                      4000, 0, 400e3, hit.GetEnergy());
    obj.FillHistogram("channel_time",
                      128, 0, 128, hit.GetFrontChannel(),
                      4096, 0, 4096, hit.Time());

    obj.FillHistogram("channel",
                      128, 0, 128, hit.GetBackChannel());
    obj.FillHistogram("channel_charge",
                      128, 0, 128, hit.GetBackChannel(),
                      4096, 0, 4096, hit.GetBackHit().Charge());
    obj.FillHistogram("channel_energy",
                      128, 0, 128, hit.GetBackChannel(),
                      4000, 0, 400e3, hit.GetBackHit().GetEnergy());
    obj.FillHistogram("channel_time",
                      128, 0, 128, hit.GetBackChannel(),
                      4096, 0, 4096, hit.GetBackHit().Time());

    obj.FillHistogram(Form("janus%d_xy", hit.GetDetnum()),
                      100,-3,3,hit.GetPosition().X(),
                      100,-3,3,hit.GetPosition().Y());

  }
}



void MakeSegaHistograms(TRuntimeObjects& obj, TSega* sega) {
  obj.FillHistogram("sega_size",
                    256,0,256,sega->Size());

  long cc_timestamp = -1;
  long segment_timestamp = -1;
  for(int i=0; i<sega->Size(); i++){
    TSegaHit& hit = sega->GetSegaHit(i);
    obj.FillHistogram("sega_energy",
                      8000, 0, 4000, hit.GetEnergy());
    obj.FillHistogram("sega_charge_summary",
                      18, 1, 19, hit.GetDetnum(),
                      32768, 0, 32768, hit.Charge());
    obj.FillHistogram("sega_energy_summary",
                      18, 1, 19, hit.GetDetnum(),
                      8000, 0, 4000, hit.GetEnergy());
    obj.FillHistogram("sega_numsegments",
                      16, 1, 17, hit.GetDetnum(),
                      33, 0, 33, hit.GetNumSegments());

    // obj.FillHistogram(Form("sega_det%d_numsegments_ts", hit.GetDetnum()),
    //                   36000, 0, 3600e9, hit.Timestamp(),
    //                   33, 0, 33, hit.GetNumSegments());

    for(unsigned int segi=0; segi<hit.GetNumSegments(); segi++){
      TSegaSegmentHit& seg = hit.GetSegment(segi);
      obj.FillHistogram(Form("sega_det%02d_segsummary", hit.GetDetnum()),
                        32, 1, 33, seg.GetSegnum(),
                        32768, 0, 32768, seg.Charge());
      obj.FillHistogram(Form("sega_energy_det%02d_segsummary", hit.GetDetnum()),
                        32, 1, 33, seg.GetSegnum(),
                        32768, 0, 32768, seg.GetEnergy());
      obj.FillHistogram("sega_allseg_summary",
                        32*16, 1, 32*16 + 1, 32*(hit.GetDetnum()-1) + seg.GetSegnum(),
                        32768, 0, 32768, seg.Charge());
    }

    if(hit.GetCrate()==1){
      cc_timestamp = hit.Timestamp();
    } else if (hit.GetCrate()==3) {
      segment_timestamp = hit.Timestamp();
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

void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega* sega, TJanus* janus) {
  bool has_60keV = false;
  // bool coinc_missing3 = false;
  // bool coinc_missing4 = false;
  for(int i=0; i<sega->Size(); i++){
    TSegaHit& hit = sega->GetSegaHit(i);
    if(hit.GetEnergy()>55 && hit.GetEnergy()<65){
      has_60keV = true;
      obj.FillHistogram("sega_has_60keV",
                        16, 1, 17, hit.GetDetnum());
    }

    // if(hit.GetDetnum()!=15 &&
    //    hit.GetDetnum()!=8  &&
    //    hit.GetDetnum()!=7  &&
    //    hit.GetDetnum()!=12){
    //   coinc_missing4 = true;
    // }

    // if(hit.GetDetnum()!=15 &&
    //    hit.GetDetnum()!=8  &&
    //    hit.GetDetnum()!=7){
    //   coinc_missing3 = true;
    // }
  }

  obj.FillHistogram("sega_janus_tdiff",
                    1000, -5000, 5000, sega->Timestamp() - janus->Timestamp());

  for(int i=0; i<janus->Size(); i++){
    TJanusHit& hit = janus->GetJanusHit(i);
    if(has_60keV){
      obj.FillHistogram("channel_energy_60keV_coinc",
                        128, 0, 128, hit.GetFrontChannel(),
                        4096, 0, 4096, hit.Charge());
      obj.FillHistogram("channel_energy_60keV_coinc",
                        128, 0, 128, hit.GetBackChannel(),
                        4096, 0, 4096, hit.GetBackHit().Charge());
    }




    for(int j=0; j<sega->Size(); j++){
      TSegaHit& s_hit = sega->GetSegaHit(j);
      if(hit.GetFrontChannel()==87){
        obj.FillHistogram("senergy_jcharge_innermost_ring",
                          4096, 0, 4096, hit.Charge(),
                          8000, 0, 4000, s_hit.GetEnergy());
      }

      // ---- Added by JAB 1/20/16 ----
      TVector3 SegaPos = s_hit.GetPosition();
      TVector3 JanusPos = hit.GetPosition();
      double CT_sj = SegaPos.Dot(JanusPos);

      obj.FillHistogram(Form("senergy_sjtheta_ring%02i",hit.GetRing()),
			280,-70,70, CT_sj,
			8000,0,4000,s_hit.GetEnergy());


      obj.FillHistogram(Form("dopper_with_ring%02i_gate",hit.GetRing()),
			//280,-70,70, s_hit.GetPosition().Angle(hit.GetPosition())*TMath::RadToDeg(),
			180,0,180, hit.GetPosition().Theta()*TMath::RadToDeg(),
			8000,0,4000,s_hit.GetDoppler(BETA,hit.GetPosition())); //Energy());



      // ---- ^^ End JAB ^^ ----
    }

    // ---- Added by JAB 1/20/16 ----

    obj.FillHistogram(Form("janus_theta_v_energy_det%02i",hit.GetDetnum()),
		      180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
		      4096,0,4096,hit.Charge());

    obj.FillHistogram(Form("janus_ring_v_energy_det%02i",hit.GetDetnum()),
		      30,0,30,hit.GetRing(),
		      4096,0,4096,hit.Charge());

    obj.FillHistogram(Form("janus_phi_v_energy_det%02i",hit.GetDetnum()),
		      360,-180,180,hit.GetPosition().Phi()*TMath::RadToDeg(),
		      4096,0,4096,hit.Charge());

    obj.FillHistogram(Form("janus_secotr_v_energy_det%02i",hit.GetDetnum()),
		      64,0,64,hit.GetSector(),
		      4096,0,4096,hit.Charge());




    // ---- ^^ End JAB ^^ ----

    obj.FillHistogram("channel_energy_any_coinc",
                      128, 0, 128, hit.GetFrontChannel(),
                      4096, 0, 4096, hit.Charge());
    obj.FillHistogram("channel_energy_any_coinc",
                      128, 0, 128, hit.GetBackChannel(),
                      4096, 0, 4096, hit.GetBackHit().Charge());


    // if(coinc_missing3){
    //   obj.FillHistogram("channel_energy_any_coinc_missing3",
    //                     128, 0, 128, hit.GetFrontChannel(),
    //                     4096, 0, 4096, hit.Charge());
    //   obj.FillHistogram("channel_energy_any_coinc_missing3",
    //                     128, 0, 128, hit.GetBackChannel(),
    //                     4096, 0, 4096, hit.GetBackHit().Charge());
    // }

    // if(coinc_missing4){
    //   obj.FillHistogram("channel_energy_any_coinc_missing4",
    //                     128, 0, 128, hit.GetFrontChannel(),
    //                     4096, 0, 4096, hit.Charge());
    //   obj.FillHistogram("channel_energy_any_coinc_missing4",
    //                     128, 0, 128, hit.GetBackChannel(),
    //                     4096, 0, 4096, hit.GetBackHit().Charge());
    // }
  }
}

void MakeLaBrCoincPlots(TRuntimeObjects& obj, TSega* sega, TJanus* janus) {
  bool has_labr_1332 = false;
  for(int i=0; i<janus->Size(); i++){
    TJanusHit& hit = janus->GetJanusHit(i);
    obj.FillHistogram("labr_energy",
                      8000, 0, 4000, hit.GetEnergy());
    if(hit.GetFrontChannel() == 26 &&
       hit.GetEnergy() > 1310 &&
       hit.GetEnergy() < 1350){
      obj.FillHistogram("labr_energy_gated",
                        8000, 0, 4000, hit.GetEnergy());
      has_labr_1332 = true;
      break;
    }
  }

  if(has_labr_1332){
    for(int i=0; i<sega->Size(); i++){
      TSegaHit& hit = sega->GetSegaHit(i);
      obj.FillHistogram("sega_energy_labr1332coinc",
                        8000, 0, 4000, hit.GetEnergy());
      obj.FillHistogram("sega_energy_summary_labr1332coinc",
                        16, 1, 17, hit.GetDetnum(),
                        8000, 0, 4000, hit.GetEnergy());
    }
  }
}

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
        crate1_ts = hit.Timestamp();
      } else if(hit.GetCrate()==2){
        crate2_ts = hit.Timestamp();
      } else if(hit.GetCrate()==3){
        crate3_ts = hit.Timestamp();
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
