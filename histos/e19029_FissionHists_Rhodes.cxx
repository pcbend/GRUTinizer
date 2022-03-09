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

  int index = -1;
  //for(auto hit : sega) {
  for(unsigned int i=0;i<sega.Size();i++) {

    TSegaHit hit = sega.GetSegaHit(i);
    int num_segs = hit.GetNumSegments();

    if(hit.GetDetnum() == 1) {
      index = i;
    }

    obj.FillHistogram(dirname,"CoreEnergy",4000,0,4000,hit.GetEnergy());
    obj.FillHistogram(dirname,"CoreCharge",10000,0,33000,hit.Charge());
    
    obj.FillHistogram("SeGA_Detectors",Form("CoreEnergy_det%02d",hit.GetDetnum()),
		      4000,0,4000,hit.GetEnergy());
    
    obj.FillHistogram("SeGA_Detectors",Form("CoreCharge_det%02d",hit.GetDetnum()),
		      9000,0,36000,hit.Charge());

    obj.FillHistogram("SeGA_Detectors",Form("SegSummary_det%02d",hit.GetDetnum()),
		      34,0,34,hit.GetMainSegnum(),3000,0,36000,hit.Charge());

    obj.FillHistogram(dirname,"CoreEnergy_Summary",18,0,18,hit.GetDetnum(),
		      4000,0,4000,hit.GetEnergy());
    
    obj.FillHistogram(dirname,"CoreCharge_Summary",18,0,18,hit.GetDetnum(),5000,0,33000,hit.Charge());
    
    obj.FillHistogram("SeGA_Detectors",Form("SegMult_det%02d",hit.GetDetnum()),33,0,33,num_segs);

    //obj.FillHistogram("SeGA_Detectors",Form("CoreEnergy_v_SegMult_det%02d",hit.GetDetnum()),
    //		      33,0,33,num_segs,
    //		      2000,0,4000,hit.GetEnergy());

    if(!num_segs) {
      obj.FillHistogram(dirname,"CoreEnergy_NoSegs",4000,0,4000,hit.GetEnergy());
      obj.FillHistogram("SeGA_Detectors_NoSegs",Form("GammaEnergy_det%02d",hit.GetDetnum()),
			4000,0,4000,hit.GetEnergy());
    }
    /*
    else {
      obj.FillHistogram(dirname,"CoreEnergy_YesSegs",4000,0,4000,hit.GetEnergy());
      obj.FillHistogram("SeGA_Detectors_YesSegs",Form("GammaEnergy_det%02d",hit.GetDetnum()),
			4000,0,4000,hit.GetEnergy());
    }
    */
    
    
    long cc_ts = hit.Timestamp();
    double seg_sum_charge = 0;
    
    for(int j=0;j<num_segs;j++) {

      TSegaSegmentHit seg = hit.GetSegment(j);
      long seg_ts = seg.Timestamp();
      seg_sum_charge += seg.Charge();

      obj.FillHistogram(dirname,"Core_Seg_tdiff",2000,-20000,20000,seg_ts-cc_ts);

      obj.FillHistogram(dirname,"CoreEn_v_Core_Seg_tdiff",2000,-20000,20000,cc_ts-seg_ts,
			 2000,0,4000,hit.GetEnergy());

    }//end segment loop

    if(hit.Charge() <= 0) {

      obj.FillHistogram("SeGA_Detectors_NoCoreCharge",Form("SegMult_det%02d",hit.GetDetnum()),
			33,0,33,num_segs);
      
      obj.FillHistogram("SeGA_Detectors_NoCoreCharge",Form("SegSumCharge_det%02d",hit.GetDetnum()),
			33000,0,33000,seg_sum_charge);
      
    }

    obj.FillHistogram("SeGA_Core_Seg",Form("SegSum_v_CoreCharge_det%02d",hit.GetDetnum()),
		      2048,0,32768,hit.Charge(),2048,0,32768,seg_sum_charge);
   
   
  }//end hit loop

  if(index > -1 && sega.Size() > 1) {

    TSegaHit ref_hit = sega.GetSegaHit(index);
    auto tref = ref_hit.Timestamp();
    
    for(unsigned int i=0;i<sega.Size();i++) {

      if(i == index) {
	continue;
      }

      TSegaHit hit = sega.GetSegaHit(i);

      auto tdiff = hit.Timestamp() - tref;

      obj.FillHistogram(dirname,"CoreEnergy_v_tdiff",
			300,-3000,3000,tdiff,
			2500,0,5000,hit.GetEnergy());

    }
    
  }
}

void BasicJanusSpectra(TRuntimeObjects& obj, TJanusDDASHit& hit, std::string dirname, std::string h_suf) {

  const TVector3 pos = hit.GetPosition(false);
  
  obj.FillHistogram(dirname,Form("%sCharge_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),3000,0,30000,hit.Charge());
  obj.FillHistogram(dirname,Form("%sCharge_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),3000,0,30000,hit.GetBackHit().Charge());
  
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

  obj.FillHistogram(dirname,Form("%sCharge_v_Sector_det%d",h_suf.c_str(),hit.GetDetnum()),
		    34,0,34,hit.GetSector(),3000,0,30000,hit.Charge());

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
  /*
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
  */
 
}

void LeveledJanusSpectra(TRuntimeObjects& obj, TJanusDDAS& janus, int lvl, std::vector<GCutG*> gates) {

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
  std::string dirname1 = d_pref + "Janus_Angles";

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
    
    /*
      for(size_t j=i+1;j<janus.GetHits(lvl).size();j++) {
      TJanusDDASHit& hit1 = janus.GetHits(lvl).at(j);

      if(hit.GetDetnum() == hit1.GetDetnum()) {

      obj.FillHistogram(dirname,Form("All_MultiHit_TDiff_det%d",hit.GetDetnum()),
      2750,-11000,11000,hit.Timestamp()-hit1.Timestamp());

      }
      }
    */

  }

  GCutG* gate = gates.at(0);
  dirname = d_pref + "Janus_" + gate->GetName();
  for(size_t i=0;i<janus.GetHits(lvl).size();i++) {

    TJanusDDASHit& hit = janus.GetHits(lvl).at(i);
    if(!hit.GetDetnum()) {
      continue;
    }
    if(!gate->IsInside(hit.GetSector(),hit.Charge())) {
      continue;
    }
    
    BasicJanusSpectra(obj,hit,dirname,"All_");

  }
  

  /*
  for(int j=0;j<gates.size();j++) {

    GCutG* gate = gates.at(j);
    std::string tag = gate->GetTag();
    dirname = d_pref + "Janus_" + gates.at(j)->GetName();

    for(size_t i=0;i<janus.GetHits(lvl).size();i++) {
	
      TJanusDDASHit& hit = janus.GetHits(lvl).at(i);
      int det = hit.GetDetnum();

      if(det && !tag.compare("D1")) {
	if(!gate->IsInside(hit.GetSector(),hit.Charge()))
	  continue;
	
	BasicJanusSpectra(obj,hit,dirname,"All_");
      }
      else if(!det && !tag.compare("D0")) {
	if(!gate->IsInside(hit.GetSector(),hit.Charge()))
	  continue;
	  
	BasicJanusSpectra(obj,hit,dirname,"All_"); 
      }
      else {
	continue;
      }
    } 
  }
  */

  if(janus.GetHits(lvl).size() == 2) {

    auto &hit1 = janus.GetHits(lvl).at(0);
    auto &hit2 = janus.GetHits(lvl).at(1);

    if(hit1.GetDetnum() != hit2.GetDetnum()) {

      bool h1D1 = false;
      bool h1A = false;
      if(hit1.GetDetnum()) {
	h1D1 = true;
	if(hit1.Charge() < 480) {
	  h1A = true;
	}
      }
      else {
	if(hit1.Charge() < 1600) {
	  h1A = true;
	}
      }

      bool h2A = false;
      if(hit2.GetDetnum()) {
	if(hit2.Charge() < 480) {
	  h2A = true;
	}
      }
      else {
	if(hit2.Charge() < 1600) {
	  h2A = true;
	}
      }
    
      TVector3 pos1 = hit1.GetPosition(false);
      TVector3 pos2 = hit2.GetPosition(false);

      double angle = pos1.Angle(pos2);
      angle *= TMath::RadToDeg();
    
      obj.FillHistogram(dirname1,"Angle_Mult2",100,-10,190,angle);

      if(h1A && h2A) {
	obj.FillHistogram(dirname1,"Angle_Mult2_TwoAlpha",100,-10,190,angle);
      }

      if((h1A && !h2A) || (h2A && !h1A)) {
	obj.FillHistogram(dirname1,"Angle_Mult2_OneAlpha",100,-10,190,angle);
      }

      if(!h1A && !h2A) {
	obj.FillHistogram(dirname1,"Angle_Mult2_NoAlpha",100,-10,190,angle); 
      }
    
      int h1 = 0;
      if(h1D1) {
	for(int j=0;j<gates.size();j++) {
	
	  GCutG* gate = gates.at(j);
	  std::string name = gate->GetName();
	  std::string tag = gate->GetTag();
	
	  if(!(name.compare("Alpha")) || !(tag.compare("D0")))
	    continue;

	  if(!gate->IsInside(hit1.GetSector(),hit1.Charge()))
	    continue;

	  if(name.find("LE") != std::string::npos) 
	    h1 = 1;  
	  else 
	    h1 = 2;	
	}
      }
      else {
	for(int j=0;j<gates.size();j++) {
	
	  GCutG* gate = gates.at(j);
	  std::string name = gate->GetName();
	  std::string tag = gate->GetTag();
	
	  if(!(name.compare("Alpha")) || !(tag.compare("D1")))
	    continue;

	  if(!gate->IsInside(hit1.GetSector(),hit1.Charge()))
	    continue;

	  if(name.find("LE") != std::string::npos) 
	    h1 = 1;  
	  else 
	    h1 = 2;
	
	}
      }

      int h2 = 0;
      if(h1D1) {
	for(int j=0;j<gates.size();j++) {
	
	  GCutG* gate = gates.at(j);
	  std::string name = gate->GetName();
	  std::string tag = gate->GetTag();
	
	  if(!(name.compare("Alpha")) || !(tag.compare("D1")))
	    continue;

	  if(!gate->IsInside(hit2.GetSector(),hit2.Charge()))
	    continue;

	  if(name.find("LE") != std::string::npos) 
	    h2 = 1;  
	  else 
	    h2 = 2;	
	}
      }
      else {
	for(int j=0;j<gates.size();j++) {
	
	  GCutG* gate = gates.at(j);
	  std::string name = gate->GetName();
	  std::string tag = gate->GetTag();
	
	  if(!(name.compare("Alpha")) || !(tag.compare("D0")))
	    continue;

	  if(!gate->IsInside(hit2.GetSector(),hit2.Charge()))
	    continue;

	  if(name.find("LE") != std::string::npos) 
	    h2 = 1;  
	  else 
	    h2 = 2;
      
	}
      }

      if((h1 && h2) && (h1 != h2)) {
	obj.FillHistogram(dirname1,"Angle_Mult2_LE_HE",100,-10,190,angle); 
      }
    }
  } //end if(size() == 2)
  
}

void MakeJanus(TRuntimeObjects& obj, TJanusDDAS& janus, std::vector<GCutG*> gates) {
  
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

  int lvl = 0;
  LeveledJanusSpectra(obj,janus,lvl,gates);
  
}

void MakeSeGAJanus(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus) {

   std::string dirname = "SeGAJanus";
   int lvl = 0;
   double beta = 0.035;

   int d1_mult = janus.GetDetectorHits(1,lvl).size();
   int d0_mult = janus.GetDetectorHits(0,lvl).size();
   
   obj.FillHistogram(dirname,"sega_mult",20,0,20,sega.Size());

   obj.FillHistogram(dirname,"All_jMult_det1",40,0,40,d1_mult);

   obj.FillHistogram(dirname,"All_jMult_det0",40,0,40,d0_mult);

   for(auto& j_hit : janus.GetHits(lvl)) {

     TVector3 j_pos = j_hit.GetPosition(false);
     
     for(auto& s_hit : sega) {

       obj.FillHistogram(dirname,"Core_Energy",5000,0,5000,s_hit.GetEnergy());
       obj.FillHistogram(dirname,"Core_Energy_Summary",18,0,18,s_hit.GetDetnum(),
			 25000,0,5000,s_hit.GetEnergy());

       auto tdiff_sec = (s_hit.Timestamp() - j_hit.Timestamp());
       obj.FillHistogram(dirname,Form("CoreEnergy_v_tdiff_det%d",j_hit.GetDetnum()),
			 1000,-1000,3000,tdiff_sec,
			 2500,0,5000,s_hit.GetEnergy());

       double dop_en = s_hit.GetDoppler(beta,j_pos,TVector3(0,0,GValue::Value("Sega_Z_offset")));
       obj.FillHistogram(dirname,"Doppler_Energy",4000,0,4000,dop_en);
       obj.FillHistogram(dirname,"Doppler_Energy_Summary",18,0,18,s_hit.GetDetnum(),4000,0,4000,dop_en);
       
     }//end sega hit loop
   }//end janus hit loop
}

void MakeGatedSeGAJanus(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus,
			std::vector<GCutG*> gates, GCutG* time_gate) {
  
  int lvl = 0;
  std::string d_pref = "";
  
  if(lvl==0)
    {d_pref="Total_";}
  else if(lvl==1)
    {d_pref="Primary_";}
  else if(lvl==2)
    {d_pref="Secondary_";}
  else if(lvl==3)
    {d_pref="Tertiary_";}
  
  double beta = 0.035;
  TVector3 incBeam = TVector3(0.0,0.0,1.0);

  for(int i=0;i<gates.size();i++) {
    
    GCutG* gate = gates.at(i);
    std::string tag = gate->GetTag();

    std::string dirname = d_pref + Form("SeGAJanus_%s_%s",gate->GetName(),time_gate->GetName());
    for(auto& j_hit : janus.GetHits(lvl)) {
      
      if(j_hit.GetDetnum()) {
	if(!tag.compare("D0"))
	  continue;
      }
      else {
	if(!tag.compare("D1"))
	  continue;
      }
      
      //if(!j_hit.GetDetnum()) {
      //continue;
      //}

      if(!gate->IsInside(j_hit.GetSector(),j_hit.Charge()))
	continue;

      
      obj.FillHistogram(dirname,"Charge_Summary",128,0,128,j_hit.GetFrontChannel(),
			3000,0,30000,j_hit.Charge());
      obj.FillHistogram(dirname,"Charge_Summary",128,0,128,j_hit.GetBackChannel(),
			3000,0,30000,j_hit.GetBackHit().Charge());
    
      TVector3 j_pos = j_hit.GetPosition(false);
    
      for(auto& s_hit : sega) {

	auto tdiff = (s_hit.Timestamp() - j_hit.Timestamp());
	double s_energy = s_hit.GetEnergy();
       
	if(!time_gate->IsInside(tdiff,s_energy)) {
	  continue;
	}

	TVector3 s_pos = s_hit.GetPosition(true);

	obj.FillHistogram(dirname,Form("CoreEnergy_v_tdiff_jdet%d",j_hit.GetDetnum()),
			  1000,-1000,3000,tdiff,
			  2500,0,5000,s_hit.GetEnergy());

	obj.FillHistogram(dirname,Form("JanusCharge_v_tdiff_jdet%d",j_hit.GetDetnum()),
			  1000,-1000,3000,tdiff,
			  2500,0,25000,j_hit.Charge());

	obj.FillHistogram(dirname,Form("ParticleCharge_v_CoreEnergy_jdet%d",j_hit.GetDetnum()),
			  2000,0,5000,s_hit.GetEnergy(),
			  1000,0,30000,j_hit.Charge());

	double zoff = GValue::Value("Sega_Z_offset");
	if(std::isnan(zoff)) {
	  zoff = 0.0;
	}
      
	double dop_en = s_hit.GetDoppler(beta,j_pos,TVector3(0,0,zoff));

	double gamma = 1./(sqrt(1.-pow(beta,2.)));
	double cos_angle = TMath::Cos(s_pos.Angle(incBeam));
	double dop_en_beta = s_energy*gamma *(1 - beta*cos_angle);

	obj.FillHistogram(dirname,Form("ParticleCharge_v_DopplerEnergy_jdet%d",j_hit.GetDetnum()),
			  2000,0,5000,dop_en,
			  1000,0,30000,j_hit.Charge());

	obj.FillHistogram(dirname,"Core_Energy",4000,0,4000,s_hit.GetEnergy());
	obj.FillHistogram(dirname,Form("Core_Energy_jDet%d",j_hit.GetDetnum()),
			  4000,0,4000,s_hit.GetEnergy());
	
	obj.FillHistogram(dirname,"Core_Energy_Summary",18,0,18,s_hit.GetDetnum(),
			  25000,0,5000,s_hit.GetEnergy());
      
	obj.FillHistogram(dirname,"Doppler_Energy",4000,0,4000,dop_en);
	obj.FillHistogram(dirname,"Doppler_Energy_Summary",18,0,18,s_hit.GetDetnum(),4000,0,4000,dop_en);
	obj.FillHistogram(dirname,Form("Doppler_Energy_jDet%d",j_hit.GetDetnum()),4000,0,4000,dop_en);

	obj.FillHistogram(dirname,"Doppler_Energy_BetaOnly",4000,0,4000,dop_en_beta);

	double angle = j_pos.Angle(s_pos)*TMath::RadToDeg();

	obj.FillHistogram(dirname,"ThetaDop_Correlation",4000,0,4000,s_energy,180,0,180,angle);
	obj.FillHistogram(dirname,"ThetaDop_Correction",4000,0,4000,dop_en,180,0,180,angle);

	TVector3 reacPlane = j_pos.Cross(incBeam);
	TVector3 detPlane = s_pos.Cross(incBeam);

	double reac_phi = reacPlane.Phi();
	double det_phi = detPlane.Phi();

	double planeAng = reac_phi - det_phi;
	if(planeAng < 0){
	  planeAng += TMath::TwoPi();
	}
	//planeAng *= TMath::RadToDeg();

	double thing = 65*TMath::Pi()/32.0;
	obj.FillHistogram(dirname,"PhiDop_Correlation",4000,0,4000,s_energy,32,0,thing,planeAng);
	obj.FillHistogram(dirname,"PhiDop_Correction",4000,0,4000,dop_en,32,0,thing,planeAng);

	for(int i=1;i<101;i++) {
	  double tmp_beta = 0.0 + 0.0005*i;
	  obj.FillHistogram(dirname,"Beta_Scan",100,0,0.0505,tmp_beta,
			    2000,0,4000,s_hit.GetDoppler(tmp_beta,j_pos,TVector3(0,0,zoff)));
	}
	/*
	for(int i=1;i<101;i++) {
	  double tmp_xoff = -5.0 + 0.1*i;
	  obj.FillHistogram(dirname,"sX_Scan",100,-5.0,5.1,tmp_xoff,
			    2000,0,4000,s_hit.GetDoppler(beta,j_pos,TVector3(tmp_xoff,0,zoff)));
	}

	for(int i=1;i<101;i++) {
	  double tmp_yoff = -5.0 + 0.1*i;
	  obj.FillHistogram(dirname,"sY_Scan",100,-5.0,5.1,tmp_yoff,
			    2000,0,4000,s_hit.GetDoppler(beta,j_pos,TVector3(0,tmp_yoff,zoff)));
	}

	for(int i=1;i<101;i++) {
	  double tmp_zoff = -5.0 + 0.1*i;
	  obj.FillHistogram(dirname,"sZ_Scan",100,-5.0,5.1,tmp_zoff,
			    2000,0,4000,s_hit.GetDoppler(beta,j_pos,TVector3(0,0,tmp_zoff)));
	}

	for(int i=1;i<101;i++) {
	  double tmp_xoff = -0.5 + 0.01*i;
	  TVector3 tmp_pos = j_hit.GetPosition(false,true,TVector3(tmp_xoff,0,0));
	
	  obj.FillHistogram(dirname,"jX_Scan",100,-0.5,0.51,tmp_xoff,
			    2000,0,4000,s_hit.GetDoppler(beta,tmp_pos,TVector3(0,0,zoff)));
	}

	for(int i=1;i<101;i++) {
	  double tmp_yoff = -0.5 + 0.01*i;
	  TVector3 tmp_pos = j_hit.GetPosition(false,true,TVector3(0,tmp_yoff,0));
	
	  obj.FillHistogram(dirname,"jY_Scan",100,-0.5,0.51,tmp_yoff,
			    2000,0,4000,s_hit.GetDoppler(beta,tmp_pos,TVector3(0,0,zoff)));
	}

	for(int i=1;i<101;i++) {
	  double tmp_zoff = -0.5 + 0.01*i;
	  TVector3 tmp_pos = j_hit.GetPosition(false,true,TVector3(0,0,tmp_zoff));
	
	  obj.FillHistogram(dirname,"jZ_Scan",100,-0.5,0.51,tmp_zoff,
			    2000,0,4000,s_hit.GetDoppler(beta,tmp_pos,TVector3(0,0,zoff)));
	}
	*/
      }//end sega hit loop
    }//end janus hit loop
  }
  
}

int gates_loaded=0;
GCutG* time_gate;
std::vector<GCutG*> janus_gates;

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
        time_gate = gate;
	gates_loaded++;
        std::cout << "Time Gate: " << gate->GetName() << std::endl;
      }
      else if(!tag.compare("D0") || !tag.compare("D1")) {
        janus_gates.push_back(gate);
	gates_loaded++;
        std::cout << "Janus Gate: " << tag << " " << gate->GetName() << std::endl;
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
    MakeJanus(obj,*janus,janus_gates);
  }

  
  if(sega && janus) {

    MakeSeGAJanus(obj,*sega,*janus);
    MakeGatedSeGAJanus(obj,*sega,*janus,janus_gates,time_gate);
    
  }
  
  
}

