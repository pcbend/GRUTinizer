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
#include "TS800.h"
#include "TFastScint.h"

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

bool map_inited=false;

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


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
    InitMap();
    TGretina *gretina = obj.GetDetector<TGretina>();
    TBank88  *bank29  = obj.GetDetector<TBank88>();
    TS800 *s800 = obj.GetDetector<TS800>();



    TList    *list    = &(obj.GetObjects());
    int numobj        = list->GetSize();

    std::string histname = "";
    std::string dirname  = "";

    if(s800) {
        obj.FillHistogram("E1Up_Singles",2000,0,2000,s800->GetScint().GetEUp()) ;



// histname = "S800_S800_timestamp_difference";
//      obj.FillHistogram(histname,400,-400,400,hit2.Timestamp()-hit.Timestamp());


    }



    if(bank29) {
        for(unsigned int x=0; x<bank29->Size(); x++) {
            TMode3Hit &hit = (TMode3Hit&)bank29->GetHit(x);
            std::string histname = Form("bank29_%i",hit.GetChannel());
            obj.FillHistogram(histname,16000,0,64000,hit.Charge());
        }
        if(s800) {
            std::string histname = "S800_Bank88_timestamp_difference";
            obj.FillHistogram(histname,400,-400,400,bank29->Timestamp()-s800->Timestamp());

            histname = "S800_Bank88_timestamp_difference_vs_bank29time";
            obj.FillHistogram(histname,7200,0,7200,bank29->Timestamp()*1e-8,400,-400,400,bank29->Timestamp()-s800->Timestamp());

            histname = "S800_Bank88_timestamp_difference_vs_s800time";
            obj.FillHistogram(histname,7200,0,7200,s800->Timestamp()*1e-8,400,-400,400,bank29->Timestamp()-s800->Timestamp());

            //obj.FillHistogram("E1_Down",1000,0,4000),s800-> GetScint().GetEDown()) ;
        }
    }
    if(!gretina)
        return;

    if(gretina) {

        // gretina->CleanHits();
        //double sumsegener = 0 ;
        for(unsigned int i=0; i<gretina->Size(); i++) {
            TGretinaHit hit = gretina->GetGretinaHit(i);
            //hit.Print();
            //hit.TrimSegments(0);
            //hit.Print();

            int hole = HoleQMap[hit.GetHoleNumber()];
            obj.FillHistogram("summary",10000,0,10000,hit.GetCoreEnergy(),
                              200,0,200,hit.GetCrystalId());
            obj.FillHistogram(Form("quad%02i",hit.GetCrystalId()),10000,0,10000,hit.GetCoreEnergy());

            int quad = HoleQMap[hit.GetHoleNumber()];
            int p    = (hit.GetCrystalId() % 4) +1;

            obj.FillHistogram(Form("q%02i_p%i",quad,p),10000,0,10000,hit.GetCoreEnergy());

            if(quad==10 && p == 1){
              obj.FillHistogram("q10p1_cc1",10000,0,100000,hit.GetCoreCharge(0));
              obj.FillHistogram("q10p1_cc2",10000,0,100000,hit.GetCoreCharge(1));
              obj.FillHistogram("q10p1_cc3",10000,0,100000,hit.GetCoreCharge(2));
              obj.FillHistogram("q10p1_cc4",10000,0,100000,hit.GetCoreCharge(3));
            }

            obj.FillHistogram("sum",10000,0,10000,hit.GetCoreEnergy());

            histname="position";
            obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
                              360,0,360,hit.GetPhiDeg());

            dirname = "GretSummCoreCharge";
            histname = Form("Q%iCoreCharge",HoleQMap[hit.GetCrystalId()/4]);
            obj.FillHistogram(dirname,histname,
                              16,0,16,4*hit.GetCrystalNumber()+0.,
                              64000,0,64000,((double)hit.GetCoreCharge(0)));
            obj.FillHistogram(dirname,histname,
                              16,0,16,4*hit.GetCrystalNumber()+1.,
                              64000,0,64000,((double)hit.GetCoreCharge(1)));
            obj.FillHistogram(dirname,histname,
                              16,0,16,4*hit.GetCrystalNumber()+2.,
                              64000,0,64000,((double)hit.GetCoreCharge(2)));
            obj.FillHistogram(dirname,histname,
                              16,0,16,4*hit.GetCrystalNumber()+3.,
                              64000,0,64000,((double)hit.GetCoreCharge(3)));



//double previous = 0 ;

//hit.TrimSegments(0);
            for(int z=0; z<hit.NumberOfInteractions(); z++) {

//histname = Form("GretinaSummaryX%02i",hit.GetCrystalId());
//      obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(z),
//     500,0,4000,hit.GetSegmentEng(z));

//histname = Form("GretinaSummary_SegEnerTry_X%02i",hit.GetCrystalId());
//      obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(z),
//      500,0,4000,hit.GetSegmentEner(z));
//                histname = Form("GretinaSummary_SegEnerTry_X%02i",hit.GetCrystalId());
//                obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(z),
//                                  4000,0,4000,hit.GetIntPreampEng(z));
                dirname = "Summary_Segments";
                histname = Form("GretinaSummary_SegEnerTry_Q%02i_p%01i",quad,p);
                obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(z),
                                  4000,0,4000,hit.GetIntPreampEng(z));

                dirname = "energy_vs_time";
                if(quad  == 1 && p == 2 && hit.GetSegmentId(z) == 29) {
                    obj.FillHistogram(dirname,Form("seg_sec_Q%02i_p%01i_ch%02i",quad,p,hit.GetSegmentId(z)),3000,0,3000,hit.GetTime()/1e8,500,0,4000,hit.GetIntPreampEng(z));
                }
                if(quad  == 3 && p == 3 && hit.GetSegmentId(z) == 11) {
                    obj.FillHistogram(dirname,Form("seg_sec_Q%02i_p%01i_ch%02i",quad,p,hit.GetSegmentId(z)),3000,0,3000,hit.GetTime()/1e8,500,0,4000,hit.GetIntPreampEng(z));
                }
                if(quad  == 6 && p == 1 && hit.GetSegmentId(z) == 11) {
                    obj.FillHistogram(dirname,Form("seg_sec_Q%02i_p%01i_ch%02i",quad,p,hit.GetSegmentId(z)),3000,0,3000,hit.GetTime()/1e8,500,0,4000,hit.GetIntPreampEng(z));
                }
                if(quad  == 7 && p == 1 && hit.GetSegmentId(z) == 22) {
                    obj.FillHistogram(dirname,Form("seg_sec_Q%02i_p%01i_ch%02i",quad,p,hit.GetSegmentId(z)),3000,0,3000,hit.GetTime()/1e8,500,0,4000,hit.GetIntPreampEng(z));
                }
                if(quad  == 8 && p == 4 && hit.GetSegmentId(z) == 0) {
                    obj.FillHistogram(dirname,Form("seg_sec_Q%02i_p%01i_ch%02i",quad,p,hit.GetSegmentId(z)),3000,0,3000,hit.GetTime()/1e8,500,0,4000,hit.GetIntPreampEng(z));
                }
                if(quad  == 8 && p == 2 && hit.GetSegmentId(z) == 11) {
                    obj.FillHistogram(dirname,Form("seg_sec_Q%02i_p%01i_ch%02i",quad,p,hit.GetSegmentId(z)),3000,0,3000,hit.GetTime()/1e8,500,0,4000,hit.GetIntPreampEng(z));
                }
                if(quad  == 8 && p == 4 && hit.GetSegmentId(z) == 3) {
                    obj.FillHistogram(dirname,Form("seg_sec_Q%02i_p%01i_ch%02i",quad,p,hit.GetSegmentId(z)),3000,0,3000,hit.GetTime()/1e8,500,0,4000,hit.GetIntPreampEng(z));
                }
                if(quad  == 11 && p == 1 && hit.GetSegmentId(z) == 35) {
                    obj.FillHistogram(dirname,Form("seg_sec_Q%02i_p%01i_ch%02i",quad,p,hit.GetSegmentId(z)),3000,0,3000,hit.GetTime()/1e8,500,0,4000,hit.GetIntPreampEng(z));
                }
//                if(hit.GetSegmentId(z) == 21 && hit.GetXtalId() == 25 ) {
//                    obj.FillHistogram("seg_sec_energy_2",3000,0,3000,hit.GetTime()/1e8,4000,0,4000,hit.GetIntPreampEng(z));
//                }


//if(hit.GetSegmentId(z)!=previous || hit.GetSegmentId(z)==previous)
//      {sumsegener += hit.GetSegmentEng(z);}
//previous=hit.GetSegmentId(z);

//histname = Form("GretinaSummary_summed_X%02i",hit.GetCrystalId());
//      obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(z),
//      500,0,4000,sumsegener);




                if(hit.GetPad()==0) {
                    histname="position_good_Decomp_in_NumberofInteractions_loop";
                    obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
                                      360,0,360,hit.GetPhiDeg());

                    // histname = Form("GretinaSummary_goodDecomp_X%02i",hit.GetCrystalId());
                    //obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(z),
                    //500,0,4000,hit.GetSegmentEng(z));

                    histname="position_good_Decomp_with_CC_gate_in_NumberofInteractions_loop";
                    if(hit.GetCoreEnergy()>500) {
                        obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
                                          360,0,360,hit.GetPhiDeg());
                    }
                }
            }

//outside of loop
//histname = Form("GretinaSummaryX%02i",hit.GetCrystalId());
//dirname = "Summary_Segments_outside_loop";
//      obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(),
//2000,0,4000,hit.GetSegmentEng(isegid));

//histname = Form("Gretina_SegIDenergy_X%02i",hit.GetCrystalId());
//      obj.FillHistogram(dirname,histname,2000,0,4000,sumsegener);


//    for(unsigned int k=i+1;k<gretina->Size();k++) {
//TGretinaHit hit2 = gretina->GetGretinaHit(z);
//histname = Form("Segment_Correlations_X%02i",hit.GetCrystalId());
//     obj.FillHistogram(histname,40,0,40,hit.GetSegmentId(z),
//     40,0,40,hit2.GetSegmentId(z));}


            //double segone = hit.GetSegmentEng(z) ;
// double totalseg = totalseg+segone ;

//histname = Form("GretinaSummaryX%02i_summed",hit.GetCrystalId());
            //    obj.FillHistogram(histname,2000,0,4000,totalseg);



            if(hit.GetPad()==0) {
                histname="position_good_Decomp";
                obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
                                  360,0,360,hit.GetPhiDeg());


                histname="position_good_Decomp_with_CC_gate_bt_1000keV";
                if(hit.GetCoreEnergy()>1000) {
                    obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
                                      360,0,360,hit.GetPhiDeg());
                }

                histname="position_good_Decomp_with_CC_gate_bt_500keV";
                if(hit.GetCoreEnergy()>500) {
                    obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
                                      360,0,360,hit.GetPhiDeg());
                }

                histname="position_good_Decomp_with_CC_gate_lt_500keV";
                if(hit.GetCoreEnergy()<500) {
                    obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
                                      360,0,360,hit.GetPhiDeg());
                }
            }



            // histname="Id_hole";
            // obj.FillHistogram(histname,100,0,100,hit.GetCrystalId(),
            // 20,0,20,hole);}

            //histname="Theta_id";
            //obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
            //200,0,200,hit.GetCrystalId());

            //histname="phi_id";
            //obj.FillHistogram(histname,360,0,360,hit.GetPhiDeg(),
            //200,0,200,hit.GetCrystalId());


            if(hole!=0) {
                dirname = Form("Det%i",hole);
                histname = Form("Gretina_X_Y_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
                obj.FillHistogram(dirname,histname,720,-360,360,hit.GetX(),
                                  720,-360,360,hit.GetY());

                histname = Form("Gretina_X_Y_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
                obj.FillHistogram(dirname,histname,720,-360,360,hit.GetX(),
                                  720,-360,360,hit.GetY());

//if(bank29){
//histname = Form("Gretina_EnergyTime_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
//obj.FillHistogram(dirname,histname,7200,0,7200,bank29->Timestamp()*1e-8,
//                                  10000,0,10000,hit.GetCoreEnergy());}

                for(int j=0; j<4; j++) {
                    histname = Form("Cores_charge_Cry%02i",hit.GetCrystalId());
                    obj.FillHistogram(dirname,histname,4,0,4,j,16000,0,64000,hit.GetCoreCharge(j));
                }


// Baseline not implemented yet
                /*
                 for(int j=0;j<4;j++){
                 histname = Form("Gretina_Baseline_Det%02i_Digi%i",hole,j);
                         obj.FillHistogram(dirname,histname,32000,0,32000,hit.GetCoreCharge(j),
                         32000,0,32000,hit.GetBaseline());}
                */


// histname = Form("Gretina_CoreEnergy_CryNum%02i_Det%02i",hit.GetCrystalNumber(),hole);
//         obj.FillHistogram(dirname,histname,4000,0,4000,hit.GetCoreEnergy());

//histname = Form("Gretina_CoreEnergy_CryNum%02i_Hole%02i",hit.GetCrystalNumber(),hit.GetHoleNumber());
                //      obj.FillHistogram(dirname,histname,4000,0,4000,hit.GetCoreEnergy());

//histname = Form("Gretina_Theta_Id_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
                //      obj.FillHistogram(dirname,histname,360,0,360,hit.GetThetaDeg(),
                //    200,0,200,hit.GetCrystalId());

//histname = Form("Gretina_Phi_Id_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
                //      obj.FillHistogram(dirname,histname,360,0,360,hit.GetPhiDeg(),
                //    200,0,200,hit.GetCrystalId());

                histname = Form("Gretina_GetPad_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
                obj.FillHistogram(dirname,histname,360,-1,141,hit.GetPad());

                histname = Form("Gretina_Position_Det%02i",hole);
                obj.FillHistogram(dirname,histname,360,0,360,hit.GetThetaDeg(),
                                  360,0,360,hit.GetPhiDeg());
            }



            //double radius=((hit.GetX())^2+(hit.GetY())^2)^0.5 ;

            //  histname="z-r";
            // obj.FillHistogram(histname,360,0,360,hit.GetZ(),
            // 360,0,360,radius);

            if(bank29) {
                histname = "Gretina_Bank88_timediff_energy";
                obj.FillHistogram(histname,600,-600,600,bank29->Timestamp()-hit.GetTime(),
                                  2000,0,4000,hit.GetCoreEnergy());

                histname = "Gretina_Bank88_timediff_vs_bank29time";
                obj.FillHistogram(histname,7200,0,7200,bank29->Timestamp()*1e-8,700,-400,1000,bank29->Timestamp()-hit.GetTime());


                histname = "Bank29_Charge";
                obj.FillHistogram(histname,2000,-10000,10000,bank29->GetCharge());


            }

            if(s800) {

                if(s800->GetScint().GetEUp()>1560 && s800->GetScint().GetEUp()<1750) {

                    histname = "Gretina_S800_coincidence_gateon_1332keV";
                    obj.FillHistogram(histname,2000,0,4000,hit.GetCoreEnergy());
                }

                histname = "Gretina_S800_time";
                obj.FillHistogram(histname,1200,-600,600,s800->Timestamp()-hit.GetTime(),
                                  2000,0,4000,hit.GetCoreEnergy());

                // obj.FillHistogram("E1_Down_coincidence_with_Gretina",2000,0,2000,s800->GetScint().GetEDown());
                obj.FillHistogram("E1_Up_coincidence_with_Gretina",2000,0,2000,s800->GetScint().GetEUp());
                //histname = "Gretina_t0_S800_time";
                //obj.FillHistogram(histname,1200,-600,600,s800->Timestamp()-hit.GetT0(),
                //2000,0,4000,hit.GetCoreEnergy());

            }

            if(s800 && bank29) {
                std::string histname = "GretinaEnergy_Bank88-S800_timestamp_difference";
                obj.FillHistogram(histname,400,-400,400,bank29->Timestamp()-s800->Timestamp(),    2000,0,4000,hit.GetCoreEnergy());
            }

        }
    }



    if(numobj!=list->GetSize())
        list->Sort();
}
