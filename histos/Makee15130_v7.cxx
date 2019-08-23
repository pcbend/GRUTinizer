#include "TRuntimeObjects.h"

#include <map>
#include <iostream>
#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>
#include <TLine.h>

#include "TGretina.h"
#include "TBank88.h"
#include "TUML.h"

#include "TChannel.h"
#include "GValue.h"
#include "GCutG.h"


//quads as of June 2019.
#define Q1 15
#define Q2 7
#define Q3 11
#define Q4 16
#define Q5 8
#define Q6 14
#define Q7 12
#define Q8 17
#define Q9 19
#define Q10 6
#define Q11 9
//#define Q12 20

//#define BETA .37

std::map<int,int> HoleQMap;
std::map<int,std::string> LayerMap;
std::vector<float> w190e;

bool map_inited=false;

bool w190_inited = false;

GCutG *e1_e2=0;
GCutG *e1_e3=0;
GCutG *HeLike=0;
std::vector<GCutG*> pid_gates;
std::vector<GCutG*> zpid_gates;
GCutG* pin1imp_gate=0;
GCutG* gg_time=0;
GCutG* ta188cut=0;


bool gates_set = false;

std::multimap<long, TGretinaHit> gretina_map;
std::multimap<long, TUML> uml_map;
std::multimap<long, TMode3Hit> bank88_map;

void InitMap() {
    HoleQMap[Q1] = 1;
    HoleQMap[Q2] = 2;
    HoleQMap[Q3] = 3;
    HoleQMap[Q4] = 4;
    HoleQMap[Q5] = 5;
    HoleQMap[Q6] = 6;
    HoleQMap[Q7] = 7;
    HoleQMap[Q8] = 8;
    HoleQMap[Q9] = 9;
    HoleQMap[Q10] = 10;
    HoleQMap[Q11] = 11;
    //  HoleQMap[Q12] = 12;
    LayerMap[0] = "alpha";
    LayerMap[1] = "beta";
    LayerMap[2] = "gamma";
    LayerMap[3] = "delta";
    LayerMap[4] = "epsilon";
    LayerMap[5] = "phi";
}



void HandleUML(TRuntimeObjects& obj) {

    TUML *uml = obj.GetDetector<TUML>();

    for(unsigned int i=0; i<uml->fSssd.size(); i++) {
        TUMLHit hit = uml->fSssd.at(i);
        if(hit.Charge()>10)
            obj.FillHistogram("strips",20,0,20,hit.GetChannel()-15); // 1 -16?
        obj.FillHistogram("strips_aligned",6400,0,64000,hit.GetEnergy(),
                          20,0,20,hit.GetChannel()-15); // 1 -16?
        obj.FillHistogram("strips_charge",6400,0,64000,hit.Charge(),
                          20,0,20,hit.GetChannel()-15); // 1 -16?
    }

    for(unsigned int i=0; i<uml->Size(); i++) {
        TUMLHit hit = uml->GetUMLHit(i);
        if(hit.Charge()>5.0) {
            obj.FillHistogram("uml_summary",6400,0,64000,hit.Charge(),
                              40,0,40,hit.GetChannel());
        }
    }

    if(uml->gamma_time>10 && uml->GetPin1().Timestamp()>10) {
        double delta = uml->GetPin1().Timestamp()-uml->gamma_time;
        //std::cout << "delta    " << delta << std::endl;
        //std::cout << "energy   " << uml->gamma_time << std::endl;
        // std::cout << "pin1     " << uml->GetPin1().Timestamp() << std::endl;
        // std::cout << "gt       " << uml->gamma_time << std::endl;
        obj.FillHistogram("ddas_gamma_time",1000,-500,500,delta,
                          4000,0,10000,uml->gamma_energy);
        obj.FillHistogram("ddas_gamma_with_particle",10000,0,10000,uml->gamma_energy);

    }


    //return;


    //if(uml->GetTof() <300) return;
    //if(uml->GetBeta() < 0.3)  return;
    //if(uml->GetBeta() > 0.9999999)  return;

    std::string histname = "";
    std::string dirname  = "";

// for(unsigned int i=0;i<uml->Size();i++) {
//   TUMLHit hit = uml->GetUMLHit(i);
//   if(hit.Charge()>5.0) {
//     obj.FillHistogram("uml_summary",6400,0,64000,hit.Charge(),
//         40,0,40,hit.GetChannel());
//   }
// }
    dirname = "pid";

    //if(e1_e2 && e1_e2->IsInside(uml->GetPin1().GetEnergy(),uml->GetPin2().GetEnergy())) {


    obj.FillHistogram(dirname,"pin1_tof",1000,300,500,uml->GetTof(),
                      1000,700,1500,uml->GetPin1().GetEnergy());

    obj.FillHistogram(dirname,"pin1_tac",1000,0,300,uml->GetTac1()/1000.,
                      750,0,1500,uml->GetPin1().GetEnergy());

    obj.FillHistogram(dirname,"pin2_tof",1000,300,500,uml->GetTof(),
                      1000,700,1500,uml->GetPin2().GetEnergy());

    obj.FillHistogram(dirname,"tke_tof",1000,300,500,uml->GetTof(),
                      1000,10000,17000,uml->GetTKE());

    obj.FillHistogram(dirname,"dE_tke",1000,10000,17000,uml->GetTKE(),
                      700,5000,12000,uml->GetPin1().GetEnergy()+uml->GetPin2().GetEnergy()+uml->GetSssdEnergy());


    obj.FillHistogram(dirname,"sssdi_tof",1000,300,500,uml->GetTof(),
                      740,0,7400,uml->GetSssdEnergy());

    obj.FillHistogram(dirname,"pin1_pin2",750,0,1500,uml->GetPin1().GetEnergy(),
                      750,0,1500,uml->GetPin2().GetEnergy());

    obj.FillHistogram(dirname,"pin1_sssd",750,0,1500,uml->GetPin1().GetEnergy(),
                      640,0,6400,uml->GetSssdEnergy());

    obj.FillHistogram(dirname,"pin1_imp",750,0,1500,uml->GetPin1().GetEnergy(),
                      700,0,7000,uml->GetImplant().GetEnergy());

    obj.FillHistogram(dirname,"pin1_tke",750,0,1500,uml->GetPin1().GetEnergy(),
                      1000,7000,17000,uml->GetTKE());

    obj.FillHistogram(dirname,"Z_AoQ",1000,2,3,uml->GetAoQ(),
                      1000,0,100,uml->GetZ());

    obj.FillHistogram(dirname,"X_AoQ",1000,2,3,uml->GetAoQ(),
                      20,-50,50,uml->GetXPosition());

    obj.FillHistogram(dirname,"Z_ZmQ",18000,-10,8,uml->ZmQ(),
                      1000,0,100,uml->GetZ());

    obj.FillHistogram(dirname,"Z_Am3Q",10000,-50,0,uml->Am3Q(),
                      1000,0,100,uml->GetZ());

    if(e1_e2 && e1_e3 && e1_e2 -> IsInside(uml->GetPin1().GetEnergy(),uml->GetPin2().GetEnergy()) && HeLike && HeLike->IsInside(uml->ZmQ(),uml->GetZ())
            && e1_e3 -> IsInside(uml->GetPin1().GetEnergy(),uml->GetSssdEnergy()) ) {
        obj.FillHistogram(dirname,"dE_tke_gated",1000,10000,17000,uml->GetTKE(),
                          700,5000,12000,uml->GetPin1().GetEnergy()+uml->GetPin2().GetEnergy()+uml->GetSssdEnergy());
        obj.FillHistogram(dirname,"Z_Am3Q_gated",1000,-50,0,uml->Am3Q(),
                          1000,0,100,uml->GetZ());

    }
    //}
    //std::cout << "tof:  " << uml->GetTof() << std::endl;
    dirname = "uml";
    obj.FillHistogram(dirname,"SSSD",6400,0,6400,uml->GetSssdEnergy());
    obj.FillHistogram(dirname,"PIN1_cal",1500,0,1500,uml->GetPin1().GetEnergy());
    obj.FillHistogram(dirname,"PIN2_cal",1500,0,1500,uml->GetPin2().GetEnergy());
    obj.FillHistogram(dirname,"IMPLANT_cal",17000,0,17000,uml->GetImplant().GetEnergy());




    obj.FillHistogram(dirname,"PIN1",16000,0,64000,uml->GetPin1().Charge());
    obj.FillHistogram(dirname,"PIN2",16000,0,64000,uml->GetPin2().Charge());
    obj.FillHistogram(dirname,"IMPLANT",16000,0,64000,uml->GetImplant().Charge());

    obj.FillHistogram(dirname,"TKE",1700,0,17000,uml->GetTKE());


    //obj.FillHistogram(dirname,"tac1",2000,0,10000,uml->GetTof(),

    obj.FillHistogram(dirname,"AoQ",1000,0,3,uml->GetAoQ());
    obj.FillHistogram(dirname,"Z",1000,0,100,uml->GetZ());
    obj.FillHistogram(dirname,"Q",1000,0,100,uml->GetQ());
    obj.FillHistogram(dirname,"A",2500,0,250,uml->A());
    obj.FillHistogram(dirname,"Brho",1000,0,5,uml->GetBrho());
    obj.FillHistogram(dirname,"Beta",1000,0,1,uml->GetBeta());

    obj.FillHistogram(dirname,"Xposition",100,-50,50,uml->GetXPosition());

// for(int i=0;i<uml->fSssd.size();i++) {
//   TUMLHit hit = uml->fSssd.at(i);
//   if(hit.Charge()>10)
//     obj.FillHistogram("strips",20,0,20,hit.GetChannel()-15); // 1 -16?
//   obj.FillHistogram("strips_aligned",6400,0,64000,hit.GetEnergy(),
//       20,0,20,hit.GetChannel()-15); // 1 -16?
// }

    dirname = "diag";

    obj.FillHistogram(dirname,"DT_Pin1_Xfp1",1000,-500,500,
                      uml->GetPin1().Timestamp() - uml->GetXfp1().Timestamp());
    obj.FillHistogram(dirname,"DT_Pin1_Xfp2",1000,-500,500,
                      uml->GetPin1().Timestamp() - uml->GetXfp2().Timestamp());

    if(uml->GetImplant().GetEnergy()>200) // 259 from beta spectra
    uml_map.insert(std::make_pair(uml->Timestamp(),*uml));

}


void HandleGretina(TRuntimeObjects& obj) {
    TGretina *gretina = obj.GetDetector<TGretina>();
    TBank88  *bank88 = obj.GetDetector<TBank88>();
    long xfp_time = -1;
    bool  pin1_fired = false;
    if(bank88) {
        TMode3Hit tmp_hit;
        for(unsigned int i=0; i<bank88->Size(); i++) {
            TMode3Hit hit = bank88->GetMode3Hit(i);
            if(hit.GetChannel()==0) {
                xfp_time = hit.Timestamp();
                tmp_hit = hit;
            }
            if(hit.GetChannel()==9)
                pin1_fired = true;
        }

    }

    double sum =0.0;
    std::string dirname  = "gretina";
    gretina->CleanHits();
    for(unsigned int i=0; i<gretina->Size(); i++) {
        TGretinaHit hit = gretina->GetGretinaHit(i);
        gretina_map.insert(std::make_pair(hit.Timestamp(),hit));
        obj.FillHistogram(dirname,"summary",2000,0,4000,hit.GetCoreEnergy(),
                          200,0,200,hit.GetCrystalId());

        obj.FillHistogram(dirname,"singles",8000,0,8000,hit.GetCoreEnergy());
        for(unsigned int j=i+1; j<gretina->Size(); j++) {
            TGretinaHit hit2 = gretina->GetGretinaHit(j);
            double dtime=0.0;
            double eng   =0.0;
            if(hit.GetCoreEnergy()<hit2.GetCoreEnergy()) {
                dtime  = hit2.GetTime() - hit.GetTime();
                eng    = hit.GetCoreEnergy();
            } else {
                dtime  = hit.GetTime() - hit2.GetTime();
                eng    = hit2.GetCoreEnergy();
            }
            obj.FillHistogram(dirname,"gg_time",1000,-500,500,dtime,
                              1000,0,4000,eng);

            if(gg_time && gg_time->IsInside(dtime,eng)) {
                obj.FillHistogramSym(dirname,"gg_matrix",4000,0,2000,hit.GetCoreEnergy(),
                                     4000,0,2000,hit2.GetCoreEnergy());
                if(pin1_fired) {
                    obj.FillHistogramSym(dirname,"gg_matrix_w88",4000,0,2000,hit.GetCoreEnergy(),
                                         4000,0,2000,hit2.GetCoreEnergy());
                } else {
                    obj.FillHistogramSym(dirname,"gg_matrix_no88",4000,0,2000,hit.GetCoreEnergy(),
                                         4000,0,2000,hit2.GetCoreEnergy());
                }

            }


        }

        if(bank88 && bank88->Timestamp()>0) {
            obj.FillHistogram(dirname,"singles_w88",8000,0,8000,hit.GetCoreEnergy());
            obj.FillHistogram(dirname,"gretinahits_bank88_energy",1000,-500,500,bank88->Timestamp()-hit.GetTime(),
                              2000,0,2000,hit.GetCoreEnergy());
            obj.FillHistogram(dirname,"gretinahits_bank88_time",3600,0,3600,hit.Timestamp()/1e8,
                              1000,-500,500,bank88->Timestamp()-hit.GetTime());

            if(xfp_time>0) {
                obj.FillHistogram(dirname,"gretinahits_bank88_xfp_energy",1000,-500,500,xfp_time-hit.GetTime(),
                                  2000,0,2000,hit.GetCoreEnergy());
                obj.FillHistogram(dirname,"gretinahits_bank88_xfp",3600,0,3600,hit.Timestamp()/1e8,
                                  1000,-500,500,xfp_time-hit.GetTime());
            }
        } else {
            obj.FillHistogram(dirname,"singles_no88",8000,0,8000,hit.GetCoreEnergy());
        }
        sum += hit.GetCoreEnergy();
    }
    obj.FillHistogram(dirname,"gretina_sum",8000,0,4000,sum);

    dirname ="clusters";

    gretina->BuildClusters();
    for(int i=0; i<gretina->ClusterSize(); i++) {
        TCluster hit = gretina->GetCluster(i);
        obj.FillHistogram(dirname,"cluster_singles",8000,0,4000,hit.GetEnergy());
        if(pin1_fired) {
            obj.FillHistogram(dirname,"cluster_singles_w88",8000,0,4000,hit.GetEnergy());
        } else {
            obj.FillHistogram(dirname,"cluster_singles_no88",8000,0,4000,hit.GetEnergy());
        }
        for(int j=i+1; j<gretina->ClusterSize(); j++) {
            TCluster hit2 = gretina->GetCluster(j);
            double dtime=0.0;
            double eng   =0.0;
            if(hit.GetEnergy()<hit2.GetEnergy()) {
                dtime  = hit2.GetTime() - hit.GetTime();
                eng    = hit.GetEnergy();
            } else {
                dtime  = hit.GetTime() - hit2.GetTime();
                eng    = hit2.GetEnergy();
            }
            obj.FillHistogram(dirname,"cc_time",1000,-500,500,dtime,
                              1000,0,4000,eng);



            if(gg_time && gg_time->IsInside(dtime,eng)) {
                obj.FillHistogramSym(dirname,"cc_matrix",4000,0,2000,hit.GetEnergy(),
                                     4000,0,2000,hit2.GetEnergy());
                if(pin1_fired) {
                    obj.FillHistogramSym(dirname,"cc_matrix_w88",4000,0,2000,hit.GetEnergy(),
                                         4000,0,2000,hit2.GetEnergy());
                } else {
                    obj.FillHistogramSym(dirname,"cc_matrix_no88",4000,0,2000,hit.GetEnergy(),
                                         4000,0,2000,hit2.GetEnergy());
                }
            }
        }

    }


}


void HandleDDAS_GRETINA(TRuntimeObjects& obj) {

    TGretina *gretina = obj.GetDetector<TGretina>();
    TUML     *uml     = obj.GetDetector<TUML>();


//  if(uml->GetTof() <300) return;
//  if(uml->GetBeta() < 0.3)  return;
//  if(uml->GetBeta() > 0.9999999)  return;


    double sum =0.0;

    std::string dirname  = "gretina_particle";

    for(unsigned int i=0; i<gretina->Size(); i++) {
        TGretinaHit hit = gretina->GetGretinaHit(i);
        obj.FillHistogram(dirname,"summary",2000,0,4000,hit.GetCoreEnergy(),
                          200,0,200,hit.GetCrystalId());
        sum += hit.GetCoreEnergy();
    }
    obj.FillHistogram(dirname,"gretina_sum",8000,0,4000,sum);


    for(unsigned int i=0; i<pid_gates.size(); i++) {
        GCutG *gate = pid_gates.at(i);
//    std::cout << gate->GetName() << "\t" << gate->GetTag() << std::endl;
        if(!gate->IsInside(uml->GetTof(),uml->GetTKE())) continue;
//    if(!gate->IsInside(uml->GetTof(),uml->GetSssdEnergy())) continue;
        for(unsigned int x=0; x<gretina->Size(); x++) {
            TGretinaHit hit = gretina->GetGretinaHit(x);
            obj.FillHistogram(dirname,Form("gamma_%s",gate->GetName()),8000,0,8000,hit.GetCoreEnergy());
            double deltatime = uml->Timestamp() - hit.Timestamp();
            obj.FillHistogram(dirname,Form("dt_%s",gate->GetName()),500,-3000,3000,deltatime,
                              4000,0,8000,hit.GetCoreEnergy());
        }
        obj.FillHistogram(dirname,Form("dE_tke_%s",gate->GetName()),1000,10000,17000,uml->GetTKE(),
                          600,3000,9000,uml->GetPin1().GetEnergy()+uml->GetPin2().GetEnergy()+uml->GetSssdEnergy());

    }


    /*
      for(unsigned int i=0;i<zpid_gates.size();i++) {
        GCutG *gate = zpid_gates.at(i);
    //    std::cout << gate->GetName() << "\t" << gate->GetTag() << std::endl;
    //    if(!gate->IsInside(uml->GetTof(),uml->GetTKE())) continue;
        double xvalue =uml->GetTKE();
        double yvalue=uml->GetPin1().GetEnergy()+uml->GetPin2().GetEnergy()+uml->GetSssdEnergy();
    //
    //
        if(!gate->IsInside(xvalue,yvalue)) continue; //  uml->GetTof(),uml->GetSssdEnergy())) continue;
        for(unsigned int x=0;x<gretina->Size();x++) {
          TGretinaHit hit = gretina->GetGretinaHit(x);
          obj.FillHistogram(dirname,Form("gamma_%s",gate->GetName()),8000,0,8000,hit.GetCoreEnergy());
          double deltatime = uml->Timestamp() - hit.Timestamp();
          obj.FillHistogram(dirname,Form("dt_%s",gate->GetName()),6000,-3000,3000,deltatime,
                                                                  4000,0,8000,hit.GetCoreEnergy());
        }

        obj.FillHistogram(dirname,Form("tke_tof_%s",gate->GetName()),1000,300,500,uml->GetTof(),
            1000,10000,17000,uml->GetTKE());

      }

    */

}


void SetGates(TRuntimeObjects &obj) {

    if(gates_set) return;

    TList *gates = &(obj.GetGates());
    TIter iter(gates);
    while(TObject *key = iter.Next()) {
        GCutG *gate = (GCutG*)key;
        if(strcmp(gate->GetTag(),"zpid")==0) {
            //std::cout << gate->GetName() << "\t" << gate->GetTag() << std::endl;
            pid_gates.push_back(gate);
        }
//    if(strcmp(gate->GetTag(),"zpid")==0) {
//      //std::cout << gate->GetName() << "\t" << gate->GetTag() << std::endl;
//      zpid_gates.push_back(gate);
//    }
        if(strcmp(gate->GetName(),"e1_e2")==0) {
            e1_e2 = gate;
        }
        if(strcmp(gate->GetName(),"e1_e3")==0) {
            e1_e3 = gate;
        }
        if(strcmp(gate->GetName(),"pin1_imp")==0) {
            pin1imp_gate = gate;
        }
        if(strcmp(gate->GetName(),"ta188cut")==0) {
            ta188cut = gate;
        }
        if(strcmp(gate->GetName(),"HeLike")==0) {
            HeLike = gate;
        }
	
	
    }
    gates_set=true;
    std::cout<<"size of pid_gates: "<<pid_gates.size()<<std::endl;
    return;
}



void pid_tagging(std::vector<std::multimap<long, TGretinaHit>::iterator> & store_ghit, TUML *uml,TRuntimeObjects &obj) {
    if(uml == NULL) return;
    if(store_ghit.size()<1) return;
    if(ta188cut && !ta188cut->IsInside(uml->Am3Q(),uml->GetZ())) return;

    long fts = uml->Timestamp();
    long twin = 1000;
    if(!w190_inited) {
//        w190e.push_back(207);
//        w190e.push_back(357);
//        w190e.push_back(484);
//        w190e.push_back(594);
//        w190e.push_back(694);
        w190e.push_back(291.9);
        w190_inited = true;
    }
    for(size_t i = 0 ; i < store_ghit.size() ; i++) {
        if (store_ghit.at(i)->first < fts - 100 || store_ghit.at(i)->first - fts> twin) continue;

        float en = store_ghit.at(i)->second.GetCoreEnergy();
        auto il = std::lower_bound(w190e.begin(),w190e.end(),en);
        auto ih = std::upper_bound(w190e.begin(),w190e.end(),en);
        if(fabs(*il - en) > 2 && fabs(*ih -en)>2 ) continue;
        std::string dirname = "pid_gated_m1";
        obj.FillHistogram(dirname,"pin1_tof",1000,300,500,uml->GetTof(),
                          1000,700,1500,uml->GetPin1().GetEnergy());

        obj.FillHistogram(dirname,"pin1_tac",1000,0,300,uml->GetTac1()/1000.,
                          750,0,1500,uml->GetPin1().GetEnergy());

        obj.FillHistogram(dirname,"pin2_tac",1000,0,300,uml->GetTac2()/1000.,
                          750,0,1500,uml->GetPin2().GetEnergy());

        obj.FillHistogram(dirname,"pin2_tof",1000,300,500,uml->GetTof(),
                          1000,700,1500,uml->GetPin2().GetEnergy());

        obj.FillHistogram(dirname,"tke_tof",1000,300,500,uml->GetTof(),
                          1000,10000,17000,uml->GetTKE());

        obj.FillHistogram(dirname,"dE_tke",1000,10000,17000,uml->GetTKE(),
                          700,5000,12000,uml->GetPin1().GetEnergy()+uml->GetPin2().GetEnergy()+uml->GetSssdEnergy());

        obj.FillHistogram(dirname,"sssdi_tof",1000,300,500,uml->GetTof(),
                          740,0,7400,uml->GetSssdEnergy());

        obj.FillHistogram(dirname,"pin1_pin2",750,0,1500,uml->GetPin1().GetEnergy(),
                          750,0,1500,uml->GetPin2().GetEnergy());

        obj.FillHistogram(dirname,"pin1_sssd",750,0,1500,uml->GetPin1().GetEnergy(),
                          640,0,6400,uml->GetSssdEnergy());

        obj.FillHistogram(dirname,"pin1_imp",750,0,1500,uml->GetPin1().GetEnergy(),
                          700,0,7000,uml->GetImplant().GetEnergy());

        obj.FillHistogram(dirname,"pin1_tke",750,0,1500,uml->GetPin1().GetEnergy(),
                          1000,7000,17000,uml->GetTKE());

        obj.FillHistogram(dirname,"Z_AoQ",1000,2,3,uml->GetAoQ(),
                          1000,0,100,uml->GetZ());

        obj.FillHistogram(dirname,"X_AoQ",1000,2,3,uml->GetAoQ(),
                          20,-50,50,uml->GetXPosition());

        obj.FillHistogram(dirname,"Z_ZmQ",18000,-10,8,uml->ZmQ(),
                          1000,0,100,uml->GetZ());

        obj.FillHistogram(dirname,"Z_Am3Q",10000,-50,0,uml->Am3Q(),
                          1000,0,100,uml->GetZ());

    }

    return;
    if(store_ghit.size()<2) return;
    for(size_t i = 0 ; i < store_ghit.size() ; i++) {
        if (store_ghit.at(i)->first < fts || store_ghit.at(i)->first - fts> twin) continue;
        size_t k = 0;
        for( k = i+1 ; k < store_ghit.size() ; k++) {
            if(store_ghit.at(k)->first - store_ghit.at(i)->first > 20 || store_ghit.at(k)->first - fts > twin) {
                break;
            }
        }
        if(k-i<2) {
            i = k + 1;
            continue;
        }
        int fillflag = 0;
        int mult = k-i;

        for(int kk = 0 ; kk < mult; kk++) {
            float en = store_ghit.at(kk)->second.GetCoreEnergy();
            auto il = std::lower_bound(w190e.begin(),w190e.end(),en);
            auto ih = std::upper_bound(w190e.begin(),w190e.end(),en);
            if(fabs((*il)-en) < 2 || fabs((*ih)-en) < 2){
                fillflag ++;
            }
        }
        if(fillflag < 2) return;
        std::string dirname = "pid_gated_m2";
        obj.FillHistogram(dirname,"pin1_tof",1000,300,500,uml->GetTof(),
                          1000,700,1500,uml->GetPin1().GetEnergy());

        obj.FillHistogram(dirname,"pin1_tac",1000,0,300,uml->GetTac1()/1000.,
                          750,0,1500,uml->GetPin1().GetEnergy());

        obj.FillHistogram(dirname,"pin2_tof",1000,300,500,uml->GetTof(),
                          1000,700,1500,uml->GetPin2().GetEnergy());

        obj.FillHistogram(dirname,"tke_tof",1000,300,500,uml->GetTof(),
                          1000,10000,17000,uml->GetTKE());

        obj.FillHistogram(dirname,"dE_tke",1000,10000,17000,uml->GetTKE(),
                          700,5000,12000,uml->GetPin1().GetEnergy()+uml->GetPin2().GetEnergy()+uml->GetSssdEnergy());


        obj.FillHistogram(dirname,"sssdi_tof",1000,300,500,uml->GetTof(),
                          740,0,7400,uml->GetSssdEnergy());

        obj.FillHistogram(dirname,"pin1_pin2",750,0,1500,uml->GetPin1().GetEnergy(),
                          750,0,1500,uml->GetPin2().GetEnergy());

        obj.FillHistogram(dirname,"pin1_sssd",750,0,1500,uml->GetPin1().GetEnergy(),
                          640,0,6400,uml->GetSssdEnergy());

        obj.FillHistogram(dirname,"pin1_tke",750,0,1500,uml->GetPin1().GetEnergy(),
                          1000,7000,17000,uml->GetTKE());

        obj.FillHistogram(dirname,"Z_AoQ",1000,2,3,uml->GetAoQ(),
                          1000,0,100,uml->GetZ());

        obj.FillHistogram(dirname,"X_AoQ",1000,2,3,uml->GetAoQ(),
                          20,-50,50,uml->GetXPosition());

        obj.FillHistogram(dirname,"Z_ZmQ",18000,-10,8,uml->ZmQ(),
                          1000,0,100,uml->GetZ());

        obj.FillHistogram(dirname,"Z_Am3Q",10000,-50,0,uml->Am3Q(),
                          1000,0,100,uml->GetZ());

    }
}


void add_back(std::vector<std::multimap<long,TGretinaHit>::iterator> &store_ghit, int index, int size,const char* name,  TRuntimeObjects &obj){
   std::vector<float> en;
   bool *fill = new bool[size];
   memset(fill,0,sizeof(bool)*size);
   for(int i = 0; i< size; i++){
     if(fill[i] == true) continue;
     TGretinaHit hit1 = store_ghit.at(index+i)->second;
     if(i == size - 1){
       en.push_back(hit1.GetCoreEnergy());
       continue;
     }
     float angle = 100;
     int idx = -1;
     
     for(int j = i + 1; j < size; j++){
        if(fill[j]==true) continue;
        TGretinaHit hit2 = store_ghit.at(index+j)->second;
        float tang;
        if(hit1.GetCoreEnergy() > hit2.GetCoreEnergy())
          tang = hit1.GetIntPosition(hit1.NumberOfInteractions()-1).Angle(hit2.GetIntPosition(0));
        else
          tang = hit2.GetIntPosition(hit2.NumberOfInteractions()-1).Angle(hit1.GetIntPosition(0));
        if(tang < angle){
          angle = tang;
          idx = j;
        }
     }

     if(angle < 22 * TMath::DegToRad()){
        en.push_back(hit1.GetCoreEnergy()+store_ghit.at(index+idx)->second.GetCoreEnergy());
        fill[idx] = true;
     } else{
        en.push_back(hit1.GetCoreEnergy());
     }
     fill[i] = true;
   }

   for( size_t ii = 0 ; ii<en.size() - 1; ii++)
     for(size_t jj = ii+1; jj<en.size(); jj++)
       obj.FillHistogramSym("large_tw",Form("gg_m2_addback_%s",name),2000,0,4000,en.at(ii),2000,0,4000,en.at(jj));
   for(size_t ii = 0 ; ii < en.size(); ii++)
     obj.FillHistogram("large_tw",Form("g_m2_single_%s",name),4000,0,4000,en.at(ii));
   obj.FillHistogram("mul_add",10,0,10,size,20,-10,10,size-en.size());
}

void SearchIosmer(TRuntimeObjects &obj) {
    long twin = 1000000; // ticks,  10,000us
    if(gretina_map.size()==0 || uml_map.size() == 0) return;
    if(gretina_map.rbegin()->first  - uml_map.begin()->first < twin) return;
//  std::cout<<"uml size: "<<uml_map.size()<<std::endl;
//  std::cout<<"gre size: "<<gretina_map.size()<<std::endl;
    std::vector<std::multimap<long, TGretinaHit>::iterator> store_ghit;
    auto itr = gretina_map.rbegin();
    std::string dirname = "large_tw";
    while ( gretina_map.size()> 0 && uml_map.size()>0 && (itr->first - uml_map.begin()->first > twin)) {
        // first, remove gretina events older than bank88 ts - twin
        while(uml_map.begin()->first - gretina_map.begin()->first > twin) {
            gretina_map.erase(gretina_map.begin()) ;
        }

        long fts = uml_map.begin()->first;

        TUML uml = uml_map.begin()->second;
        double xvalue =uml.GetTKE();
        double yvalue=uml.GetPin1().GetEnergy()+uml.GetPin2().GetEnergy()+uml.GetSssdEnergy();
        GCutG *noz1g = 0;

        bool pidflag = false;
        if(e1_e2 == NULL || e1_e3 == NULL || HeLike ==NULL) pidflag = true;
        else if(e1_e2 -> IsInside(uml.GetPin1().GetEnergy(),uml.GetPin2().GetEnergy())
                && e1_e3 -> IsInside(uml.GetPin1().GetEnergy(),uml.GetSssdEnergy()) && HeLike->IsInside(uml.ZmQ(),uml.GetZ())) pidflag = true;
	
	
//    std::cout<<"pidflag: "<<pidflag<<std::endl;
    if(pidflag)
        for( auto it = gretina_map.begin(); it!=gretina_map.end(); it++) {
            if(it->first>fts+twin) break;
            TGretinaHit tmp_hit = it->second;
//            obj.FillHistogram(dirname,"gamma_time_uml",1000,-twin,twin,it->first-fts,2000,0,4000,tmp_hit.GetCoreEnergy());
            obj.FillHistogram(dirname,"gamma_time_uml",1000,-10000,10000,it->first-fts,2000,0,4000,tmp_hit.GetCoreEnergy());
            store_ghit.push_back(it);

            // apply pid gates

            for(size_t ip = 0; ip < zpid_gates.size(); ip++) {
                GCutG *gate = zpid_gates.at(ip);
                if(!(strcmp(gate->GetName(),"not_z1"))) noz1g = gate;
                if(gate->IsInside(xvalue,yvalue)) {
//             obj.FillHistogram(dirname,Form("gamma_energy_uml_%s",gate->GetName()),8000,0,8000,tmp_hit.GetCoreEnergy());
                    double deltatime = tmp_hit.Timestamp()-uml.Timestamp();

                    obj.FillHistogram(dirname,Form("gamma_time(us)_uml_%s",gate->GetName()),1000,-twin/100.,twin/100.,deltatime/100.,
                                      4000,0,8000,tmp_hit.GetCoreEnergy());
                    obj.FillHistogram(dirname,Form("gamma_time(us)_uml_narrow_%s",gate->GetName()),1000,-300,300.,deltatime/100.,
                                      4000,0,8000,tmp_hit.GetCoreEnergy());

                }
            }

            for(size_t ip = 0; ip < pid_gates.size(); ip++) {
                GCutG *gate = pid_gates.at(ip);
                if(gate->IsInside(uml.Am3Q(),uml.GetZ())) {
//             obj.FillHistogram(dirname,Form("gamma_energy_uml_%s",gate->GetName()),8000,0,8000,tmp_hit.GetCoreEnergy());

                    double deltatime = tmp_hit.Timestamp()-uml.Timestamp();
                    obj.FillHistogram(dirname,Form("gamma_time(us)_uml_zcut_%s",gate->GetName()),1000,-twin/100.,twin/100.,deltatime/100.,
                                      4000,0,8000,tmp_hit.GetCoreEnergy());
                    obj.FillHistogram(dirname,Form("gamma_time(us)_uml_narrow_zcut_%s",gate->GetName()),1000,-300,300.,deltatime/100.,
                                      4000,0,8000,tmp_hit.GetCoreEnergy());

                }
            }


        }

        bool *fill = new bool[store_ghit.size()];
        memset(fill,0,sizeof(bool)*(store_ghit.size()));

        // deal m2 events
       if(pidflag)
        if(store_ghit.size()>1)
            for(unsigned int ip = 0; ip < pid_gates.size(); ip++) {
                GCutG *gate = pid_gates.at(ip);
                if(gate->IsInside(uml.Am3Q(),uml.GetZ())) {
                    memset(fill,0,sizeof(bool)*(store_ghit.size()));
                    for(size_t l = 0; l<store_ghit.size()-1; l++) { 
                      if(store_ghit.at(l+1)->first-store_ghit.at(l)->first < 20) {
                            float en1,en2;
                            TGretinaHit tmp_hit = store_ghit.at(l)->second;
                            en1 = tmp_hit.GetCoreEnergy();
                            if(fill[l] == false) {
//                  std::cout<<store_ghit.at(l)->first<<std::endl;
//                  std::cout<<tmp_hit.Timestamp()<<std::endl;
//                  std::cout<<"fts: l  "<<fts<<std::endl;
//                  std::cout<<"fts: l  "<<tmp_hit.Timestamp()- fts<<std::endl;

                                obj.FillHistogram(dirname,Form("gamma_time_uml_m2_%s",gate->GetName()),1000,-twin/100.,twin/100.,(tmp_hit.Timestamp()-fts)/100.,2000,0,4000,en1);
                                obj.FillHistogram(dirname,Form("gamma_time_uml_m2_narrow_%s",gate->GetName()),6000,-300.,300.,(tmp_hit.Timestamp()-fts)/100.,2000,0,4000,en1);
                                fill[l] = true;
                            }
                            tmp_hit = store_ghit.at(l+1)->second;
                            en2 = tmp_hit.GetCoreEnergy();
                            if(fill[l+1] == false) { // this if can be removed, but anyway I keep it JL
//                  std::cout<<store_ghit.at(l+1)->first<<std::endl;
//                  std::cout<<tmp_hit.Timestamp()<<std::endl;
//                  std::cout<<"fts: l+1  "<<fts<<std::endl;
//                  std::cout<<"fts: l+1  "<<tmp_hit.Timestamp()- fts<<std::endl;
                                obj.FillHistogram(dirname,Form("gamma_time_uml_m2_%s",gate->GetName()),1000,-twin/100.,twin/100.,(tmp_hit.Timestamp()-fts)/100.,2000,0,4000,en2);
                                obj.FillHistogram(dirname,Form("gamma_time_uml_m2_narrow_%s",gate->GetName()),6000,-300/100.,300/100.,(tmp_hit.Timestamp()-fts)/100.,2000,0,4000,en2);
                                fill[l+1] = true;
                            }
                        }
                    }
                    size_t l = 0;
                    while(l<store_ghit.size()) {
                        size_t k;
                        float calo_e = store_ghit.at(l)->second.GetCoreEnergy();
                        for(k = l+1; k<store_ghit.size(); k++) {
                            if(store_ghit.at(k)->first - store_ghit.at(l)->first > 20) break;
                            calo_e += store_ghit.at(k)->second.GetCoreEnergy();
                        }
                        int mul = k-l;
                        for(size_t tmp = l; tmp < k ; tmp++) {
                            obj.FillHistogram(dirname,Form("gamma_energy_multi_%s",gate->GetName()),10,0,10,mul,2000,0,4000,store_ghit.at(tmp)->second.GetCoreEnergy());
                        }
                        obj.FillHistogram(dirname,Form("gamma_time_uml_calo_%s",gate->GetName()),1000,-twin/100.,twin/100.,(store_ghit.at(l)->first - fts) / 100., 4000, 0, 8000, calo_e);
                        obj.FillHistogram(dirname,Form("gamma_time_uml_calo_narrow_%s",gate->GetName()),1000,-300,300.,(store_ghit.at(l)->first - fts) / 100., 4000, 0, 8000, calo_e);
                        if(mul > 1){
                          int nwin = 30000;
                          if(!strcmp(gate->GetName(),"Ta188"))
                            nwin = 1000;
                          else if(!strcmp(gate->GetName(),"Ta189"))
                            nwin = 2000;
                          else if(!strcmp(gate->GetName(),"W191"))
                            nwin = 1000;
                           for( int iii = 0; iii < mul -1; iii++)
                             for(int jjj = iii+1; jjj < mul; jjj++){
                              if(store_ghit.at(l)->first - fts > -100 && store_ghit.at(l)->first - fts < nwin) 
                                obj.FillHistogramSym(dirname,Form("gg_m2_sg_%s",gate->GetName()),2000,0,4000,store_ghit.at(l+iii)->second.GetCoreEnergy(),
                                                                                                 2000,0,4000,store_ghit.at(l+jjj)->second.GetCoreEnergy());
                              else
                                obj.FillHistogramSym(dirname,Form("gg_m2_bg_%s",gate->GetName()),2000,0,4000,store_ghit.at(l+iii)->second.GetCoreEnergy(),
                                                                                                 2000,0,4000,store_ghit.at(l+jjj)->second.GetCoreEnergy());
                             }
                          if(store_ghit.at(l)->first - fts > -100 && store_ghit.at(l)->first - fts < nwin) 
                            add_back(store_ghit,l,mul,gate->GetName(),obj);
                        }
                        l = k;
                    }
                }
            }


        if(pidflag)
             if(store_ghit.size()>3)
                for(unsigned int ip = 0; ip < pid_gates.size(); ip++) {
                    GCutG *gate = pid_gates.at(ip);
                    if(gate->IsInside(uml.Am3Q(),uml.GetZ())) {
                        memset(fill,0,sizeof(bool)*(store_ghit.size()));
                        for(size_t l = 0; l<store_ghit.size()-2; l++) {
                            if(store_ghit.at(l+2)->first-store_ghit.at(l)->first < 20) {
                                float en1, en2, en3;
                                TGretinaHit tmp_hit = store_ghit.at(l)->second;
                                en1 = tmp_hit.GetCoreEnergy();
                                if(fill[l] == false) {
                                    obj.FillHistogram(dirname,Form("gamma_time_uml_m3_%s",gate->GetName()),1000,-twin/100,twin/100,(tmp_hit.Timestamp()-fts)/100.,2000,0,4000,en1);
                                    obj.FillHistogram(dirname,Form("gamma_time_uml_m3_%s",gate->GetName()),1000,-300,300,(tmp_hit.Timestamp()-fts)/100.,2000,0,4000,en1);
                                    fill[l] = true;
                                }
                                tmp_hit = store_ghit.at(l+1)->second;
                                en2 = tmp_hit.GetCoreEnergy();
                                if(fill[l+1] == false) {
                                    obj.FillHistogram(dirname,Form("gamma_time_uml_m3_%s",gate->GetName()),1000,-twin/100,twin/100,(tmp_hit.Timestamp()-fts)/100.,2000,0,4000,en2);
                                    obj.FillHistogram(dirname,Form("gamma_time_uml_m3_%s",gate->GetName()),1000,-300,300,(tmp_hit.Timestamp()-fts)/100.,2000,0,4000,en2);
                                    fill[l+1] = true;
                                }
                                tmp_hit = store_ghit.at(l+2)->second;
                                en3 = tmp_hit.GetCoreEnergy();
                                if(fill[l+2] == false) {
                                    obj.FillHistogram(dirname,Form("gamma_time_uml_m3_%s",gate->GetName()),1000,-twin/100,twin/100,(tmp_hit.Timestamp()-fts)/100.,2000,0,4000,en3);
                                    obj.FillHistogram(dirname,Form("gamma_time_uml_m3_%s",gate->GetName()),1000,-300,300,(tmp_hit.Timestamp()-fts)/100.,2000,0,4000,en3);
                                    fill[l+2] = true;
                                }
                            }
                        }
                    size_t l = 0;
                        while(l<store_ghit.size()) {
                            size_t k;
                            float calo_e = store_ghit.at(l)->second.GetCoreEnergy();
                            for(k = l+1; k<store_ghit.size(); k++) {
                                if(store_ghit.at(k)->first - store_ghit.at(l)->first > 20) break;
                                calo_e += store_ghit.at(k)->second.GetCoreEnergy();
                            }
                            int mul = k-l;
                            if(mul > 2){
                               for( int iii = 0; iii < mul -1; iii++)
                                 for(int jjj = iii+1; jjj < mul; jjj++){
                                  if(store_ghit.at(l)->first - fts > -100 && store_ghit.at(l)->first - fts < 30000) 
                                    obj.FillHistogramSym(dirname,Form("gg_m3_sg_%s",gate->GetName()),2000,0,4000,store_ghit.at(l+iii)->second.GetCoreEnergy(),
                                                                                                     2000,0,4000,store_ghit.at(l+jjj)->second.GetCoreEnergy());
                                  else
                                    obj.FillHistogramSym(dirname,Form("gg_m3_bg_%s",gate->GetName()),2000,0,4000,store_ghit.at(l+iii)->second.GetCoreEnergy(),
                                                                                                     2000,0,4000,store_ghit.at(l+jjj)->second.GetCoreEnergy());
                                 }
                            }
                            l = k;
                        }
                    }
                }


        // pid tagging area

        pid_tagging(store_ghit,&uml,obj);
        delete [] fill;
        store_ghit.clear();



        uml_map.erase(uml_map.begin()); // done with the oldest hit in bank 88
    }

}





// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
    InitMap();
    TGretina *gretina = obj.GetDetector<TGretina>();
    TBank88  *bank88  = obj.GetDetector<TBank88>();
    TUML     *uml     = obj.GetDetector<TUML>();

// if(!e1_e2) {
//   TList *gates = &(obj.GetGates());
//   TIter iter(gates);
//   while(TObject *key = iter.Next()) {
//     GCutG *gate = (GCutG*)key;
//     if(!strcmp(gate->GetName(),"e1_e2")) {
//       printf("loaded e1_e2...\n");
//       e1_e2 = gate;
//     }
//   }
// }

    TList    *list    = &(obj.GetObjects());
    int numobj        = list->GetSize();

    std::string histname = "";
    std::string dirname  = "";

    SetGates(obj);
    if(uml) {
        uml->CalParameters();   ///THIS MUST BE CALLED BEFORE PID IS PLOTED OR GATED
        HandleUML(obj);
    }

    if(gretina) {
        HandleGretina(obj);
    }

    if(bank88 && uml && uml->GetPin1().GetEnergy()>1000) {
        obj.FillHistogram("bank88_ddas",3600,0,3600,(bank88->Timestamp())/1e8,
                          500,-500,500,bank88->Timestamp()-  uml->Timestamp()  );
    }

//    if(uml && gretina && uml->GetPin1().GetEnergy()>1000) {
    if(uml && gretina ) {
        obj.FillHistogram("gretina_ddas",3600,0,3600,(gretina->Timestamp())/1e8,
                          500,-500,500,gretina->Timestamp()-  uml->Timestamp());

        obj.FillHistogram("gretina_ddaspin1",17000,0,17000,uml->GetPin1().GetEnergy(),
                          500,-500,500,gretina->Timestamp()-  uml->Timestamp());
        obj.FillHistogram("gretina_ddaspin2",17000,0,17000,uml->GetPin2().GetEnergy(),
                          500,-500,500,gretina->Timestamp()-  uml->Timestamp());

        for(unsigned int i=0; i<uml->Size(); i++) {

            obj.FillHistogram("gretina_ddasihit",40,0,40,uml->GetUMLHit(i).GetChannel(),
                              500,-500,500,gretina->Timestamp()-  uml->Timestamp());

        }

        HandleDDAS_GRETINA(obj);
    }


    SearchIosmer(obj);


    if(numobj!=list->GetSize())
        list->Sort();
}
