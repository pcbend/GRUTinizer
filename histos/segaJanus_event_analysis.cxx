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
static long prev_ring_ts [48] = {};
void MakeJanus(TRuntimeObjects& obj, TJanusDDAS& janus, GCutG* charge_gate, GCutG* janus_gate) {
  
  std::string dirname = "Janus_Ungated";

  if(prev_ts > 0) {
    obj.FillHistogram(dirname,"Event_TDiff",100000,0,10000000000,janus.Timestamp()-prev_ts);
  }
  prev_ts = janus.Timestamp();
  
  std::vector<TJanusDDASHit> rings;
  std::vector<TJanusDDASHit> sectors;
  int chanNum;
  int ringNum;
  for(auto& chan : janus.GetAllChannels()) {

    chanNum = chan.GetFrontChannel();
    if(chan.IsRing()) {
      rings.push_back(chan);
      if(chan.GetDetnum() == 1) {
        if(chanNum >= 112) {
          ringNum = chanNum - 16;
        }
        else if(chanNum >= 104) {
          ringNum = chanNum;
        }
        else if(chanNum >= 96) {
          ringNum = chanNum + 16;
        }
      }
      else if(chan.GetDetnum() == 0) {
        if(chanNum >= 48) {
          ringNum = chanNum - 16;
        }
        else if(chanNum >= 40) {
          ringNum = chanNum;
        }
        else if(chanNum >= 32) {
          ringNum = chanNum + 16;
        }
      }
      else {
        ringNum = chanNum;
      }
    }
    else {
      sectors.push_back(chan);
      ringNum = chanNum;
    }

    if(ringNum >= 96) {
      obj.FillHistogram(dirname,"Charge_Summary_D1Rings",
                        26,0,26,ringNum-95,
                        3500,4000,35000,chan.Charge());
    }

    if((ringNum >= 32) && (ringNum < 56)) {
      obj.FillHistogram(dirname,"Charge_Summary_D0Rings",
                        26,0,26,ringNum-31,
                        3500,0,35000,chan.Charge());
    }

    if(janus_gate->IsInside(ringNum-95,chan.Charge())) {
      if(ringNum >= 96) {
        obj.FillHistogram(dirname,"Charge_Summary_D1Rings_Gated",
                          26,0,26,ringNum-95,
                          3500,4000,35000,chan.Charge());
      }
    }

    if(janus_gate->IsInside(ringNum-31,chan.Charge())) {
      if((ringNum >= 32) && (ringNum < 56)) {
        obj.FillHistogram(dirname,"Charge_Summary_D0Rings_Gated",
                          26,0,26,ringNum-31,
                          3500,0,35000,chan.Charge());
      }
    }

    if(!charge_gate->IsInside(ringNum,chan.Charge())) {
      continue;
    }
    
    obj.FillHistogram(dirname,"Charge_Summary_AllChans",
                      128,0,128,ringNum,
                      3500,0,35000,chan.Charge());

  }//end all channel loop

  for(auto& hit : janus.GetSingleHits(0)) {
    if(hit.GetDetnum() == 1) {
      obj.FillHistogram(dirname,"Charge_Summary_Paired_Events_D1",
                        26,0,26,hit.GetRing(),
                        3500,4000,35000,hit.GetBackHit().Charge());
      if(janus_gate->IsInside(hit.GetRing(),hit.GetBackHit().Charge())) {
        obj.FillHistogram(dirname,"Charge_Summary_Paired_Events_D1_Gated",
                          26,0,26,hit.GetRing(),
                          3500,4000,35000,hit.GetBackHit().Charge());
      }
    }
    else if(hit.GetDetnum() == 0) {
      obj.FillHistogram(dirname,"Charge_Summary_Paired_Events_D0",
                        26,0,26,hit.GetRing(),
                        3500,0,35000,hit.GetBackHit().Charge());
      if(janus_gate->IsInside(hit.GetRing(),hit.GetBackHit().Charge())) {
        obj.FillHistogram(dirname,"Charge_Summary_Paired_Events_D0_Gated",
                          26,0,26,hit.GetRing(),
                          3500,0,35000,hit.GetBackHit().Charge());
      }
    }
  }

  obj.FillHistogram(dirname,"Janus_Unused_Multiplicity",500,0,500,janus.GetUnusedChannels().size());
  obj.FillHistogram(dirname,"JANUS_Multiplicity",500,0,500,janus.GetChannels().size());
  obj.FillHistogram(dirname,"JANUS_All_Multiplicity",500,0,500,janus.GetAllChannels().size());
  obj.FillHistogram(dirname,"JANUS_Ring_Multiplicity",500,0,500,janus.GetRings().size());
  obj.FillHistogram(dirname,"JANUS_Sector_Multiplicity",500,0,500,janus.GetSectors().size());
  for(size_t i=0; i<janus.GetChannels().size(); i++) {
    auto &chan = janus.GetChannels().at(i);

    if(prev_chan_ts > 0) {
      obj.FillHistogram(dirname,"Chan_TDiff",100000,0,10000000000,chan.Timestamp()-prev_chan_ts);
    }
    prev_chan_ts = chan.Timestamp();
    
    if(chan.IsRing() && chan.GetDetnum() ==1) {
      int chanNum = chan.GetFrontChannel();
      int ringNum = 0;
      if(chanNum >= 112) {
        ringNum = chanNum - 111;
      }
      else if(chanNum >= 104) {
        ringNum = chanNum - 95;
      }
      else if(chanNum >= 96) {
        ringNum = chanNum - 79;
      }
      obj.FillHistogram(dirname,"RingEnergy_v_RingNum",
                        25,0,25,ringNum,
                        3000,0,30000,chan.Charge());
    }
      
  }//end janus chan loop

  for(auto& hit : janus.GetHits(0)) {
    if(hit.GetRing() == 1 && hit.GetDetnum() == 1) {
      auto tdiff = hit.GetBackHit().Timestamp() - hit.Timestamp();
   
      obj.FillHistogram(dirname,Form("FrontBack_Tdiff_Hit_sector%d",hit.GetSector()),
		        2750,-11000,11000,tdiff);
    }
    if(hit.GetSector() == 1 && hit.GetDetnum() == 1) {
      auto tdiff = hit.GetBackHit().Timestamp() - hit.Timestamp();

      obj.FillHistogram(dirname,Form("FrontBack_Tdiff_Hit_ring%d",hit.GetRing()),
                        2750,-11000,11000,tdiff);
    }
  }

  for(size_t i=0; i<janus.GetUnusedSectors().size(); i++) {

    auto &chan = janus.GetUnusedSectors().at(i);

    if(chan.GetDetnum() == 1) {
      obj.FillHistogram(dirname,"SectorEnergy_v_SectorNum_UnusedChannels",
                        32,0,32,chan.GetSector(),
                        3000,0,30000,chan.Charge());
    }
  }
  for(size_t i=0; i<janus.GetUnusedRings().size(); i++) {

    auto &chan = janus.GetUnusedRings().at(i);

    if(chan.GetDetnum() == 1) {
      int chanNum = chan.GetFrontChannel();
      int ringNum = 0;
      if(chanNum >= 112) {
        ringNum = chanNum - 111;
      }
      else if(chanNum >= 104) {
        ringNum = chanNum - 95;
      }
      else if(chanNum >= 96) {
        ringNum = chanNum - 79;
      }
      obj.FillHistogram(dirname,"RingEnergy_v_RingNum_UnusedChannels",
                        25,0,25,ringNum,
                        3000,0,30000,chan.Charge());
      }
  }


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
    if(hit.GetRing() == 1 && hit.GetDetnum() == 1) {
      auto tdiff = hit.GetBackHit().Timestamp() - hit.Timestamp();
   
      obj.FillHistogram(dirname,Form("FrontBack_Tdiff_Hit_sector%d",hit.GetSector()),
		        2750,-11000,11000,tdiff);
    }
    if(hit.GetSector() == 1 && hit.GetDetnum() == 1) {
      auto tdiff = hit.GetBackHit().Timestamp() - hit.Timestamp();

      obj.FillHistogram(dirname,Form("FrontBack_Tdiff_Hit_ring%d",hit.GetRing()),
                        2750,-11000,11000,tdiff);
    }

      
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

  static TSRIM srimB("112Sn_in_196Pt");
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
  
  int particle_gamma_count = 0;
  int janus_gate_count = 0;
  TVector3 incBeam = TVector3(0.0,0.0,1.0);
  obj.FillHistogram(dirname,"Janus_Multiplicity",150,0,150,janus.GetHits(lvl).size());
  for(auto& j_hit : janus.GetSingleHits(0)) {

    obj.FillHistogram(dirname,"Charge_Summary_PID",
                      26,0,26,j_hit.GetRing(),
                      3500,4000,35000,j_hit.GetBackHit().Charge());

    if(j_hit.GetDetnum() != detNum || !janus_gate->IsInside(j_hit.GetRing(),j_hit.GetBackHit().Charge())) {
      continue;
    }

    obj.FillHistogram(dirname,"Charge_Summary_PID_Gated",
                      26,0,26,j_hit.GetRing(),
                      3500,4000,35000,j_hit.GetBackHit().Charge());

    janus_gate_count += 1;
    for(auto& s_hit : sega) {
      
      auto tdiff = (s_hit.Timestamp() - j_hit.Timestamp());
      double s_energy = s_hit.GetEnergy();

      if(time_gate) {
	if(!time_gate->IsInside(tdiff,s_energy)) {
	  continue;
	}
      }
      particle_gamma_count += 1;

    }//end sega hit loop
    
  }//end janus hit loop
  obj.FillHistogram(dirname,"Particle_Gamma_Multiplicity",500,0,500,particle_gamma_count);
  obj.FillHistogram(dirname,"Janus_Gate_Multiplicity",100,0,100,janus_gate_count);

  return;
}

int gates_loaded=0;
GCutG* time_gate=0;
std::vector<GCutG*> janus_gates;
GCutG* charge_gate=0;

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
      else if(!tag.compare("charge")) {
        charge_gate = gate;
        gates_loaded++;
        std::cout << "Charge Gate: " << name << std::endl;
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
  
  for (auto &gate : janus_gates) {
    if(janus) {
      MakeJanus(obj,*janus,charge_gate,gate);
    }
  }
  
  if(sega && janus) {
    for (auto &gate : janus_gates) {
      MakeGatedSeGAJanus(obj,*sega,*janus,gate,time_gate);
    }
  }

  return;
}

