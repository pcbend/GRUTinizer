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


static auto beam = std::make_shared<TNucleus>("76Se");
static auto targ = std::make_shared<TNucleus>("208Pb");
static TSRIM srim("se72_in_pb208.txt");
double thickness = (GValue::Value("targetthick") / 11342.0) * 1e4; // (0.75 mg/cm^2) / (11342 mg/cm^3) * (10^4 um/cm)

//double collision_pos = gRandom->Uniform();
//double collision_energy = srim.GetAdjustedEnergy(GValue::Value("beamenergy")*1e3, thickness*collision_pos)/1e3;
double energy_mid = srim.GetAdjustedEnergy(GValue::Value("beamenergy")*1e3, thickness*0.5)/1e3;

//TReaction reaction(beam, targ, beam, targ, collision_energy);
TReaction reaction(beam, targ, beam, targ, energy_mid);

TVector3 tmpvec;

int gates_loaded=0;
std::vector<GCutG*> kin_gates;
GCutG *timing=0;

void MakeSega(TRuntimeObjects& obj) {
  TSega* sega = obj.GetDetector<TSega>();
  TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();
  for(size_t x=0;x<sega->Size();x++) {
    TSegaHit &hit = sega->GetSegaHit(x);
    obj.FillHistogram("sega_summary",16,1,17,hit.GetDetnum(),
        3000,0,3000,hit.GetEnergy());
    obj.FillHistogram("sega_uptime",540,0,5400,hit.Timestamp()/1e9,
        20,0,20,hit.GetDetnum());

    //obj.FillHistogram(Form("Sega%02i",hit.GetDetnum()),"energy/time",3600,0,7200,hit.Timestamp()/1e9,
    //                                                                 1000,0,4000,hit.GetEnergy());
    obj.FillHistogram("sega",Form("energy_time_%02i",hit.GetDetnum()),3600,0,7200,hit.Timestamp()/1e9,
        100,1400,1500,hit.GetEnergy());

    for(int y=0;y<hit.GetNumSegments();y++) {
      TSegaSegmentHit shit = hit.GetSegment(y);
      obj.FillHistogram("sega",Form("Segments%02i",hit.GetDetnum()),
          1000,0,15000,shit.Charge(),
          32,0,32,shit.GetSegnum()-1);
    }

  }
  return; 
}

void MakeRawJanus(TRuntimeObjects& obj) {
  TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();
  std::vector<double> charge;
  for(size_t x=0; x<janus->GetAllChannels().size();x++) {
    TJanusDDASHit hit = janus->GetJanusChannel(x);
    obj.FillHistogram("multi",100,0,100,janus->GetAllChannels().size(),
        1500,0,15000,hit.Charge());
    obj.FillHistogram("janus_uptime",540,0,5400,hit.Timestamp()/1e9,
        200,0,200,hit.GetFrontChannel());


    obj.FillHistogram("janus_summary",200,0,200,hit.GetFrontChannel(),
        4000,0,40000,hit.Charge());

    if(hit.GetDetnum()==0)
      obj.FillHistogram("janus_charge_energy",250,0,10,hit.Charge()/hit.GetEnergy());


    for(size_t y=x+1; y<janus->GetAllChannels().size();y++) {
      TJanusDDASHit hit2 = janus->GetJanusChannel(y);

      if(hit.GetDetnum()==hit2.GetDetnum() && hit.IsRing()!=hit2.IsRing()){
        if(hit2.Charge()>150 && hit2.Charge()<32000){
          if(hit.Charge()>150 && hit.Charge()<32000){
            obj.FillHistogram("janus_raw_time",2000,-10000,10000,hit.Timestamp()-hit2.Timestamp());
            obj.FillHistogram("janus_raw_time_charge",2000,-10000,10000,hit.Timestamp()-hit2.Timestamp(),
                1000,-15000,15000,hit.Charge()-hit2.Charge());

          }
        }
      }
      if(hit.GetDetnum()!=hit2.GetDetnum() || hit.GetDetnum()==0 || hit2.GetDetnum()==0) continue;

      obj.FillHistogramSym("janus_sym",1500,0,15000,hit.Charge(),
          1500,0,15000,hit2.Charge());

      if(hit.IsSector() && hit2.IsRing())
        obj.FillHistogram("janus_unused_fb",1500,0,15000,hit.Charge(),
            1500,0,15000,hit2.Charge());
      else if(hit2.IsSector() && hit.IsRing())
        obj.FillHistogram("janus_unused_fb",1500,0,15000,hit2.Charge(),
            1500,0,15000,hit.Charge());
    }

    if(hit.Charge()>150 && hit.Charge()<32000)
      charge.push_back(hit.Charge());
  }

  for(size_t x=0;x<charge.size();x++) {
    obj.FillHistogram("multi_150gated",20,0,20,charge.size(),
        1500,0,15000,charge.at(x));
  }

  return;
}

void MakeCalJanus(TRuntimeObjects &obj) {
  TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();

  obj.FillHistogram("janus_hits_channels",40,0,40,janus->Size(),
      40,0,40,janus->GetAllChannels().size());


  TSega* sega = obj.GetDetector<TSega>();
  for(int x=0;x<janus->Size();x++) {
    TJanusDDASHit jhit = janus->GetJanusHit(x);
    obj.FillHistogram("janus_kin",90,0,180,jhit.GetPosition(0).Theta()*TMath::RadToDeg(),
        1800,0,18000,jhit.GetEnergy());
    obj.FillHistogram("janus_kin_rebin",90,0,180,jhit.GetPosition(0).Theta()*TMath::RadToDeg(),
                                        256,0,16384,jhit.GetEnergy());


    obj.FillHistogram(Form("janus_ring_E_det%i",jhit.GetDetnum()),24,0,24,jhit.GetRing()-1,
                                                1800,0,18000,jhit.GetEnergy());

    obj.FillHistogram(Form("janus_fb_dtime_det%i",jhit.GetDetnum()),2000,-10000,10000,jhit.Timestamp()-jhit.GetBackHit().Timestamp());
    obj.FillHistogram(Form("janus_fb_energy_det%i",jhit.GetDetnum()),2048,0,65536,jhit.GetEnergy(),
        2048,0,65536,jhit.GetBackHit().GetEnergy());
    obj.FillHistogram(Form("janus_fb_dtime_front_energy_det%i",jhit.GetDetnum()),200,-1000,1000,jhit.Timestamp()-jhit.GetBackHit().Timestamp(),
        512,0,16384,jhit.GetEnergy());
    obj.FillHistogram(Form("janus_fb_dtime_ring_num_det%i",jhit.GetDetnum()),200,-1000,1000,jhit.Timestamp()-jhit.GetBackHit().Timestamp(),
        24,0,24,jhit.GetRing()-1);
    obj.FillHistogram(Form("janus_fb_dtime_sector_num_det%i",jhit.GetDetnum()),200,-1000,1000,jhit.Timestamp()-jhit.GetBackHit().Timestamp(),
        32,0,32,jhit.GetSector()-1);

    obj.FillHistogram(Form("janus_fb_time_janushits_size%i",jhit.GetDetnum()),200,-1000,1000,jhit.Timestamp()-jhit.GetBackHit().Timestamp(),
        40,0,40,janus->Size());
    obj.FillHistogram(Form("janus_fb_time_januschan_size%i",jhit.GetDetnum()),200,-1000,1000,jhit.Timestamp()-jhit.GetBackHit().Timestamp(),
        40,0,40,janus->GetAllChannels().size());

    tmpvec = jhit.GetPosition();

    double perp, theta, phi, mag;

    double xoff = 0.;
    double yoff = 0.;

    for(int i=0;i<1;i++){ // NOTE 10x STATS ON MAPS - makes things clearer
      perp = jhit.GetPosition().Perp() + gRandom->Rndm()*.1 - .045;
      theta = TMath::ATan(perp/3);
      phi = jhit.GetPosition().Phi() + (gRandom->Rndm()*1. - .5) * (TMath::Pi() / 16.);
      mag = 3./TMath::Cos(theta);
      tmpvec.SetMagThetaPhi(mag,theta,phi);
      tmpvec.SetY(tmpvec.Y()+yoff);
      tmpvec.SetX(tmpvec.X()+xoff);
    //  if(jhit.GetRing()<2)
      obj.FillHistogram(Form("theta_phi%i",jhit.GetDetnum()),300,10,60,tmpvec.Theta()*TMath::RadToDeg(),
            180,-180,180,tmpvec.Phi()*TMath::RadToDeg());


      obj.FillHistogram(Form("map_det%i",jhit.GetDetnum()),160,-4.,4.,tmpvec.X(),
          160,-4.,4.,tmpvec.Y());

      /*float xxoff=-0.4;
        if(jhit.GetRing()<2)
        while(xxoff<=0.4) {
        float yyoff=-0.4;
        while(yyoff<=0.4) {
        TVector3 v(xxoff,yyoff,0);
        v= v+tmpvec;
        obj.FillHistogram(Form("det%i",jhit.GetDetnum()),Form("phi_det%i_x%.02f_y%.02f",jhit.GetDetnum(),xxoff,yyoff),
        180,-180,180,v.Phi()*TMath::RadToDeg());
      //160,-4.,4.,tmpvec.X()+v.X(),
      //160,-4.,4.,tmpvec.Y()+v.Y());

      yyoff+=0.02;

      }

      xxoff+=0.02;

      }*/
    }
    for(int z=0;z<kin_gates.size();z++) {
      GCutG *cut = kin_gates.at(z);
      if(!cut->IsInside(jhit.GetPosition().Theta()*TMath::RadToDeg(),jhit.GetEnergy())) continue;
      obj.FillHistogram(Form("janus_uptime_%s",cut->GetName()),540,0,5400,jhit.Timestamp()/1e9,
          10,0,10,jhit.GetDetnum());
      for(int i=0;i<10;i++){  // NOTE 10x STATS ON MAPS - makes things clearer
        perp = jhit.GetPosition().Perp() + gRandom->Rndm()*.09 - .045;
        theta = TMath::ATan(perp/3);
        phi = jhit.GetPosition().Phi() + (gRandom->Rndm()*.9 - .5) * (TMath::Pi() / 16.);
        mag = 3./TMath::Cos(theta);
        tmpvec.SetMagThetaPhi(mag,theta,phi);
        tmpvec.SetY(tmpvec.Y()+yoff);
        tmpvec.SetX(tmpvec.X()+xoff);

        obj.FillHistogram(Form("theta_phi_%s",cut->GetName()),300,10,60,tmpvec.Theta()*TMath::RadToDeg(),
            180,-180,180,tmpvec.Phi()*TMath::RadToDeg());

        obj.FillHistogram(Form("map_det_%s",cut->GetName()),160,-4.,4.,tmpvec.X(),
            160,-4.,4.,tmpvec.Y());
      }
      obj.FillHistogram(Form("janus_ringhits_%s",cut->GetName()),24,0,24,jhit.GetRing()-1);
      obj.FillHistogram(Form("janus_secthits_%s",cut->GetName()),32,0,32,jhit.GetSector()-1);
      obj.FillHistogram(Form("map_det%i_%s",jhit.GetDetnum(),cut->GetName()),32,-3.5,3.5,jhit.GetPosition().Phi(),
          70,-3.5,3.5,jhit.GetPosition().Perp());
      if(!strcmp(cut->GetName(),"upstream")) 
        obj.FillHistogram("upstream_ring_E",24,0,24,jhit.GetRing()-1,
                                            256,0,16384,jhit.GetEnergy());
    }


    if(sega)  {                                     
      for(int y=0;y<sega->Size();y++) {
        TSegaHit shit = sega->GetSegaHit(y);


        obj.FillHistogram("doppler_plus_any",1000,0,3000,shit.GetDoppler(0.08,jhit.GetPosition()));
        obj.FillHistogram("sega_plus_any",1500,0,3000,shit.GetEnergy());

        obj.FillHistogram("timing",2000,-10000,10000,jhit.Timestamp()-shit.Timestamp(),
            1500,0,3000,shit.GetEnergy());

        if(!timing || !timing->IsInside(jhit.Timestamp()-shit.Timestamp(),shit.GetEnergy())) continue;        

        double b;
        b=reaction.AnalysisBetaFromThetaLab(jhit.GetPosition().Theta(),2);
        if(jhit.GetDetnum()==0)
          obj.FillHistogram("Upstream_Coinc",1024,0,2048,shit.GetDoppler(b,jhit.GetPosition()),
                                              512,0,8192,jhit.GetEnergy());

        TVector3 segaoff;
        segaoff.SetXYZ(0,0,3);
        TVector3 newsega = segaoff + shit.GetPosition();
        obj.FillHistogram("SegaPositions",Form("ThetaPhi_%i",shit.GetDetnum()),180,0,180,newsega.Theta()*TMath::RadToDeg(),
                                                                        180,-180,180,newsega.Phi()*TMath::RadToDeg());

        for(int z=0;z<kin_gates.size();z++) {
          GCutG *cut = kin_gates.at(z);
          if(!cut->IsInside(jhit.GetPosition().Theta()*TMath::RadToDeg(),
                jhit.GetEnergy())) continue;
          if(!strcmp(cut->GetName(),"Se76") || !strcmp(cut->GetName(),"upstream")) {
              //std::cout << "Found Se76" << std::endl;
            b=reaction.AnalysisBetaFromThetaLab(jhit.GetPosition().Theta(),2);
            obj.FillHistogram(Form("doppler_%s",cut->GetName()),1000,0,3000,
                shit.GetDoppler(b,jhit.GetPosition(),segaoff));
            obj.FillHistogram(Form("timing_%s",cut->GetName()),2000,-10000,10000,jhit.Timestamp()-shit.Timestamp(),
                1500,0,3000,shit.GetDoppler(b,jhit.GetPosition(),segaoff));
            obj.FillHistogram(Form("doppler_ring_%s",cut->GetName()),
                24,0,24,jhit.GetRing()-1,
                1000,0,3000,shit.GetDoppler(b,jhit.GetPosition(),segaoff));                                              
            obj.FillHistogram(Form("doppler_e_Si_e_%s",cut->GetName()),512,0,2048,shit.GetDoppler(b,jhit.GetPosition()),
                256,0,16384,jhit.GetEnergy());
            obj.FillHistogram(Form("energy_angle_%s",cut->GetName()),
                90,0,180,shit.GetPosition().Angle(jhit.GetPosition())*TMath::RadToDeg(),
                1000,0,3000,shit.GetEnergy());
            if(!strcmp(cut->GetName(),"Se76")){
                obj.FillHistogram(Form("SeGated_Doppler_SeGADet%i",shit.GetDetnum()),2048,0,2048,shit.GetDoppler(b,jhit.GetPosition(),segaoff),
                                                                    32,0,32,shit.GetMainSegnum());
            }
            for(int zz=y+1;zz<sega->Size();zz++) {
              TSegaHit shit2 = sega->GetSegaHit(zz);
              obj.FillHistogramSym(Form("dmat_%s",cut->GetName()),
                  1000,0,3000,shit.GetDoppler(b,jhit.GetPosition(),segaoff),
                  1000,0,3000,shit2.GetDoppler(b,jhit.GetPosition(),segaoff));
            }
            if(!strcmp(cut->GetName(),"upstream")){
              obj.FillHistogram("si_e_uncorr_sega_upstream",512,0,2048,shit.GetEnergy(),
                                                            512,0,16384,jhit.GetEnergy());
              obj.FillHistogram("ring_uncorr_sega_upstream",512,0,2048,shit.GetEnergy(),
                                                            24,0,24,jhit.GetRing()-1);
            }
          } else {
            b=reaction.AnalysisBetaFromThetaLab(jhit.GetReconPosition().Theta(),2);
            obj.FillHistogram(Form("doppler_E_angle_%s",cut->GetName()),1024,0,2048,shit.GetDoppler(b,jhit.GetReconPosition(),segaoff),
                                                                90,0,180,jhit.GetReconPosition().Angle(shit.GetPosition())*TMath::RadToDeg());
            b = b/2;
            obj.FillHistogram(Form("doppler_E_angle_newbeta_%s",cut->GetName()),1024,0,2048,shit.GetDoppler(b,jhit.GetReconPosition(),segaoff),
                                                                90,0,180,jhit.GetReconPosition().Angle(shit.GetPosition())*TMath::RadToDeg());

            obj.FillHistogram(Form("doppler_%s",cut->GetName()),1000,0,3000,
                shit.GetDoppler(b,jhit.GetReconPosition(),segaoff));
            obj.FillHistogram(Form("doppler_ring_%s",cut->GetName()),
                24,0,24,jhit.GetRing()-1,
                1000,0,3000,shit.GetDoppler(b,jhit.GetReconPosition(),segaoff));        
            obj.FillHistogram(Form("energy_angle_%s",cut->GetName()),
                90,0,180,shit.GetPosition().Angle(jhit.GetReconPosition())*TMath::RadToDeg(),
                1000,0,3000,shit.GetEnergy());
            for(int zz=y+1;zz<sega->Size();zz++) {
              TSegaHit shit2 = sega->GetSegaHit(zz);
              obj.FillHistogramSym(Form("dmat_%s",cut->GetName()),
                  1000,0,3000,shit.GetDoppler(b,jhit.GetReconPosition(),segaoff),
                  1000,0,3000,shit2.GetDoppler(b,jhit.GetReconPosition(),segaoff));
            }

          }          
          obj.FillHistogram(Form("BetaDistribution_%s",cut->GetName()),
              90,0,180,jhit.GetPosition().Theta()*TMath::RadToDeg(),
              100,0,0.1,b);
          obj.FillHistogram("Ring_theta",90,0,180,jhit.GetPosition().Theta()*TMath::RadToDeg(),24,0,24,jhit.GetRing()-1);

          //obj.FillHistogram(Form("doppler_%s",cut->GetName()),1000,0,3000,
          //    shit.GetDoppler(0.08,jhit.GetPosition()));
        }
      }
    }
  }
}


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
      if(!tag.compare("kin")) {
        kin_gates.push_back(gate);
        std::cout << "kin: << " << gate->GetName() << std::endl;
      }else if(!tag.compare("time")) {
        timing = gate; 
        std::cout << "time: << " << gate->GetName() << std::endl;
      }
      gates_loaded++;
    }
  }


  if(janus) {
    MakeRawJanus(obj);
    janus->BuildCorrelatedHits(1000.,200.);
    MakeRawJanus(obj);
    MakeCalJanus(obj);
  }
  if(sega) 
    MakeSega(obj);



}
