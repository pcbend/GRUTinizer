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

extern "C"
void MakeHistograms(TRuntimeObjects &obj) {

    TGretina *gretina = obj.GetDetector<TGretina>();
    TS800    *s800    = obj.GetDetector<TS800>();
    TBank88  *bank29  = obj.GetDetector<TBank88>();
    TMode3   *mode3   = obj.GetDetector<TMode3>();


    std::string dirname;
    std::string histname;


    dirname = "mode3";
    //histname = Form("gretina_%s",time->GetName());
    if(mode3)
    for( size_t i = 0; i< mode3->Size();i++){
        TMode3Hit hit = (mode3->GetMode3Hit(i));
        int cry_id = hit.GetCrystalId();
        histname = Form("cry_e0_%02d",cry_id);
        int seg_id = hit.GetSegmentId();
        obj.FillHistogram(dirname,histname,40,0,40,seg_id,4000,0,16000,hit.Charge());
    }

}
