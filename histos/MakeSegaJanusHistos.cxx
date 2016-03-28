#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
#include <string>
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
#include "TFile.h"

#include "GValue.h"
#include "TObject.h"
#include "TSega.h"
#include "TJanus.h"
#include "TKinematics.h"
#include "TPreserveGDirectory.h"

TCutG* pid_low = NULL;
TCutG* pid_middle = NULL;
TCutG* pid_high = NULL;
TCutG* time_energy = NULL;

void LoadGates(){
  if(pid_low == NULL){
    TPreserveGDirectory preserve;
    std::string cuts_file = std::string(std::getenv("GRUTSYS")) + "/config/cuts_new.root";
    TFile* f = new TFile(cuts_file.c_str());
    pid_low = (TCutG*)f->Get("pid_low");
    //pid_middle = (TCutG*)f->Get("pid_middle");
    pid_high = (TCutG*)f->Get("pid_high");
    time_energy = (TCutG*)f->Get("time_energy");
  }
}

void MakeJanusHistograms(TRuntimeObjects& obj, TJanus* janus);
void MakeSegaHistograms(TRuntimeObjects& obj, TSega* sega);
void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega* sega, TJanus* janus);
void MakeTimestampDiffs(TRuntimeObjects& obj, TSega* sega, TJanus* janus);
void MakeLaBrCoincPlots(TRuntimeObjects& obj, TSega* sega, TJanus* janus);

long event_num = -1;

extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  event_num++;

  LoadGates();

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
    // MakeLaBrCoincPlots(obj, sega, janus);
  }

  MakeTimestampDiffs(obj, sega, janus);
}

void MakeJanusHistograms(TRuntimeObjects& obj, TJanus* janus) {
  for(auto& chan : janus->GetAllChannels()){
    obj.FillHistogram("channel",
                      128, 0, 128, chan.GetFrontChannel());
    obj.FillHistogram("channel_charge",
                      128, 0, 128, chan.GetFrontChannel(),
                      6000, -4, 6000, chan.Charge());
    obj.FillHistogram("channel_energy",
                      128, 0, 128, chan.GetFrontChannel(),
                      4000, 0, 400e3, chan.GetEnergy());
    obj.FillHistogram("channel_time",
                      128, 0, 128, chan.GetFrontChannel(),
                      6000, 0, 6000, chan.Time());
  }


  obj.FillHistogram("janus_size",
                    256, 0, 256, janus->Size());


  bool any_is_inside_blob = false;
  bool any_is_inside_blob2 = false;
  for(int i=0; i<janus->Size(); i++){
    TJanusHit& hit = janus->GetJanusHit(i);
    static TCutG* blob = NULL;
    if(!blob){
      blob = new TCutG("blob",6);
      blob->SetVarX("janus_hits.GetFrontChannel()");
      blob->SetVarY("janus_hits.Charge()");
      blob->SetTitle("Graph");
      blob->SetFillColor(1);
      blob->SetPoint(0,117.729,2405.3);
      blob->SetPoint(1,117.729,2608.23);
      blob->SetPoint(2,122.646,2614.99);
      blob->SetPoint(3,121.752,2412.07);
      blob->SetPoint(4,118.921,2337.66);
      blob->SetPoint(5,117.729,2405.3);
    }

    static TCutG* blob2 = NULL;
    if(!blob2){
      blob2 = new TCutG("blob2",5);
      blob2->SetVarX("channel_charge_gated");
      blob2->SetVarY("");
      blob2->SetTitle("Graph");
      blob2->SetFillColor(1);
      blob2->SetPoint(0,70.015,4383.94);
      blob2->SetPoint(1,90.4662,4383.94);
      blob2->SetPoint(2,91.4687,2239.66);
      blob2->SetPoint(3,71.619,2305.03);
      blob2->SetPoint(4,70.015,4383.94);
    }


    if(blob->IsInside(hit.GetFrontChannel(), hit.Charge())){
      any_is_inside_blob = true;
    }
    if(blob2->IsInside(hit.GetFrontChannel(), hit.Charge())){
      any_is_inside_blob2 = true;
    }
  }

  for(auto& hit : janus->GetAllHits()){
    obj.FillHistogram("hit_channel",
                      128, 0, 128, hit.GetFrontChannel());
    obj.FillHistogram("hit_channel_charge",
                      128, 0, 128, hit.GetFrontChannel(),
                      6000, -4, 6000, hit.Charge());
    obj.FillHistogram("hit_channel_energy",
                      128, 0, 128, hit.GetFrontChannel(),
                      4000, 0, 400e3, hit.GetEnergy());
    obj.FillHistogram("hit_channel_time",
                      128, 0, 128, hit.GetFrontChannel(),
                      6000, 0, 6000, hit.Time());
  }

  for(int i=0; i<janus->Size(); i++){
    TJanusHit& hit = janus->GetJanusHit(i);
    int hit_detnum = hit.GetDetnum();


    for(int j=0; j<janus->Size(); j++){
      TJanusHit& hit2 = janus->GetJanusHit(j);
      if(hit.GetFrontChannel()==118 &&
	 hit2.GetFrontChannel()==119){
	obj.FillHistogram("chan118_chan119",
			  6000,-4,6000, hit.Charge(),
			  6000,-4,6000, hit2.Charge());

      }
    }


    if(any_is_inside_blob){
      obj.FillHistogram("channel_charge_inblob",
			128, 0, 128, hit.GetFrontChannel(),
			6000,-4,6000, hit.Charge());
    }
    if(any_is_inside_blob && !any_is_inside_blob2){
      obj.FillHistogram("channel_charge_inblob_notinblob2",
			128, 0, 128, hit.GetFrontChannel(),
			6000,-4,6000, hit.Charge());
    }


    obj.FillHistogram(Form("janus%d_xy", hit_detnum),
                      100,-3,3,hit.GetPosition().X(),
                      100,-3,3,hit.GetPosition().Y());

    double theta_deg = hit.GetPosition().Theta() * TMath::RadToDeg();
    obj.FillHistogram("janus_theta",
                      180, 0, 180, theta_deg);
    //if(pid_low->IsInside(theta_deg, hit.GetEnergy())) {
    if(pid_low->IsInside(hit.GetFrontChannel(), hit.Charge())) {
      static TKinematics kin("78Kr","208Pb","78Kr","208Pb",3.9*78);
      kin.SetAngles(theta_deg * TMath::DegToRad(), 3); // Set the 208Pb angle
      double theta_78Kr = kin.GetThetalab(2) * TMath::RadToDeg(); // Get the 78Kr angle
      obj.FillHistogram("janus_theta_recon",
                        180, 0, 180, theta_78Kr);
    }

    // ---- Added by JAB 1/28/16 ----
    if(hit_detnum==1){
      obj.FillHistogram(Form("janus_Sector%02i_v_ring_det%02i",hit.GetSector(),hit_detnum),
			40,0,40,hit.GetRing(),
			6000,0,6000,hit.Charge());
    }

    // ---- Added by JAB 1/20/16 ----

    obj.FillHistogram(Form("janus_theta_v_energy_det%02i",hit_detnum),
		      180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
		      4000,0,400e3,hit.GetEnergy());

    obj.FillHistogram(Form("janus_theta_v_charge_det%02i",hit_detnum),
		      180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
		      6000,0,6000,hit.Charge());

    obj.FillHistogram(Form("janus_ring_v_energy_det%02i",hit_detnum),
		      30,0,30,hit.GetRing(),
		      4000,0,400e3,hit.GetEnergy());

    obj.FillHistogram(Form("janus_phi_v_energy_det%02i",hit_detnum),
		      360,-180,180,hit.GetPosition().Phi()*TMath::RadToDeg(),
		      4000,0,400e3,hit.GetEnergy());

    obj.FillHistogram(Form("janus_secotr_v_energy_det%02i",hit_detnum),
		      64,0,64,hit.GetSector(),
		      4000,0,400e3,hit.GetEnergy());

    // ---- ^^ End JAB ^^ ----
  }
}

// Returns the timestamp in nanoseconds since the start of the first production run.
// Minimum value: 0
// Maximum value: 2.141e14 (End of Pb-208 target runs)
// Total seconds: ~214100 seconds
double global_timestamp(unsigned int run_start, unsigned long timestamp) {
  unsigned int experiment_start = 1453953429; // Wed Jan 27 22:57:09 2016
  return (run_start - experiment_start)*1e9 + timestamp;
}

void MakeSegaHistograms(TRuntimeObjects& obj, TSega* sega) {
  obj.FillHistogram("sega_size",
                    256,0,256,sega->Size());

  long cc_timestamp = -1;
  long segment_timestamp = -1;
  for(int i=0; i<sega->Size(); i++){
    TSegaHit& hit = sega->GetSegaHit(i);
    int hit_detnum = hit.GetDetnum();
    double energy = hit.GetEnergy();
    obj.FillHistogram("sega_energy",
                      8000, 0, 4000, energy);
    obj.FillHistogram("sega_charge_summary",
                      18, 1, 19, hit_detnum,
                      32768, 0, 32768, hit.Charge());
    obj.FillHistogram("sega_energy_summary",
                      18, 1, 19, hit_detnum,
                      8000, 0, 4000, energy);
    obj.FillHistogram("sega_numsegments",
                      16, 1, 17, hit_detnum,
                      33, 0, 33, hit.GetNumSegments());

    // obj.FillHistogram(Form("sega_det%d_numsegments_ts", hit_detnum),
    //                   36000, 0, 3600e9, hit.Timestamp(),
    //                   33, 0, 33, hit.GetNumSegments());

    // obj.FillHistogram(Form("sega_energy_det%02d_time", hit_detnum),
    //                   214100/60, 0, 2.141e14, hit.Timestamp(),
    //                   1000, 0, 2000, energy);

    // if(hit_detnum==1 || hit_detnum==12 ||
    //    hit_detnum==7 || hit_detnum==8){
    //   obj.FillHistogram(Form("sega_charge_det%02d_time", hit_detnum),
    //                     214100/60, 0, 2.141e14, hit.Timestamp(),
    //                     22000, 0, 22000, hit.Charge());
    // }


    // for(unsigned int segi=0; segi<hit.GetNumSegments(); segi++){
    //   TSegaSegmentHit& seg = hit.GetSegment(segi);
    //   int segnum = seg.GetSegnum();
    //   obj.FillHistogram(Form("sega_det%02d_segsummary", hit_detnum),
    //                     32, 1, 33, segnum,
    //                     32768, 0, 32768, seg.Charge());
    //   obj.FillHistogram(Form("sega_energy_det%02d_segsummary", hit_detnum),
    //                     32, 1, 33, segnum,
    //                     32768, 0, 32768, seg.GetEnergy());
    //   obj.FillHistogram("sega_allseg_summary",
    //                     32*16, 1, 32*16 + 1, 32*(hit_detnum-1) + segnum,
    //                     32768, 0, 32768, seg.Charge());
    // }

    if(hit.GetCrate()==1){
      cc_timestamp = hit.Timestamp();
    } else if (hit.GetCrate()==3) {
      segment_timestamp = hit.Timestamp();
    }
  }

  // for(int i=0; i<sega->Size(); i++) {
  //   TSegaHit& s_hit = sega->GetSegaHit(i);
  //   for(int beta_i = 0; beta_i<150; beta_i++) {
  //     double beta = 0.0 + beta_i*((0.15-0.00)/150);
  //     obj.FillHistogram("sega_DCenergy_beamaxis_beta",
  //       		150, 0.0, 0.15, beta,
  //       		8000, 0, 4000, s_hit.GetDoppler(beta));
  //   }
  // }

  obj.FillHistogram("hascore_hassegment",
                    2, 0, 2, cc_timestamp!=-1,
                    2, 0, 2, segment_timestamp!=-1);
  if(cc_timestamp>0 && segment_timestamp>0){
    obj.FillHistogram("segment_core_tdiff",
                      1000, -5000, 5000, cc_timestamp - segment_timestamp);
  }
}

void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega* sega, TJanus* janus) {
  bool has_455keV = false;
  // bool coinc_missing3 = false;
  // bool coinc_missing4 = false;

  // for(int i=0; i<sega->Size(); i++) {
  //   TSegaHit& s_hit = sega->GetSegaHit(i);
  //   for(int beta_i = 0; beta_i<150; beta_i++) {
  //     double beta = 0.0 + beta_i*((0.15-0.00)/150);
  //     obj.FillHistogram("sega_DCenergy_beamaxis_beta_coinc",
  //       		150, 0.0, 0.15, beta,
  //       		8000, 0, 4000, s_hit.GetDoppler(beta));
  //   }


  //   double beta = GValue::Value("beta");
  //   for(int z=-20;z<20;z++) {
  //     TVector3 offset(0,0,z);

  //     if(s_hit.GetNumSegments()>0) {
  //       double tmp = 0.0;
  //       double gamma = 1/(sqrt(1-pow(beta,2)));
  //       tmp = s_hit.GetEnergy()*gamma *(1 - beta*TMath::Cos((s_hit.GetPosition()+offset).Theta()));
  //       obj.FillHistogram("sega_DCenergy_beamaxis_zoffset_coinc",
  //                         40, -20, 20, z,
  //                         8000, 0, 4000, tmp);
  //     }
  //   }
  // }



  for(int i=0; i<sega->Size(); i++){
    TSegaHit& hit = sega->GetSegaHit(i);
    int hit_detnum = hit.GetDetnum();
    if(hit.GetEnergy()>425 && hit.GetEnergy()<485){
      has_455keV = true;
      obj.FillHistogram("sega_has_455keV",
                        16, 1, 17, hit.GetDetnum());
    }

    obj.FillHistogram("sega_detnum_janus_tdiff",
                      16, 1, 17, hit_detnum,
                      1000, -5000, 5000, hit.Timestamp() - janus->Timestamp());
    obj.FillHistogram("sega_energy_janus_tdiff",
                      4000, 0, 4000, hit.GetEnergy(),
                      1000, -5000, 5000, hit.Timestamp() - janus->Timestamp());
    // obj.FillHistogram("sega_traceheight_summary",
    //                   16, 1, 17, hit.GetDetnum(),
    //                   3100, -100, 3000, hit.GetTraceHeight());
    // obj.FillHistogram(Form("sega_traceheight_det%02d_timestamp",hit.GetDetnum()),
    //                   214100/60, 0, 2.141e14, hit.Timestamp(),
    //                   3100, -100, 3000, hit.GetTraceHeight());
  }

  obj.FillHistogram("sega_janus_tdiff",
                    1000, -5000, 5000, sega->Timestamp() - janus->Timestamp());


  for(int i=0; i<janus->Size(); i++){
    TJanusHit& hit = janus->GetJanusHit(i);
    if(has_455keV){
      obj.FillHistogram("channel_energy_455keV_coinc",
                        128, 0, 128, hit.GetFrontChannel(),
                        6000, 0, 6000, hit.Charge());
      obj.FillHistogram("channel_energy_455keV_coinc",
                        128, 0, 128, hit.GetBackChannel(),
                        6000, 0, 6000, hit.GetBackHit().Charge());
    }




    for(int j=0; j<sega->Size(); j++){
      TSegaHit& s_hit = sega->GetSegaHit(j);
      double energy = s_hit.GetEnergy();
      if(hit.GetFrontChannel()==87){
        obj.FillHistogram("senergy_jcharge_innermost_ring",
                          6000, 0, 6000, hit.Charge(),
                          8000, 0, 4000, energy);
      }

      // ---- Added by JAB 1/20/16 ----
      TVector3 SegaPos = s_hit.GetPosition();
      TVector3 JanusPos = hit.GetPosition();
      double CT_sj = SegaPos.Dot(JanusPos);
      double angle = hit.GetPosition().Angle(s_hit.GetPosition());
      double dc_energy = s_hit.GetDoppler(GValue::Value("beta"),hit.GetPosition());

      obj.FillHistogram(Form("senergy_sjtheta_ring%02i",hit.GetRing()),
			280,-70,70, CT_sj,
			8000,0,4000, energy);


      obj.FillHistogram(Form("doppler_with_ring%02i_gate",hit.GetRing()),
			//280,-70,70, s_hit.GetPosition().Angle(hit.GetPosition())*TMath::RadToDeg(),
			180,0,180, angle*TMath::RadToDeg(),
			8000,0,4000, dc_energy); //Energy());

      obj.FillHistogram("sega_DCenergy",
                        8000,0,4000,dc_energy);


      obj.FillHistogram("sega_DCenergy_theta",
			180,0,180, angle*TMath::RadToDeg(),
			8000,0,4000,dc_energy); //Energy());



      // ---- ^^ End JAB ^^ ----
    }



    obj.FillHistogram("channel_energy_any_coinc",
                      128, 0, 128, hit.GetFrontChannel(),
                      6000, 0, 6000, hit.Charge());
    obj.FillHistogram("channel_energy_any_coinc",
                      128, 0, 128, hit.GetBackChannel(),
                      6000, 0, 6000, hit.GetBackHit().Charge());


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


  bool in_pid_low = false;
  TJanusHit* j_hit_pidlow = NULL;
  //bool in_pid_middle = false;
  //TJanusHit* j_hit_pidmiddle = NULL;
  bool in_pid_high = false;
  TJanusHit* j_hit_pidhigh = NULL;
  for(auto& j_hit : janus->GetAllHits()){
    double theta_deg = j_hit.GetPosition().Theta() * TMath::RadToDeg();
    double energy = j_hit.GetEnergy();
    //if( pid_low->IsInside(theta_deg, energy) ) {
    if(pid_low->IsInside(j_hit.GetFrontChannel(), j_hit.Charge())) {
      in_pid_low = true;
      j_hit_pidlow = &j_hit;
    }
    // if( pid_middle->IsInside(theta_deg, energy) ) {
    //   in_pid_middle = true;
    //   j_hit_pidmiddle = &j_hit;
    // }
    //if( pid_high->IsInside(theta_deg, energy) ) {
    if(pid_high->IsInside(j_hit.GetFrontChannel(), j_hit.Charge())) {
      in_pid_high = true;
      j_hit_pidhigh = &j_hit;
    }
  }

  for(int i=0; i<sega->Size(); i++){
    TSegaHit& s_hit = sega->GetSegaHit(i);
    if(in_pid_low){
      obj.FillHistogram("sega_energy_pidlow",
			8000, 0, 4000, s_hit.GetEnergy());
      obj.FillHistogram("sega_energy_summary_pidlow",
			18,1,19, s_hit.GetDetnum(),
			8000, 0, 4000, s_hit.GetEnergy());
      obj.FillHistogram("sega_DCenergy_beamaxis_pidlow",
			8000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta")));
      obj.FillHistogram("sega_DCenergy_janus_pidlow",
			8000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), j_hit_pidlow->GetPosition()));

      obj.FillHistogram("sega_DCenergy_janus_pidlow_conjugate",
                        8000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), j_hit_pidlow->GetConjugateDirection()));
      if(time_energy->IsInside(s_hit.GetEnergy(),
                             s_hit.Timestamp() - j_hit_pidlow->Timestamp())){

        TVector3 kr_direction = j_hit_pidlow->GetConjugateDirection();
        double dc_energy = s_hit.GetDoppler(GValue::Value("beta_reconstructed"), kr_direction);
        obj.FillHistogram("sega_DCenergy_janus_pidlow_conjugate_timegate",
                          8000, 0, 4000, dc_energy);
        obj.FillHistogram("sega_energy_janus_pidlow_conjugate_timegate",
                          8000, 0, 4000, s_hit.GetEnergy());

        double dop_theta = s_hit.GetPosition().Angle(kr_direction);
        obj.FillHistogram("sega_energy_angle_janus_pidlow_conjugate_timegate",
                          2000, 0, 2000, s_hit.GetEnergy(),
                          180, 0, 180, dop_theta*(180/3.1415926));

        double kr_theta_degree = (180/3.1415926)*kr_direction.Theta();
        if(75 < kr_theta_degree && kr_theta_degree < 105){
          obj.FillHistogram("sega_energy_angle_janus_pidlow_conjugate_timegate_near90",
                            2000, 0, 2000, s_hit.GetEnergy(),
                            180, 0, 180, dop_theta*(180/3.1415926));

          obj.FillHistogram("sega_DCenergy_janus_pidlow_conjugate_timegate_near90",
                            8000, 0, 4000, dc_energy);

          obj.FillHistogram("sega_energy_janus_pidlow_conjugate_timegate_near90",
                            8000, 0, 4000, s_hit.GetEnergy());
        } else {
          obj.FillHistogram("sega_energy_angle_janus_pidlow_conjugate_timegate_notnear90",
                            2000, 0, 2000, s_hit.GetEnergy(),
                            180, 0, 180, dop_theta*(180/3.1415926));

          obj.FillHistogram("sega_DCenergy_janus_pidlow_conjugate_timegate_notnear90",
                            8000, 0, 4000, dc_energy);

          obj.FillHistogram("sega_energy_janus_pidlow_conjugate_timegate_notnear90",
                            8000, 0, 4000, s_hit.GetEnergy());


          for(int beta_i = 0; beta_i<150; beta_i++) {
            double beta = 0.0 + beta_i*((0.15-0.00)/150);
            obj.FillHistogram("sega_DCenergy_janus_pidlow_conjugate_timegate_notnear90_beta",
                              150, 0.0, 0.15, beta,
                              8000, 0, 4000, s_hit.GetDoppler(beta,kr_direction));
          }
        }


        obj.FillHistogram(Form("sega_energy_angle_janus_pidlow_conjugate_timegate_ring%02d",j_hit_pidlow->GetRing()),
                          2000, 0, 2000, s_hit.GetEnergy(),
                          180, 0, 180, dop_theta*(180/3.1415926));
      }
    }
    if(in_pid_high){
      obj.FillHistogram("sega_energy_pidhigh",
			8000, 0, 4000, s_hit.GetEnergy());
      obj.FillHistogram("sega_energy_summary_pidhigh",
			18,1,19, s_hit.GetDetnum(),
			8000, 0, 4000, s_hit.GetEnergy());
      obj.FillHistogram("sega_DCenergy_beamaxis_pidhigh",
			8000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta")));
      obj.FillHistogram("sega_DCenergy_janus_pidhigh",
			8000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), j_hit_pidhigh->GetPosition()));

      if(time_energy->IsInside(s_hit.GetEnergy(),
                               s_hit.Timestamp() - j_hit_pidhigh->Timestamp())){
        double dc_energy = s_hit.GetDoppler(GValue::Value("beta"), j_hit_pidhigh->GetPosition());
        obj.FillHistogram("sega_energy_janus_pidhigh_timegate",
                          8000, 0, 4000, s_hit.GetEnergy());
        obj.FillHistogram("sega_DCenergy_janus_pidhigh_timegate",
                          8000, 0, 4000, dc_energy);

        double theta = s_hit.GetPosition().Angle(j_hit_pidhigh->GetPosition());
        obj.FillHistogram("sega_energy_angle_janus_pidhigh_timegate",
                          2000, 0, 2000, s_hit.GetEnergy(),
                          180, 0, 180, theta*(180/3.1415926));


        // double timestamp = s_hit.Timestamp();
        // obj.FillHistogram(Form("sega_DCenergy_janus_pidhigh_timegate_det%02d_time",s_hit.GetDetnum()),
        //                   214100/60, 0, 2.141e14, timestamp,
        //                   1000, 0, 2000, dc_energy);

        // double dc_traceheight = s_hit.GetTraceHeightDoppler(GValue::Value("beta"), j_hit_pidhigh->GetPosition());
        // obj.FillHistogram(Form("sega_traceheight_janus_pidhigh_timegate_det%02d_timestamp",s_hit.GetDetnum()),
        //                   214100/60, 0, 2.141e14, timestamp,
        //                   3100, -100, 3000, dc_traceheight);
      }
    }
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
