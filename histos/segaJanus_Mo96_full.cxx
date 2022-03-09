#include "TRuntimeObjects.h"
#include "TJanusDDAS.h"
#include "TSega.h"
#include "GCutG.h"
#include "GValue.h"

double beam_mass;
double beam_Z;
double targ_mass;
double targ_Z;
double Ex;
double reac_en;

/////////Kinematics/////////
double Theta_CM_FP(double ThetaLAB, double Ep, bool sol2=false) {
  
  double tau = (beam_mass/targ_mass)/std::sqrt(1 - (Ex/Ep)*(1 + beam_mass/targ_mass));
  
  if(std::sin(ThetaLAB) > 1.0/tau) {
    ThetaLAB = std::asin(1.0/tau);

    if(ThetaLAB < 0) {
      ThetaLAB += TMath::Pi();
    }

    return std::asin(tau*std::sin(ThetaLAB)) + ThetaLAB;
  }

  if(!sol2) {
    return std::asin(tau*std::sin(ThetaLAB)) + ThetaLAB;
  }
  
  return std::asin(tau*std::sin(-ThetaLAB)) + ThetaLAB + TMath::Pi();

}

double Theta_CM_FR(double ThetaLAB, double Ep, bool sol2=false) {
  
  double tau = 1.0/std::sqrt(1 - (Ex/Ep)*(1 + beam_mass/targ_mass));
  
  if(std::sin(ThetaLAB) > 1.0/tau) {
    ThetaLAB = std::asin(1.0/tau);

    if(ThetaLAB < 0) {
      ThetaLAB += TMath::Pi();
    }

    return std::asin(tau*std::sin(ThetaLAB)) + ThetaLAB;
  }

  if(!sol2) {
    return TMath::Pi() - (std::asin(tau*std::sin(ThetaLAB)) + ThetaLAB);
  }
  
  return -std::asin(tau*std::sin(-ThetaLAB)) - ThetaLAB;

}

double Theta_LAB(double thetaCM, double Ep) {

  double tau = (beam_mass/targ_mass)/std::sqrt(1 - (Ex/Ep)*(1 + beam_mass/targ_mass));
  double tanTheta = std::sin(thetaCM)/(std::cos(thetaCM) + tau);

  if(tanTheta > 0) {
    return std::atan(tanTheta);
  }
  
  return std::atan(tanTheta) + TMath::Pi();
  
}

double Theta_LAB_Max(double Ep) {

  double tau = (beam_mass/targ_mass)/std::sqrt(1 - (Ex/Ep)*(1 + beam_mass/targ_mass));

  if(tau < 1.0) {
    return TMath::Pi();
  }
  
  return std::asin(1.0/tau);
  
}

double Recoil_Theta_LAB(double thetaCM, double Ep) {

  double tau = 1.0/std::sqrt(1 - (Ex/Ep)*(1 + beam_mass/targ_mass));
  double tanTheta = std::sin(TMath::Pi() - thetaCM)/(std::cos(TMath::Pi() - thetaCM) + tau);
  
  return std::atan(tanTheta);
  
}

double Recoil_Theta_LAB_Max(double Ep) {

  double tau = 1.0/std::sqrt(1 - (Ex/Ep)*(1 + beam_mass/targ_mass));
  
  return std::asin(1.0/tau);
    
}

double KE_LAB(double thetaCM, double Ep) {

  double tau = (beam_mass/targ_mass)/std::sqrt(1 - (Ex/Ep)*(1 + beam_mass/targ_mass));

  double term1 = std::pow(targ_mass/(beam_mass + targ_mass),2);
  double term2 = 1 + tau*tau + 2*tau*std::cos(thetaCM);
  double term3 = Ep - Ex*(1 + beam_mass/targ_mass);
  
  return term1*term2*term3;
}

double Recoil_KE_LAB(double thetaCM, double Ep) {

  double tau = 1.0/std::sqrt(1 - (Ex/Ep)*(1 + beam_mass/targ_mass));

  double term1 = beam_mass*targ_mass/std::pow(beam_mass + targ_mass,2);
  double term2 = 1 + tau*tau + 2*tau*std::cos(TMath::Pi() - thetaCM);
  double term3 = Ep - Ex*(1 + beam_mass/targ_mass);
  
  return term1*term2*term3;
}

/*
double RuthCM(double thetaCM, double Ep) {

  double factor = 1.29596*TMath::Power(beam_Z*targ_Z,2.0);

  double Ecm = Ep/(1.0 + (beam_mass/targ_mass));
  double Esym2 = TMath::Power(Ecm,1.5)*TMath::Sqrt(Ecm-Ex);
  
  return factor/(Esym2*TMath::Power(TMath::Sin(thetaCM/2.0),4));
}

double RuthLAB(double thetaLAB, double Ep, bool sol2=false) {

  double thetaCM = Theta_CM_FP(thetaLAB,Ep,sol2);
 
  double num = TMath::Power(TMath::Sin(thetaCM)/TMath::Sin(thetaLAB),2.0);
  double denom = TMath::Abs(TMath::Cos(thetaCM-thetaLAB));
  double jacobian = num/denom;

  return RuthCM(thetaCM,Ep)*jacobian;
  
}

double Beta_LAB(double thetaCM, double Ep) {

  double energy = KE_LAB(thetaCM,Ep);
  double gam = energy/beam_mass + 1.0;

  return TMath::Sqrt(1.0 - 1.0/(gam*gam));
  
}

double Recoil_Beta_LAB(double thetaCM, double Ep) {

  double energy = Recoil_KE_LAB(thetaCM,Ep);
  double gam = energy/targ_mass + 1.0;

  return TMath::Sqrt(1.0 - 1.0/(gam*gam));
  
}
*/

double Beta(double energy, double mass) {

  double gam = energy/mass + 1.0;
  
  return TMath::Sqrt(1.0 - 1.0/(gam*gam));
  
}
////////////////////////////

std::string Prefix(int level) {

  switch(level) {

  case 0:
    return "Total_";

  case 1:
    return "Primary_";

  case 2:
    return "Secondary_";

  case 3:
    return "Tertiary_";

  }

  return "_";
  
}

void BasicJanusSpectra(TRuntimeObjects& obj, TJanusDDASHit& hit, std::string dirname, std::string h_suf) {

  const TVector3 pos = hit.GetPosition(false);
  
  obj.FillHistogram(dirname,Form("%sSectorCharge_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),3000,0,30000,hit.Charge());
  obj.FillHistogram(dirname,Form("%sRingCharge_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),3000,0,30000,hit.GetBackHit().Charge());
  
  obj.FillHistogram(dirname,Form("%sCharge_Summary_Hit",h_suf.c_str()),
                    128,0,128,hit.GetFrontChannel(),
                    3000,0,30000,hit.Charge());
  obj.FillHistogram(dirname,Form("%sCharge_Summary_Hit",h_suf.c_str()),
                    128,0,128,hit.GetBackChannel(),
                    3000,0,30000,hit.GetBackHit().Charge());

  obj.FillHistogram(dirname,Form("%sRing_det%d",h_suf.c_str(),hit.GetDetnum()),26,0,26,hit.GetRing());
  obj.FillHistogram(dirname,Form("%sSector_det%d",h_suf.c_str(),hit.GetDetnum()),34,0,34,hit.GetSector());

  obj.FillHistogram(dirname,Form("%sSectorCharge_v_Sector_det%d",h_suf.c_str(),hit.GetDetnum()),
  		    34,0,34,hit.GetSector(),3000,0,30000,hit.Charge());

  obj.FillHistogram(dirname,Form("%sSectorCharge_v_Ring_det%d",h_suf.c_str(),hit.GetDetnum()),
		    26,0,26,hit.GetRing(),3000,0,30000,hit.Charge());

  if(hit.GetRing()<=6) {
    obj.FillHistogram(dirname,Form("%sSectorCharge_v_Ring_det%d_Expt1",h_suf.c_str(),hit.GetDetnum()),
		      8,0,8,hit.GetRing(),3000,0,30000,hit.Charge());
  }
  else if((hit.GetRing()<=12) && (hit.GetRing()>6)) {
    obj.FillHistogram(dirname,Form("%sSectorCharge_v_Ring_det%d_Expt2",h_suf.c_str(),hit.GetDetnum()),
		      8,6,14,hit.GetRing(),3000,0,30000,hit.Charge());
  }
  else if((hit.GetRing()<=18) && (hit.GetRing()>12)) {
    obj.FillHistogram(dirname,Form("%sSectorCharge_v_Ring_det%d_Expt3",h_suf.c_str(),hit.GetDetnum()),
		      8,12,20,hit.GetRing(),3000,0,30000,hit.Charge());
  }
  else if(hit.GetRing()>18) {
    obj.FillHistogram(dirname,Form("%sSectorCharge_v_Ring_det%d_Expt4",h_suf.c_str(),hit.GetDetnum()),
		      8,18,26,hit.GetRing(),3000,0,30000,hit.Charge());
  }

  obj.FillHistogram(dirname,Form("%sRingCharge_v_Sector_det%d",h_suf.c_str(),hit.GetDetnum()),
		    34,0,34,hit.GetSector(),3000,0,30000,hit.GetBackHit().Charge());
  
  obj.FillHistogram(dirname,Form("%sRingCharge_v_Ring_det%d",h_suf.c_str(),hit.GetDetnum()),
		    26,0,26,hit.GetRing(),3000,0,30000,hit.GetBackHit().Charge());

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

  auto tdiff = hit.GetBackHit().Timestamp() - hit.Timestamp();
   
  obj.FillHistogram(dirname,Form("%sFrontBack_Tdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    2750,-11000,11000,tdiff);

  auto cdiff = hit.GetBackHit().Charge() - hit.Charge();
  
  obj.FillHistogram(dirname,Form("%sCdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),2000,0,20000,cdiff);

  obj.FillHistogram(dirname,Form("%sCdiff_v_Tdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    2500,-10000,10000,tdiff,
		    1000,0,30000,cdiff);

  obj.FillHistogram(dirname,Form("%sRingCharge_v_Tdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    2500,-10000,10000, tdiff,
		    1000,0,30000,hit.GetBackHit().Charge());

  obj.FillHistogram(dirname,Form("%sRingCharge_v_Cdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    5000,0,20000,cdiff,
		    1000,0,30000,hit.GetBackHit().Charge());

  obj.FillHistogram(dirname,Form("%sSectorCharge_v_Tdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    2500,-10000,10000,tdiff,
		    1000,0,30000,hit.Charge());

  obj.FillHistogram(dirname,Form("%sSectorCharge_v_Cdiff_Hit_det%d",h_suf.c_str(),hit.GetDetnum()),
		    5000,0,20000,cdiff,
		    1000,0,30000,hit.Charge());

  return;
 
}

void LeveledJanusSpectra(TRuntimeObjects& obj, TJanusDDAS& janus, int lvl) {

  std::string dirname = Prefix(lvl) + "Janus_Ungated";
  
  for(size_t i=0; i<janus.GetUnusedChannels(lvl).size(); i++) {
    
    auto &chan = janus.GetUnusedChannels(lvl).at(i);
    
    obj.FillHistogram(dirname,"Charge_Summary_UnusedChans",
		      128,0,128,chan.GetFrontChannel(),
		      3500,0,35000,chan.Charge());
    
    for(size_t j=i+1;j<janus.GetUnusedChannels(lvl).size();j++) {
      
      auto &chan1 = janus.GetUnusedChannels(lvl).at(j);
      
      if(chan1.GetDetnum() == chan.GetDetnum()) {
        obj.FillHistogram(dirname,Form("Tdiff_UnusedChans_det%d",chan.GetDetnum()),
	  		  2500,-11000,11000,chan.Timestamp()-chan1.Timestamp());
      }
        
    }//end second janus chan loop 
  }//end janus chan loop

  for(size_t i=0;i<janus.GetUnusedSectors(lvl).size();i++) {
    for(size_t j=0;j<janus.GetUnusedRings(lvl).size();j++) {
      
      auto &sect = janus.GetUnusedSectors(lvl).at(i);
      auto &ring = janus.GetUnusedRings(lvl).at(j);

      auto tdiff = ring.Timestamp() - sect.Timestamp();

      if(ring.GetDetnum() == sect.GetDetnum()) {

	obj.FillHistogram(dirname,Form("SectorCharge_v_TDiff_UnusedChans_det%d",sect.GetDetnum()),
		          2500,-11000,11000,tdiff,
		          2000,0,30000,sect.Charge());
	
        obj.FillHistogram(dirname,Form("SectorCharge_v_RingCharge_UnusedChans_det%d",sect.GetDetnum()),
		          2000,0,30000,ring.Charge(),
		          2000,0,30000,sect.Charge());

	obj.FillHistogram(dirname,Form("Sector_Ring_TDiff_UnusedChans_det%d",sect.GetDetnum()),
			  2500,-11000,11000,tdiff);
      }
    }
  }

  obj.FillHistogram(dirname,"All_HitMult_det1",50,0,50,janus.GetSpecificHits(1,lvl,0).size());   
  obj.FillHistogram(dirname,"All_HitMult_det0",50,0,50,janus.GetSpecificHits(0,lvl,0).size());
  for(auto& hit : janus.GetHits(lvl)) {
    BasicJanusSpectra(obj,hit,dirname,"All_"); 
  }

  obj.FillHistogram(dirname,"Single_HitMult_det1",50,0,50,janus.GetSpecificHits(1,lvl,1).size());   
  obj.FillHistogram(dirname,"Single_HitMult_det0",50,0,50,janus.GetSpecificHits(0,lvl,1).size());
  for(auto& hit : janus.GetSingleHits(lvl)) {
    BasicJanusSpectra(obj,hit,dirname,"Single_"); 
  }

  obj.FillHistogram(dirname,"Addback_HitMult_det1",50,0,50,janus.GetSpecificHits(1,lvl,3).size());   
  obj.FillHistogram(dirname,"Addback_HitMult_det0",50,0,50,janus.GetSpecificHits(0,lvl,3).size());
  for(auto& hit : janus.GetAddbackHits(lvl)) {
    BasicJanusSpectra(obj,hit,dirname,"Addback_"); 
  }
  
  obj.FillHistogram(dirname,"TwoHit_HitMult_det1",50,0,50,janus.GetSpecificHits(1,lvl,2).size());   
  obj.FillHistogram(dirname,"TwoHit_HitMult_det0",50,0,50,janus.GetSpecificHits(0,lvl,2).size());
  for(auto& hit : janus.GetDoubleHits(lvl)) {
    BasicJanusSpectra(obj,hit,dirname,"TwoHit_"); 
  }
  return;

}

static long prev_ts = 0;
static long prev_chan_ts = 0;
void MakeJanus(TRuntimeObjects& obj, TJanusDDAS& janus) {
  
  std::string dirname = "Janus_Ungated";

  if(prev_ts > 0) {
    obj.FillHistogram(dirname,"Event_TDiff",100000,0,10000000000,janus.Timestamp()-prev_ts);
  }
  prev_ts = janus.Timestamp();
  
  std::vector<TJanusDDASHit> rings;
  std::vector<TJanusDDASHit> sectors;
  for(auto& chan : janus.GetAllChannels()) {

    obj.FillHistogram(dirname,"Charge_Summary_AllChans",
		      128,0,128,chan.GetFrontChannel(),
		      3500,0,35000,chan.Charge());

    if(chan.IsRing()) {
      rings.push_back(chan);
    }
    else {
      sectors.push_back(chan);
    }

  }//end all channel loop

  std::string dir_tmp = "Janus_Ungated_Rings";
  std::string dir_tmp1 = "Janus_Ungated_Sectors";
  for(auto& sect : sectors) {
    for(auto& ring : rings) {

      if(ring.GetDetnum() == sect.GetDetnum()) {
	
        obj.FillHistogram(dirname,Form("SectorCharge_v_RingCharge_AllChans_det%d",sect.GetDetnum()),
		          2000,0,36000,ring.Charge(),
		          2000,0,36000,sect.Charge());

	
	if(ring.GetSector() > 1) {

	  obj.FillHistogram(dirname,Form("SectorCharge_v_RingCharge_AllChans_NoR01_det%d",sect.GetDetnum()),
		          2000,0,36000,ring.Charge(),
		          2000,0,36000,sect.Charge());
	}
	
	obj.FillHistogram(dir_tmp,Form("SectorCharge_v_RingCharge_AllChans_det%d_R%02d",
				       sect.GetDetnum(),ring.GetSector()),
		          2000,0,36000,ring.Charge(),
		          2000,0,36000,sect.Charge());

	obj.FillHistogram(dir_tmp1,Form("SectorCharge_v_RingCharge_AllChans_det%d_S%02d",
				       sect.GetDetnum(),sect.GetSector()),
		          2000,0,36000,ring.Charge(),
		          2000,0,36000,sect.Charge());
      }
    } //end all ring loop
  } //end all sector loop
  
  for(size_t i=0; i<janus.GetChannels().size(); i++) {
    auto &chan = janus.GetChannels().at(i);

    if(prev_chan_ts > 0) {
      obj.FillHistogram(dirname,"Chan_TDiff",100000,0,10000000000,chan.Timestamp()-prev_chan_ts);
    }
    prev_chan_ts = chan.Timestamp();
    
    obj.FillHistogram(dirname,"Charge_Summary_UsableChans",
		      128,0,128,chan.GetFrontChannel(),
		      3500,0,35000,chan.Charge());

    for(size_t j=i+1;j<janus.GetChannels().size();j++) {
      auto &chan1 = janus.GetChannels().at(j);

      if(chan1.GetDetnum() == chan.GetDetnum()) {
        obj.FillHistogram(dirname,Form("Tdiff_UsableChans_det%d",chan.GetDetnum()),
	  		  2500,-11000,11000,chan.Timestamp()-chan1.Timestamp());
      }
      
    }//end second janus chan loop 
  }//end janus chan loop

  for(auto& sect : janus.GetSectors()) {
    for(auto& ring : janus.GetRings()) {
  
      auto tdiff = ring.Timestamp() - sect.Timestamp();
      
      if(ring.GetDetnum() == sect.GetDetnum()) {

	obj.FillHistogram(dirname,Form("SectorCharge_v_TDiff_UsableChans_det%d",sect.GetDetnum()),
		          2500,-11000,11000,tdiff,
		          2000,0,30000,sect.Charge());
	
        obj.FillHistogram(dirname,Form("SectorCharge_v_RingCharge_UsableChans_det%d",sect.GetDetnum()),
		          2000,0,30000,ring.Charge(),
		          2000,0,30000,sect.Charge());

	obj.FillHistogram(dirname,Form("Sector_Ring_TDiff_UsableChans_det%d",sect.GetDetnum()),
			  6000,-11000,11000,tdiff);

	
	if(ring.GetSector() > 1) {
	  obj.FillHistogram(dirname,Form("SectorCharge_v_RingCharge_UsableChans_NoR01_det%d",
					 sect.GetDetnum()),
			    2000,0,30000,ring.Charge(),
			    2000,0,30000,sect.Charge());

	  obj.FillHistogram(dirname,Form("SectorCharge_v_TDiff_UsableChans_NoR01_det%d",sect.GetDetnum()),
		          2500,-11000,11000,tdiff,
		          2000,0,30000,sect.Charge());
	}
	
      } 
    }//end ring loop
  }//end sector loop
  
  LeveledJanusSpectra(obj,janus,1);

  return;
  
}

void MakeGatedJanus(TRuntimeObjects& obj, TJanusDDAS& janus, GCutG* gate) {

  if(!gate)
    {return;}

  int lvl = 1;
  std::string dirname = Prefix(lvl) + Form("Janus_%s",gate->GetName());

  int detNum = 1;
  std::string tag = gate->GetTag();
  if(!tag.compare("D0")) {
    detNum = 0;
  }

  int mult = 0;
  for(auto& hit : janus.GetHits(lvl)) {
      
    if(hit.GetDetnum() != detNum || !gate->IsInside(hit.GetRing(),hit.Charge())) {
      continue;
    }

    mult++;
    BasicJanusSpectra(obj,hit,dirname,"All_");

  }
  obj.FillHistogram(dirname,Form("All_GateMult_det%d",detNum),20,0,20,mult);

  int multS = 0;
  for(auto& hit : janus.GetSingleHits(lvl)) {
      
    if(hit.GetDetnum() != detNum || !gate->IsInside(hit.GetRing(),hit.Charge())) {
      continue;
    }

    multS++;
    BasicJanusSpectra(obj,hit,dirname,"Single_");

  }
  obj.FillHistogram(dirname,Form("Single_GateMult_det%d",detNum),20,0,20,multS);

  int multA = 0;
  for(auto& hit : janus.GetAddbackHits(lvl)) {
      
    if(hit.GetDetnum() != detNum || !gate->IsInside(hit.GetRing(),hit.Charge())) {
      continue;
    }

    multA++;
    BasicJanusSpectra(obj,hit,dirname,"Addback_");

  }
  obj.FillHistogram(dirname,Form("Addback_GateMult_det%d",detNum),20,0,20,multA);

  return;

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

  return;
}

void MakeSeGAJanus(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus) {

  int lvl = 1;
  std::string dirname = Prefix(lvl) + "SeGAJanus";
 
  obj.FillHistogram(dirname,"SegaMult",20,0,20,sega.Size());
  
  obj.FillHistogram(dirname,"All_jHitMult_det1",40,0,40,janus.GetSpecificHits(1,lvl,1).size());
  obj.FillHistogram(dirname,"All_jHitMult_det0",40,0,40,janus.GetSpecificHits(1,lvl,0).size());

  obj.FillHistogram(dirname,"Single_jHitMult_det1",50,0,50,janus.GetSpecificHits(1,lvl,1).size());   
  obj.FillHistogram(dirname,"Single_jHitMult_det0",50,0,50,janus.GetSpecificHits(0,lvl,1).size());

  obj.FillHistogram(dirname,"Addback_jHitMult_det1",50,0,50,janus.GetSpecificHits(1,lvl,3).size());   
  obj.FillHistogram(dirname,"Addback_jHitMult_det0",50,0,50,janus.GetSpecificHits(0,lvl,3).size());
  
  for(auto& j_hit : janus.GetHits(lvl)) {

    BasicJanusSpectra(obj,j_hit,dirname,"All_");
     
    for(auto& s_hit : sega) {

      double s_energy = s_hit.GetEnergy();
      
      obj.FillHistogram(dirname,"Core_Energy",5000,0,5000,s_energy);
      obj.FillHistogram(dirname,"Core_Energy_Summary",18,0,18,s_hit.GetDetnum(),
			25000,0,5000,s_energy);

      auto tdiff = (s_hit.Timestamp() - j_hit.Timestamp());
      obj.FillHistogram(dirname,Form("CoreEnergy_v_tdiff_det%d",j_hit.GetDetnum()),
			1000,-1000,3000,tdiff,
			2500,0,5000,s_energy);
       
    }//end sega hit loop
  }//end janus hit loop

  return;
}

void MakeGatedSeGAJanus(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus, GCutG* janus_gate,
			GCutG* time_gate) {

  static TSRIM srimB("96Mo_in_196Pt");
  static TSRIM srimT("196Pt_in_196Pt");
  
  if(!janus_gate){
    return;
  }

  int lvl = 1;
  std::string name = janus_gate->GetName();
  std::string dirname = Prefix(lvl) + Form("SeGAJanus_%s",name.c_str());
  
  if(time_gate) {
    dirname += Form("_%s",time_gate->GetName());
  }

  bool recoil = false;
  if((!name.compare("pt196")) || (!name.compare("pb208"))) {
    recoil = true;
  }

  int detNum = 1;
  std::string tag = janus_gate->GetTag();
  if(!tag.compare("D0")) {
    detNum = 0;
  }
  
  TVector3 incBeam = TVector3(0.0,0.0,1.0);
  for(auto& j_hit : janus.GetHits(lvl)) {

    if(j_hit.GetDetnum() != detNum || !janus_gate->IsInside(j_hit.GetRing(),j_hit.Charge())) {
      continue;
    }
     
    //BasicJanusSpectra(obj,j_hit,Prefix(lvl) + Form("SeGAJanus_%s_JanSpec",name.c_str()),"All_");
    
    TVector3 j_pos = j_hit.GetPosition(false);
    double beta;
    double recon_beta;
    TVector3 recon_pos(0,0,1);
    double target_width = GValue::Value("TARGET_WIDTH");
    if(recoil) {

      double theta_CM = Theta_CM_FR(j_pos.Theta(),reac_en);
      
      double En = Recoil_KE_LAB(theta_CM,reac_en);
      double distance = target_width/std::abs(std::cos(j_pos.Theta()));
      En += 0.001*srimT.GetEnergyChange(1000.*En,distance);
      
      beta = Beta(En,targ_mass);

      recon_pos.SetTheta(Theta_LAB(theta_CM,reac_en));
      
      double Recon_En = KE_LAB(theta_CM,reac_en);
      double recon_distance = target_width/std::abs(std::cos(recon_pos.Theta()));
      Recon_En += 0.001*srimB.GetEnergyChange(1000.*Recon_En,recon_distance);
      
      recon_beta = Beta(Recon_En,beam_mass);  
	
    }
    else {
      
      double theta_CM = Theta_CM_FP(j_pos.Theta(),reac_en);
      
      double En = KE_LAB(theta_CM,reac_en);
      double distance = target_width/std::abs(std::cos(j_pos.Theta()));
      En += 0.001*srimB.GetEnergyChange(1000.*En,distance);
      
      beta = Beta(En,beam_mass);

      recon_pos.SetTheta(Recoil_Theta_LAB(theta_CM,reac_en));
      
      double Recon_En = Recoil_KE_LAB(theta_CM,reac_en);
      double recon_distance = target_width/std::abs(std::cos(recon_pos.Theta()));
      Recon_En += 0.001*srimT.GetEnergyChange(1000.*Recon_En,recon_distance);
      
      recon_beta = Beta(Recon_En,targ_mass);
	
    }
    recon_pos.SetPhi(j_pos.Phi() + TMath::Pi());
    
    for(auto& s_hit : sega) {
      
      auto tdiff = (s_hit.Timestamp() - j_hit.Timestamp());
      double s_energy = s_hit.GetEnergy();

      if(time_gate) {
	if(!time_gate->IsInside(tdiff,s_energy)) {
	  continue;
	}
      }

      TVector3 s_pos = s_hit.GetPosition(true);

      obj.FillHistogram(dirname,Form("CoreEnergy_v_tdiff_jdet%d",detNum),
			1000,-1000,3000,tdiff,
			2500,0,5000,s_energy);

      obj.FillHistogram(dirname,Form("ParticleCharge_v_CoreEnergy_jdet%d",detNum),
			2000,0,5000,s_energy,
			1000,0,30000,j_hit.Charge());

      double dop_en = s_hit.GetDoppler(beta,j_pos);

      obj.FillHistogram(dirname,Form("ParticleCharge_v_DopplerEnergy_jdet%d",detNum),
			2000,0,5000,dop_en,
			1000,0,30000,j_hit.Charge());

      obj.FillHistogram(dirname,"Core_Energy",4000,0,4000,s_energy);
      obj.FillHistogram(dirname,Form("Core_Energy_jDet%d",detNum),
			4000,0,4000,s_energy);
	
      obj.FillHistogram(dirname,"Core_Energy_Summary",18,0,18,s_hit.GetDetnum(),
			25000,0,5000,s_energy);
      
      obj.FillHistogram(dirname,"Doppler_Energy",4000,0,4000,dop_en);
      obj.FillHistogram(dirname,"Doppler_Energy_Summary",18,0,18,s_hit.GetDetnum(),4000,0,4000,dop_en);
      obj.FillHistogram(dirname,Form("Doppler_Energy_jDet%d",detNum),4000,0,4000,dop_en);

/*
      int ringNum = j_hit.GetRing();
      obj.FillHistogram(dirname,Form("Doppler_Energy_Ring%d",ringNum),4000,0,4000,dop_en);
      if(detNum == 0) {
        if(ringNum <= 8) {
          obj.FillHistogram(dirname,"Doppler_Energy_Expt1",4000,0,4000,dop_en);
        }
        else if((ringNum > 8) && (ringNum <= 16)) {
          obj.FillHistogram(dirname,"Doppler_Energy_Expt2",4000,0,4000,dop_en);
        }
        else if(ringNum > 16) {
          obj.FillHistogram(dirname,"Doppler_Energy_Expt3",4000,0,4000,dop_en);
        }
      }
*/
      obj.FillHistogram(dirname,Form("Doppler_Energy_v_tdiff_jdet%d",detNum),
			1000,-1000,3000,tdiff,
			2500,0,5000,dop_en);

      obj.FillHistogram(dirname,Form("Beta_v_Ring_jdet%d",detNum),26,0,26,j_hit.GetRing(),100,0,0.1,beta);

      double angle = j_pos.Angle(s_pos)*TMath::RadToDeg();

      obj.FillHistogram(dirname,"Theta_Correlation",4000,0,4000,s_energy,180,0,180,angle);
      obj.FillHistogram(dirname,"Theta_Correction",4000,0,4000,dop_en,180,0,180,angle);

      TVector3 reacPlane = j_pos.Cross(incBeam);
      TVector3 detPlane = s_pos.Cross(incBeam);

      double reac_phi = reacPlane.Phi();
      double det_phi = detPlane.Phi();

      double planeAng = reac_phi - det_phi;
      if(planeAng < 0) {
	planeAng += TMath::TwoPi();
      }

      double thing = 65*TMath::Pi()/32.0;
      obj.FillHistogram(dirname,"Phi_Correlation",4000,0,4000,s_energy,32,0,thing,planeAng);
      obj.FillHistogram(dirname,"Phi_Correction",4000,0,4000,dop_en,32,0,thing,planeAng);

      /*
      if(!recoil) {
	for(int i=1;i<101;i++) {
	  double dE = 100 - i;
	  double tmp_beta = Beta_LAB(theta_CM,dE);
	  obj.FillHistogram(dirname,"dE_Scan",100,0,100,dE,
			    2000,0,4000,s_hit.GetDoppler(tmp_beta,j_pos));
	}
      }
      */
      
      for(int i=1;i<101;i++) {
	double tmp_xoff = -5.0 + 0.1*i;
	obj.FillHistogram(dirname,"sX_Scan",100,-5.0,5.1,tmp_xoff,
			  2000,0,4000,s_hit.GetDoppler(beta,j_pos,TVector3(tmp_xoff,0,0)));
      }

      for(int i=1;i<101;i++) {
	double tmp_yoff = -5.0 + 0.1*i;
	obj.FillHistogram(dirname,"sY_Scan",100,-5.0,5.1,tmp_yoff,
			  2000,0,4000,s_hit.GetDoppler(beta,j_pos,TVector3(0,tmp_yoff,0)));
      }

      for(int i=1;i<101;i++) {
	double tmp_zoff = -5.0 + 0.1*i;
	obj.FillHistogram(dirname,"sZ_Scan",100,-5.0,5.1,tmp_zoff,
			  2000,0,4000,s_hit.GetDoppler(beta,j_pos,TVector3(0,0,tmp_zoff)));
      }

      for(int i=1;i<101;i++) {
	double tmp_xoff = -0.5 + 0.01*i;
	TVector3 tmp_pos = j_hit.GetPosition(false,true);
	
	obj.FillHistogram(dirname,"jX_Scan",100,-0.5,0.51,tmp_xoff,
			  2000,0,4000,s_hit.GetDoppler(beta,tmp_pos));
      }

      for(int i=1;i<101;i++) {
	double tmp_yoff = -0.5 + 0.01*i;
	TVector3 tmp_pos = j_hit.GetPosition(false,true);
	
	obj.FillHistogram(dirname,"jY_Scan",100,-0.5,0.51,tmp_yoff,
			  2000,0,4000,s_hit.GetDoppler(beta,tmp_pos));
      }

      for(int i=1;i<101;i++) {
	double tmp_zoff = -0.5 + 0.01*i;
	TVector3 tmp_pos = j_hit.GetPosition(false,true);
	
	obj.FillHistogram(dirname,"jZ_Scan",100,-0.5,0.51,tmp_zoff,
			  2000,0,4000,s_hit.GetDoppler(beta,tmp_pos));	
      }

      obj.FillHistogram(dirname,Form("ReconBeta_v_Ring_jdet%d",detNum),26,0,26,j_hit.GetRing(),
			100,0,0.1,recon_beta);
      
      double recon_en = s_hit.GetDoppler(recon_beta,recon_pos);
      obj.FillHistogram(dirname,"Recon_Energy",4000,0,4000,recon_en);
      obj.FillHistogram(dirname,"Recon_Energy_Summary",18,0,18,s_hit.GetDetnum(),4000,0,4000,recon_en);
      obj.FillHistogram(dirname,Form("Recon_Energy_jDet%d",detNum),4000,0,4000,recon_en);

/*
      obj.FillHistogram(dirname,Form("Recon_Energy_Ring%d",ringNum),4000,0,4000,recon_en);
      if(detNum == 0) {
        if(ringNum <= 8) {
          obj.FillHistogram(dirname,"Recon_Energy_Expt1",4000,0,4000,recon_en);
        }
        else if((ringNum > 8) && (ringNum <= 16)) {
          obj.FillHistogram(dirname,"Recon_Energy_Expt2",4000,0,4000,recon_en);
        }
        else if(ringNum > 16) {
          obj.FillHistogram(dirname,"Recon_Energy_Expt3",4000,0,4000,recon_en);
        }
      }
*/

      obj.FillHistogram(dirname,Form("ParticleCharge_v_ReconEnergy_jdet%d",detNum),
			2000,0,5000,recon_en,
			1000,0,30000,j_hit.Charge());

      double recon_angle = recon_pos.Angle(s_pos)*TMath::RadToDeg();
      obj.FillHistogram(dirname,"ReconTheta_Correlation",4000,0,4000,s_energy,180,0,180,recon_angle);
      obj.FillHistogram(dirname,"ReconTheta_Correction",4000,0,4000,recon_en,180,0,180,recon_angle);

      TVector3 reacPlane_rec = recon_pos.Cross(incBeam);
      double reac_phi_rec = reacPlane_rec.Phi();

      double planeAng_rec = reac_phi_rec - det_phi;
      if(planeAng_rec < 0) {
	planeAng_rec += TMath::TwoPi();
      }

      obj.FillHistogram(dirname,"ReconPhi_Correlation",4000,0,4000,s_energy,32,0,thing,planeAng_rec);
      obj.FillHistogram(dirname,"ReconPhi_Correction",4000,0,4000,recon_en,32,0,thing,planeAng_rec);
      
    }//end sega hit loop
    
  }//end janus hit loop

  return;
}

int gates_loaded=0;
GCutG* time_gate=0;
std::vector<GCutG*> janus_gates;

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  
  TSega* sega = obj.GetDetector<TSega>();
  TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();
  beam_mass = GValue::Value("BEAM_MASS");
  beam_Z = GValue::Value("BEAM_Z");
  targ_mass = GValue::Value("TARGET_MASS");
  targ_Z = GValue::Value("TARGET_Z");
  reac_en = GValue::Value("REAC_EN");

  TList *gates = &(obj.GetGates());
  if(gates_loaded!=gates->GetSize()) {
    TIter iter(gates);
    while(TObject *obj = iter.Next()) {
      GCutG *gate = (GCutG*)obj;
      std::string tag = gate->GetTag();
      std::string name = gate->GetName();
      if(!tag.compare("time")) {
        time_gate = gate;
	gates_loaded++;
        std::cout << "Time Gate: " << name << std::endl;
      }
      else if(!tag.compare("D0") || !tag.compare("D1")) {
        janus_gates.push_back(gate);
	gates_loaded++;
        std::cout << "Janus Gate: " << tag << " " << name << std::endl;
      }
      else {
	std::cout << "Unknown Gate: Name = " << name << ", Tag = " << tag << std::endl;
      }
    }
  } 

  
/*
  if(sega) {
    MakeSega(obj,*sega);
  }
*/
  
  
  if(sega && janus) {

    //MakeSeGAJanus(obj,*sega,*janus);

    for(auto &gate : janus_gates) {
      MakeGatedSeGAJanus(obj,*sega,*janus,gate,time_gate);
    }
  }

  return;
}

