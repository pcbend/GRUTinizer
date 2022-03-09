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
void MakeCoincidenceHistogramsAlphaSource(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus);
void MakeCoincidenceHistogramsFissionSource(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus);
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

class BinCenters {
public:
  BinCenters(int bins, double low, double high)
    : bins(bins), low(low), high(high) { }

  class iterator {
  public:
    iterator(BinCenters& axis, int binnum)
      : axis(axis), binnum(binnum) { }

    double operator*() const;

    bool operator==(const iterator& other) const {
      return
        (&axis == &other.axis) &&
        (binnum == other.binnum);
    }

    bool operator!=(const iterator& other) const {
      return !(*this == other);
    }

    iterator& operator++() {
      binnum++;
      return *this;
    }

  private:
    BinCenters& axis;
    int binnum;
  };
  friend class BinCenters::iterator;

  iterator begin() {
    return iterator(*this, 0);
  }

  iterator end() {
    return iterator(*this, bins);
  }

private:
  int bins;
  double low;
  double high;
};

double BinCenters::iterator::operator*() const {
  return axis.low + (binnum+0.5) * (axis.high-axis.low)/axis.bins;
}

void MakeJanusHistograms(TRuntimeObjects& obj, TJanusDDAS& janus) {

  //std::cout << "Inside MakeJanusHistograms" << std::endl;

  obj.FillHistogram("janus", "num_channels",
                    150, -10, 140, janus.GetAllChannels().size());
  obj.FillHistogram("janus", "num_hits",
                    150, -10, 140, janus.GetAllHits().size());


  for(auto& chan : janus.GetAllChannels()) {
    obj.FillHistogram("janus", "charge_summary",
                      128, 0, 128, chan.GetFrontChannel(),
                      32768, 0, 32768, chan.Charge());
    obj.FillHistogram("janus", "energy_summary",
                      128, 0, 128, chan.GetFrontChannel(),
                      32768, 0, 32768, chan.GetEnergy());


    obj.FillHistogram("janus", "channel_detnum",
                      15,-5,10, chan.GetDetnum());
  }


  bool has_fission = false;
  for(auto& hit : janus.GetAllHits()) {
    if(hit.GetEnergy() > 6500) {
      has_fission = true;
    }
  } 
  if(has_fission) {
    obj.FillHistogram("janus", "fission_num_hits",
                      150, -10, 140, janus.GetAllHits().size());
    if(janus.GetAllHits().size() == 2) {
      auto& hit1 = janus.GetJanusHit(0);
      auto& hit2 = janus.GetJanusHit(1);

      auto& low_hit = (hit1.GetEnergy() < hit2.GetEnergy()) ? hit1 : hit2;
      auto& high_hit = (hit1.GetEnergy() > hit2.GetEnergy()) ? hit1 : hit2;

      obj.FillHistogram("janus", "fission_energy_energy",
                        1000, 0, 100e3, high_hit.GetEnergy(),
                        1000, 0, 100e3, low_hit.GetEnergy());
    }
  }

  for(auto& hit : janus.GetAllHits()) {
    obj.FillHistogram("janus", "charge_summary_hit",
                      128, 0, 128, hit.GetFrontChannel(),
                      32768, 0, 32768, hit.Charge());
    obj.FillHistogram("janus", "charge_summary_hit",
                      128, 0, 128, hit.GetBackChannel(),
                      32768, 0, 32768, hit.GetBackHit().Charge());
    obj.FillHistogram("janus", "energy_summary_hit",
                      128, 0, 128, hit.GetFrontChannel(),
                      1200, -20, 100e3, hit.GetEnergy());
    obj.FillHistogram("janus", "energy_summary_hit",
                      128, 0, 128, hit.GetBackChannel(),
                      1200, -20, 100e3, hit.GetBackHit().GetEnergy());

    obj.FillHistogram("janus", Form("det%d_ringsummary_charge", hit.GetDetnum()),
                      35, -5, 30, hit.GetRing(),
                      32768, 0, 32768, hit.Charge());
    obj.FillHistogram("janus", Form("det%d_ringsummary_energy", hit.GetDetnum()),
                      35, -5, 30, hit.GetRing(),
                      2000, 0, 20e3, hit.GetEnergy());

    // obj.FillHistogram("janus", "charge_frontback",
    //                   32768, 0, 32768, hit.Charge(),
    //                   32768, 0, 32768, hit.GetBackHit().Charge());
    obj.FillHistogram("janus", "energy_frontback",
                      1200, -20, 100e3, hit.GetEnergy(),
                      1200, -20, 100e3, hit.GetBackHit().GetEnergy());

    obj.FillHistogram("janus", Form("det%d_ringnum_sectornum", hit.GetDetnum()),
                      30, 0, 30, hit.GetRing(),
                      40, 0, 40, hit.GetSector());

    obj.FillHistogram("janus", Form("det%d_ringnum", hit.GetDetnum()),
                      30, 0, 30, hit.GetRing());

    auto tdiff = hit.Timestamp() - hit.GetBackHit().Timestamp();
    obj.FillHistogram("janus", Form("det%d_frontback_tdiff", hit.GetDetnum()),
                      2000, -1000, 1000, tdiff);


    obj.FillHistogram("janus",Form("det%d_xy", hit.GetDetnum()),
                      200,-4,4,hit.GetPosition().X(),
                      200,-4,4,hit.GetPosition().Y());

    if(hit.GetRing() == 1) {
      obj.FillHistogram("janus", Form("det%d_charge_innermost_ring", hit.GetDetnum()),
                        32768, 0, 32768, hit.Charge());
    }
  }


  for(unsigned int i=0; i<janus.Size(); i++) {
    for(unsigned int j=i+1; j<janus.Size(); j++) {
      auto& hit1 = janus.GetJanusHit(i);
      auto& hit2 = janus.GetJanusHit(j);

      auto pos1 = hit1.GetPosition();
      auto pos2 = hit2.GetPosition();

      double angle = pos1.Angle(pos2);
      obj.FillHistogram("janus", "relative_angle",
                        200, 0, 200, (180/3.1415926)*angle);

      if(hit1.GetEnergy() > 6500 &&
         hit2.GetEnergy() > 6500) {
        obj.FillHistogram("janus", "fission_relative_angle",
                          200, 0, 200, (180/3.1415926)*angle);
      }
    }
  }
}


void MakeSegaHistograms(TRuntimeObjects& obj, TSega& sega) {

  //std::cout << "Inside MakeSegaHistograms" << std::endl;
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
                      8000, 0, 8000, energy);

    obj.FillHistogram("sega","detnum",
                      18, 1, 19, hit_detnum);

    obj.FillHistogram("sega","numsegments",
                      16, 1, 17, hit_detnum,
                      33, 0, 33, hit.GetNumSegments());

    // obj.FillHistogram("sega", Form("det%d_numsegments_ts", hit_detnum),
    //                   36000, 0, 3600e9, hit.Timestamp(),
    //                   33, 0, 33, hit.GetNumSegments());

    // obj.FillHistogram("sega", Form("energy_det%02d_ts", hit_detnum),
    //                   214100/60, 0, 2.141e14, hit.Timestamp(),
    //                   1000, 0, 2000, energy);

    //obj.FillHistogram("sega", Form("charge_det%02d_ts", hit_detnum),
    //                  214100/60, -1, 1, hit.Timestamp()/1e14,
    //                  22000, 0, 22000, hit.Charge());

    //std::cout << "ts is: " << hit.Timestamp() << std::endl;
    //std::cout << "det is: " << hit.GetDetnum() << std::endl;


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

    if(hit.GetCrate()==1 && hit.GetSlot()==2){
      cc_timestamp = hit.Timestamp();
    } else
      segment_timestamp = hit.Timestamp();

  
}

    //obj.FillHistogram("sega",Form("energy_segment_core_tdiff_Det%i",hit.GetDetnum()),
    //                  1000,-5000,5000, cc_timestamp - segment_timestamp,
    //                  1000,0,8000,hit.GetEnergy()); 

  if(cc_timestamp>0 && segment_timestamp>0){
    obj.FillHistogram("sega","segment_core_tdiff",
                      1000,-5000, 5000, cc_timestamp - segment_timestamp);

    
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
  //MakeCoincidenceHistogramsAlphaSource(obj, sega, janus);
  MakeCoincidenceHistogramsFissionSource(obj, sega, janus);
}

void MakeCoincidenceHistogramsAlphaSource(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus) {
  for(auto& j_hit : janus.GetAllHits()) {
    if(j_hit.GetDetnum()==1 && j_hit.GetRing()==1) {
      const char* iso = NULL;
      if(j_hit.Charge()>1450 && j_hit.Charge() < 1570) {
        iso = "239Pu";
      } else if(j_hit.Charge()>1570 && j_hit.Charge() < 1690) {
        iso = "241Am";
      } else if(j_hit.Charge()>1690 && j_hit.Charge() < 1800) {
        iso = "244Cm";
      }

      if(iso) {
        for(unsigned int i=0; i<sega.Size(); i++) {
          TSegaHit& s_hit = sega.GetSegaHit(i);
          obj.FillHistogram("coinc",Form("charge_summary_%s", iso),
                            18, 1, 19, s_hit.GetDetnum(),
                            32768, 0, 32768, s_hit.Charge());
          obj.FillHistogram("coinc",Form("energy_summary_%s", iso),
                            18, 1, 19, s_hit.GetDetnum(),
                            8000, 0, 4000, s_hit.GetEnergy());
        }
      }
    }
  }
}

void MakeCoincidenceHistogramsFissionSource(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus) {
  for(auto& j_hit : janus.GetAllHits()) {
    //if(j_hit.GetEnergy() > 6500) {
    if(j_hit.GetEnergy() > -1) {
      for(unsigned int i=0; i<sega.Size(); i++) {
        TSegaHit& s_hit = sega.GetSegaHit(i);

        auto s_pos = s_hit.GetPosition();
        auto j_pos = j_hit.GetPosition();

        auto angle = s_pos.Angle(j_pos);

        double s_energy = s_hit.GetEnergy();

        auto tdiff = s_hit.Timestamp() - j_hit.Timestamp();

        obj.FillHistogram("coinc", "fission_energy_angle",
                          4000, 0, 4000, s_energy,
                          180, 0, 180, (180/3.1415926)*angle);

        //obj.FillHistogram("coinc", Form("fission_energy_angle_Det%s",s_hit.GetDetnum()),
        //                4000, 0, 4000, s_energy,
        //              180, 0, 180, (180/3.1415926)*angle);

        obj.FillHistogram("coinc", Form("egam_tdiff_Det_%i",s_hit.GetDetnum()),
                          200, 0, 2000, tdiff,
                          4000, 0, 4000, s_energy);

        obj.FillHistogram("coinc", "fission_gamma_energy",
                          4000, 0, 4000, s_energy);

        obj.FillHistogram("coinc", "tdiff_fission",
                          200, 0, 2000, tdiff);

        obj.FillHistogram("coinc", "egam_tdiff_fission",
                          200, 0, 2000, tdiff,
                          4000, 0, 4000, s_energy);

        if(tdiff > 1000 && tdiff < 1600) {
          obj.FillHistogram("coinc", "fission_energy_angle_timegated",
                            4000, 0, 4000, s_energy,
                            180, 0, 180, (180/3.1415926)*angle);

          obj.FillHistogram("coinc", "fission_gamma_energy_timegated",
                            4000, 0, 4000, s_energy);

        }

      }
    }
  }

  if(janus.GetAllHits().size() == 2 &&
     janus.GetJanusHit(0).GetEnergy() > 6500 &&
     janus.GetJanusHit(1).GetEnergy() > 6500) {
    auto& hit1 = janus.GetJanusHit(0);
    auto& hit2 = janus.GetJanusHit(1);

    auto& low_hit = (hit1.GetEnergy() < hit2.GetEnergy()) ? hit1 : hit2;
    auto& high_hit = (hit1.GetEnergy() > hit2.GetEnergy()) ? hit1 : hit2;

    for(unsigned int i=0; i<sega.Size(); i++) {
      TSegaHit& s_hit = sega.GetSegaHit(i);

      auto s_pos = s_hit.GetPosition();
      auto low_pos = low_hit.GetPosition();
      auto high_pos = high_hit.GetPosition();

      auto low_angle = s_pos.Angle(low_pos);
      auto high_angle = s_pos.Angle(high_pos);

      double s_energy = s_hit.GetEnergy();

      obj.FillHistogram("coinc", "fission_low_energy_angle",
                        4000, 0, 4000, s_energy,
                        180, 0, 180, (180/3.1415926)*low_angle);

      obj.FillHistogram("coinc", "fission_high_energy_angle",
                        4000, 0, 4000, s_energy,
                        180, 0, 180, (180/3.1415926)*high_angle);

      obj.FillHistogram("coinc", "fission_high_DCenergy_angle",
                        4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), high_pos),
                        180, 0, 180, (180/3.1415926)*high_angle);

      // obj.FillHistogram("coinc", "fission_low_DCenergy_beta10",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.010, low_pos));
      // obj.FillHistogram("coinc", "fission_low_DCenergy_beta15",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.015, low_pos));
      // obj.FillHistogram("coinc", "fission_low_DCenergy_beta20",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.020, low_pos));
      // obj.FillHistogram("coinc", "fission_low_DCenergy_beta25",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.025, low_pos));
      // obj.FillHistogram("coinc", "fission_low_DCenergy_beta30",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.030, low_pos));
      // obj.FillHistogram("coinc", "fission_low_DCenergy_beta35",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.035, low_pos));
      // obj.FillHistogram("coinc", "fission_low_DCenergy_beta40",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.040, low_pos));
      // obj.FillHistogram("coinc", "fission_low_DCenergy_beta45",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.045, low_pos));

      // obj.FillHistogram("coinc", "fission_high_DCenergy_beta10",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.010, high_pos));
      // obj.FillHistogram("coinc", "fission_high_DCenergy_beta15",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.015, high_pos));
      // obj.FillHistogram("coinc", "fission_high_DCenergy_beta20",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.020, high_pos));
      // obj.FillHistogram("coinc", "fission_high_DCenergy_beta25",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.025, high_pos));
      // obj.FillHistogram("coinc", "fission_high_DCenergy_beta30",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.030, high_pos));
      // obj.FillHistogram("coinc", "fission_high_DCenergy_beta35",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.035, high_pos));
      // obj.FillHistogram("coinc", "fission_high_DCenergy_beta40",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.040, high_pos));
      // obj.FillHistogram("coinc", "fission_high_DCenergy_beta45",
      //                   4000, 0, 4000, s_hit.GetDoppler(0.045, high_pos));


      obj.FillHistogram("coinc", Form("fission_high_DCenergy_beta25_det%d", s_hit.GetDetnum()),
                        4000, 0, 4000, s_hit.GetDoppler(0.025, high_pos));
      obj.FillHistogram("coinc", Form("fission_high_DCenergy_beta35_det%d", s_hit.GetDetnum()),
                        4000, 0, 4000, s_hit.GetDoppler(0.035, high_pos));

      for(double beta : BinCenters(30, 0.015, 0.045)) {
        obj.FillHistogram("coinc", "fission_high_DCenergy_betascan",
                          30, 0.015, 0.045, beta,
                          500, 0, 500, s_hit.GetDoppler(beta, high_pos));
      }

      for(double zpos : BinCenters(60, -5, 10)) {
        obj.FillHistogram("coinc", Form("fission_high_DCenergy_beta25_det%d_zscan", s_hit.GetDetnum()),
                          60, -5, 10, zpos,
                          500, 0, 500, s_hit.GetDoppler(0.025, high_pos, TVector3(0, 0, zpos)));

        obj.FillHistogram("coinc", Form("fission_high_DCenergy_beta35_det%d_zscan", s_hit.GetDetnum()),
                          60, -5, 10, zpos,
                          500, 0, 500, s_hit.GetDoppler(0.035, high_pos, TVector3(0, 0, zpos)));


        obj.FillHistogram("coinc", "fission_high_DCenergy_beta25_zscan",
                          60, -5, 10, zpos,
                          4000, 0, 4000, s_hit.GetDoppler(0.025, high_pos, TVector3(0, 0, zpos)));
        obj.FillHistogram("coinc", "fission_high_DCenergy_beta35_zscan",
                          60, -5, 10, zpos,
                          4000, 0, 4000, s_hit.GetDoppler(0.035, high_pos, TVector3(0, 0, zpos)));
      }


    }
  }
}
