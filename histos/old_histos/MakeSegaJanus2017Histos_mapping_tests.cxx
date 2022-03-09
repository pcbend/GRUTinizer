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
    //MakeSegaHistograms(obj, *sega);
  }
  if(sega && janus){
    MakeCoincidenceHistograms(obj, *sega, *janus);
  }
  if(scalers){
    //MakeScalerHistograms(obj, *scalers);
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
    if(hit.GetEnergy() > 3500) {
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
    obj.FillHistogram("janus", "charge_summary_hit_front",
                      128, 0, 128, hit.GetFrontChannel(),
                      32768, 0, 32768, hit.Charge());
    obj.FillHistogram("janus", "charge_summary_hit_back",
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

    obj.FillHistogram("janus_seg_summaries", Form("charge_det%d_ring%d",hit.GetDetnum(),hit.GetRing()),
                      128, 0, 128, hit.GetFrontChannel(),
                      32768, 0, 32768, hit.Charge());
    obj.FillHistogram("janus_seg_summaries", "charge_summary_hit",
                      128, 0, 128, hit.GetBackChannel(),
                      32768, 0, 32768, hit.GetBackHit().Charge());

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

    obj.FillHistogram("janus",Form("det%d_phitheta", hit.GetDetnum()),
                      400,-4,4,hit.GetPosition().Theta(),
                      400,-4,4,hit.GetPosition().Phi());

    

    if(hit.GetRing() == 1) {
      obj.FillHistogram("janus", Form("det%d_charge_innermost_ring", hit.GetDetnum()),
                        32768, 0, 32768, hit.Charge());
    }


    //std::cout << "sector: " << hit.GetSector() << std::endl;
    //std::cout << "ring: " << hit.GetRing() << std::endl;
    //std::cout << "-------------------------------------------------------" << std::endl;
    //if(hit.IsSector()) {
      //std::cout << "in sector" << std::endl;
      obj.FillHistogram("janus_segs",Form("det%d_sec%i_xy",hit.GetDetnum(),hit.GetSector()),
                      200,-4,4,hit.GetPosition().X(),
                      200,-4,4,hit.GetPosition().Y());
    //}

    //if(hit.IsRing()) {      
      obj.FillHistogram("janus_segs",Form("det%d_ring%i_xy",hit.GetDetnum(),hit.GetRing()),
                      200,-4,4,hit.GetPosition().X(),
                      200,-4,4,hit.GetPosition().Y());
    //}

      int address = hit.Address();

      int crate = (address & 0x00ff0000) >> 16;
      int slot  = (address & 0x0000ff00) >> 8;
      int chan  = (address & 0x000000ff) >> 0;
      
     
      obj.FillHistogram("janus_segs",Form("Crate%i_slot%i_chan%i_xy",crate,slot,chan),
                        200,-4,4,hit.GetPosition().X(),
                        200,-4,4,hit.GetPosition().Y());

      int address_b = hit.GetBackHit().Address();

      int crate_b = (address_b & 0x00ff0000) >> 16;
      int slot_b  = (address_b & 0x0000ff00) >> 8;
      int chan_b  = (address_b & 0x000000ff) >> 0;

      obj.FillHistogram("janus_segs",Form("back_Crate%i_slot%i_chan%i_xy",crate_b,slot_b,chan_b),
                        200,-4,4,hit.GetPosition().X(),
                        200,-4,4,hit.GetPosition().Y());
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

      if(hit1.GetEnergy() > 3000 &&
         hit2.GetEnergy() > 3000) {
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
  //long timestamp_2 = -1;
  //long segment_timestamp_2 = -1; 


  for(unsigned int i=0; i<sega.Size(); i++){
    TSegaHit& hit = sega.GetSegaHit(i);
    int hit_detnum = hit.GetDetnum();
    double energy = hit.GetEnergy();


    obj.FillHistogram("sega","energy",
                      16384, 0, 32768, energy);
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

    obj.FillHistogram("sega",Form("numsegments_vs_core_energy_det%i",hit_detnum),
                      34, -1, 33, hit.GetNumSegments(),
                      8101, -100, 8000, energy);

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

    double total_seg_charge = 0;
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

      total_seg_charge = seg.Charge() + total_seg_charge;
      if(segi==hit.GetNumSegments()-1){
        obj.FillHistogram("sega",Form("total_seg_charge_vs_core_charge_det%i", hit_detnum),
                          4096, 0, 32768, hit.Charge(),
                          4096, 0, 32768, total_seg_charge);
      }
   
      if(hit.GetCrate()==1 && hit.GetSlot()==2){
        //if(segnum==0){  
        cc_timestamp = hit.Timestamp();
        segment_timestamp = seg.Timestamp();
        //timestamp_2 = seg.Timestamp();
        //std::cout << "------------------------------------" << std::endl ;
        //std::cout << "cc_timestamp is: " << cc_timestamp << std::endl ;
        //std::cout << "------------------------------------" << std::endl ;    
        //std::cout << "segnum!!!!!!!!!!!: " << segnum << std::endl ;
        obj.FillHistogram("sega","cc_timestamp",1500,-0.5,1,cc_timestamp/1e13);
        if(segment_timestamp!=-1){
          obj.FillHistogram("sega","cc_timestamp_seg",1500,-0.5,1,segment_timestamp/1e13);
        }
      } 
    
      //else if(hit.GetCrate()!=4)
      else if(seg.GetCrate())
      {
        segment_timestamp = hit.GetSegment(segi).Timestamp();
        //segment_timestamp_2 = hit.Timestamp() ;
        //std::cout << "------------------------------------" << std::endl ;
        //std::cout << "segment_timestamp is: " << segment_timestamp << std::endl ;
        //std::cout << "------------------------------------" << std::endl ;     
        //std::cout << "segment_timestamp is: " << segment_timestamp_2 << std::endl ;
     
        //std::cout << "segnum: " << segnum << std::endl ;

        obj.FillHistogram("sega","segment_timestamp",1500,-0.5,1,segment_timestamp/1e13);
        //obj.FillHistogram("sega","segment_timestamp",1500,-0.5,1,segment_timestamp_2/1e13);

      }


      if(cc_timestamp>0 && segment_timestamp>0){
         obj.FillHistogram("sega","segment_core_tdiff",
                             1000,-1000, 1000, (cc_timestamp - segment_timestamp));

         //obj.FillHistogram("sega","segment_core_tdiff2",
         //                   101,-0.01, 100.99, (cc_timestamp - segment_timestamp_2)/1e11);

         obj.FillHistogram("sega",Form("segment_core_tdiff_det%02d", hit_detnum),
	      		     1000,-1000, 1000, (cc_timestamp - segment_timestamp),
			     8192, 0, 32768, energy);
      }

    }

  }

    

    //obj.FillHistogram("sega",Form("energy_segment_core_tdiff_Det%i",hit.GetDetnum()),
    //                  1000,-5000,5000, cc_timestamp - segment_timestamp,
    //                  1000,0,8000,hit.GetEnergy()); 

 
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
                            4000, 0, 4000, s_hit.GetEnergy());
        }
      }
    }
  }
}

void MakeCoincidenceHistogramsFissionSource(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus) {
   //for(unsigned int i=0; i<janus.Size(); i++) {
   //    for(unsigned int j=i+1; j<janus.Size(); j++) {
   //        auto& hit1 = janus.GetJanusHit(i);
   //        auto& hit2 = janus.GetJanusHit(j);

   //        auto pos1 = hit1.GetPosition();
   //        auto pos2 = hit2.GetPosition();
   //        auto j_pos = hit1.GetPosition(); 

           //double angle = pos1.Angle(pos2);
           //for (unsigned int k = 0; k < sega.Size(); k++){
           //       TSegaHit& s_hit = sega.GetSegaHit(k);
           //        double s_energy = s_hit.GetEnergy();
           //        if(angle*(180./TMath::Pi() > 10)){
           //                obj.FillHistogram("coinc", "fission_gamma_DCenergy_relangle10",
           //                                4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), j_pos));
           //        }
           //}
   //    }
   //}

  for(auto& j_hit : janus.GetAllHits()) {
    //if(j_hit.GetEnergy() > 6500) {
    if(j_hit.GetEnergy() > 2000) {
      for(unsigned int i=0; i<sega.Size(); i++) {
        TSegaHit& s_hit = sega.GetSegaHit(i);

        auto s_pos = s_hit.GetPosition();
        auto j_pos = j_hit.GetPosition(); 

        auto angle = s_pos.Angle(j_pos);
        //if(isnan(angle)){
          //std::cout << "sega is: " << s_hit.GetDetnum() << std::endl; 
          //std::cout << "sega energy is: " << s_hit.GetEnergy() << std::endl;
          //std::cout << "sega x: " << s_pos.X() << std::endl;
        //}

        double s_energy = s_hit.GetEnergy();

        auto tdiff = (s_hit.Timestamp()*1.25 - j_hit.Timestamp());

        //std::cout << "-------------------------------------" << std::endl ;
        //std::cout << "sega timestamp is: " << s_hit.Timestamp() << std::endl ;
        //std::cout << "-------------------------------------" << std::endl ;
        //std::cout << "janus timestamp is: " << j_hit.Timestamp() << std::endl ;
        //std::cout << "-------------------------------------" << std::endl ;
        //std::cout << "tdiff is: " << tdiff << std::endl ;
        //std::cout << "-------------------------------------" << std::endl ;
       

        obj.FillHistogram("coinc", "fission_energy_angle",
                          4000, 0, 4000, s_energy,
                          180, 0, 180, (180/3.1415926)*angle);

        //obj.FillHistogram("coinc", Form("fission_energy_angle_Det%s",s_hit.GetDetnum()),
        //                4000, 0, 4000, s_energy,
        //              180, 0, 180, (180/3.1415926)*angle);

        obj.FillHistogram("coinc", Form("egam_tdiff_Det_%i",s_hit.GetDetnum()),
                          1000, -500, 500, tdiff,
                          4000, 0, 4000, s_energy);

        obj.FillHistogram("coinc", "fission_gamma_energy",
                          4000, 0, 4000, s_energy);

        obj.FillHistogram("coinc", "fission_gamma_DCenergy",
                          4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), j_pos));
                        
        obj.FillHistogram("coinc", "tdiff_fission",
                          1000, -500, 500, tdiff);

        obj.FillHistogram("coinc", "egam_tdiff_fission",
                          1000, -500, 500, tdiff,
                          4000, 0, 4000, s_energy);

        if(tdiff > 140 && tdiff < 210) {
          obj.FillHistogram("coinc", "fission_energy_angle_timegated",
                            4000, 0, 4000, s_energy,
                            180, 0, 180, (180/3.1415926)*angle);

          obj.FillHistogram("coinc", "fission_gamma_energy_timegated",
                            4000, 0, 4000, s_energy);
 
          obj.FillHistogram("coinc", "fission_gamma_DCenergy_timegated",
                            4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), j_pos));
	  if(j_hit.GetDetnum() == 0){
              obj.FillHistogram("coinc", "fission_gamma_DCenergy_upstream", 
                              4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), j_pos));
          }
         if(j_hit.GetDetnum() == 1){
              obj.FillHistogram("coinc", "fission_gamma_DCenergy_upstream", 
                              4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), j_pos));
	  }
	  obj.FillHistogram("coinc", "fission_gamma_DCenergy_upstream", 
                            4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), j_pos));
        }
      }
    }
  }

  if(janus.GetAllHits().size() == 2 &&
     janus.GetJanusHit(0).GetEnergy() > 2000 &&
     janus.GetJanusHit(1).GetEnergy() > 2000) {
    auto& hit1 = janus.GetJanusHit(0);
    auto& hit2 = janus.GetJanusHit(1);

    auto& low_hit = (hit1.GetEnergy() < hit2.GetEnergy()) ? hit1 : hit2;
    auto& high_hit = (hit1.GetEnergy() > hit2.GetEnergy()) ? hit1 : hit2;

    TVector3 pos_1;
    TVector3 pos_2;


    if(hit1.GetDetnum()==0){
      pos_1 = hit1.GetPosition();
      pos_2 = hit2.GetPosition();
    }else{

      pos_2 = hit1.GetPosition();
      pos_1 = hit2.GetPosition();
    }

//  for(double zpos : BinCenters(120, -10, 20)) {
//  
//    TVector3 pos_1scan(pos_1.X(),pos_1.Y(),pos_1.Z()+zpos);
//    TVector3 pos_2scan(pos_2.X(),pos_2.Y(),pos_2.Z()+zpos);

//    obj.FillHistogram("coinc", "relative_angle_2hitsabove2000_vs_zpos",
//                          120,-10,20 ,zpos,
//                          180,0,180, 180./TMath::Pi()*(pos_1scan.Angle(pos_2scan)));
//  }

//  for(double zpos : BinCenters(120, -10, 20)) {
//  
//    TVector3 pos_1scan(pos_1.X(),pos_1.Y()+zpos,pos_1.Z());
//    TVector3 pos_2scan(pos_2.X(),pos_2.Y()+zpos,pos_2.Z());

//    obj.FillHistogram("coinc", "relative_angle_2hitsabove2000_vs_ypos",
//                          120,-10,20 ,zpos,
//                          180,0,180, 180./TMath::Pi()*(pos_1scan.Angle(pos_2scan)));
//  }

//  for(double zpos : BinCenters(120, -10, 20)) {
//  
//    TVector3 pos_1scan(pos_1.X()+zpos,pos_1.Y(),pos_1.Z());
//    TVector3 pos_2scan(pos_2.X()+zpos,pos_2.Y(),pos_2.Z());

//    obj.FillHistogram("coinc", "relative_angle_2hitsabove2000_vs_xpos",
//                          120,-10,20 ,zpos,
//                          180,0,180, 180./TMath::Pi()*(pos_1scan.Angle(pos_2scan)));
//  }

//  for(double phi : BinCenters(360, -180, 180)) {
//  
//    TVector3 pos_1scan(pos_1.X(),pos_1.Y(),pos_1.Z());
//    pos_1scan.SetPhi(pos_1.Phi()+phi*TMath::Pi()/180.);
//    TVector3 pos_2scan(pos_2.X(),pos_2.Y(),pos_2.Z());

//    obj.FillHistogram("coinc", "relative_angle_2hitsabove2000_vs_phi",
//                          360,-180,180 ,phi,
//                          180,0,180, 180./TMath::Pi()*(pos_1scan.Angle(pos_2scan)));
//  }

    for(unsigned int i=0; i<sega.Size(); i++) {    
      TSegaHit& s_hit = sega.GetSegaHit(i);

      if(s_hit.GetEnergy()==-1)
        continue;

      if(180./TMath::Pi()*(hit1.GetPosition().Angle(hit2.GetPosition()))<10)
        continue;

      auto tdiff = (s_hit.Timestamp()*1.25 - high_hit.Timestamp());

      auto s_pos = s_hit.GetPosition();
      auto low_pos = low_hit.GetPosition();
      auto low_pos_22(low_pos);
      auto high_pos = high_hit.GetPosition();
      auto high_pos_22(high_pos);
      high_pos_22.SetPhi(high_pos.Phi() + 22.*TMath::Pi()/180.);
      auto low_angle = s_pos.Angle(low_pos);
      auto high_angle = s_pos.Angle(high_pos);

      double s_energy = s_hit.GetEnergy();

      obj.FillHistogram("coinc", "fission_high_energy_det",
                        4000, 0, 4000, s_hit.GetEnergy(),
                        20, 0, 20, s_hit.GetDetnum());


      //if(isnan(s_hit.GetDoppler(GValue::Value("beta"), high_pos))){
        //std::cout << "energy is: " << s_hit.GetEnergy() << std::endl;
        //std::cout << "det is: " << s_hit.GetDetnum() << std::endl;
        //std::cout << "seg is: " << s_hit.GetMainSegnum() << std::endl;
        //std::cout << "size is: " << sega.Size() << std::endl;
        //std::cout << "i is: " << i << std::endl;
        //std::cout << "num segments is: " << s_hit.GetNumSegments() << std::endl;
      //}

      obj.FillHistogram("coinc", "fission_high_DCenergy_det",
                        4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), high_pos),
                        20, 0, 20, s_hit.GetDetnum());
      obj.FillHistogram("coinc", "fission_high_DCenergy_det_22shift",
                        4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), high_pos_22),
                        20, 0, 20, s_hit.GetDetnum());
      obj.FillHistogram("coinc", "fission_low_energy_angle",
                        4000, 0, 4000, s_energy,
                        180, 0, 180, (180/3.1415926)*low_angle);

      obj.FillHistogram("coinc", "fission_high_energy_angle",
                        4000, 0, 4000, s_energy,
                        180, 0, 180, (180/3.1415926)*high_angle);

      obj.FillHistogram("coinc", Form("fission_high_energy_angle_det%i",s_hit.GetDetnum()),
                        4000, 0, 4000, s_energy,
                        180, 0, 180, (180/3.1415926)*high_angle);

      //obj.FillHistogram("coinc", "fission_low_DCenergy_angle",
      //                  4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), low_pos),
      //                  180, 0, 180, (180/3.1415926)*low_angle);

      obj.FillHistogram("coinc", "fission_high_DCenergy_angle",
                        4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), high_pos),
                        180, 0, 180, (180/3.1415926)*high_angle);

      obj.FillHistogram("coinc", "fission_high_DCenergy_tdiff",
                        1000, -500, 500, tdiff,
                        4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), high_pos));

      //obj.FillHistogram("coinc", "fission_low_DCenergy_janusenergy",
      //                  4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), low_pos),
      //                  24, 1000, 12000, low_hit.GetEnergy());

      obj.FillHistogram("coinc", "fission_high_DCenergy_janusenergy",
                        4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), high_pos),
                        24, 0, 12000, low_hit.GetEnergy());    

      if(tdiff>145 && tdiff<165){
        obj.FillHistogram("coinc", "fission_high_DCenergy_janusenergy_timegated",
                        4000, 0, 4000, s_hit.GetDoppler(GValue::Value("beta"), high_pos),
                        24, 0, 12000, low_hit.GetEnergy());    
      }   

      //obj.FillHistogram("coinc", Form("fission_high_DCenergy_beta25_det%d", s_hit.GetDetnum()),
      //                  4000, 0, 4000, s_hit.GetDoppler(0.025, high_pos));
      //obj.FillHistogram("coinc", Form("fission_high_DCenergy_beta35_det%d", s_hit.GetDetnum()),
      //                  4000, 0, 4000, s_hit.GetDoppler(0.035, high_pos));

      for(double beta : BinCenters(60, 0.000, 0.060)) {
        obj.FillHistogram("coinc", "fission_high_DCenergy_betascan",
                          60, 0.000, 0.060, beta,
                          4000, 0, 4000, s_hit.GetDoppler(beta, high_pos));

        obj.FillHistogram("coinc", Form("fission_high_DCenergy_betascan_det%i",s_hit.GetDetnum()),
                          60, 0.000, 0.060, beta,
                          4000, 0, 4000, s_hit.GetDoppler(beta, high_pos));

        //obj.FillHistogram("coinc", "fission_low_DCenergy_betascan",
        //                  60, 0.000, 0.060, beta,
        //                  4000, 0, 4000, s_hit.GetDoppler(beta, low_pos));
        //obj.FillHistogram("coinc", "fission_high_DCenergy_betascan_z10",
        //                  60, 0.000, 0.060, beta,
        //                  4000, 0, 4000, s_hit.GetDoppler(beta, high_pos, TVector3(0, 0, 10)));
        //obj.FillHistogram("coinc", "fission_low_DCenergy_betascan_z10",
        //                  60, 0.000, 0.060, beta,
        //                  4000, 0, 4000, s_hit.GetDoppler(beta, low_pos, TVector3(0, 0, 10)));
        obj.FillHistogram("coinc", "fission_high_DCenergy_betascan_z3y7",
                          60, 0.000, 0.060, beta,
                          4000, 0, 4000, s_hit.GetDoppler(beta, high_pos, TVector3(0, 7, 3)));

        if(tdiff>145 && tdiff<165){
          obj.FillHistogram("coinc", "fission_high_DCenergy_betascan_timegated",
                          60, 0.000, 0.060, beta,
                          4000, 0, 4000, s_hit.GetDoppler(beta, high_pos));
          obj.FillHistogram("coinc", "fission_high_DCenergy_betascan_z3y7_timegated",
                          60, 0.000, 0.060, beta,
                          4000, 0, 4000, s_hit.GetDoppler(beta, high_pos, TVector3(0, 7, 3)));
        }

      }

      for(double zpos : BinCenters(120, -10, 20)) {
        //obj.FillHistogram("coinc", Form("fission_high_DCenergy_beta25_det%d_zscan", s_hit.GetDetnum()),
        //                  60, -5, 10, zpos,
        //                  4000, 0, 4000, s_hit.GetDoppler(0.025, high_pos, TVector3(0, 0, zpos)));

        //obj.FillHistogram("coinc", Form("fission_high_DCenergy_beta35_det%d_zscan", s_hit.GetDetnum()),
        //                  60, -5, 10, zpos,
        //                  4000, 0, 4000, s_hit.GetDoppler(0.035, high_pos, TVector3(0, 0, zpos)));


        //obj.FillHistogram("coinc", "fission_high_DCenergy_beta25_zscan",
        //                  60, -5, 10, zpos,
        //                  4000, 0, 4000, s_hit.GetDoppler(0.025, high_pos, TVector3(0, 0, zpos)));
        obj.FillHistogram("coinc", "fission_high_DCenergy_beta35_zscan",
                          120, -10, 20, zpos,
                          4000, 0, 4000, s_hit.GetDoppler(0.035, high_pos, TVector3(0, 0, zpos)));
        //obj.FillHistogram("coinc", "fission_low_DCenergy_beta35_zscan",
        //                  120, -10, 20, zpos,
        //                  4000, 0, 4000, s_hit.GetDoppler(0.035, low_pos, TVector3(0, 0, zpos)));
        obj.FillHistogram("coinc", "fission_high_DCenergy_beta35_yscan",
                          120, -10, 20, zpos,
                          4000, 0, 4000, s_hit.GetDoppler(0.035, high_pos, TVector3(0, zpos, 0)));
        //obj.FillHistogram("coinc", "fission_low_DCenergy_beta35_yscan",
        //                 120, -10, 20, zpos,
        //                  4000, 0, 4000, s_hit.GetDoppler(0.035, low_pos, TVector3(0, zpos, 0)));
        obj.FillHistogram("coinc", "fission_high_DCenergy_beta35_xscan",
                          120, -10, 20, zpos,
                          4000, 0, 4000, s_hit.GetDoppler(0.035, high_pos, TVector3(zpos, 0, 0)));
        //obj.FillHistogram("coinc", "fission_low_DCenergy_beta35_xscan",
        //                  120, -10, 20, zpos,
        //                  4000, 0, 4000, s_hit.GetDoppler(0.035, low_pos, TVector3(zpos, 0, 0)));


        if(tdiff>145 && tdiff<165){
          obj.FillHistogram("coinc", "fission_high_DCenergy_beta35_zscan_timegated",
                          120, -10, 20, zpos,
                          4000, 0, 4000, s_hit.GetDoppler(0.035, high_pos, TVector3(0, 0, zpos)));
          obj.FillHistogram("coinc", "fission_high_DCenergy_beta35_yscan_timegated",
                          120, -10, 20, zpos,
                          4000, 0, 4000, s_hit.GetDoppler(0.035, high_pos, TVector3(0, zpos, 0)));
          obj.FillHistogram("coinc", "fission_high_DCenergy_beta35_xscan_timegated",
                          120, -10, 20, zpos,
                          4000, 0, 4000, s_hit.GetDoppler(0.035, high_pos, TVector3(zpos, 0, 0)));
        }
      }


    }
  }
}
