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
#include "GCutG.h"

GCutG *scatteredRb = 0;
GCutG *tcut = 0;
int gates_loaded=0;
void MakeSegaHistograms(TRuntimeObjects& obj, TSega& sega);
void MakeJanusHistograms(TRuntimeObjects& obj, TJanusDDAS& janus);
void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus);
void MakeGatedCoincidenceHistograms(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus, GCutG* tcut, GCutG * scatteredRb);
void MakeTCUTGatedCoincidenceHistograms(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus, GCutG *tcut);

void MakeScalerHistograms(TRuntimeObjects& obj, TNSCLScalers& scalers) {
    if(scalers.GetSourceID() == 3) {
        for(unsigned int i=0; i<scalers.Size(); i++) {
            int value = scalers.GetScaler(i);

            obj.FillHistogram("scalers", "cumulative",
                    128, 0, 128, i, value);

            obj.FillHistogram("scalers", Form("chan%02d_timedep", i),
                    1800, 0, 3600, scalers.GetIntervalStart(), value);
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
    TNSCLScalers* scalers = obj.GetDetector<TNSCLScalers>();
    TList    *list    = &(obj.GetObjects());
    int numobj = list->GetSize();

    TList *gates = &(obj.GetGates());
    if(gates_loaded!=gates->GetSize()) {
        TIter iter(gates);
        while(TObject *obj = iter.Next()) {
            GCutG *gate = (GCutG*)obj;
            std::string tag = gate->GetTag();
            if(!tag.compare("scattered")) {
                scatteredRb = new GCutG(*gate);
                std::cout << "scattered: << " << gate->GetName() << std::endl;
            } else if(!tag.compare("time")){
                tcut = new GCutG(*gate);
                std::cout << "time: << " << gate->GetName() << std::endl;
            } else {
                std::cout << "unknown: << " << gate->GetName() << std::endl;
            }
            gates_loaded++;
        }
    }

    int hitpat = bool(sega) + bool(janus)*2;
    obj.FillHistogram("sega_janus_coinc",
            4, 0, 4, hitpat);

//  if(janus){
//      MakeJanusHistograms(obj, *janus);
//  }
    if(sega){
        MakeSegaHistograms(obj, *sega);

    }
//  if(sega && janus){
//      MakeCoincidenceHistograms(obj, *sega, *janus);
//      if (scatteredRb&&tcut){
//          MakeGatedCoincidenceHistograms(obj, *sega, *janus, tcut, scatteredRb);
//          MakeTCUTGatedCoincidenceHistograms(obj, *sega, *janus, tcut, scatteredRb);
//      }
//  }

//  if(scalers){
//      MakeScalerHistograms(obj, *scalers);
//  }

    if(numobj!=list->GetSize()){
        list->Sort();
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

        obj.FillHistogram("janus", "detnum",
                15,-5,10, chan.GetDetnum());

    }

    for(auto& hit : janus.GetAllHits()) {

        obj.FillHistogram("janus","ringNum",26,0,26,hit.GetRing()); 
        obj.FillHistogram("janus","SectorNum",34,0,34,hit.GetSector()); 

        obj.FillHistogram("janus",Form("HitPattern_det%i",hit.GetDetnum()),
                           40,0,40,hit.GetSector(),
                           40,0,40,hit.GetRing());

        obj.FillHistogram("janus","ring_v_sector",26,0,26,hit.GetRing(),34,0,34,hit.GetSector());

        obj.FillHistogram("janus", "charge_summary_hit",
                128, 0, 128, hit.GetFrontChannel(),
                32768, 0, 32768, hit.Charge());
        obj.FillHistogram("janus", "charge_summary_hit",
                128, 0, 128, hit.GetBackChannel(),
                32768, 0, 32768, hit.GetBackHit().Charge());

        obj.FillHistogram("janus", "energy_summary_hit",
                128, 0, 128, hit.GetFrontChannel(),
                32768, 0, 32768, hit.GetEnergy());
        obj.FillHistogram("janus", "energy_summary_hit",
                128, 0, 128, hit.GetBackChannel(),
                32768, 0, 32768, hit.GetBackHit().GetEnergy());


        auto tdiff = hit.Timestamp() - hit.GetBackHit().Timestamp();
        obj.FillHistogram("janus", Form("det%d_frontback_tdiff", hit.GetDetnum()),
                2000, -1000, 1000, tdiff);

        obj.FillHistogram("janus",Form("det%d_xy", hit.GetDetnum()),
                200,-4,4,hit.GetPosition().X(),
                200,-4,4,hit.GetPosition().Y());

        obj.FillHistogram("janus","RingEnergy_v_Angle",
                200,0,200,hit.GetPosition().Theta()*(180./TMath::Pi()),
                3000,0,30000,hit.GetEnergy());

        obj.FillHistogram("janus","SectorEnergy_v_Angle",
                200,0,200,hit.GetPosition().Theta()*(180./TMath::Pi()),
                3000,0,30000,hit.GetBackHit().GetEnergy());


    }
}

void MakeSegaHistograms(TRuntimeObjects& obj, TSega& sega) {

    obj.FillHistogram("sega","num_hits",
            256,0,256,sega.Size());

    double cc_ts = -1;
    for(unsigned int i=0; i<sega.Size(); i++){
        TSegaHit& hit = sega.GetSegaHit(i);
        int hit_detnum = hit.GetDetnum();
        double energy = hit.GetEnergy();

//      obj.FillHistogram("sega","detnum",
//              18, 1, 19, hit_detnum);

        obj.FillHistogram("sega","charge_summary",
                18, 1, 19, hit_detnum,
                32768, 0, 32768, hit.Charge());

        obj.FillHistogram("sega","energy_summary",
                19, 0, 19, hit_detnum,
                8000, 0, 8000, energy);

//      obj.FillHistogram("sega","energy",
//              16384, 0, 32768, energy);

        cc_ts = hit.Timestamp(); 


        //std::cout << "-------------------------" << std::endl ;
        //std::cout << "Timestamp:" << cc_ts << std::endl ;
        //std::cout << "--------------------------" << std::endl ;

        obj.FillHistogram("sega", Form("energy_vs_ts_det%02d", hit_detnum),
                2000,0,2000, cc_ts/1e09,
                2000, 0, 4000, energy); 
                
        

      //for(unsigned int segi=0; segi<hit.GetNumSegments(); segi++){
      //    TSegaSegmentHit& seg = hit.GetSegment(segi);
      //    int segnum = seg.GetSegnum();

      //    obj.FillHistogram("sega", "allseg_summary",
      //            32*16, 1, 32*16 + 1, 32*(hit_detnum-1) + segnum,
      //            32768, 0, 32768, seg.Charge());

      //}
    }
}

void MakeCoincidenceHistograms(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus) {

    for(auto& j_hit : janus.GetAllHits()) {
        for(unsigned int i=0; i<sega.Size(); i++) {
            TSegaHit& s_hit = sega.GetSegaHit(i);

            auto s_pos = s_hit.GetPosition();
            auto j_pos = j_hit.GetPosition();

            auto angle = s_pos.Angle(j_pos);
            double s_energy = s_hit.GetEnergy();
            auto tdiff = (s_hit.Timestamp()*1.25 - j_hit.Timestamp());

            obj.FillHistogram("coinc", "GammaEnergy",
                    4000, 0, 4000, s_energy);

            obj.FillHistogram("coinc", "GammaEnergy_v_angle",
                    4000, 0, 4000, s_energy,
                    90, 0, 180, (180/3.1415926)*angle);

            //obj.FillHistogram("coinc", Form("GammaEnergy_angle_Det%i",s_hit.GetDetnum()),
            //                  4000, 0, 4000, s_energy,
            //                  90, 0, 180, (180/3.1415926)*angle);

           double s_energy_dc = s_hit.GetDoppler(0.078, j_pos); 

            obj.FillHistogram("coinc", "GammaEnergyDC",
                    4000, 0, 4000, s_energy_dc);

            obj.FillHistogram("coinc", "GammaEnergyDC_v_angle",
                    4000, 0, 4000, s_energy_dc,
                    180, 0, 180, (180/3.1415926)*angle);

            if (s_energy_dc > 399 && s_energy_dc < 413){
                obj.FillHistogram("coinc","RingEnergy_v_Angle_gated_399_413",
                        200,0,200,j_hit.GetPosition().Theta()*(180./TMath::Pi()),
                        3000,0,30000,j_hit.GetEnergy());
                obj.FillHistogram("coinc","SectorEnergy_v_Angle_gated_399_413",
                        200,0,200,j_hit.GetPosition().Theta()*(180./TMath::Pi()),
                        3000,0,30000,j_hit.GetBackHit().GetEnergy());
            }
            //obj.FillHistogram("coinc", Form("GammaEnergyDC_angle_Det%i",s_hit.GetDetnum()),
            //                  4000, 0, 4000, s_hit.GetDoppler(0.078,j_pos),
            //                 180, 0, 180, (180/3.1415926)*angle);


            obj.FillHistogram("coinc", "GammaEnergy_v_tdiff",
                    1000, -500, 500, tdiff,
                    4000, 0, 4000, s_energy);

            //obj.FillHistogram("coinc", Form("GammaEnergy_v_tdiff_Det%i",s_hit.GetDetnum()),
            //                  1000, -500, 500, tdiff,
            //                  4000, 0, 4000, s_energy);

            for(double beta : BinCenters(60, 0.0350, 0.0950)) {

                obj.FillHistogram("coinc", "GammaEnergyDC_betascan",
                        60, 0.0350, 0.0950, beta,
                        4000, 0, 4000, s_hit.GetDoppler(beta, j_pos));

                //obj.FillHistogram("coinc", Form("GammaEnergyDC_betascan_det%i", s_hit.GetDetnum()),
                //                 60, 0.0350, 0.0950, beta,
                //                 4000, 0, 4000, s_hit.GetDoppler(beta, j_pos));
            }

            for(double pos : BinCenters(120, -10, 20)) {

                obj.FillHistogram("coinc", "GammaEnergyDC_beta078_zscan",
                        120, -10, 20, pos,
                        4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(0,0,pos)));

                /*
                   obj.FillHistogram("coinc", "GammaEnergyDC_beta078_yscan",
                   120, -10, 20, pos,
                   4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(0,pos,0)));

                   obj.FillHistogram("coinc", "GammaEnergyDC_beta078_xscan",
                   120, -10, 20, pos,
                   4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(pos,0,0)));
                   */

            }
        }
    }
} 



void MakeGatedCoincidenceHistograms(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus, GCutG *tcut, GCutG* scattered) {

    std::string dirname = "coinc_tcut_scatteredRb";
    for(auto& j_hit : janus.GetAllHits()) {
        for(unsigned int i=0; i<sega.Size(); i++) {
            TSegaHit& s_hit = sega.GetSegaHit(i);

            double s_energy = s_hit.GetEnergy();
            auto tdiff = (s_hit.Timestamp()*1.25 - j_hit.Timestamp());
            auto j_pos = j_hit.GetPosition();
            if (!tcut->IsInside(tdiff, s_energy) || 
                !scattered->IsInside(j_pos.Theta()*(180./TMath::Pi()), j_hit.GetEnergy())){
                continue;
            }

            auto s_pos = s_hit.GetPosition();

            auto angle = s_pos.Angle(j_pos);


            obj.FillHistogram(dirname, "GammaEnergy",
                    4000, 0, 4000, s_energy);

            obj.FillHistogram(dirname, "GammaEnergy_v_angle",
                    4000, 0, 4000, s_energy,
                    90, 0, 180, (180/3.1415926)*angle);

            //obj.FillHistogram(dirname, Form("GammaEnergy_angle_Det%i",s_hit.GetDetnum()),
            //                  4000, 0, 4000, s_energy,
            //                  90, 0, 180, (180/3.1415926)*angle);


           double s_energy_dc = s_hit.GetDoppler(0.078, j_pos); 
            obj.FillHistogram(dirname, "GammaEnergyDC",
                    4000, 0, 4000, s_energy_dc);

            obj.FillHistogram(dirname, "GammaEnergyDC_v_angle",
                    4000, 0, 4000, s_energy_dc,
                    180, 0, 180, (180/3.1415926)*angle);

            //obj.FillHistogram(dirname, Form("GammaEnergyDC_angle_Det%i",s_hit.GetDetnum()),
            //                  4000, 0, 4000, s_hit.GetDoppler(0.078,j_pos),
            //                 180, 0, 180, (180/3.1415926)*angle);


            obj.FillHistogram(dirname, "GammaEnergy_v_tdiff",
                    1000, -500, 500, tdiff,
                    4000, 0, 4000, s_energy);

            //obj.FillHistogram(dirname, Form("GammaEnergy_v_tdiff_Det%i",s_hit.GetDetnum()),
            //                  1000, -500, 500, tdiff,
            //                  4000, 0, 4000, s_energy);
            if (s_energy_dc > 399 && s_energy_dc < 413){
                obj.FillHistogram("coinc","RingEnergy_v_Angle_gated_399_413",
                        200,0,200,j_hit.GetPosition().Theta()*(180./TMath::Pi()),
                        3000,0,30000,j_hit.GetEnergy());
                obj.FillHistogram("coinc","SectorEnergy_v_Angle_gated_399_413",
                        200,0,200,j_hit.GetPosition().Theta()*(180./TMath::Pi()),
                        3000,0,30000,j_hit.GetBackHit().GetEnergy());
            }
            for(double beta : BinCenters(60, 0.0350, 0.0950)) {

                obj.FillHistogram(dirname, "GammaEnergyDC_betascan",
                        60, 0.0350, 0.0950, beta,
                        4000, 0, 4000, s_hit.GetDoppler(beta, j_pos));

                //obj.FillHistogram(dirname, Form("GammaEnergyDC_betascan_det%i", s_hit.GetDetnum()),
                //                 60, 0.0350, 0.0950, beta,
                //                 4000, 0, 4000, s_hit.GetDoppler(beta, j_pos));
            }

            for(double pos : BinCenters(120, -10, 20)) {

                obj.FillHistogram(dirname, "GammaEnergyDC_beta078_zscan",
                        120, -10, 20, pos,
                        4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(0,0,pos)));

                /*
                   obj.FillHistogram(dirname, "GammaEnergyDC_beta078_yscan",
                   120, -10, 20, pos,
                   4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(0,pos,0)));

                   obj.FillHistogram(dirname, "GammaEnergyDC_beta078_xscan",
                   120, -10, 20, pos,
                   4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(pos,0,0)));
                   */

            }
        }
    }
} 

void MakeTCUTGatedCoincidenceHistograms(TRuntimeObjects& obj, TSega& sega, TJanusDDAS& janus, GCutG *tcut) {

    std::string dirname = "coinc_tcut";
    for(auto& j_hit : janus.GetAllHits()) {
        for(unsigned int i=0; i<sega.Size(); i++) {
            TSegaHit& s_hit = sega.GetSegaHit(i);

            double s_energy = s_hit.GetEnergy();
            auto tdiff = (s_hit.Timestamp()*1.25 - j_hit.Timestamp());
            auto j_pos = j_hit.GetPosition();
            if (!tcut->IsInside(tdiff, s_energy)){
                continue;
            }

            auto s_pos = s_hit.GetPosition();

            auto angle = s_pos.Angle(j_pos);


            obj.FillHistogram(dirname, "GammaEnergy",
                    4000, 0, 4000, s_energy);

            obj.FillHistogram(dirname, "GammaEnergy_v_angle",
                    4000, 0, 4000, s_energy,
                    90, 0, 180, (180/3.1415926)*angle);

            //obj.FillHistogram(dirname, Form("GammaEnergy_angle_Det%i",s_hit.GetDetnum()),
            //                  4000, 0, 4000, s_energy,
            //                  90, 0, 180, (180/3.1415926)*angle);


           double s_energy_dc = s_hit.GetDoppler(0.078, j_pos); 
            obj.FillHistogram(dirname, "GammaEnergyDC",
                    4000, 0, 4000, s_energy_dc);

            obj.FillHistogram(dirname, "GammaEnergyDC_v_angle",
                    4000, 0, 4000, s_energy_dc,
                    180, 0, 180, (180/3.1415926)*angle);

            //obj.FillHistogram(dirname, Form("GammaEnergyDC_angle_Det%i",s_hit.GetDetnum()),
            //                  4000, 0, 4000, s_hit.GetDoppler(0.078,j_pos),
            //                 180, 0, 180, (180/3.1415926)*angle);


            obj.FillHistogram(dirname, "GammaEnergy_v_tdiff",
                    1000, -500, 500, tdiff,
                    4000, 0, 4000, s_energy);

            //obj.FillHistogram(dirname, Form("GammaEnergy_v_tdiff_Det%i",s_hit.GetDetnum()),
            //                  1000, -500, 500, tdiff,
            //                  4000, 0, 4000, s_energy);
            if (s_energy_dc > 399 && s_energy_dc < 413){
                obj.FillHistogram("coinc","RingEnergy_v_Angle_gated_399_413",
                        200,0,200,j_hit.GetPosition().Theta()*(180./TMath::Pi()),
                        3000,0,30000,j_hit.GetEnergy());
                obj.FillHistogram("coinc","SectorEnergy_v_Angle_gated_399_413",
                        200,0,200,j_hit.GetPosition().Theta()*(180./TMath::Pi()),
                        3000,0,30000,j_hit.GetBackHit().GetEnergy());
            }
            for(double beta : BinCenters(60, 0.0350, 0.0950)) {

                obj.FillHistogram(dirname, "GammaEnergyDC_betascan",
                        60, 0.0350, 0.0950, beta,
                        4000, 0, 4000, s_hit.GetDoppler(beta, j_pos));

                //obj.FillHistogram(dirname, Form("GammaEnergyDC_betascan_det%i", s_hit.GetDetnum()),
                //                 60, 0.0350, 0.0950, beta,
                //                 4000, 0, 4000, s_hit.GetDoppler(beta, j_pos));
            }

            for(double pos : BinCenters(120, -10, 20)) {

                obj.FillHistogram(dirname, "GammaEnergyDC_beta078_zscan",
                        120, -10, 20, pos,
                        4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(0,0,pos)));

                /*
                   obj.FillHistogram(dirname, "GammaEnergyDC_beta078_yscan",
                   120, -10, 20, pos,
                   4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(0,pos,0)));

                   obj.FillHistogram(dirname, "GammaEnergyDC_beta078_xscan",
                   120, -10, 20, pos,
                   4000, 0, 4000, s_hit.GetDoppler(0.078, j_pos,TVector3(pos,0,0)));
                   */

            }
        }
    }
} 

