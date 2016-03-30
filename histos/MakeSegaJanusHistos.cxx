#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
#include <string>
#include <cstdio>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TRandom.h"

#include "GValue.h"
#include "TObject.h"
#include "TSega.h"
#include "TJanus.h"
#include "TReaction.h"
#include "TPreserveGDirectory.h"

TCutG* pid_low = NULL;
TCutG* pid_high = NULL;
TCutG* time_energy = NULL;

void LoadGates(TRuntimeObjects& obj){
  if(!pid_low){
    pid_low = obj.GetCut("pid_low");
    if(!pid_low){
      std::cout << "Warning: could not find cut \"pid_low\"" << std::endl;
    }
  }

  if(!pid_high){
    pid_high = obj.GetCut("pid_high");
    if(!pid_high){
      std::cout << "Warning: could not find cut \"pid_high\"" << std::endl;
    }
  }

  if(!time_energy){
    time_energy = obj.GetCut("time_energy");
    if(!time_energy){
      std::cout << "Warning: could not find cut \"time_energy\"" << std::endl;
    }
  }
}

void MakeJanusHistograms(TRuntimeObjects& obj, TJanus& janus);
void MakeSegaHistograms(TRuntimeObjects& obj, TSega& sega);
void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega& sega, TJanus& janus);
void MakeTimestampDiffs(TRuntimeObjects& obj, TSega* sega, TJanus* janus);

// Returns the timestamp in nanoseconds since the start of the first production run.
// Minimum value: 0
// Maximum value: 2.141e14 (End of Pb-208 target runs)
// Total seconds: ~214100 seconds

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  LoadGates(obj);

  TSega* sega = obj.GetDetector<TSega>();
  TJanus* janus = obj.GetDetector<TJanus>();


  if(janus){
    MakeJanusHistograms(obj, *janus);
  }
  if(sega){
    MakeSegaHistograms(obj, *sega);
  }
  if(sega && janus){
    MakeCoincidenceHistograms(obj, *sega, *janus);
  }

  MakeTimestampDiffs(obj, sega, janus);
}

void MakeJanusHistograms(TRuntimeObjects& obj, TJanus& janus) {
  for(auto& chan : janus.GetAllChannels()){
    obj.FillHistogram("janus","channel",
                      128, 0, 128, chan.GetFrontChannel());
    obj.FillHistogram("janus","channel_charge",
                      128, 0, 128, chan.GetFrontChannel(),
                      6000, -4, 6000, chan.Charge());
    obj.FillHistogram("janus","channel_energy",
                      128, 0, 128, chan.GetFrontChannel(),
                      4000, 0, 400e3, chan.GetEnergy());
    obj.FillHistogram("janus","channel_time",
                      128, 0, 128, chan.GetFrontChannel(),
                      6000, 0, 6000, chan.Time());
  }


  obj.FillHistogram("janus","num_hits",
                    256, 0, 256, janus.Size());

  for(auto& hit : janus.GetAllHits()){
    obj.FillHistogram("janus","hit_channel",
                      128, 0, 128, hit.GetFrontChannel());
    obj.FillHistogram("janus","hit_channel_charge",
                      128, 0, 128, hit.GetFrontChannel(),
                      6000, -4, 6000, hit.Charge());
    obj.FillHistogram("janus","hit_channel_energy",
                      128, 0, 128, hit.GetFrontChannel(),
                      4000, 0, 400e3, hit.GetEnergy());
    obj.FillHistogram("janus","hit_channel_time",
                      128, 0, 128, hit.GetFrontChannel(),
                      6000, 0, 6000, hit.Time());
  }

  for(int i=0; i<janus.Size(); i++){
    TJanusHit& hit = janus.GetJanusHit(i);
    int hit_detnum = hit.GetDetnum();

    obj.FillHistogram("janus",Form("det%d_xy", hit_detnum),
                      100,-3,3,hit.GetPosition().X(),
                      100,-3,3,hit.GetPosition().Y());

    double theta = hit.GetPosition().Theta();
    double theta_deg = theta * TMath::RadToDeg();
    obj.FillHistogram("janus","theta",
                      180, 0, 180, theta_deg);
    if(pid_low->IsInside(hit.GetFrontChannel(), hit.Charge())) {
      static TReaction reac("78Kr","208Pb","78Kr","208Pb",3.9*78);
      // Convert from 208Pb angle to 78Kr angle
      double theta_78kr = reac.ConvertThetaLab(theta, 3, 2);
      obj.FillHistogram("janus","theta78Kr_recon",
                        180, 0, 180, theta_78kr * TMath::RadToDeg());
    }

    obj.FillHistogram("janus",Form("janus_Sector%02i_v_ring_det%02i",hit.GetSector(),hit_detnum),
                      40,0,40,hit.GetRing(),
                      6000,0,6000,hit.Charge());

    obj.FillHistogram("janus", "theta_v_energy",
		      180,0,180,theta_deg,
		      4000,0,400e3,hit.GetEnergy());

    obj.FillHistogram("janus","theta_v_charge",
		      180,0,180,theta_deg,
		      6000,0,6000,hit.Charge());

    obj.FillHistogram("janus",Form("ring_v_energy_det%02i",hit_detnum),
		      30,0,30,hit.GetRing(),
		      4000,0,400e3,hit.GetEnergy());

    obj.FillHistogram("janus",Form("phi_v_energy_det%02i",hit_detnum),
		      360,-180,180,hit.GetPosition().Phi()*TMath::RadToDeg(),
		      4000,0,400e3,hit.GetEnergy());

    obj.FillHistogram("janus",Form("sector_v_energy_det%02i",hit_detnum),
		      32,0,32,hit.GetSector(),
		      4000,0,400e3,hit.GetEnergy());

  }
}


void MakeSegaHistograms(TRuntimeObjects& obj, TSega& sega) {
  obj.FillHistogram("sega","num_hits",
                    256,0,256,sega.Size());

  long cc_timestamp = -1;
  long segment_timestamp = -1;
  for(int i=0; i<sega.Size(); i++){
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

void Make78KrPlots(TRuntimeObjects& obj, TSegaHit& s_hit, TJanusHit& j_hit) {
  double energy = s_hit.GetEnergy();
  TVector3 particle_position = j_hit.GetPosition();
  double time_diff = s_hit.Timestamp() - j_hit.Timestamp();
  double beta = GValue::Value("beta");

  obj.FillHistogram("kr78","energy_notimegate",
                    8000, 0, 4000, energy);
  obj.FillHistogram("kr78","energy_summary_notimegate",
                    18, 1, 19, s_hit.GetDetnum(),
                    8000, 0, 4000, energy);
  obj.FillHistogram("kr78","DCenergy_beamaxis_notimegate",
                    8000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta")));
  obj.FillHistogram("kr78","DCenergy_notimegate",
                    8000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), particle_position));



  if(time_energy->IsInside(energy, time_diff)){
    // Doppler corrected energies, using janus
    double dc_energy = s_hit.GetDoppler(beta, particle_position);

    obj.FillHistogram("kr78","energy",
                      4000, 0, 4000, s_hit.GetEnergy());
    obj.FillHistogram("kr78","DCenergy",
                      4000, 0, 4000, dc_energy);

    double theta_deg = s_hit.GetPosition().Angle(particle_position) * (180/3.1415926);
    obj.FillHistogram("kr78","energy_angle",
                      2000, 0, 2000, s_hit.GetEnergy(),
                      180, 0, 180, theta_deg);
    obj.FillHistogram("kr78","DCenergy_angle",
                      2000, 0, 2000, dc_energy,
                      180, 0, 180, theta_deg);


    obj.FillHistogram("kr78",Form("energy_angle_ring%02d", j_hit.GetRing()),
                      2000, 0, 2000, energy,
                      180, 0, 180, theta_deg);
    obj.FillHistogram("kr78",Form("DCenergy_angle_ring%02d", j_hit.GetRing()),
                      2000, 0, 2000, dc_energy,
                      180, 0, 180, theta_deg);


    // // Scan along beta
    // for(int beta_i = 0; beta_i<150; beta_i++) {
    //   double beta = 0.0 + beta_i*((0.15-0.00)/150);

    //   obj.FillHistogram("kr78","beamaxis_betascan",
    //                     150, 0.0, 0.15, beta,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta));

    //   obj.FillHistogram("kr78","betascan",
    //                     150, 0.0, 0.15, beta,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta, particle_position));
    // }

    // // Scan along z
    // for(int z=-20; z<20; z++) {
    //   TVector3 offset(0,0,z);
    //   obj.FillHistogram("kr78","DCenergy_zscan",
    //                     40, -20, 20, z,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta, particle_position, offset));
    // }
  }
}

void Make78KrPlots_Reconstructed(TRuntimeObjects& obj, TSegaHit& s_hit, TJanusHit& j_hit) {
  TVector3 particle_position = j_hit.GetConjugateDirection();
  double time_diff = s_hit.Timestamp() - j_hit.Timestamp();
  double beta = GValue::Value("beta_reconstructed");
  double energy = s_hit.GetEnergy();
  double dc_energy = s_hit.GetDoppler(beta, particle_position);

  obj.FillHistogram("kr78_recon","energy_notimegate",
                    8000, 0, 4000, energy);
  obj.FillHistogram("kr78_recon","energy_summary_notimegate",
                    18, 1, 19, s_hit.GetDetnum(),
                    8000, 0, 4000, energy);
  obj.FillHistogram("kr78_recon","DCenergy_beamaxis_notimegate",
                    8000, 0, 4000, s_hit.GetDoppler(beta));
  obj.FillHistogram("kr78_recon","DCenergy_notimegate",
                    8000, 0, 4000, dc_energy);



  if(time_energy->IsInside(energy, time_diff)){
    // Doppler corrected energies, using janus
    obj.FillHistogram("kr78_recon","energy",
                      4000, 0, 4000, energy);
    obj.FillHistogram("kr78_recon","DCenergy",
                      4000, 0, 4000, dc_energy);

    double theta_deg = s_hit.GetPosition().Angle(particle_position) * (180/3.14159);
    obj.FillHistogram("kr78_recon","energy_angle",
                      2000, 0, 2000, energy,
                      180, 0, 180, theta_deg);
    obj.FillHistogram("kr78_recon","DCenergy_angle",
                      2000, 0, 2000, dc_energy,
                      180, 0, 180, theta_deg);

    obj.FillHistogram("kr78_recon",Form("energy_angle_ring%02d", j_hit.GetRing()),
                      2000, 0, 2000, energy,
                      180, 0, 180, theta_deg);
    obj.FillHistogram("kr78_recon",Form("DCenergy_angle_ring%02d", j_hit.GetRing()),
                      2000, 0, 2000, dc_energy,
                      180, 0, 180, theta_deg);

    // // Scan along beta
    // for(int beta_i = 0; beta_i<150; beta_i++) {
    //   double beta = 0.0 + beta_i*((0.15-0.00)/150);
    //   obj.FillHistogram("kr78_recon","beamaxis_betascan",
    //                     150, 0.0, 0.15, beta,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta));

    //   obj.FillHistogram("kr78_recon","betascan",
    //                     150, 0.0, 0.15, beta,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta, particle_position));
    // }

    // // Scan along z
    // for(int z=-20; z<20; z++) {
    //   TVector3 offset(0,0,z);
    //   obj.FillHistogram("kr78_recon","DCenergy_zscan",
    //                     40, -20, 20, z,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta, particle_position, offset));
    // }

    double kr_theta_deg = particle_position.Theta() * (180/3.1415926);
    if(75 > kr_theta_deg || 105 < kr_theta_deg) {
      // DC energies ignoring area around 90 degrees
      obj.FillHistogram("kr78_recon","energy_notnear90",
                        4000, 0, 4000, energy);
      obj.FillHistogram("kr78_recon","DCenergy_notnear90",
                        4000, 0, 4000, dc_energy);

      double theta_deg = s_hit.GetPosition().Angle(particle_position) * (180/3.14159);
      obj.FillHistogram("kr78_recon","energy_angle_notnear90",
                        2000, 0, 2000, energy,
                        180, 0, 180, theta_deg);
      obj.FillHistogram("kr78_recon","DCenergy_angle_notnear90",
                        2000, 0, 2000, dc_energy,
                        180, 0, 180, theta_deg);

      // // Scan along z
      // for(int z=-20; z<20; z++) {
      //   TVector3 offset(0,0,z);
      //   obj.FillHistogram("kr78_recon","DCenergy_zscan_notnear90",
      //                     40, -20, 20, z,
      //                     8000, 0, 4000, s_hit.GetDoppler(beta, particle_position, offset));
      // }
    }
  }
}

void Make208PbPlots(TRuntimeObjects& obj, TSegaHit& s_hit, TJanusHit& j_hit) {
  double energy = s_hit.GetEnergy();
  TVector3 particle_position = j_hit.GetPosition();
  double time_diff = s_hit.Timestamp() - j_hit.Timestamp();

  obj.FillHistogram("pb208","energy_notimegate",
                    8000, 0, 4000, energy);
  obj.FillHistogram("pb208","energy_summary_notimegate",
                    18, 1, 19, s_hit.GetDetnum(),
                    8000, 0, 4000, energy);
  obj.FillHistogram("pb208","DCenergy_beamaxis_notimegate",
                    8000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta")));
  obj.FillHistogram("pb208","DCenergy_notimegate",
                    8000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), particle_position));



  if(time_energy->IsInside(energy, time_diff)){
    // Doppler corrected energies, using janus
    double dc_energy = s_hit.GetDoppler(GValue::Value("beta"), particle_position);

    obj.FillHistogram("pb208","energy",
                      4000, 0, 4000, s_hit.GetEnergy());
    obj.FillHistogram("pb208","DCenergy",
                      4000, 0, 4000, dc_energy);

    double theta_deg = s_hit.GetPosition().Angle(particle_position);
    obj.FillHistogram("pb208","energy_angle",
                      2000, 0, 2000, s_hit.GetEnergy(),
                      180, 0, 180, theta_deg);
    obj.FillHistogram("pb208","DCenergy_angle",
                      2000, 0, 2000, dc_energy,
                      180, 0, 180, theta_deg);


    // // Scan along beta
    // for(int beta_i = 0; beta_i<150; beta_i++) {
    //   double beta = 0.0 + beta_i*((0.15-0.00)/150);

    //   obj.FillHistogram("pb208","beamaxis_betascan",
    //                     150, 0.0, 0.15, beta,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta));

    //   obj.FillHistogram("pb208","betascan",
    //                     150, 0.0, 0.15, beta,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta, particle_position));
    // }

    // // Scan along z
    // for(int z=-20; z<20; z++) {
    //   double beta = GValue::Value("beta");
    //   TVector3 offset(0,0,z);
    //   obj.FillHistogram("pb208","DCenergy_zscan",
    //                     40, -20, 20, z,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta, particle_position, offset));
    // }
  }
}

void MakeTimeDependentHistograms(TRuntimeObjects& obj, TSega& sega, TJanus& janus) {
  for(int i=0; i<sega.Size(); i++){
    TSegaHit& hit = sega.GetSegaHit(i);
    obj.FillHistogram("traceheight","sega_traceheight_summary",
                      16, 1, 17, hit.GetDetnum(),
                      3100, -100, 3000, hit.GetTraceHeight());
    obj.FillHistogram("traceheight",Form("sega_traceheight_det%02d_timestamp",hit.GetDetnum()),
                      214100/60, 0, 2.141e14, hit.Timestamp(),
                      3100, -100, 3000, hit.GetTraceHeight());
  }

  for(auto& j_hit : janus.GetAllHits()){
    bool in_pid_high = pid_high->IsInside(j_hit.GetFrontChannel(), j_hit.Charge());
    for(int i=0; i<sega.Size(); i++){
      TSegaHit& s_hit = sega.GetSegaHit(i);
      if(in_pid_high){
        if(time_energy->IsInside(s_hit.GetEnergy(),
                                 s_hit.Timestamp() - j_hit.Timestamp())){
          int detnum = s_hit.GetDetnum();
          double timestamp = s_hit.Timestamp();
          double beta = GValue::Value("beta");
          TVector3 particle_pos = j_hit.GetPosition();

          double dc_energy = s_hit.GetDoppler(beta, particle_pos);
          obj.FillHistogram("traceheight",Form("DCenergy_det%02d_time",detnum),
                            214100/60, 0, 2.141e14, timestamp,
                            1000, 0, 2000, dc_energy);

          double dc_traceheight = s_hit.GetTraceHeightDoppler(beta, particle_pos);
          obj.FillHistogram("traceheight",Form("DCtraceheight_det%02d_timestamp",detnum),
                            214100/60, 0, 2.141e14, timestamp,
                            3100, -100, 3000, dc_traceheight);
        }
      }
    }
  }
}

void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega& sega, TJanus& janus) {
  bool has_455keV = false;

  for(int i=0; i<sega.Size(); i++){
    TSegaHit& hit = sega.GetSegaHit(i);
    if(hit.GetEnergy()>425 && hit.GetEnergy()<485){
      has_455keV = true;
    }
    obj.FillHistogram("coinc","sega_energy_janus_tdiff",
                      4000, 0, 4000, hit.GetEnergy(),
                      1000, -5000, 5000, hit.Timestamp() - janus.Timestamp());
  }


  for(int i=0; i<janus.Size(); i++){
    TJanusHit& j_hit = janus.GetJanusHit(i);
    if(has_455keV){
      obj.FillHistogram("coinc","janus_channel_energy_455keV_coinc",
                        128, 0, 128, j_hit.GetFrontChannel(),
                        6000, 0, 6000, j_hit.Charge());
      obj.FillHistogram("coinc","janus_channel_energy_455keV_coinc",
                        128, 0, 128, j_hit.GetBackChannel(),
                        6000, 0, 6000, j_hit.GetBackHit().Charge());
    }


    obj.FillHistogram("coinc","janus_channel_energy",
                      128, 0, 128, j_hit.GetFrontChannel(),
                      6000, 0, 6000, j_hit.Charge());
    obj.FillHistogram("coinc","janus_channel_energy",
                      128, 0, 128, j_hit.GetBackChannel(),
                      6000, 0, 6000, j_hit.GetBackHit().Charge());
  }

  for(auto& j_hit : janus.GetAllHits()) {
    bool in_pid_low = pid_low->IsInside(j_hit.GetFrontChannel(), j_hit.Charge());
    bool in_pid_high = pid_high->IsInside(j_hit.GetFrontChannel(), j_hit.Charge());

    for(int i=0; i<sega.Size(); i++){
      TSegaHit& s_hit = sega.GetSegaHit(i);
      if(in_pid_low){
        Make208PbPlots(obj, s_hit, j_hit);
        Make78KrPlots_Reconstructed(obj, s_hit, j_hit);
      }
      if(in_pid_high){
        Make78KrPlots(obj, s_hit, j_hit);
      }
    }
  }
}

void MakeTimestampDiffs(TRuntimeObjects& obj, TSega* sega, TJanus* janus) {

  if(sega && janus) {
    obj.FillHistogram("tdiff","sega_janus_tdiff",
                      1000, -5000, 5000, sega->Timestamp() - janus->Timestamp());

    for(int i=0; i<sega->Size(); i++){
      TSegaHit& hit = sega->GetSegaHit(i);
      obj.FillHistogram("tdiff","sega_detnum_janus_tdiff",
                        16, 1, 17, hit.GetDetnum(),
                        1000, -5000, 5000, hit.Timestamp() - janus->Timestamp());
    }
  }

  long crate1_ts = -1;
  long crate2_ts = -1;
  long crate3_ts = -1;
  long analog_ts = -1;
  std::map<int,long> source_ids;

  if(janus){
    analog_ts = janus->Timestamp();
    source_ids[4] = analog_ts;
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

      source_ids[hit.GetCrate()] = hit.Timestamp();
    }
  }

  for(auto& item : source_ids) {
    obj.FillHistogram("tdiff","timestamp_sourceid",
                      2.141e14/60e9, 0, 2.141e14, item.second,
                      4, 1, 5, item.first);
  }


  if(crate1_ts!=-1 && crate2_ts!=-1){
    obj.FillHistogram("tdiff","crate1_crate2",
                      600, -3000, 3000, crate2_ts - crate1_ts);
  }
  if(crate1_ts!=-1 && crate3_ts!=-1){
    obj.FillHistogram("tdiff", "crate1_crate3",
                      600, -3000, 3000, crate3_ts - crate1_ts);
  }
  if(crate2_ts!=-1 && crate3_ts!=-1){
    obj.FillHistogram("tdiff", "crate2_crate3",
                      600, -3000, 3000, crate3_ts - crate2_ts);
  }
  if(analog_ts!=-1 && crate1_ts!=-1){
    obj.FillHistogram("tdiff", "analog_crate1",
                      600, -3000, 3000, analog_ts - crate1_ts);
  }
  if(analog_ts!=-1 && crate2_ts!=-1){
    obj.FillHistogram("tdiff", "analog_crate2",
                      600, -3000, 3000, analog_ts - crate2_ts);
  }
  if(analog_ts!=-1 && crate3_ts!=-1){
    obj.FillHistogram("tdiff", "analog_crate3",
                      600, -3000, 3000, analog_ts - crate3_ts);
  }
}
