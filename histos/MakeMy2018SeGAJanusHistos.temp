#include "TRuntimeObjects.h"

#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

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

//static auto rb = std::make_shared<TNucleus>("87Rb");
//static auto pb = std::make_shared<TNucleus>("208Pb");
//static TSRIM srim("rb87_in_pb208.txt");
//double thickness = (0.75 / 11342.0) * 1e4; // (0.75 mg/cm^2) / (11342 mg/cm^3) * (10^4 um/cm)
//double energy_mid = srim.GetAdjustedEnergy(4.0*1e3, thickness*0.5)/1e3;

double get_beta(double betamax, double rb_angle_rad, bool energy_loss) {
  // Factors of 1e3 are because TNucleus and TReaction use MeV, while TSRIM uses keV.

  static auto rb = std::make_shared<TNucleus>("87Rb");
  static auto pb = std::make_shared<TNucleus>("208Pb");
  static TSRIM srim("rb87_in_pb208");

  double thickness = (0.75 / 11342.0) * 1e4; // (1 mg/cm^2) / (11342 mg/cm^3) * (10^4 um/cm)

  //double pre_collision_energy_MeV = rb->GetEnergyFromBeta(0.09237);
  double pre_energy_MeV = rb->GetEnergyFromBeta(betamax);

  if(energy_loss) {
    pre_energy_MeV = srim.GetAdjustedEnergy(pre_energy_MeV*1e3, thickness/2.0)/1e3;
  }

  TReaction reac(rb, pb, rb, pb, pre_energy_MeV);

  double post_energy_MeV = reac.GetTLab(rb_angle_rad, 2);

  if(energy_loss) {
    double distance_travelled = (thickness/2)/std::abs(std::cos(rb_angle_rad));
    post_energy_MeV = srim.GetAdjustedEnergy(post_energy_MeV*1e3, distance_travelled)/1e3;
  }

  double beta = rb->GetBetaFromEnergy(post_energy_MeV);
  return beta;
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

void MakeSega(TRuntimeObjects& obj, TSega& sega) {

    std::string dirname = "SeGA";

    obj.FillHistogram(dirname,"num_hits",
            256,0,256,sega.Size());

    double cc_ts = -1;
    for(unsigned int i=0; i<sega.Size(); i++){
        TSegaHit& hit = sega.GetSegaHit(i);
        int hit_detnum = hit.GetDetnum();
        double energy = hit.GetEnergy();

        obj.FillHistogram(dirname,"detnum",
                18, 1, 19, hit_detnum);

        obj.FillHistogram(dirname,"charge_summary",
                18, 1, 19, hit_detnum,
                32768, 0, 32768, hit.Charge());

        obj.FillHistogram(dirname,"energy_summary",
                19, 0, 19, hit_detnum,
                8000, 0, 8000, energy);

        obj.FillHistogram(dirname,"energy",
                16384, 0, 32768, energy);

        cc_ts = hit.Timestamp(); 

        obj.FillHistogram(dirname, Form("energy_vs_ts_det%02d", hit_detnum),
                2000,0,2000, cc_ts/1e09,
                2000, 0, 4000, energy); 

        obj.FillHistogram(dirname,"energy_vs_ts",
                2000,0,2000, cc_ts/1e09,
                2000, 0, 4000, energy); 
 
      /*
      for(unsigned int segi=0; segi<hit.GetNumSegments(); segi++){
          TSegaSegmentHit& seg = hit.GetSegment(segi);
          int segnum = seg.GetSegnum();

          obj.FillHistogram(dirname, "allseg_summary",
                  32*16, 1, 32*16 + 1, 32*(hit_detnum-1) + segnum,
                  32768, 0, 32768, seg.Charge());

      }//loop over sega segment hits
      */

    }//loop over sega hits
}

void MakeJanus(TRuntimeObjects& obj, TJanusDDAS& janus) {

    std::string dirname = "Janus";

    obj.FillHistogram(dirname, "num_channels",150, -10, 140, janus.GetAllChannels().size());
    obj.FillHistogram(dirname, "num_hits",150, -10, 140, janus.GetAllHits().size());

   for(size_t i=0;i<janus.GetAllChannels().size();i++){
      auto &jch = janus.GetJanusChannel(i);
      //obj.FillHistogram(dirname, "frontch_vs_charge_chan", 
      //                  200,0,200, jch.GetFrontChannel(),
      //                  3000, 0,30000, jch.Charge());

      obj.FillHistogram(dirname, "charge_summary_chan",
                        128, 0, 128, jch.GetFrontChannel(),
                        32768, 0, 32768, jch.Charge());

      obj.FillHistogram(dirname, "energy_summary_chan",
                        128, 0, 128, jch.GetFrontChannel(),
                        32768, 0, 32768, jch.GetEnergy());

      obj.FillHistogram(dirname,"detnum_chan",15,-5,10, jch.GetDetnum());
    }

   for(auto& hit : janus.GetAllHits()) { 

      obj.FillHistogram(dirname,Form("ringNum_det%i",hit.GetDetnum()),26,0,26,hit.GetRing()); 
      obj.FillHistogram(dirname,Form("SectorNum_det%i",hit.GetDetnum()),34,0,34,hit.GetSector()); 

      obj.FillHistogram(dirname,Form("HitPattern_det%i",hit.GetDetnum()),
                        40,0,40,hit.GetSector(),
                        40,0,40,hit.GetRing());

      obj.FillHistogram(dirname,Form("sector_v_ring_det%i",hit.GetDetnum()),26,0,26,
                        hit.GetRing(),34,0,34,hit.GetSector());

      obj.FillHistogram(dirname, "charge_summary_hit",
                        128, 0, 128, hit.GetFrontChannel(),
                        32768, 0, 32768, hit.Charge());
      obj.FillHistogram(dirname, "charge_summary_hit",
                        128, 0, 128, hit.GetBackChannel(),
                        32768, 0, 32768, hit.GetBackHit().Charge());

      obj.FillHistogram(dirname, "energy_summary_hit",
                        128, 0, 128, hit.GetFrontChannel(),
                        32768, 0, 32768, hit.GetEnergy());
      obj.FillHistogram(dirname, "energy_summary_hit",
                        128, 0, 128, hit.GetBackChannel(),
                        32768, 0, 32768, hit.GetBackHit().GetEnergy());

      auto tdiff = hit.Timestamp() - hit.GetBackHit().Timestamp();

      obj.FillHistogram(dirname, Form("det%d_frontback_tdiff_hit",hit.GetDetnum()),
                        250, -1000, 1000, tdiff);

      obj.FillHistogram(dirname, Form("det%d_frontback_tdiff_ringenergy",hit.GetDetnum()),
                        250, -1000, 1000, tdiff,
                        3000,0,30000, hit.GetEnergy());

      obj.FillHistogram(dirname, Form("det%d_frontback_tdiff_sectorenergy",hit.GetDetnum()),
                        100, -1000, 1000, tdiff,
                        3000,0,30000, hit.GetBackHit().GetEnergy());

//    obj.FillHistogram(dirname,Form("det%d_xy", hit.GetDetnum()),
//                      200,-4,4,hit.GetPosition().X(),
//                      200,-4,4,hit.GetPosition().Y());

      obj.FillHistogram(dirname,Form("RingEnergy_v_SectorEnergy_det%i",hit.GetDetnum()),
                        1500,0,30000,hit.GetBackHit().GetEnergy(),
                        1500,0,30000,hit.GetEnergy());

      obj.FillHistogram(dirname,"RingEnergy_v_Angle",
                        100,0,200,hit.GetPosition().Theta()*(180./TMath::Pi()),
                        3000,0,30000,hit.GetEnergy());

      obj.FillHistogram(dirname,"SectorEnergy_v_Angle",
                        100,0,200,hit.GetPosition().Theta()*(180./TMath::Pi()),
                        3000,0,30000,hit.GetBackHit().GetEnergy());

      TVector3 tmpvec = hit.GetPosition();

      double perp, theta, phi, mag;
      //double xoff = -0.13;
      //double yoff = 0.17;
      double xoff = 0.0;
      double yoff = 0.0;

      perp = hit.GetPosition().Perp() + gRandom->Rndm()*.1 - .045;
      theta = TMath::ATan(perp/3);
      phi = hit.GetPosition().Phi() + (gRandom->Rndm()*1. - .5) * (TMath::Pi() / 16.);
      mag = 3./TMath::Cos(theta);
      tmpvec.SetMagThetaPhi(mag,theta,phi);
      tmpvec.SetY(tmpvec.Y()+yoff);
      tmpvec.SetX(tmpvec.X()+xoff);

      obj.FillHistogram(dirname,Form("janus_theta_phi_det%i",hit.GetDetnum()),
                        90,0,180,tmpvec.Theta()*TMath::RadToDeg(),
                        180,-180,180,tmpvec.Phi()*TMath::RadToDeg());

      obj.FillHistogram(dirname,Form("janus_XY_det%i",hit.GetDetnum()),
                        160,-4,4,hit.GetPosition().X(),
                        160,-4,4,hit.GetPosition().Y());

      obj.FillHistogram(dirname,Form("map_det%i",hit.GetDetnum()),
                        160,-4,4,tmpvec.X(),
                        160,-4,4,tmpvec.Y());

    }
}

void MakeSeGAJanus(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus) {

    std::string dirname = "SeGAJanus";

    obj.FillHistogram(dirname,"sega_mult",20,0,20,sega.Size());

    obj.FillHistogram(dirname, "janus_mult",150, -10, 140, janus.GetAllHits().size());

    for(auto& j_hit : janus.GetAllHits()) {

        obj.FillHistogram(dirname,Form("ringNum_det%i",j_hit.GetDetnum()),26,0,26,j_hit.GetRing()); 
        obj.FillHistogram(dirname,Form("sectorNum_det%i",j_hit.GetDetnum()),34,0,34,j_hit.GetSector());

        for(unsigned int i=0; i<sega.Size(); i++) {
            TSegaHit& s_hit = sega.GetSegaHit(i);

            auto s_pos = s_hit.GetPosition();
            auto j_pos = j_hit.GetPosition();

            auto angle = s_pos.Angle(j_pos);
            double s_energy = s_hit.GetEnergy();
            auto tdiff = (s_hit.Timestamp() - j_hit.Timestamp());

            obj.FillHistogram(dirname,"GammaEnergy",4000, 0, 4000, s_energy);

            obj.FillHistogram(dirname, "GammaEnergy_v_angle",
                              4000, 0, 4000, s_energy,
                              90, 0, 180, (180/3.1415926)*angle);

           double s_energy_dc = s_hit.GetDoppler(0.078, j_pos); 

            obj.FillHistogram(dirname, "GammaEnergyDC",4000, 0, 4000, s_energy_dc);

            obj.FillHistogram(dirname, "GammaEnergyDC_v_angle",
                              4000, 0, 4000, s_energy_dc,
                              180, 0, 180, (180/3.1415926)*angle);


            obj.FillHistogram(dirname, "GammaEnergy_v_tdiff",
                              1000, -2000, 2000, tdiff,
                              2500, 0, 5000, s_energy);


            for(double beta : BinCenters(60, 0.0350, 0.0950)) {

                obj.FillHistogram(dirname, "GammaEnergyDC_betascan",
                        60, 0.0350, 0.0950, beta,
                        4000, 0, 4000, s_hit.GetDoppler(beta,j_pos));
            }

            for(double pos : BinCenters(120, -10, 20)) {

                obj.FillHistogram(dirname, "GammaEnergyDC_beta078_zscan",
                        120, -10, 20, pos,
                        4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(0,0,pos)));
            }

           if(s_energy_dc < 420 && s_energy_dc > 390) {

           obj.FillHistogram(dirname,Form("ringNum_5/2_det%i",j_hit.GetDetnum()),
                             26,0,26,j_hit.GetRing()); 
           obj.FillHistogram(dirname,Form("sectoNum_5/2_det%i",j_hit.GetDetnum()),
                             34,0,34,j_hit.GetSector());  

           }
        }
    }
} 

void MakeScatterGatedSeGAJanus(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus, GCutG* time_gate, GCutG* scatter_gate) {
    if(!scatter_gate) 
      {return;}

    std::string dirname = "SeGAJanus_Scattered";
    dirname += scatter_gate->GetName();
    if(time_gate){
      dirname += "_";
      dirname += time_gate->GetName();
    }

    for(auto& j_hit : janus.GetAllHits()) {
       if(!scatter_gate->IsInside(j_hit.GetPosition().Theta()*(180./TMath::Pi()),j_hit.GetEnergy())) 
         {continue;}

       obj.FillHistogram(dirname,Form("ringNum_noTimeGare_det%i",j_hit.GetDetnum()),
                         26,0,26,j_hit.GetRing()); 
       obj.FillHistogram(dirname,Form("sectorNum_noTimeGate_det%i",j_hit.GetDetnum()),
                         34,0,34,j_hit.GetSector());
        
       obj.FillHistogram(dirname,"sega_mult_noTimeGate",20,0,20,sega.Size());

       for(unsigned int i=0; i<sega.Size(); i++) {
          TSegaHit& s_hit = sega.GetSegaHit(i);

          double s_energy = s_hit.GetEnergy();
          auto tdiff = (s_hit.Timestamp() - j_hit.Timestamp());
          auto j_pos = j_hit.GetPosition();
             
          if(time_gate) {
            if(!time_gate->IsInside(tdiff,s_energy)) 
              {continue;}
          }
  
          auto s_pos = s_hit.GetPosition();
          auto angle = s_pos.Angle(j_pos);
          double s_energy_dc = s_hit.GetDoppler(GValue::Value("beta"), j_pos);

          obj.FillHistogram(dirname, "GammaEnergy",4000, 0, 4000, s_energy);

          obj.FillHistogram(dirname,"GammaEnergy_v_angle",
                            4000, 0, 4000, s_energy,
                            90, 0, 180, (180/3.1415926)*angle);
             
          obj.FillHistogram(dirname, "GammaEnergyDC",4000, 0, 4000, s_energy_dc);

          obj.FillHistogram(dirname,"GammaEnergyDC_summary",
                            20,0,20, s_hit.GetDetnum(),
                            4000, 0, 4000, s_energy_dc);

          obj.FillHistogram(dirname, "GammaEnergyDC_v_angle",
                            4000, 0, 4000, s_energy_dc,
                            180, 0, 180, (180/TMath::Pi())*angle);

          obj.FillHistogram(dirname, "GammaEnergy_v_tdiff",
                            2000, -2000, 2000, tdiff,
                            4000, 0, 4000, s_energy);

          for(double beta : BinCenters(60, 0.0350, 0.0950)) {

             obj.FillHistogram(dirname, "GammaEnergyDC_betascan",
                               60, 0.0350, 0.0950, beta,
                               4000, 0, 4000, s_hit.GetDoppler(beta,j_pos));
          }

          for(double pos : BinCenters(120, -10, 20)) {

             obj.FillHistogram(dirname,"GammaEnergyDC_beta078_zscan",
                               120, -10, 20, pos,
                               4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(0,0,pos)));

             obj.FillHistogram(dirname,"GammaEnergyDC_beta078_yscan",
                               120, -10, 20, pos,
                               4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(0,pos,0)));

             obj.FillHistogram(dirname,"GammaEnergyDC_beta078_xscan",
                               120, -10, 20, pos,
                               4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(pos,0,0)));
                   
            }

          if(s_energy_dc < 420 && s_energy_dc > 390) {

           obj.FillHistogram(dirname,Form("ringNum_5/2_det%i",j_hit.GetDetnum()),
                             26,0,26,j_hit.GetRing()); 
           obj.FillHistogram(dirname,Form("sectoNum_5/2_det%i",j_hit.GetDetnum()),
                             34,0,34,j_hit.GetSector());  

           } 
        }
    }

}

void MakeRecoilGatedSeGAJanus(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus, GCutG* time_gate, GCutG* recoil_gate) {

    if(!recoil_gate) 
      {return;}

    std::string dirname = "SeGAJanus_Recoiling";
    dirname += recoil_gate->GetName();
    if(time_gate){
      dirname += "_";
      dirname += time_gate->GetName();
    }
 
    for(auto& j_hit : janus.GetAllHits()) {
       if(!recoil_gate->IsInside(j_hit.GetPosition().Theta()*(180./TMath::Pi()),j_hit.GetEnergy())) 
         {continue;}

       obj.FillHistogram(dirname,Form("ringNum__noTimeGate_det%i",j_hit.GetDetnum()),
                         26,0,26,j_hit.GetRing()); 
       obj.FillHistogram(dirname,Form("sectorNum_noTimeGate_det%i",j_hit.GetDetnum()),
                         34,0,34,j_hit.GetSector());
        
       obj.FillHistogram(dirname,"sega_mult_noTimeGate",20,0,20,sega.Size());
        
       for(unsigned int i=0; i<sega.Size(); i++) {

          TSegaHit& s_hit = sega.GetSegaHit(i);
          double s_energy = s_hit.GetEnergy();
          auto tdiff = (s_hit.Timestamp() - j_hit.Timestamp());

          if(time_gate) {
            if(!time_gate->IsInside(tdiff,s_energy)) 
              {continue;}
          }

          auto j_pos = j_hit.GetPosition();
          auto recon_pos = j_hit.GetReconPosition();
          auto s_pos = s_hit.GetPosition();
          double recon_angle = s_pos.Angle(recon_pos);
          double s_energy_recon_dop = s_hit.GetDoppler(get_beta(0.09237,recon_angle,1),recon_pos);
          
          obj.FillHistogram(dirname, "GammaEnergy",4000, 0, 4000, s_energy);

          obj.FillHistogram(dirname,"GammaEnergy_v_ReconAngle",
                            4000, 0, 4000, s_energy,
                            180, 0, 180, (180/TMath::Pi())*recon_angle);

          obj.FillHistogram(dirname, "GammaEnergyRecoilDC",4000, 0, 4000, s_energy_recon_dop);

          obj.FillHistogram(dirname,"GammaEnergyRecoilDC_v_ReconAngle",
                            4000, 0, 4000, s_energy_recon_dop,
                            180, 0, 180, (180/TMath::Pi())*recon_angle);


        }
    }
}

int gates_loaded=0;
std::vector<GCutG*> scatter_gates;
std::vector<GCutG*> recoil_gates;
GCutG *time_gate=0;

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
      if(!tag.compare("ScatteredBeam")) {
        scatter_gates.push_back(gate);
        std::cout << "Scattered Beam Gate: << " << gate->GetName() << std::endl;
      }else if(!tag.compare("TargetRecoil")) {
        recoil_gates.push_back(gate); 
        std::cout << "Target Recoil Gate: << " << gate->GetName() << std::endl;
      }else if(!tag.compare("time")) {
        time_gate = gate; 
        std::cout << "Time Gate: << " << gate->GetName() << std::endl;
      }
      gates_loaded++;
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

    if(time_gate) {

      for(auto &scatter_gate : scatter_gates){
         MakeScatterGatedSeGAJanus(obj, *sega, *janus, time_gate, scatter_gate);
      } 

      for(auto &recoil_gate : recoil_gates){
         MakeRecoilGatedSeGAJanus(obj, *sega, *janus, time_gate, recoil_gate);
      }  
    }

    if(!time_gate) {

      for(auto &scatter_gate : scatter_gates){
         MakeScatterGatedSeGAJanus(obj, *sega, *janus, 0, scatter_gate);
      } 

      for(auto &recoil_gate : recoil_gates){
         MakeRecoilGatedSeGAJanus(obj, *sega, *janus, 0, recoil_gate);
      }  
    }
  }

}
