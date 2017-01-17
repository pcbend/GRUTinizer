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
#include "TJanus.h"
#include "TNSCLScalers.h"
#include "TReaction.h"
#include "TSega.h"
#include "TSRIM.h"

enum class WhichJanus {
  Upstream,
    Downstream,
    Recon
};

std::string to_string(WhichJanus which_janus) {
  switch(which_janus) {
    case WhichJanus::Upstream:
      return "upstream";

    case WhichJanus::Downstream:
      return "downstream";

    case WhichJanus::Recon:
      return "recon";

    default:
      return "";
  }
}

enum class GammaSeen {
  keV_455,
    keV_561,
    keV_665,
    keV_691,
    keV_857,
    keV_1146,
    keV_1298
};

std::string to_string(GammaSeen gamma_seen) {
  switch(gamma_seen) {
    case GammaSeen::keV_455:
      return "0455";

    case GammaSeen::keV_561:
      return "0561";

    case GammaSeen::keV_665:
      return "0665";

    case GammaSeen::keV_691:
      return "0691";

    case GammaSeen::keV_857:
      return "0857";

    case GammaSeen::keV_1146:
      return "1146";

    case GammaSeen::keV_1298:
      return "1298";

    default:
      return "";
  }
}

struct GammaEnergyCut {
  int ring;
  WhichJanus which_janus;
  GammaSeen gamma_seen;
  TCutG* DCgamma_angle_gate;
};

TCutG* pid_low = NULL;
TCutG* pid_high = NULL;
TCutG* time_energy = NULL;
std::vector<GammaEnergyCut> gates;

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

  if(gates.size() == 0) {
    for(int ringnum=1; ringnum<25; ringnum++) {
      std::string name = Form("downstream_ring%02d_455cut",ringnum);
      TCutG* gate = obj.GetCut(name);
      if(gate) {
        GammaEnergyCut cut{ringnum, WhichJanus::Downstream, GammaSeen::keV_455, gate};
        gates.push_back(cut);
      } else {
        std::cout << "Warning: could not find cut \"" << name << "\"" << std::endl;
      }
    }

    for(int ringnum=1; ringnum<25; ringnum++) {
      std::string name = Form("recon_ring%02d_455cut",ringnum);
      TCutG* gate = obj.GetCut(name);
      if(gate) {
        GammaEnergyCut cut{ringnum, WhichJanus::Recon, GammaSeen::keV_455, gate};
        gates.push_back(cut);
      } else {
        std::cout << "Warning: could not find cut \"" << name << "\"" << std::endl;
      }
    }

    for(int ringnum=1; ringnum<25; ringnum++) {
      std::string name = Form("upstream_ring%02d_455cut",ringnum);
      TCutG* gate = obj.GetCut(name);
      if(gate) {
        GammaEnergyCut cut{ringnum, WhichJanus::Upstream, GammaSeen::keV_455, gate};
        gates.push_back(cut);
      } else {
        std::cout << "Warning: could not find cut \"" << name << "\"" << std::endl;
      }
    }
  }
}

double get_beta(double betamax, double kr_angle_rad, bool energy_loss=false) {
  // Factors of 1e3 are because TNucleus and TReaction use MeV, while TSRIM uses keV.

  static auto kr = std::make_shared<TNucleus>("78Kr");
  static auto pb = std::make_shared<TNucleus>("208Pb");
  static TSRIM srim("kr78_in_pb208");

  double thickness = (1.0 / 11342.0) * 1e4; // (1 mg/cm^2) / (11342 mg/cm^3) * (10^4 um/cm)

  double pre_collision_energy_MeV = kr->GetEnergyFromBeta(betamax);
  if(energy_loss) {
    pre_collision_energy_MeV = srim.GetAdjustedEnergy(pre_collision_energy_MeV*1e3, thickness/2)/1e3;
  }

  TReaction reac(kr, pb, kr, pb, pre_collision_energy_MeV);

  double post_collision_energy_MeV = reac.GetTLab(kr_angle_rad, 2);

  if(energy_loss) {
    double distance_travelled = (thickness/2)/std::abs(std::cos(kr_angle_rad));
    post_collision_energy_MeV = srim.GetAdjustedEnergy(post_collision_energy_MeV*1e3, distance_travelled)/1e3;
  }

  double beta = kr->GetBetaFromEnergy(post_collision_energy_MeV);
  return beta;
}

void MakeJanusHistograms(TRuntimeObjects& obj, TJanus& janus, const char* dirname="janus");
void MakeJanusChan64_70Histograms(TRuntimeObjects& obj, TJanus& janus, const char* dirname="janus");
void MakeSegaHistograms(TRuntimeObjects& obj, TSega& sega);
void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega& sega, TJanus& janus);
void MakeScalerHistograms(TRuntimeObjects& obj, TNSCLScalers& scalers);
void MakeTimestampDiffs(TRuntimeObjects& obj, TSega* sega, TJanus* janus);

void Make78Kr_Gamma_Plots(TRuntimeObjects& obj, TSegaHit& s_hit, TJanusHit& j_hit,
                          double dc_energy, double gamma_angle,
                          WhichJanus which_janus, GammaSeen gamma_seen);

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
  //TNSCLScalers* scalers = obj.GetDetector<TNSCLScalers>();


  if(janus){
    MakeJanusHistograms(obj, *janus);
  }
  if(sega){
    MakeSegaHistograms(obj, *sega);
  }
  if(sega && janus){
    MakeCoincidenceHistograms(obj, *sega, *janus);
  }
  // if(scalers){
  //   MakeScalerHistograms(obj, *scalers);
  // }

  // MakeTimestampDiffs(obj, sega, janus);
}

void MakeJanusHistograms(TRuntimeObjects& obj, TJanus& janus, const char* dirname) {
  for(auto& chan : janus.GetAllChannels()){
    int channum = chan.GetFrontChannel();
    obj.FillHistogram(dirname,"channel",
                      128, 0, 128, channum);
    if(chan.Charge() >= 0) {
      obj.FillHistogram(dirname,"channel_charge",
                        128, 0, 128, channum,
                        6000, -4, 6000, chan.Charge());
      obj.FillHistogram(dirname,"channel_energy",
                        128, 0, 128, channum,
                        4000, 0, 400e3, chan.GetEnergy());
    }
    if(chan.Time() >= 0) {
      obj.FillHistogram(dirname,"channel_time",
                        128, 0, 128, channum,
                        6000, 0, 6000, chan.Time());
    }


  }


  // MakeJanusChan64_70Histograms(obj, janus, dirname);

  // int num_adc = 0;
  // int num_tdc = 0;
  // for(auto& chan : janus.GetAllChannels()) {
  //   if(chan.Charge() > -1) {
  //     num_adc++;
  //   }
  //   if(chan.Time() > -1) {
  //     num_tdc++;
  //   }
  // }
  // obj.FillHistogram(dirname,"total_adc_tdc",
  //                   128, 0, 128, num_adc,
  //                   128, 0, 128, num_tdc);

  obj.FillHistogram(dirname, "total_bytes",
                    2000, 0, 2000, janus.TotalBytes());

  // for(auto& chan_adc : janus.GetAllChannels()) {
  //   for(auto& chan_tdc : janus.GetAllChannels()) {
  //     if((chan_tdc.Time() > 150 || chan_tdc.GetTDCOverflowBit()) &&
  //        (chan_adc.Charge() > 150 || chan_adc.GetADCOverflowBit())) {
  //       obj.FillHistogram(dirname,"validadc_validtdc",
  //                         128, 0, 128, chan_adc.GetFrontChannel(),
  //                         128, 0, 128, chan_tdc.GetFrontChannel());
  //     }

  //     if( chan_tdc.Time()>=0 && chan_adc.Charge()>=0) {
  //       obj.FillHistogram(dirname,"presentadc_presenttdc",
  //                         128, 0, 128, chan_adc.GetFrontChannel(),
  //                         128, 0, 128, chan_tdc.GetFrontChannel());
  //       obj.FillHistogram(dirname,Form("presentadc_presenttdc_size%04d", janus.TotalBytes()),
  //                         128, 0, 128, chan_adc.GetFrontChannel(),
  //                         128, 0, 128, chan_tdc.GetFrontChannel());
  //     }
  //   }
  // }

  // for(auto& chan : janus.GetAllChannels()) {
  //   if((chan.Time() > 150 || chan.GetTDCOverflowBit()) &&
  //      chan.Charge() >= 0) {
  //     obj.FillHistogram(dirname,"channel_charge_validtdc",
  //                       150, -5, 145, chan.GetFrontChannel(),
  //                       9000, -500, 8500, chan.Charge() + 4096*chan.GetADCOverflowBit());
  //   }
  // }


  obj.FillHistogram(dirname,"num_hits",
                    256, 0, 256, janus.Size());

  for(auto& hit : janus.GetAllHits()){
    bool in_pid_low = pid_low->IsInside(hit.GetFrontChannel(), hit.Charge());
    bool in_pid_high = pid_high->IsInside(hit.GetFrontChannel(), hit.Charge());

    obj.FillHistogram(dirname,"hit_channel",
                      128, 0, 128, hit.GetFrontChannel());
    obj.FillHistogram(dirname,"hit_channel",
                      128, 0, 128, hit.GetBackChannel());

    obj.FillHistogram(dirname,"hit_channel_charge",
                      128, 0, 128, hit.GetFrontChannel(),
                      6000, -4, 6000, hit.Charge());
    obj.FillHistogram(dirname,"hit_channel_charge",
                      128, 0, 128, hit.GetBackChannel(),
                      6000, -4, 6000, hit.GetBackHit().Charge());

    if(hit.GetADCOverflowBit()) {
      obj.FillHistogram(dirname,"hit_channel_charge_overflow",
                        128, 0, 128, hit.GetFrontChannel(),
                        6000, -4, 6000, hit.Charge());
    } else {
      obj.FillHistogram(dirname,"hit_channel_charge_nonoverflow",
                        128, 0, 128, hit.GetFrontChannel(),
                        6000, -4, 6000, hit.Charge());
    }

    obj.FillHistogram(dirname,"hit_channel_energy",
                      128, 0, 128, hit.GetFrontChannel(),
                      4000, 0, 400e3, hit.GetEnergy());
    obj.FillHistogram(dirname,"hit_channel_energy",
                      128, 0, 128, hit.GetBackChannel(),
                      4000, 0, 400e3, hit.GetBackHit().GetEnergy());

    obj.FillHistogram(dirname,"hit_channel_time",
                      128, 0, 128, hit.GetFrontChannel(),
                      6000, 0, 6000, hit.Time());
    obj.FillHistogram(dirname,"hit_channel_time",
                      128, 0, 128, hit.GetBackChannel(),
                      6000, 0, 6000, hit.GetBackHit().Time());

    obj.FillHistogram(dirname,Form("hit_det%d_ringnum",hit.GetDetnum()),
                      24, 1, 25, hit.GetRing());
    obj.FillHistogram(dirname,Form("hit_det%d_sectornum",hit.GetDetnum()),
                      32, 1, 33, hit.GetSector());

    obj.FillHistogram(dirname,Form("hit_det%d_ringsector",hit.GetDetnum()),
                      24, 1, 25, hit.GetRing(),
                      32, 1, 33, hit.GetSector());

    if(in_pid_low) {
      obj.FillHistogram(dirname,"channel_charge_forwardPb",
                        128, 0, 128, hit.GetFrontChannel(),
                        6000, -4, 6000, hit.Charge());

      obj.FillHistogram(dirname,"ringsector_forward_pb",
                        24, 1, 25, hit.GetRing(),
                        32, 1, 33, hit.GetSector());
    }
    if(in_pid_high) {
      obj.FillHistogram(dirname,"channel_charge_forwardKr",
                        128, 0, 128, hit.GetFrontChannel(),
                        6000, -4, 6000, hit.Charge());

      obj.FillHistogram(dirname,"ringsector_forward_kr",
                        24, 1, 25, hit.GetRing(),
                        32, 1, 33, hit.GetSector());
    }
    bool in_upstream_pid_low  = hit.GetDetnum() == 0 && !(hit.Charge() > 3600 || hit.GetADCOverflowBit());
    if(in_upstream_pid_low) {
      obj.FillHistogram(dirname,"channel_charge_backwardKr",
                        128, 0, 128, hit.GetFrontChannel(),
                        6000, -4, 6000, hit.Charge());

      obj.FillHistogram(dirname,"ringsector_backward_kr",
                        24, 1, 25, hit.GetRing(),
                        32, 1, 33, hit.GetSector());
    }
  }

  for(unsigned int i=0; i<janus.Size(); i++){
    TJanusHit& hit = janus.GetJanusHit(i);
    int hit_detnum = hit.GetDetnum();

    obj.FillHistogram(dirname,Form("det%d_xy", hit_detnum),
                      100,-4,4,hit.GetPosition().X(),
                      100,-4,4,hit.GetPosition().Y());

    double theta = hit.GetPosition().Theta();
    double theta_deg = theta * TMath::RadToDeg();
    obj.FillHistogram(dirname,"theta",
                      180, 0, 180, theta_deg);
    if(pid_low->IsInside(hit.GetFrontChannel(), hit.Charge())) {
      static TReaction reac("78Kr","208Pb","78Kr","208Pb",3.9*78);
      // Convert from 208Pb angle to 78Kr angle
      double theta_78kr = reac.ConvertThetaLab(theta, 3, 2);
      obj.FillHistogram(dirname,"theta78Kr_recon",
                        180, 0, 180, theta_78kr * TMath::RadToDeg());
    }

    obj.FillHistogram(dirname,Form("janus_Sector%02i_v_ring_det%02i",hit.GetSector(),hit_detnum),
                      40,0,40,hit.GetRing(),
                      6000,0,6000,hit.Charge());

    obj.FillHistogram(dirname, "theta_v_energy",
		      180,0,180,theta_deg,
		      4000,0,400e3,hit.GetEnergy());

    obj.FillHistogram(dirname,"theta_v_charge",
		      180,0,180,theta_deg,
		      6000,0,6000,hit.Charge());

    obj.FillHistogram(dirname,Form("ring_v_energy_det%02i",hit_detnum),
		      30,0,30,hit.GetRing(),
		      4000,0,400e3,hit.GetEnergy());

    obj.FillHistogram(dirname,Form("phi_v_energy_det%02i",hit_detnum),
		      360,-180,180,hit.GetPosition().Phi()*TMath::RadToDeg(),
		      4000,0,400e3,hit.GetEnergy());

    obj.FillHistogram(dirname,Form("sector_v_energy_det%02i",hit_detnum),
		      32,0,32,hit.GetSector(),
		      4000,0,400e3,hit.GetEnergy());

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

void Make78KrPlots(TRuntimeObjects& obj, TSegaHit& s_hit, TJanusHit& j_hit, std::string dir="kr78") {
  double energy = s_hit.GetEnergy();
  TVector3 particle_position = j_hit.GetPosition();
  double time_diff = s_hit.Timestamp() - j_hit.Timestamp();
  double beta = GValue::Value("beta");

  obj.FillHistogram(dir.c_str(),"energy_notimegate",
                    8000, 0, 4000, energy);
  obj.FillHistogram(dir.c_str(),"energy_summary_notimegate",
                    18, 1, 19, s_hit.GetDetnum(),
                    8000, 0, 4000, energy);
  obj.FillHistogram(dir.c_str(),"DCenergy_beamaxis_notimegate",
                    8000, 0, 4000, s_hit.GetDoppler(beta));
  obj.FillHistogram(dir.c_str(),"DCenergy_notimegate",
                    8000, 0, 4000, s_hit.GetDoppler(beta, particle_position));

  if(time_energy->IsInside(energy, time_diff)){
    // Doppler corrected energies, using janus
    double dc_energy = s_hit.GetDoppler(beta, particle_position);

    if(dc_energy > 440 && dc_energy < 465) {
      obj.FillHistogram(dir.c_str(),"janus_channel_time_455keV_coinc",
                        128, 0, 128, j_hit.GetFrontChannel(),
                        6000, 0, 6000, j_hit.Time());
      obj.FillHistogram(dir.c_str(),"janus_channel_time_455keV_coinc",
                        128, 0, 128, j_hit.GetBackChannel(),
                        6500, -500, 6000, j_hit.GetBackHit().Time());
    }

    obj.FillHistogram(dir.c_str(),"energy",
                      4000, 0, 4000, s_hit.GetEnergy());
    obj.FillHistogram(dir.c_str(),"DCenergy",
                      4000, 0, 4000, dc_energy);

    double theta_deg = s_hit.GetPosition().Angle(particle_position) * (180/3.1415926);
    obj.FillHistogram(dir.c_str(),"energy_angle",
                      2000, 0, 2000, s_hit.GetEnergy(),
                      180, 0, 180, theta_deg);
    obj.FillHistogram(dir.c_str(),"DCenergy_angle",
                      2000, 0, 2000, dc_energy,
                      180, 0, 180, theta_deg);


    obj.FillHistogram(dir.c_str(),Form("energy_angle_ring%02d", j_hit.GetRing()),
                      2000, 0, 2000, energy,
                      180, 0, 180, theta_deg);
    obj.FillHistogram(dir.c_str(),Form("DCenergy_angle_ring%02d", j_hit.GetRing()),
                      2000, 0, 2000, dc_energy,
                      180, 0, 180, theta_deg);

    // if(dc_energy > 430 && dc_energy < 480) {
    //   Make78Kr_455keVPlots(obj, s_hit, j_hit, dc_energy, theta_deg, "1d", "downstream");
    // }
    for(auto& gate : gates) {
      if(gate.which_janus == WhichJanus::Downstream &&
         gate.ring == j_hit.GetRing() &&
         gate.DCgamma_angle_gate->IsInside(dc_energy, theta_deg)) {
        Make78Kr_Gamma_Plots(obj, s_hit, j_hit, dc_energy, theta_deg,
                             gate.which_janus, gate.gamma_seen);
      }
    }


    double kr_theta = particle_position.Theta();
    //double kr_theta_deg = kr_theta * (180/3.1415926);

    // // Angle-dependent beta
    // {
    //   double betamax = GValue::Value("betamax");

    //   obj.FillHistogram(dir.c_str(),Form("DCenergy_angle_varybeta_ring%02d", j_hit.GetRing()),
    //                     2000, 0, 2000, s_hit.GetDoppler(get_beta(betamax,kr_theta), particle_position),
    //                     180, 0, 180, theta_deg);

    //   obj.FillHistogram(dir.c_str(),Form("DCenergy_angle_varybeta_Ecorr_ring%02d", j_hit.GetRing()),
    //                     2000, 0, 2000, s_hit.GetDoppler(get_beta(betamax,kr_theta,true), particle_position),
    //                     180, 0, 180, theta_deg);

    //   // phi_deg varies -180 to 180
    //   double phi_deg = particle_position.Phi() * TMath::RadToDeg();
    //   int quadrant;
    //   if(phi_deg < -90) {
    //     quadrant = 3;
    //   } else if (phi_deg >= -90 && phi_deg < 0) {
    //     quadrant = 4;
    //   } else if (phi_deg >= 0 && phi_deg < 90) {
    //     quadrant = 1;
    //   } else {
    //     quadrant = 2;
    //   }
    //   obj.FillHistogram(dir.c_str(),Form("DCenergy_angle_varybeta_Ecorr_ring%02d_quad%d", j_hit.GetRing(), quadrant),
    //                     2000, 0, 2000, s_hit.GetDoppler(get_beta(betamax,kr_theta,true), particle_position),
    //                     180, 0, 180, theta_deg);

    //   // //Scan along betamax
    //   // for(int betamax_i = 0; betamax_i<150; betamax_i++) {
    //   //   double betamax = 0.0 + betamax_i*((0.15-0.00)/150);
    //   //   obj.FillHistogram(dir.c_str(),"DCenergy_varybeta_betascan",
    //   //                     150, 0.0, 0.15, betamax,
    //   //                     8000, 0, 4000, s_hit.GetDoppler(get_beta(betamax, kr_theta), particle_position));
    //   //   obj.FillHistogram(dir.c_str(),"DCenergy_varybeta_betascan_Ecorr",
    //   //                     150, 0.0, 0.15, betamax,
    //   //                     8000, 0, 4000, s_hit.GetDoppler(get_beta(betamax, kr_theta,true), particle_position));
    //   // }
    // }

    // // Scan along beta
    // for(int beta_i = 0; beta_i<150; beta_i++) {
    //   double beta = 0.0 + beta_i*((0.15-0.00)/150);

    //   obj.FillHistogram(dir.c_str(),"beamaxis_betascan",
    //                     150, 0.0, 0.15, beta,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta));

    //   obj.FillHistogram(dir.c_str(),"betascan",
    //                     150, 0.0, 0.15, beta,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta, particle_position));
    // }

    // // Scan along z
    // for(int z_i=-20; z_i<40; z_i++) {
    //   double z = z_i * 0.25;
    //   TVector3 offset(0,0,z);
    //   obj.FillHistogram(dir.c_str(),"DCenergy_zscan",
    //                     60, -5, 10, z,
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

    double kr_theta = particle_position.Theta();
    double kr_theta_deg = kr_theta * (180/3.1415926);

    // if(dc_energy > 430 && dc_energy < 480) {
    //   Make78Kr_455keVPlots(obj, s_hit, j_hit, dc_energy, theta_deg, "1d", "recon");
    // }
    for(auto& gate : gates) {
      if(gate.which_janus == WhichJanus::Recon &&
         gate.ring == j_hit.GetRing() &&
         gate.DCgamma_angle_gate->IsInside(dc_energy, theta_deg)) {
        Make78Kr_Gamma_Plots(obj, s_hit, j_hit, dc_energy, theta_deg,
                             gate.which_janus, gate.gamma_seen);
      }
    }

    // // Angle-dependent beta
    // {
    //   double betamax = GValue::Value("betamax");
    //   obj.FillHistogram("kr78_recon",Form("DCenergy_angle_varybeta_ring%02d", j_hit.GetRing()),
    //                     2000, 0, 2000, s_hit.GetDoppler(get_beta(betamax, kr_theta), particle_position),
    //                     180, 0, 180, theta_deg);

    //   obj.FillHistogram("kr78_recon",Form("DCenergy_angle_varybeta_Ecorr_ring%02d", j_hit.GetRing()),
    //                     2000, 0, 2000, s_hit.GetDoppler(get_beta(betamax, kr_theta,true), particle_position),
    //                     180, 0, 180, theta_deg);

    //         // phi_deg varies -180 to 180
    //   double phi_deg = particle_position.Phi() * TMath::RadToDeg();
    //   int quadrant;
    //   if(phi_deg < -90) {
    //     quadrant = 3;
    //   } else if (phi_deg >= -90 && phi_deg < 0) {
    //     quadrant = 4;
    //   } else if (phi_deg >= 0 && phi_deg < 90) {
    //     quadrant = 1;
    //   } else {
    //     quadrant = 2;
    //   }
    //   obj.FillHistogram("kr78_recon",Form("DCenergy_angle_varybeta_Ecorr_ring%02d_quad%d", j_hit.GetRing(), quadrant),
    //                     2000, 0, 2000, s_hit.GetDoppler(get_beta(betamax, kr_theta,true), particle_position),
    //                     180, 0, 180, theta_deg);

    // }

    // //Scan along betamax
    // for(int betamax_i = 0; betamax_i<150; betamax_i++) {
    //   double betamax = 0.0 + betamax_i*((0.15-0.00)/150);
    //   obj.FillHistogram("kr78_recon","DCenergy_varybeta_betascan",
    //                     150, 0.0, 0.15, betamax,
    //                     8000, 0, 4000, s_hit.GetDoppler(get_beta(betamax, kr_theta), particle_position));
    //   obj.FillHistogram("kr78_recon","DCenergy_varybeta_betascan_Ecorr",
    //                     150, 0.0, 0.15, betamax,
    //                     8000, 0, 4000, s_hit.GetDoppler(get_beta(betamax, kr_theta,true), particle_position));
    // }

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
    // for(int z_i=-20; z_i<40; z_i++) {
    //   double z = z_i * 0.25;
    //   TVector3 offset(0,0,z);
    //   obj.FillHistogram("kr78_recon","DCenergy_zscan",
    //                     60, -5, 10, z,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta, particle_position, offset));
    // }

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


      // //Scan along betamax
      // for(int betamax_i = 0; betamax_i<150; betamax_i++) {
      //   double betamax = 0.0 + betamax_i*((0.15-0.00)/150);
      //   obj.FillHistogram("kr78_recon","DCenergy_varybeta_betascan_notnear90",
      //                     150, 0.0, 0.15, betamax,
      //                     8000, 0, 4000, s_hit.GetDoppler(get_beta(betamax, kr_theta), particle_position));
      //   obj.FillHistogram("kr78_recon","DCenergy_varybeta_betascan_Ecorr_notnear90",
      //                     150, 0.0, 0.15, betamax,
      //                     8000, 0, 4000, s_hit.GetDoppler(get_beta(betamax, kr_theta, true), particle_position));
      // }

      // // Scan along z
      // for(int z_i=-20; z_i<40; z_i++) {
      //   double z = z_i * 0.25;
      //   TVector3 offset(0,0,z);
      //   obj.FillHistogram("kr78_recon","DCenergy_zscan_notnear90",
      //                     60, -5, 10, z,
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

void MakeUpstream78KrPlots(TRuntimeObjects& obj, TSegaHit& s_hit, TJanusHit& j_hit, std::string cutname) {
  obj.FillHistogram("upstream", "time_energy",
                    4000, 0, 4000, s_hit.GetEnergy(),
                    1000, -5000, 5000, s_hit.Timestamp() - j_hit.Timestamp());

  double beta_nominal = GValue::Value("beta_upstream");
  TVector3 particle_position = j_hit.GetPosition();
  double energy = s_hit.GetEnergy();
  double time_diff = s_hit.Timestamp() - j_hit.Timestamp();
  double kr_theta = particle_position.Theta();

  // obj.FillHistogram("upstream", Form("energy_%s_notimegate",cutname.c_str()),
  //                   4000, 0, 4000, s_hit.GetEnergy());

  // obj.FillHistogram("upstream", Form("energyDC_%s_notimegate", cutname.c_str()),
  //                   4000, 0, 4000, s_hit.GetDoppler(beta_nominal, particle_position));

  if(time_energy->IsInside(energy, time_diff)) {


    obj.FillHistogram("upstream", Form("energy_%s",cutname.c_str()),
                      4000, 0, 4000, s_hit.GetEnergy());

    double dc_energy = s_hit.GetDoppler(beta_nominal, particle_position);
    obj.FillHistogram("upstream", Form("DCenergy_%s", cutname.c_str()),
                      4000, 0, 4000, dc_energy);

    // if(j_hit.GetRing() <= 8) {
    //   obj.FillHistogram("upstream", Form("DCenergy_inner8rings_%s", cutname.c_str()),
    //                     4000, 0, 4000, dc_energy);
    // }


    double theta_deg = s_hit.GetPosition().Angle(particle_position) * (180/3.1415926);
    obj.FillHistogram("upstream",Form("energy_angle_%s", cutname.c_str()),
                      2000, 0, 2000, s_hit.GetEnergy(),
                      180, 0, 180, theta_deg);
    obj.FillHistogram("upstream",Form("DCenergy_angle_%s",cutname.c_str()),
                      2000, 0, 2000, dc_energy,
                      180, 0, 180, theta_deg);

    obj.FillHistogram("upstream",Form("DCenergy_angle_ring%02d_%s", j_hit.GetRing(), cutname.c_str()),
                      2000, 0, 2000, dc_energy,
                      180, 0, 180, theta_deg);

    obj.FillHistogram("upstream",Form("janus_pos_%s", cutname.c_str()),
                      100, -4, 4, j_hit.GetPosition().X(),
                      100, -4, 4, j_hit.GetPosition().Y());

    obj.FillHistogram("upstream",Form("janus_ringnum_%s", cutname.c_str()),
                      40, -5, 35, j_hit.GetRing());

    obj.FillHistogram("upstream",Form("janus_sectornum_%s", cutname.c_str()),
                      40, -5, 35, j_hit.GetSector());

    // if(dc_energy > 430 && dc_energy < 480) {
    //   Make78Kr_455keVPlots(obj, s_hit, j_hit, dc_energy, theta_deg, "1d", "upstream");
    // }

    for(auto& gate : gates) {
      if(gate.which_janus == WhichJanus::Upstream &&
         gate.ring == j_hit.GetRing() &&
         gate.DCgamma_angle_gate->IsInside(dc_energy, theta_deg)) {
        Make78Kr_Gamma_Plots(obj, s_hit, j_hit, dc_energy, theta_deg,
                             gate.which_janus, gate.gamma_seen);
      }
    }

    // // Scan along beta
    // for(int beta_i = 0; beta_i<150; beta_i++) {
    //   double beta = 0.0 + beta_i*((0.15-0.00)/150);

    //   obj.FillHistogram("upstream",Form("betascan_%s",cutname.c_str()),
    //                     150, 0.0, 0.15, beta,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta, particle_position));
    // }

    double betamax = GValue::Value("betamax");
    double dc_energy_angledep = s_hit.GetDoppler(get_beta(betamax, kr_theta,true), particle_position);
    obj.FillHistogram("upstream", Form("DCenergy_angle_angledep_%s", cutname.c_str()),
                      4000, 0, 4000, dc_energy_angledep);

    obj.FillHistogram("upstream", Form("DCenergy_angle_angledep_ring%02d_%s", j_hit.GetRing(), cutname.c_str()),
                      4000, 0, 4000, dc_energy_angledep);

    // // Scan along z
    // for(int z_i=-20; z_i<40; z_i++) {
    //   double z = z_i * 0.25;
    //   TVector3 offset(0,0,z);
    //   obj.FillHistogram("upstream","DCenergy_zscan",
    //                     60, -5, 10, z,
    //                     8000, 0, 4000, s_hit.GetDoppler(beta_nominal, particle_position, offset));
    // }
  }
}

void MakeTimeDependentHistograms(TRuntimeObjects& obj, TSega& sega, TJanus& janus) {
  for(unsigned int i=0; i<sega.Size(); i++){
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
    for(unsigned int i=0; i<sega.Size(); i++){
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
  for(unsigned int i=0; i<sega.Size(); i++){
    TSegaHit& hit = sega.GetSegaHit(i);
    obj.FillHistogram("coinc","sega_energy_janus_tdiff",
                      4000, 0, 4000, hit.GetEnergy(),
                      1000, -5000, 5000, hit.Timestamp() - janus.Timestamp());
  }


  for(unsigned int i=0; i<janus.Size(); i++){
    TJanusHit& j_hit = janus.GetJanusHit(i);

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

    bool in_upstream_pid_low  = j_hit.GetDetnum() == 0 && !(j_hit.Charge() > 3600 || j_hit.GetADCOverflowBit());
    bool in_upstream_pid_high = j_hit.GetDetnum() == 0 &&  (j_hit.Charge() > 3600 || j_hit.GetADCOverflowBit());

    // bool is_chan64_low  = (j_hit.GetFrontChannel() == 64 &&
    //                        j_hit.Charge() > 175 && j_hit.Charge() < 400);
    // bool is_chan64_mid  = (j_hit.GetFrontChannel() == 64 &&
    //                        j_hit.Charge() > 700 && j_hit.Charge() < 900);
    // bool is_chan64_high = (j_hit.GetFrontChannel() == 64 &&
    //                        j_hit.Charge() > 1120 && j_hit.Charge() < 1340);

    // bool is_chan65_kr = (j_hit.GetFrontChannel() == 65 &&
    //                      j_hit.Charge() > 1800 && j_hit.Charge() < 2500);
    // bool is_chan65_pb = (j_hit.GetFrontChannel() == 65 &&
    //                      j_hit.Charge() > 213 && j_hit.Charge() < 560);


    // bool is_chan70_low  = (j_hit.GetFrontChannel() == 70 &&
    //                        j_hit.Charge() > 300 && j_hit.Charge() < 600);
    // bool is_chan70_mid  = (j_hit.GetFrontChannel() == 70 &&
    //                        j_hit.Charge() > 700 && j_hit.Charge() < 870);
    // bool is_chan70_high = (j_hit.GetFrontChannel() == 70 &&
    //                        j_hit.Charge() > 1200 && j_hit.Charge() < 1340);


    for(unsigned int i=0; i<sega.Size(); i++){
      TSegaHit& s_hit = sega.GetSegaHit(i);
      if(in_pid_low){
        Make208PbPlots(obj, s_hit, j_hit);
        Make78KrPlots_Reconstructed(obj, s_hit, j_hit);
      }
      if(in_pid_high){
        Make78KrPlots(obj, s_hit, j_hit);
      }
      if(in_upstream_pid_low) {
        MakeUpstream78KrPlots(obj, s_hit, j_hit, "low");
      }
      if(in_upstream_pid_high) {
        MakeUpstream78KrPlots(obj, s_hit, j_hit, "high");
      }

      // if(is_chan64_low) {
      //   Make78KrPlots(obj, s_hit, j_hit, "chan64_low");
      // } else if(is_chan64_mid) {
      //   Make78KrPlots(obj, s_hit, j_hit, "chan64_mid");
      // } else if(is_chan64_high) {
      //   Make78KrPlots(obj, s_hit, j_hit, "chan64_high");
      // } else if(is_chan70_low) {
      //   Make78KrPlots(obj, s_hit, j_hit, "chan70_low");
      // } else if(is_chan70_mid) {
      //   Make78KrPlots(obj, s_hit, j_hit, "chan70_mid");
      // } else if(is_chan70_high) {
      //   Make78KrPlots(obj, s_hit, j_hit, "chan70_high");
      // } else if(is_chan65_kr) {
      //   Make78KrPlots(obj, s_hit, j_hit, "chan65_kr");
      // } else if(is_chan65_pb) {
      //   Make78KrPlots(obj, s_hit, j_hit, "chan65_pb");
      // }
    }
  }
}

void MakeTimestampDiffs(TRuntimeObjects& obj, TSega* sega, TJanus* janus) {

  if(sega && janus) {
    obj.FillHistogram("tdiff","sega_janus_tdiff",
                      1000, -5000, 5000, sega->Timestamp() - janus->Timestamp());

    for(unsigned int i=0; i<sega->Size(); i++){
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
    for(unsigned int i=0; i<sega->Size(); i++){
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

void MakeJanusChan64_70Histograms(TRuntimeObjects& obj, TJanus& janus, const char* dirname) {
  bool has_chan64_mid = false;
  bool has_chan70_mid = false;
  bool has_chan64_high = false;
  bool has_chan70_high = false;
  for(auto& chan : janus.GetAllChannels()) {
    if(chan.GetFrontChannel() == 64 &&
       chan.Charge() > 700 &&
       chan.Charge() < 900) {
      has_chan64_mid = true;
    }

    if(chan.GetFrontChannel() == 70 &&
       chan.Charge() > 700 &&
       chan.Charge() < 870) {
      has_chan70_mid = true;
    }

    if(chan.GetFrontChannel() == 64 &&
       chan.Charge() > 1120 &&
       chan.Charge() < 1340) {
      has_chan64_high = true;
    }

    if(chan.GetFrontChannel() == 70 &&
       chan.Charge() > 1200 &&
       chan.Charge() < 1340) {
      has_chan70_high = true;
    }
  }

  if(has_chan64_mid) {
    for(auto& chan : janus.GetAllChannels()) {
      obj.FillHistogram(dirname,"mid64_channel_charge",
                        128, 0, 128, chan.GetFrontChannel(),
                        6000, -4, 6000, chan.Charge());
    }
  }

  if(has_chan70_mid) {
    for(auto& chan : janus.GetAllChannels()) {
      obj.FillHistogram(dirname,"mid70_channel_charge",
                        128, 0, 128, chan.GetFrontChannel(),
                        6000, -4, 6000, chan.Charge());
    }
  }

  if(has_chan64_high) {
    for(auto& chan : janus.GetAllChannels()) {
      obj.FillHistogram(dirname,"high64_channel_charge",
                        128, 0, 128, chan.GetFrontChannel(),
                        6000, -4, 6000, chan.Charge());
    }
  }

  if(has_chan70_high) {
    for(auto& chan : janus.GetAllChannels()) {
      obj.FillHistogram(dirname,"high70_channel_charge",
                        128, 0, 128, chan.GetFrontChannel(),
                        6000, -4, 6000, chan.Charge());
    }
  }
}

void Make78Kr_Gamma_Plots(TRuntimeObjects& obj, TSegaHit& s_hit, TJanusHit& j_hit,
                          double dc_energy, double gamma_angle,
                          WhichJanus which_janus, GammaSeen gamma_seen) {
  std::string det = to_string(which_janus);
  std::string en = to_string(gamma_seen);

  std::string dirname = Form("cut_%skeV",en.c_str());

  obj.FillHistogram(dirname.c_str(), "DCenergy_angle",
                    2000, 0, 2000, dc_energy,
                    180, 0, 180, gamma_angle);
  obj.FillHistogram(dirname.c_str(), Form("DCenergy_angle_ring%02d", j_hit.GetRing()),
                    2000, 0, 2000, dc_energy,
                    180, 0, 180, gamma_angle);

  obj.FillHistogram(dirname.c_str(), Form("ringsector_%s",det.c_str()),
                    24,1,25,j_hit.GetRing(),
                    32,1,33,j_hit.GetSector());
}

// void Make78Kr_455keVPlots(TRuntimeObjects& obj, TSegaHit& s_hit, TJanusHit& j_hit,
//                           double dc_energy, double gamma_angle_deg, std::string which_cut,
//                           std::string which_region) {
//   obj.FillHistogram("cuts_compare", Form("DCenergy_angle_455_cut%s",which_cut.c_str()),
//                     2000, 0, 2000, dc_energy,
//                     180, 0, 180, gamma_angle_deg);
//   obj.FillHistogram("cuts_compare", Form("DCenergy_angle_455_%s_ring%02d_cut%s",
//                                          which_region.c_str(), j_hit.GetRing(), which_cut.c_str()),
//                     2000, 0, 2000, dc_energy,
//                     180, 0, 180, gamma_angle_deg);

//   obj.FillHistogram("cuts_compare", Form("DCenergy_455_cut%s",which_cut.c_str()),
//                     2000, 0, 2000, dc_energy);

//   obj.FillHistogram("cuts_compare", Form("channel_energy_455_cut%s",which_cut.c_str()),
//                     128, 0, 128, j_hit.GetFrontChannel(),
//                     4000, 0, 400e3, j_hit.GetEnergy());
//   obj.FillHistogram("cuts_compare", Form("channel_energy_455_cut%s",which_cut.c_str()),
//                     128, 0, 128, j_hit.GetBackChannel(),
//                     4000, 0, 400e3, j_hit.GetBackHit().GetEnergy());

//   obj.FillHistogram("cuts_compare", Form("ringsector_455_cut%s", which_cut.c_str()),
//                     24, 1, 25, j_hit.GetRing(),
//                     32, 1, 33, j_hit.GetSector());
// }
