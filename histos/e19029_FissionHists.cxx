#include "TRuntimeObjects.h"

#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <limits>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TObject.h"
#include "TRandom.h"

#include "GValue.h"
#include "TJanusDDAS.h"
#include "DDASDataFormat.h"
#include "TNSCLScalers.h"
#include "TSega.h"
#include "GCutG.h"

#include "TVector3.h"
#include "TReaction.h"
#include "TSRIM.h"
#include "TRandom.h"
#include "TNucleus.h"

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

void MakeSega(TRuntimeObjects& obj, TSega& sega) {

  std::string dirname = "SeGA";

  obj.FillHistogram(dirname,"Mult",20,0,20,sega.Size());

  double seg_sum_charge = 0;
  double seg_sum_energy = 0;

  //for(auto hit : sega) {
  for(unsigned int i=0;i<sega.Size();i++) {

    TSegaHit hit = sega.GetSegaHit(i);
    int num_segs = hit.GetNumSegments();

    obj.FillHistogram(dirname,"CoreEnergy",4000,0,4000,hit.GetEnergy());
    obj.FillHistogram(dirname,"CoreCharge",10000,0,33000,hit.Charge());
    
    obj.FillHistogram("SeGA_Detectors",Form("CoreEnergy_det%02d",hit.GetDetnum()),
		      4000,0,4000,hit.GetEnergy());
    
    obj.FillHistogram("SeGA_Detectors",Form("CoreCharge_det%02d",hit.GetDetnum()),
		      9000,0,36000,hit.Charge());

    obj.FillHistogram(dirname,"SeGA_CoreEnergy_Summary",18,0,18,hit.GetDetnum(),
		      4000,0,4000,hit.GetEnergy());
    
    obj.FillHistogram(dirname,"SeGA_CoreCharge_Summary",18,0,18,hit.GetDetnum(),5000,0,33000,hit.Charge());
    
    obj.FillHistogram("SeGA_Detectors",Form("SegMult_det%02d",hit.GetDetnum()),33,0,33,num_segs);

    //obj.FillHistogram("SeGA_Detectors",Form("CoreEnergy_v_SegMult_det%02d",hit.GetDetnum()),
    //		      33,0,33,num_segs,
    //		      2000,0,4000,hit.GetEnergy());

    if(!num_segs) {
      obj.FillHistogram(dirname,"CoreEnergy_NoSegs",4000,0,4000,hit.GetEnergy());
      obj.FillHistogram("SeGA_Detectors_NoSegs",Form("GammaEnergy_det%02d",hit.GetDetnum()),
			4000,0,4000,hit.GetEnergy());
    }
    else {
      obj.FillHistogram(dirname,"CoreEnergy_YesSegs",4000,0,4000,hit.GetEnergy());
      obj.FillHistogram("SeGA_Detectors_YesSegs",Form("GammaEnergy_det%02d",hit.GetDetnum()),
			4000,0,4000,hit.GetEnergy());
    }

    long cc_ts = hit.Timestamp();
    seg_sum_charge = 0;
    seg_sum_energy = 0;
    
    for(int j=0;j<num_segs;j++) {

      TSegaSegmentHit seg = hit.GetSegment(j);
      long seg_ts = seg.Timestamp();
      seg_sum_charge += seg.Charge();
      seg_sum_energy += seg.GetEnergy();

      obj.FillHistogram(dirname,"Core_Seg_tdiff",2000,-20000,20000,cc_ts-seg_ts);

      obj.FillHistogram(dirname,"CoreEn_v_Core_Seg_tdiff",2000,-20000,20000,cc_ts-seg_ts,
			 2000,0,4000,hit.GetEnergy());

    } //end segment loop

    if(hit.Charge() <= 0) {

      obj.FillHistogram("SeGA_Detectors_NoCoreCharge",Form("SegMult_det%02d",hit.GetDetnum()),
			33,0,33,num_segs);
      
      obj.FillHistogram("SeGA_Detectors_NoCoreCharge",Form("SegSumCharge_det%02d",hit.GetDetnum()),
			33000,0,33000,seg_sum_charge);
      
      obj.FillHistogram("SeGA_Detectors_NoCoreCharge",Form("SegSumEnergy_det%02d",hit.GetDetnum()),
			4000,0,4000,seg_sum_energy);
      
      obj.FillHistogram(dirname,"SegSumEnergy_NoCoreCharge",4000,0,4000,seg_sum_energy);
      
    }
    
    obj.FillHistogram("SeGA_Core_Seg",Form("SegSum_v_CoreCharge_det%02d",hit.GetDetnum()),
		      2048,0,32768,hit.Charge(),2048,0,32768,seg_sum_charge);

    //obj.FillHistogram("SeGA_Core_Seg",Form("SegSum_v_CoreEnergy_det%02d",hit.GetDetnum()),
    //		      2000,0,4000,hit.GetEnergy(),3000,0,30000,seg_sum_energy);
   
   
  }//end hit loop
}

void BasicJanusSpectra(TRuntimeObjects& obj, TJanusDDASHit& hit, std::string dirname, std::string h_suf) {

  const TVector3 pos = hit.GetPosition(false);

  obj.FillHistogram(dirname,"Charge_v_Sector",34,0,34,hit.GetSector(),3000,0,30000,hit.Charge());
  
  obj.FillHistogram(dirname,Form("%sCharge_Hit",h_suf.c_str()),3000,0,30000,hit.Charge());
  obj.FillHistogram(dirname,Form("%sCharge_Hit",h_suf.c_str()),3000,0,30000,hit.GetBackHit().Charge());
  
  obj.FillHistogram(dirname,Form("%sSectorCharge_Hit",h_suf.c_str()),3000,0,30000,hit.Charge());
  obj.FillHistogram(dirname,Form("%sRingCharge_Hit",h_suf.c_str()),3000,0,30000,hit.GetBackHit().Charge());
  
  obj.FillHistogram(dirname,Form("%sCharge_Summary_Hit",h_suf.c_str()),
                    128,0,128,hit.GetFrontChannel(),
                    3000,0,30000,hit.Charge());
  obj.FillHistogram(dirname,Form("%sCharge_Summary_Hit",h_suf.c_str()),
                    128,0,128,hit.GetBackChannel(),
                    3000,0,30000,hit.GetBackHit().Charge());

  obj.FillHistogram(dirname,Form("%sRing_det%d",h_suf.c_str(),hit.GetDetnum()),26,0,26,hit.GetRing());
  obj.FillHistogram(dirname,Form("%sSector_det%d",h_suf.c_str(),hit.GetDetnum()),34,0,34,hit.GetSector());

  obj.FillHistogram(dirname,Form("%sRing_v_Sector_det%d",h_suf.c_str(),hit.GetDetnum()),
		    26,0,26,hit.GetRing(),34,0,34,hit.GetSector());

  double shift = 0.5*TMath::TwoPi()/32.0;
  obj.FillHistogram(dirname,Form("%sPerp_v_Phi_det%d",h_suf.c_str(),hit.GetDetnum()),
		    32,-TMath::Pi()+shift,TMath::Pi()+shift,pos.Phi(),24,1.1,3.5,pos.Perp());

  obj.FillHistogram(dirname,Form("%sY_v_X_det%d",h_suf.c_str(),hit.GetDetnum()),
		    200,-4,4,pos.X(),
		    200,-4,4,pos.Y());

  obj.FillHistogram(dirname,Form("%sSectorCharge_v_RingCharge_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    2000,0,30000,hit.GetBackHit().Charge(),
		    2000,0,30000,hit.Charge());
  
  auto tdiff = hit.Timestamp() - hit.GetBackHit().Timestamp();
   
  obj.FillHistogram(dirname,Form("%sFrontBack_Tdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    2750,-11000,11000,tdiff);

  
  auto cdiff = std::abs(hit.Charge() - hit.GetBackHit().Charge());
  
  obj.FillHistogram(dirname,Form("Cdiff_Hit_det%d",hit.GetDetnum()),2000,0,20000,cdiff);

  obj.FillHistogram(dirname,Form("%sCdiff_v_Tdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    2000,-10000,10000,tdiff,
		    2000,0,20000,cdiff);

  obj.FillHistogram(dirname,Form("%sRingCharge_v_Tdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    2000,-10000,10000, tdiff,
		    2000,0,30000,hit.GetBackHit().Charge());

  obj.FillHistogram(dirname,Form("%sRingCharge_v_Cdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    2000,0,20000,cdiff,
		    2000,0,30000,hit.GetBackHit().Charge());

  obj.FillHistogram(dirname,Form("%sSectorCharge_v_Tdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    2000,-10000,10000,tdiff,
		    2000,0,30000,hit.Charge());

  obj.FillHistogram(dirname,Form("%sSectorCharge_v_Cdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    2000,0,20000,cdiff,
		    2000,0,30000,hit.Charge());
 
}

void LeveledJanusSpectra(TRuntimeObjects& obj, TJanusDDAS& janus, int lvl) {

  std::string d_pref = "";
  
  if(lvl==0)
    {d_pref="Total_";}
  else if(lvl==1)
    {d_pref="Primary_";}
  else if(lvl==2)
    {d_pref="Secondary_";}
  else if(lvl==3)
    {d_pref="Tertiary_";}

  std::string dirname = d_pref + "Janus_Ungated";

  obj.FillHistogram(dirname,"All_HitMult_det1",50,0,50,janus.GetDetectorHits(1,lvl).size());   
  obj.FillHistogram(dirname,"All_HitMult_det0",50,0,50,janus.GetDetectorHits(0,lvl).size());   
  
  for(size_t i=0; i<janus.GetUnusedChannels(lvl).size(); i++) {
    auto &chan = janus.GetUnusedChannels(lvl).at(i);
    obj.FillHistogram(dirname,"Charge_Summary_UnusedChans",
		      128,0,128,chan.GetFrontChannel(),
		      3500,0,35000,chan.Charge());
    
    for(size_t j=i+1;j<janus.GetUnusedChannels(lvl).size();j++) {
      auto &chan1 = janus.GetUnusedChannels(lvl).at(j);
      
      if(chan1.GetDetnum() == chan.GetDetnum()) {
        obj.FillHistogram(dirname,Form("Tdiff_UnusedChans_det%d",chan.GetDetnum()),
	  		  2750,-11000,11000,chan.Timestamp()-chan1.Timestamp());
      }
        
    }//end second janus chan loop 
  }//end janus chan loop

  for(size_t i=0;i<janus.GetUnusedSectors(lvl).size();i++) {
    for(size_t j=0;j<janus.GetUnusedRings(lvl).size();j++) {
      auto &sect = janus.GetUnusedSectors(lvl).at(i);
      auto &ring = janus.GetUnusedRings(lvl).at(j);

      if(ring.GetDetnum() == sect.GetDetnum()) {
        obj.FillHistogram(dirname,Form("SectorCharge_v_RingCharge_UnusedChans_det%d",sect.GetDetnum()),
		          2000,0,30000,ring.Charge(),
		          2000,0,30000,sect.Charge());

	obj.FillHistogram(dirname,Form("Sector_Ring_TDiff_UnusedChans_det%d",sect.GetDetnum()),
			  2750,-11000,11000,sect.Timestamp()-ring.Timestamp());
      }
    }
  }

  
  for(size_t i=0;i<janus.GetHits(lvl).size();i++) {
    TJanusDDASHit& hit = janus.GetHits(lvl).at(i);
    
    BasicJanusSpectra(obj,hit,dirname,"All_");  
       
    for(size_t j=i+1;j<janus.GetHits(lvl).size();j++) {
      TJanusDDASHit& hit1 = janus.GetHits(lvl).at(j);

      if(hit.GetDetnum() == hit1.GetDetnum()) {

	obj.FillHistogram(dirname,Form("All_MultiHit_TDiff_det%d",hit.GetDetnum()),
			  2750,-11000,11000,hit.Timestamp()-hit1.Timestamp());


      }
    } 
  }

  /*
  for(size_t i=0;i<janus.GetDoubleHits(lvl).size();i++) {
    TJanusDDASHit& hit = janus.GetDoubleHits(lvl).at(i);
    BasicJanusSpectra(obj,hit,dirname,"TwoHit_");
  }

  for(size_t i=0;i<janus.GetSingleHits(lvl).size();i++) {
    TJanusDDASHit& hit = janus.GetSingleHits(lvl).at(i);
    BasicJanusSpectra(obj,hit,dirname,"Singles_");    
  }

  for(size_t i=0;i<janus.GetAddbackHits(lvl).size();i++) {
    TJanusDDASHit& hit = janus.GetAddbackHits(lvl).at(i);
    BasicJanusSpectra(obj,hit,dirname,"Addback_");  
  }
  */
  
}

void MakeJanus(TRuntimeObjects& obj, TJanusDDAS& janus) {
  
  std::string dirname = "Janus_Ungated";
  
  for(size_t i=0; i<janus.GetAllChannels().size(); i++) {
    auto &chan = janus.GetJanusChannel(i);

    obj.FillHistogram(dirname,"Charge_Summary_AllChans",
		      128,0,128,chan.GetFrontChannel(),
		      3500,0,35000,chan.Charge());

  }//end all channel loop

  for(size_t i=0; i<janus.GetChannels().size(); i++) {
    
    auto &chan = janus.GetChannels().at(i);
    obj.FillHistogram(dirname,"Charge_Summary_UsableChans",
		      128,0,128,chan.GetFrontChannel(),
		      3500,0,35000,chan.Charge());

    for(size_t j=i+1;j<janus.GetChannels().size();j++) {
      auto &chan1 = janus.GetChannels().at(j);

      if(chan1.GetDetnum() == chan.GetDetnum()) {
        obj.FillHistogram(dirname,Form("Tdiff_UsableChans_det%d",chan.GetDetnum()),
	  		  2750,-11000,11000,chan.Timestamp()-chan1.Timestamp());
      }
      
    }//end second janus chan loop 
  }//end janus chan loop

  for(size_t i=0;i<janus.GetSectors().size();i++) {
    for(size_t j=0;j<janus.GetRings().size();j++) {
      
      auto &sect = janus.GetSectors().at(i);
      auto &ring = janus.GetRings().at(j);

      if(ring.GetDetnum() == sect.GetDetnum()) {
        obj.FillHistogram(dirname,Form("SectorCharge_v_RingCharge_UsableChans_det%d",sect.GetDetnum()),
		          2000,0,30000,ring.Charge(),
		          2000,0,30000,sect.Charge());

	obj.FillHistogram(dirname,Form("Sector_Ring_TDiff_UsableChans_det%d",sect.GetDetnum()),
			  2750,-11000,11000,sect.Timestamp()-ring.Timestamp());
      }
      
    }//end ring loop
  }//end sector loop

  //You could add a charge gate here too
  //or even pick out the hits that satisfy the charge gate, then check there are only two
  if(janus.GetHits(1).size() == 2) {

    auto &hit1 = janus.GetHits(1).at(0);
    auto &hit2 = janus.GetHits(1).at(1);

    TVector3 pos1 = hit1.GetPosition(false);
    TVector3 pos2 = hit2.GetPosition(false);

    double angle = pos1.Angle(pos2);
    angle *= TMath::RadToDeg();

    obj.FillHistogram("Primary_Janus_Ungated","Angle_Mult2",100,-10,190,angle);
  }

  
  LeveledJanusSpectra(obj,janus,1);
  
}

void MakeSeGAJanus(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus) {

   std::string dirname = "SeGAJanus";
   int lvl = 1;

   int d1_mult = janus.GetDetectorHits(1,lvl).size();
   int d0_mult = janus.GetDetectorHits(0,lvl).size();
   
   obj.FillHistogram(dirname,"sega_mult",20,0,20,sega.Size());

   obj.FillHistogram(dirname,"All_jMult_det1",40,0,40,d1_mult);

   obj.FillHistogram(dirname,"All_jMult_det0",40,0,40,d0_mult);

   for(auto& j_hit : janus.GetHits(lvl)) {
     for(auto& s_hit : sega) {

       auto tdiff_sec = (s_hit.Timestamp() - j_hit.Timestamp());

       obj.FillHistogram(dirname,Form("CoreEnergy_v_tdiff_det%d",j_hit.GetDetnum()),
			 1000,-1000,3000,tdiff_sec,
			 2500,0,5000,s_hit.GetEnergy());

       obj.FillHistogram(dirname,Form("ParticleCharge_v_CoreEnergy_jdet%d",j_hit.GetDetnum()),
			 2500,0,5000,s_hit.GetEnergy(),
			 3000,0,30000,j_hit.Charge());
       
     }//end sega hit loop
   }//end janus hit loop
}

void MakeGatedSeGAJanus(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus, GCutG* time_gate, GCutG* charge_gate) {

  std::string dirname = Form("SeGAJanus_%s_%s",time_gate->GetName(),charge_gate->GetName());
  int lvl = 1;

  double beta = 0.03;
  for(auto& j_hit : janus.GetHits(lvl)) {

    /*
    //Charge gate to pick out fission fragments
    if(j_hit.Charge() < ???? || j_hit.Charge() > ????) {
      continue;
    }
    */
    
    TVector3 j_pos = j_hit.GetPosition(false);
    
    for(auto& s_hit : sega) {

      auto tdiff = (s_hit.Timestamp() - j_hit.Timestamp());
      double s_energy = s_hit.GetEnergy();
       
      if(!time_gate->IsInside(tdiff,s_energy)) {
	continue;
      }
      if(!charge_gate->IsInside(j_hit.GetSector(),j_hit.Charge())) {
        continue;
      }

      obj.FillHistogram(dirname,"Core_Energy",5000,0,5000,s_hit.GetEnergy());

      obj.FillHistogram(dirname,Form("CoreEnergy_v_tdiff_jdet%d",j_hit.GetDetnum()),
			1000,-1000,3000,tdiff,
			2500,0,5000,s_hit.GetEnergy());

      obj.FillHistogram(dirname,Form("ParticleCharge_v_CoreEnergy_jdet%d",j_hit.GetDetnum()),
			2000,0,5000,s_hit.GetEnergy(),
			1000,0,30000,j_hit.Charge());

      double zoff;
      if(std::isnan(GValue::Value("Sega_Z_offset"))) {
	zoff = 0.0;
      }
      else {
	zoff = GValue::Value("Sega_Z_offset");
      }
      
      double dop_en = s_hit.GetDoppler(beta,j_pos,TVector3(0,0,zoff));

      obj.FillHistogram(dirname,Form("ParticleCharge_v_DopplerEnergy_jdet%d",j_hit.GetDetnum()),
			2000,0,5000,dop_en,
			1000,0,30000,j_hit.Charge());
      
      obj.FillHistogram(dirname,"Doppler_Energy",4000,0,4000,dop_en);
      obj.FillHistogram(dirname,Form("Doppler_Energy_jDet%d",j_hit.GetDetnum()),4000,0,4000,dop_en);
       
    }//end sega hit loop
  }//end janus hit loop
  
}

int gates_loaded=0;
std::vector<GCutG*> time_gates;
std::vector<GCutG*> charge_gates;

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  
  TSega* sega = obj.GetDetector<TSega>();
  TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();

  TList *gates = &(obj.GetGates());
  if(gates_loaded!=gates->GetSize()) {
    TIter iter(gates);
    while(TObject *obj = iter.Next()) {
      GCutG *gate = (GCutG*)obj;
      std::string tag = gate->GetTag();
      if(!tag.compare("time")) {
        time_gates.push_back(gate);
	gates_loaded++;
        std::cout << "Time Gate: << " << gate->GetName() << std::endl;
      }
      if(!tag.compare("charge")) {
        charge_gates.push_back(gate);
        gates_loaded++;
        std::cout << "Charge Gate: << " << gate->GetName() << std::endl;
      }
      else {
	std::cout << "Unknown Gate: " << gate->GetName() << std::endl;
      }
    }
  } 

  if(sega) {
    MakeSega(obj,*sega);
  }
  
  if(janus) {
    MakeJanus(obj,*janus);
  }
  
  if(sega && janus) {

    MakeSeGAJanus(obj,*sega,*janus);
    
    for(auto &tgate : time_gates) {
      for(auto &cgate : charge_gates) {
        MakeGatedSeGAJanus(obj,*sega,*janus,tgate,cgate);
      }
    }    
  }
  
}

