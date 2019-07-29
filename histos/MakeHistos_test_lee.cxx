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
#include "TChain.h"

std::vector<GCutG*> incoming_cuts = {0};
std::vector<GCutG*> outgoing_cuts = {0};
std::vector<GCutG*> time_energy_cuts = {0};
int gates_loaded=0;

//quads as of June 2019.
#define Q1 15
#define Q2 7
#define Q3 11
#define Q4 16
#define Q5 8
#define Q6 14
#define Q7 12
#define Q8 17
#define Q9 9
#define Q10 6
#define Q11 19
//#define Q12 20

#define BETA .39

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

/*

------------------------------
INCOMING (to be done if XFP used)
------------------------------

*/


bool IncomingBeam(TRuntimeObjects& obj,GCutG *outgoing) {
    TS800    *s800    = obj.GetDetector<TS800>();

    if(!s800)
        return false;

    if(outgoing) {
        if(!outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),s800->GetIonChamber().Charge()))
            return false;
    }

    std::string dirname;
    if(outgoing)
        dirname = Form("incoming_%s",outgoing->GetName());
    else
        dirname = "incoming";

    std::string histname;
    //TMTof &mtof = s800->GetMTof();
    unsigned int reg = s800->GetReg();

    histname = "OBJ_TAC";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetTof().GetTacOBJ());
    histname = "XFP_TAC";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetTof().GetTacXFP());

    histname = "OBJ_TAC-XFP_TAC";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetTof().GetTacOBJ()-s800->GetTof().GetTacXFP());

    histname = "OBJ_TAC_XFP_TAC_2D";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetCorrTOF_OBJTAC(),4000,-2000,2000,s800->GetCorrTOF_XFPTAC());

    if(reg&1) {
        histname = "OBJ_TAC-XFP_TAC_Reg1";
        obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetTof().GetTacOBJ()-s800->GetTof().GetTacXFP());
    }


    if(reg&2) {
        histname = "OBJ_TAC-XFP_TAC_Reg2";
        obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetTof().GetTacOBJ()-s800->GetTof().GetTacXFP());
    }

    /*
      histname = "Obj_E1";
      obj.FillHistogram(dirname,histname,8000,-32000,32000,s800->GetMTOF_ObjE1());
      histname = "Obj_Rf";
      obj.FillHistogram(dirname,histname,8000,-3200,32000,s800->GetMTOF_ObjRf());
      histname = "Xfp_E1";
      obj.FillHistogram(dirname,histname,8000,-32000,32000,s800->GetMTOF_XfpE1());
      histname = "Rf_E1";
      obj.FillHistogram(dirname,histname,8000,-32000,64000,s800->GetMTOF_RfE1());

      histname = "obj_rf_2d";
      obj.FillHistogram(dirname,histname,1600,-8000,0,s800->GetMTOF_ObjE1(),
                                         1600,-24000,-16000,s800->GetMTOF_RfE1());
    */

    //histname = "obj_xfp_2d";
    //obj.FillHistogram(dirname,histname,800,0,64000,s800->GetMTOF_ObjE1(),
    //                                   800,0,64000,s800->GetMTOF_XfpE1());

    histname = "trigger_bit";
    unsigned short bits = s800->GetTrigger().GetRegistr();
    for(int j=0; j<16; j++) {
        if(((bits>>j)&0x0001))
            obj.FillHistogram(dirname,histname,20,0,20,j);
    }
    histname = "trigger_raw";
    obj.FillHistogram(dirname,histname,20,0,20,s800->GetTrigger().GetRegistr());

    return true;
}

/*

------------------------------
OUTGOING
------------------------------

*/

bool OutgoingBeam(TRuntimeObjects& obj,GCutG *incoming) {
    TS800    *s800    = obj.GetDetector<TS800>();

    double objtime1 = s800->GetTof().GetOBJ();
    double xfptime1 = s800->GetTof().GetXFP();
    //double yfp1       = s800->GetYFP(0) ;
    //double ic_sum     = s800->GetIonChamber().GetAve();

//  double obj_corr   = s800->GetCorrTOF_OBJ();
//  double ic_sum     = s800->GetIonChamber().GetAve();

    if(!s800)
        return false;

    std::string dirname;
    if(incoming)
        dirname = Form("outgoing_%s",incoming->GetName());
    else
        dirname = "outgoing";

//  if(incoming) {
//    if(!incoming->IsInside(s800->GetMTOF_ObjE1(),s800->GetMTOF_RfE1()))
//      return false;
//  }


    std::string histname;
    TIonChamber ion = s800->GetIonChamber();
    histname = "ion_summary";
    for(int i=0; i<ion.Size(); i++) {
        obj.FillHistogram(dirname,histname,16,0,16,ion.GetChannel(i),
                          300,0,6000,ion.GetCalData(i));
    }


//  histname = "Event_Counter";
//  obj.FillHistogram(dirname,histname,1e+7,0,1e+7,s800->GetEventCounter());

//  histname = "Event_Counter_vs_Timestamp";
//  obj.FillHistogram(dirname,histname,7200,0,7200,s800->GetTimestamp()*1e-8,1e+7,0,1e+7,s800->GetEventCounter());


    histname = "IC_vs_OBJTOF";
    obj.FillHistogram(dirname,histname,
                      1000,0,2000,s800->GetCorrTOF_OBJTAC(),
                      1000,0,2000,s800->GetIonChamber().GetAve());

    obj.FillHistogram(dirname,"IC_dE_vs_OBJC",
                      4000,-2000,2000,s800->GetCorrTOF_OBJTAC(),
                      4000,0,4000,s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()));


    double crdc1 = s800->GetCrdc(0).GetDispersiveX();

    histname = "IC_Sum";
    obj.FillHistogram(dirname,histname,1000,0,2000,s800->GetIonChamber().GetAve());

    histname = "IC_dE";
    obj.FillHistogram(dirname,histname,1000,0,5000,s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()));

    histname = "CRDC1_X_vs_ICdE_Corr";
    obj.FillHistogram(dirname,histname,800,-400,400,s800->GetCrdc(0).GetDispersiveX(),
                      5000,0,5000,s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()));

    histname = "CRDC1_Y_vs_Time";
    obj.FillHistogram(dirname,histname,800,-400,400,s800->GetCrdc(0).GetNonDispersiveY(),
                      3600,0,3600,s800->Timestamp()*1e-8);

    histname = "CRDC1_Time_vs_Timestamp";
    obj.FillHistogram(dirname,histname,1000,-2000,2000,s800->GetCrdc(0).GetTime(),3600,0,3600,s800->Timestamp()*1e-8);


    //histname = "CRDC1_Pad";
    //obj.FillHistogram(dirname,histname,256,0,256,s800->GetCrdc(0).GetPad());




    histname = "CRDC1_X";
    obj.FillHistogram(dirname,histname,800,-400,400,s800->GetCrdc(0).GetDispersiveX());

    histname = "CRDC2_X";
    obj.FillHistogram(dirname,histname,800,-400,400,s800->GetCrdc(1).GetDispersiveX());

    histname = "CRDC1_Y";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetCrdc(0).GetNonDispersiveY());

    histname = "CRDC2_Y";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetCrdc(1).GetNonDispersiveY());

    obj.FillHistogram(dirname,"CRDC1",
                      800,-400,400,s800->GetCrdc(0).GetDispersiveX(),
                      4000,-2000,2000,s800->GetCrdc(0).GetNonDispersiveY());

    obj.FillHistogram(dirname,"CRDC2",
                      8000,-400,400,s800->GetCrdc(1).GetDispersiveX(),
                      4000,-2000,2000,s800->GetCrdc(1).GetNonDispersiveY());

    //histname = "OBJ";
    //obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetTof().GetOBJ());

    //histname = "OBJ_Mesy";
    //obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetRawOBJ_MESY());

    int ObjSize = s800->GetMTof().ObjSize();
    int XfpSize = s800->GetMTof().XfpSize();
    int E1UpSize = s800->GetMTof().E1UpSize();

    for(int i=0; i<ObjSize; i++) {
        for(int j=0; j<E1UpSize; j++) {
            histname = "OBJ-E1";
            obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(j));
        }
    }

    for(int i=0; i<XfpSize; i++) {
        for(int j=0; j<E1UpSize; j++) {
            histname = "XFP-E1";
            obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().fXfp.at(i)-s800->GetMTof().fE1Up.at(j));
        }
    }

    histname = "OBJ_function";
    obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().GetCorrelatedObjE1());

    histname = "OBJ_Corrected";
    obj.FillHistogram(dirname,histname,1200,-60000,60000,s800->GetCorrTOF_OBJ_MESY());

    histname = "OBJ_MESY_vs_Timestamp";
    obj.FillHistogram(dirname,histname,4000,-20000,20000,s800->GetMTof().GetCorrelatedObjE1(),3600,0,3600,s800->Timestamp()*1e-8);

    histname = "XFP_function";
    obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().GetCorrelatedXfpE1());

    histname = "XFP_function";
    obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().GetCorrelatedXfpE1());

    histname = "XFPE1-OBJE1_OBJCorr_Correlation";
    obj.FillHistogram(dirname,histname,600,3600,4200,s800->GetMTof().GetCorrelatedXfpE1()-s800->GetMTof().GetCorrelatedObjE1(),4000,-4000,0,s800->GetCorrTOF_OBJ_MESY());

    if(s800->Timestamp()*1e-8>400) {
        histname = "XFPE1-OBJE1_OBJCorr_Correlation_withclosedslits";
        obj.FillHistogram(dirname,histname,600,3600,4200,s800->GetMTof().GetCorrelatedXfpE1()-s800->GetMTof().GetCorrelatedObjE1(),4000,-4000,0,s800->GetCorrTOF_OBJ_MESY());
    }

    if(s800->Timestamp()*1e-8<400) {
        histname = "XFPE1-OBJE1_OBJCorr_Correlation_withopenslits";
        obj.FillHistogram(dirname,histname,600,3600,4200,s800->GetMTof().GetCorrelatedXfpE1()-s800->GetMTof().GetCorrelatedObjE1(),4000,-4000,0,s800->GetCorrTOF_OBJ_MESY());
    }

    histname = "XFP_MESY_vs_Timestamp";
    obj.FillHistogram(dirname,histname,4000,-20000,20000,s800->GetMTof().GetCorrelatedXfpE1(),3600,0,3600,s800->Timestamp()*1e-8);


    obj.FillHistogram(dirname,"IC_dE_vs_OBJCorrE1",
                      8000,-4000,4000,s800->GetCorrTOF_OBJ_MESY(),
                      4000,0,4000,s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()));

    obj.FillHistogram(dirname,"IC_dE_vs_XFPCorrE1",
                      8000,-4000,4000,s800->GetCorrTOF_XFP_MESY(),
                      4000,0,4000,s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()));


    histname = "AFP_vs_OBJC";
    obj.FillHistogram(dirname,histname,
                      1000,-0.5,0.5,s800->GetAFP(),
                      4000,-20000,20000,s800->GetCorrTOF_OBJ_MESY()); // check units of AFP

    histname = "XFP_vs_OBJC";
    obj.FillHistogram(dirname,histname,
                      600,-300,300,s800->GetXFP(),
                      4000,-20000,20000,s800->GetCorrTOF_OBJ_MESY());

    histname = "AFP_vs_XFPC";
    obj.FillHistogram(dirname,histname,
                      1000,-0.5,0.5,s800->GetAFP(),
                      4000,-20000,20000,s800->GetCorrTOF_XFP_MESY()); // check units of AFP

    histname = "XFP_vs_XFPC";
    obj.FillHistogram(dirname,histname,
                      600,-300,300,s800->GetXFP(),
                      4000,-20000,20000,s800->GetCorrTOF_XFP_MESY());

    histname = "E1_vs_OBJ";
    obj.FillHistogram(dirname,histname,
                      6000,-30000,30000,s800->GetMTof().GetCorrelatedE1Up(),
                      6000,-30000,30000,s800->GetMTof().GetCorrelatedObjE1());


    histname = "OBJ_TAC";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetTof().GetTacOBJ());

    histname = "OBJ_TAC_Corr";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetCorrTOF_OBJTAC());

    histname = "OBJ_ToFE1_TAC";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetTofE1_TAC());

    //histname = "RF";
    //obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetTof().GetRF());

    histname = "OBJ_Corr";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetCorrTOF_OBJ());

    histname = "XFP_TAC_ICdE";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetCorrTOF_XFPTAC(),4000,0,4000,s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()));


    histname = "Time_E1Up";
    obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetScint().GetTimeUp());

    histname = "AFP_vs_OBJTOF_Corr_TAC";
    obj.FillHistogram(dirname,histname,
                      1000,-0.5,0.5,s800->GetAFP(),
                      4000,-2000,2000,s800->GetCorrTOF_OBJTAC()); // check units of AFP

    histname = "XFP_vs_OBJTOF_Corr_TAC";
    obj.FillHistogram(dirname,histname,
                      600,-300,300,s800->GetXFP(),
                      4000,-2000,2000,s800->GetCorrTOF_OBJTAC());


    histname = "AFP_vs_XFPTOF_Corr_TAC";
    obj.FillHistogram(dirname,histname,
                      1000,-0.5,0.5,s800->GetAFP(),
                      4000,-2000,2000,s800->GetCorrTOF_XFPTAC()); // check units of AFP

    histname = "XFP_vs_XFPTOF_Corr_TAC";
    obj.FillHistogram(dirname,histname,
                      600,-300,300,s800->GetXFP(),
                      4000,-2000,2000,s800->GetCorrTOF_XFPTAC());

    /*
      histname = "AFP_vs_OBJTOF";
      obj.FillHistogram(dirname,histname,
            	    1000,-0.5,0.5,s800->GetAFP(),
      		    1000,0,1000,obj_corr); // check units of AFP

      histname = "XFP_vs_OBJTOF";
      obj.FillHistogram(dirname,histname,
            	    600,-300,300,s800->GetXFP(0),
            	    1000,0,1000,obj_corr);
    */


    return true;
}



/*

------------------------------
GRETINA
------------------------------

*/


bool HandleGretinaGated(TRuntimeObjects &obj, GCutG *outgoing) {

    TGretina *gretina = obj.GetDetector<TGretina>();
    TS800    *s800    = obj.GetDetector<TS800>();
    TBank88  *bank29  = obj.GetDetector<TBank88>();

    if(!gretina || !s800)
        return false;

    std::string dirname;
    std::string histname;

    if(outgoing)
        dirname = Form("gretina_%s",outgoing->GetName());
    else
        dirname = "gretina";
//std::cout << "Gate name: " << time_energy->GetName() << std::endl ;

    //histname = Form("gretina_%s",time->GetName());

    for(unsigned int i=0; i<gretina->Size(); i++) {
        TGretinaHit hit = gretina->GetGretinaHit(i);

        obj.FillHistogram(dirname,"DetNumber_vs_Timestamp",
                          80,0,80,hit.GetCrystalId(),
                          3600,0,3600,s800->Timestamp()*1e-8);

        if(hit.GetPad()==0) {
            histname="position_good_Decomp";
            obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
                              360,0,360,hit.GetPhiDeg());
        }
        /*
            for(unsigned int j=i+1;j<gretina->Size();j++) {
            TGretinaHit hit2 = gretina->GetGretinaHit(j);

             histname ="timediff_hit_hit2";
             obj.FillHistogram(histname,7200,0,7200,hit.GetTime()*1e-8,7200,0,1000,hit2.GetTime()*1e-8);

             //std::cout << "hit1:" << hit.GetTime()*1e-8 << "hit2:" << hit2.GetTime()*1e-8 << std::endl ;


        //histname = "CoreEnergy_Doppler_gg";
        //      obj.FillHistogram(dirname,histname,2000,0,4000,hit.GetDoppler(GValue::Value("BETA")),
        //      2000,0,4000,hit2.GetDoppler(GValue::Value("BETA")));
        //histname = "CoreEnergy_Doppler_gg";
        //      obj.FillHistogram(dirname,histname,2000,0,4000,hit2.GetDoppler(GValue::Value("BETA")),
        //      2000,0,4000,hit.GetDoppler(GValue::Value("BETA")));

            }
        */


        double time_diff = s800->Timestamp()-hit.GetTime() ;
        double energy = hit.GetCoreEnergy();
        unsigned int reg = s800->GetReg();
        int hole = HoleQMap[hit.GetHoleNumber()];




// obj.FillHistogram(dirname,"IC_dE_vs_OBJC",
//      400,1100,1500,s800->GetCorrTOF_OBJTAC(),
//     4000,0,4000,s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()));

        obj.FillHistogram(dirname,"IC_dE_vs_OBJCorrE1",
                          8000,-4000,4000,s800->GetCorrTOF_OBJ_MESY(),
                          4000,0,4000,s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()));

        histname = "Gretina_S800_time";
        obj.FillHistogram(dirname,histname,1200,-600,600,time_diff,
                          2000,0,4000,hit.GetCoreEnergy());

        histname = "Gretina_S800_CoreEnergy";
        obj.FillHistogram(dirname,histname,2000,0,4000,hit.GetCoreEnergy());

        histname = "Pad";
        obj.FillHistogram(dirname,histname,361,-1,360,hit.GetPad());
        /*
        if(hole!=0){
         histname = Form("Gretina_CoreEnergy_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
                 obj.FillHistogram(dirname,histname,2000,0,4000,hit.GetCoreEnergy());

         for(int j=0;j<4;j++){
         histname = Form("Cores_charge_Cry%02i",hit.GetCrystalId());
              obj.FillHistogram(dirname,histname,4,0,4,j,16000,0,64000,hit.GetCoreCharge(j));
        }

        }
        */

        /*
         if(bank29) {
         histname = Form("Gretina_Bank88_TD");
              obj.FillHistogram(histname,600,-600,600,bank29->Timestamp()-hit.GetTime(),
                                         5000,0,10000,hit.GetCoreEnergy());

        if(reg&2){
        histname = Form("Gretina_Bank88_TD_Reg2");
              obj.FillHistogram(histname,600,-600,600,bank29->Timestamp()-hit.GetTime(),
                                         5000,0,10000,hit.GetCoreEnergy());

        histname = Form("Gretina_Bank88_TD_Reg2_Cry%02i",hit.GetCrystalId());
              obj.FillHistogram(histname,600,-600,600,bank29->Timestamp()-hit.GetTime(),
                                         5000,0,10000,hit.GetCoreEnergy());

         }

        if(reg&1){
        histname = Form("Gretina_Bank88_TD_Reg1_Cry%02i",hit.GetCrystalId());
              obj.FillHistogram(histname,600,-600,600,bank29->Timestamp()-hit.GetTime(),
                                         5000,0,10000,hit.GetCoreEnergy());
            }

        }

        if(bank29 && s800){

        if(reg&2){
        histname = Form("S800_Bank88_TD-Energy_Reg2");
              obj.FillHistogram(histname,600,-600,600,bank29->Timestamp()-s800->Timestamp(),
                                         5000,0,10000,hit.GetCoreEnergy());}

        if(reg&1){
        histname = Form("S800_Bank88_TD-Energy_Reg1");
              obj.FillHistogram(histname,600,-600,600,bank29->Timestamp()-s800->Timestamp(),
                                         5000,0,10000,hit.GetCoreEnergy());
        }}
        */

        if(outgoing) {

            if(outgoing->IsInside(s800->GetCorrTOF_OBJTAC(),
                                  s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()))) {

//if(outgoing->IsInside(s800->GetCorrTOF_RF_MESY(),
//s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()))){


                histname = "Gretina_S800_CoreEnergy_wCut";
                obj.FillHistogram(dirname,histname,2000,0,4000,hit.GetCoreEnergy());

                /*
                 obj.FillHistogram(dirname,"IC_dE_vs_RfCorrE1_wCut",
                     6000,16000,22000,s800->GetCorrTOF_RF_MESY(),
                     4000,0,4000,s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()));
                */


                obj.FillHistogram(dirname,"IC_dE_vs_CorrOBJTAC_wCut",
                                  2000,0,2000,s800->GetCorrTOF_OBJTAC(),
                                  4000,0,4000,s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()));


                histname = "Gretina_S800_CoreEnergy_Doppler";
                obj.FillHistogram(dirname,histname,2000,0,4000,hit.GetDoppler(GValue::Value("BETA")));


                obj.FillHistogram(dirname,"summary_Doppler",10000,0,10000,hit.GetDoppler(GValue::Value("BETA")),
                                  200,0,200,hit.GetCrystalId());

            } //gate (real)


            if(outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
                                  s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()))) {


                obj.FillHistogram(dirname,"IC_dE_vs_OBJCorrE1_Mesy_wCut",
                                  8000,-4000,4000,s800->GetCorrTOF_OBJ_MESY(),
                                  4000,0,4000,s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()));

                histname = "Gretina_S800_CoreEnergy_Doppler_wMesyCut";
                obj.FillHistogram(dirname,histname,2000,0,4000,hit.GetDoppler(GValue::Value("BETA")));

                histname = "CRDC1_X";
                obj.FillHistogram(dirname,histname,800,-400,400,s800->GetCrdc(0).GetDispersiveX());

                histname = "CRDC1_Y";
                obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetCrdc(0).GetNonDispersiveY());

            }


            if(outgoing->IsInside(time_diff,hit.GetCoreEnergy())) {

                histname = "Gretina_S800_CoreEnergy_wCut_fakedtime";
                obj.FillHistogram(dirname,histname,2000,0,4000,hit.GetCoreEnergy());

                histname = "Gretina_S800_time_wfaketimecut";
                obj.FillHistogram(dirname,histname,1200,-600,600,time_diff,
                                  2000,0,4000,hit.GetCoreEnergy());


                if(s800->GetScint().GetEUp()>1400 && s800->GetScint().GetEUp()<1850) {
                    histname = "Gretina_S800_CoreEnergy_wCut_fakedtime_and_energy";
                    obj.FillHistogram(dirname,histname,2000,0,4000,hit.GetCoreEnergy());

                }

            } //gate (faked time gate)

        }  //outgoing

//if(time_energy->IsInside(hit.GetCoreEnergy(),time_diff)){

//if(time_diff>260 && time_diff<300){

        if(s800 && gretina) {
            obj.FillHistogram(dirname,"E1Up_Singles_inGretina",2000,0,2000,s800->GetScint().GetEUp()) ;
        }


    }
    return true;
}



////////////////////////////
/*

---------------------------------------
Histograms are made. Gates are loaded.
---------------------------------------

*/
////////////////////////////

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {


    InitMap();
    TGretina *gretina = obj.GetDetector<TGretina>();
    TBank88  *bank29  = obj.GetDetector<TBank88>();
    TS800    *s800    = obj.GetDetector<TS800>();

    TList *gate_list = obj.GetGatesPtr();
    if(gate_list->GetSize() > 0){
    }

    //*************** s800 *********************//
    if(s800) {

        float x0 = s800->GetXFP(0);
        float x1 = s800->GetXFP(1);
        float y0 = s800->GetYFP(0);
        float y1 = s800->GetYFP(1);
        float afp = s800->GetAFP();

        if( !(std::isnan(x0) || std::isnan(x1) || std::isnan(y0) || std::isnan(y1) ) ) {
            std::string dirname = "outgoing";
            obj.FillHistogram(dirname,"crdc1_pos",1000,-1000,1000,x0,200,-100,100,y0);
            obj.FillHistogram(dirname,"crdc2_pos",1000,-1000,1000,x1,200,-100,100,y1);

            obj.FillHistogram(dirname,"crdc1_x_distri",1000,-1000,1000,x0);
            obj.FillHistogram(dirname,"crdc1_y_distri",200,-100,100,y0);
            obj.FillHistogram(dirname,"crdc2_x_distri",1000,-1000,1000,x1);
            obj.FillHistogram(dirname,"crdc2_y_distri",200,-100,100,y1);

            float x_slope = (x1-x0)/100.;
            float y_slope = (y1-y0)/100.;

            for(int i = 0; i<100; i++) {
                obj.FillHistogram(dirname,"trace_x",100,-50,50,i+1-50,1000,-1000,1000,x0+x_slope*(i+1));
                obj.FillHistogram(dirname,"trace_y",100,-50,50,i+1-50,1000,-1000,1000,y0+y_slope*(i+1));
            }

            // pid tof-de
            
            float tof_obj_tac = s800->GetCorrTOF_OBJTAC();
            float dE = s800->GetIonChamber().GetdE(x0,y0);
            float tof_obj_mesy = s800->GetCorrTOF_OBJ_MESY(0); // 0 1st hit
            if (!( std::isnan(dE)) ) {
                obj.FillHistogram(dirname,"dE",1000,0,2000,dE);
                if(!std::isnan(tof_obj_tac)){
                  obj.FillHistogram(dirname,"pid_tof_obj_tac_dE",500,1000,1500,tof_obj_tac,1000,200,1200,dE);
                  obj.FillHistogram(dirname,"tof_obj_tac",1000,-3000,3000,tof_obj_tac);
                  obj.FillHistogram(dirname,"tof_obj_tac_crdc1x_2d",1000,-1000,1000,x0,1000,-3000,3000,tof_obj_tac);
                  obj.FillHistogram(dirname,"tof_obj_tac_afp_2d",1000,-1,1,afp,1000,-3000,3000,tof_obj_tac);
                }
                if( ! (std::isnan(tof_obj_mesy))){
                    obj.FillHistogram(dirname,"PID_tof_obj_mesy_dE",500,-2500,-1500,tof_obj_mesy,1000,200,1200,dE);
                    obj.FillHistogram(dirname,"tof_obj_mesy",1000,-3000,3000,tof_obj_mesy);
                    obj.FillHistogram(dirname,"tof_obj_mesy_crdc1x_2d",1000,-1000,1000,x0,2000,-4000,4000,tof_obj_mesy);
                    obj.FillHistogram(dirname,"tof_obj_mesy_afp_2d",1000,-1,1,afp,2000,-4000,4000,tof_obj_mesy);
                }
            }

            
        }

    }
    //******************* S800 *************************//
    
    //******************* Gretina **********************//
    
    if(gretina){
        std::string dirname = "gretina" ;
        std::string histname;
        
        for (size_t i = 0 ; i<gretina->Size(); i++){
            TGretinaHit hit = gretina->GetGretinaHit(i);
            int cry_id = hit.GetCrystalId();
            int hole_num = hit.GetHoleNumber();
            dirname = "gretina_energy";
            // cry_id vs energy, check the CC calibration
            obj.FillHistogram(dirname,"core_cryid_2d",4000,0,4000,hit.GetCoreEnergy(),128,0,128,cry_id);
            // check pad distribution, too many pad hits larger than 0 indicates problem
            obj.FillHistogram(dirname,"cry_id_pad",128,0,128,cry_id,210,-10,200,hit.GetPad());
            if(hit.GetPad()!=0) continue;
            histname = TString::Format("cyrstal_segenergy_Q%02d_x%02d",HoleQMap[hole_num],cry_id);
            std::string histname_geo,histname_time;
            histname_geo = TString::Format("cry_xy_Q%02d_x%02d",HoleQMap[hole_num],cry_id);
            histname_time = TString::Format("cry_tdiff_Q%02d_x%02d",HoleQMap[hole_num],cry_id);

            if(s800) {
                // for some reason, the Toffset is not correctly set, so we only rely one timestamp difference
                //obj.FillHistogram("gretia_time","gretina_s800_tdiff",2000,-1000,1000,hit.GetTFit()+hit.GetTime()-s800->Timestamp(),128,0,128,cry_id);
                obj.FillHistogram("gretia_time","gretina_s800_tdiff_ts",2000,-1000,1000,hit.GetTime()-s800->Timestamp(),128,0,128,cry_id);
            }


            for ( int j = 0; j < hit.NumberOfInteractions(); j++){
                // Check segment energy calibration for each xtal
              dirname = "gretina_energy";
              //obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(j),4000,0,4000,hit.GetSegmentEner(j));
              obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(j),4000,0,4000,hit.GetIntPreampEng(j));
              double phi = hit.GetIntPosition(j).Phi()*TMath::RadToDeg();
              if(phi < 0) phi += 360;
              // check the hit pattern, one way to check whether decomp works well
              dirname = "gretina_geo";
              obj.FillHistogram(dirname,"hit_pattern",360,0,360,hit.GetIntPosition(j).Theta()*TMath::RadToDeg(),360,0,360,phi);
              obj.FillHistogram(dirname,histname_geo,200,-500,500,hit.GetIntPosition(j).X(),200,-500,500,hit.GetIntPosition(j).Y());

            }
        }
    }

    //******************* Gretina **********************//
    
    //******************* Bank 29 **********************//
    if(bank29 && s800) {
        static bool tflag = true;
        Long64_t ts_start = 0;
        if(tflag){
            tflag = false;
            ts_start = bank29->Timestamp();
        }
        std::string dirname = "bank29";
        obj.FillHistogram(dirname,"bank29_s800_tdiff",1000,-1000,1000,bank29->Timestamp()-s800->Timestamp());
        obj.FillHistogram(dirname,"bank29_s800_tdiff_runtime",1000,0,5000,(bank29->Timestamp()-ts_start)/1e8, 1000,-1000,1000,bank29->Timestamp()-s800->Timestamp());
    }
    //***************** Bank 29 **********************//

}
