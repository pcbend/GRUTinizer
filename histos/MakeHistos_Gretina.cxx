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
#include "TOBJ.h"
#include "TFastScint.h"

#include "TChannel.h"
#include "GValue.h"
#include "GCutG.h"
#include "TChain.h"

std::vector<GCutG*> incoming_cuts ;
std::vector<GCutG*> outgoing_cuts ;
std::vector<GCutG*> tofcuts ;
std::vector<GCutG*> pidcuts;
std::vector<GCutG*> tmppidcuts;
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

#define BETA .36

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

  histname = "Obj-Xfp_Mesytech";
  obj.FillHistogram(dirname,histname,10000,-10000,0,s800->GetMTof().GetCorrelatedObj()-s800->GetMTof().GetCorrelatedXfp());

  histname = "Obj_Xfp_Mesytech_2D";
  obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().GetCorrelatedObjE1(),8000,-40000,40000,s800->GetMTof().GetCorrelatedXfpE1());

  if(reg&1){
  histname = "OBJ_TAC-XFP_TAC_Reg1";
  obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetTof().GetTacOBJ()-s800->GetTof().GetTacXFP());

  histname = "Obj-Xfp_Mesytech_Reg1";
  obj.FillHistogram(dirname,histname,10000,-10000,0,s800->GetMTof().GetCorrelatedObj()-s800->GetMTof().GetCorrelatedXfp());

  histname = "Obj_Xfp_Mesytech_2D_Reg1";
  obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().GetCorrelatedObjE1(),8000,-40000,40000,s800->GetMTof().GetCorrelatedXfpE1());


  }


  if(reg&2){
  histname = "OBJ_TAC-XFP_TAC_Reg2";
  obj.FillHistogram(dirname,histname,4000,-2000,2000,s800->GetTof().GetTacOBJ()-s800->GetTof().GetTacXFP());

  histname = "Obj-Xfp_Mesytech_Reg2";
  obj.FillHistogram(dirname,histname,10000,-10000,0,s800->GetMTof().GetCorrelatedObj()-s800->GetMTof().GetCorrelatedXfp());

  histname = "Obj_Xfp_Mesytech_2D_Reg2";
  obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().GetCorrelatedObjE1(),8000,-40000,40000,s800->GetMTof().GetCorrelatedXfpE1());
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
 
  //histname = "trigger_bit";
  //unsigned short bits = s800->GetTrigger().GetRegistr();
  //for(int j=0;j<16;j++) {
   // if(((bits>>j)&0x0001))
    //  obj.FillHistogram(dirname,histname,20,0,20,j);
 // }
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
//  double afp        = s800->GetAFP();
//  double bfp        = s800->GetBFP();
 // double ic_sum     = s800->GetIonChamber().GetAve();
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
  std::string histname2;
  TIonChamber ion = s800->GetIonChamber();
  histname = "ion_summary";
  histname2 = "ion_summary2";
  for(int i=0;i<ion.Size();i++) {
      obj.FillHistogram(dirname,histname,16,0,16,ion.GetChannel(i),
                                         300,0,6000,ion.GetCalData(i));

      obj.FillHistogram(dirname,histname2,16,0,16,ion.GetChannel(i),
                                        300,0,6000,ion.GetAve());
  }


//  histname = "Event_Counter";
//  obj.FillHistogram(dirname,histname,1e+7,0,1e+7,s800->GetEventCounter());

//  histname = "Event_Counter_vs_Timestamp";
//  obj.FillHistogram(dirname,histname,7200,0,7200,s800->GetTimestamp()*1e-8,1e+7,0,1e+7,s800->GetEventCounter());

  
//  histname = "AFP_vs_BFP";
//  obj.FillHistogram(dirname,histname,
//        	    1000,-500,500,afp,
//        	    1000,-500,500,bfp);

  histname = "AFP_vs_BFP";
  obj.FillHistogram(dirname,histname,2000,-10,10,s800->GetAFP(),2000,-10,10,s800->GetBFP());


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
                                     3600,0,7200,s800->Timestamp()*1e-8);

  histname = "CRDC1_Time_vs_Timestamp";
  obj.FillHistogram(dirname,histname,1000,-2000,2000,s800->GetCrdc(0).GetTime(),3600,0,7200,s800->Timestamp()*1e-8);


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

for(int i=0;i<ObjSize;i++){
for(int j=0;j<E1UpSize;j++){
  histname = "OBJ-E1";
  obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(j));}}

for(int i=0;i<XfpSize;i++){
for(int j=0;j<E1UpSize;j++){
  histname = "XFP-E1";
  obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().fXfp.at(i)-s800->GetMTof().fE1Up.at(j));}}

  histname = "OBJ_function";
  obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().GetCorrelatedObjE1());

 histname = "OBJ_Corrected";
  obj.FillHistogram(dirname,histname,1200,-60000,60000,s800->GetCorrTOF_OBJ_MESY());

  histname = "OBJ_MESY_vs_Timestamp";
  obj.FillHistogram(dirname,histname,4000,-20000,20000,s800->GetMTof().GetCorrelatedObjE1(),3600,0,7200,s800->Timestamp()*1e-8);

  histname = "XFP_function";
  obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().GetCorrelatedXfpE1());

  histname = "XFP_function";
  obj.FillHistogram(dirname,histname,8000,-40000,40000,s800->GetMTof().GetCorrelatedXfpE1());

 histname = "XFPE1-OBJE1_OBJCorr_Correlation";
  obj.FillHistogram(dirname,histname,600,3600,4200,s800->GetMTof().GetCorrelatedXfpE1()-s800->GetMTof().GetCorrelatedObjE1(),4000,-4000,0,s800->GetCorrTOF_OBJ_MESY());

if(s800->Timestamp()*1e-8>400){
 histname = "XFPE1-OBJE1_OBJCorr_Correlation_withclosedslits";
  obj.FillHistogram(dirname,histname,600,3600,4200,s800->GetMTof().GetCorrelatedXfpE1()-s800->GetMTof().GetCorrelatedObjE1(),4000,-4000,0,s800->GetCorrTOF_OBJ_MESY());}

if(s800->Timestamp()*1e-8<400){
 histname = "XFPE1-OBJE1_OBJCorr_Correlation_withopenslits";
  obj.FillHistogram(dirname,histname,600,3600,4200,s800->GetMTof().GetCorrelatedXfpE1()-s800->GetMTof().GetCorrelatedObjE1(),4000,-4000,0,s800->GetCorrTOF_OBJ_MESY());}

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

void CheckDAQCorrelation(TRuntimeObjects& obj){

  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank88  *bank88  = obj.GetDetector<TBank88>();
  TS800    *s800    = obj.GetDetector<TS800>();
  TOBJ     *obj_sci = obj.GetDetector<TOBJ>();

  std::string dirname = "DAQ_Correlation";

  if(bank88&&s800){
    obj.FillHistogram(dirname,"tdiff_bank88_s800",3600,0,7200,s800->GetTimestamp()/1e8,1000,-2000,2000,s800->GetTimestamp()-bank88->GetTimestamp());
  }
  if(bank88&&gretina){
    obj.FillHistogram(dirname,"tdiff_bank88_gretina",3600,0,7200,bank88->GetTimestamp(),1000,-2000,2000,gretina->GetGretinaHit(0).Timestamp()-bank88->GetTimestamp());
  }
  if(obj_sci&&s800){
    obj.FillHistogram(dirname,"tdiff_ddas_s800",3600,0,7200,s800->GetTimestamp()/1e8,1000,-2000,2000,s800->GetTimestamp()-obj_sci->GetOBJHit(0).GetExternalTimestamp()*8);
  }
  if(s800&&gretina){
    obj.FillHistogram(dirname,"tdiff_s800_gretina",3600,0,7200,s800->GetTimestamp()/1e8,1000,-2000,2000,s800->GetTimestamp()-gretina->GetGretinaHit(0).Timestamp());
  }
  if(obj_sci&&gretina){
    obj.FillHistogram(dirname,"tdiff_gretina_ddas",3600,0,7200,obj_sci->GetOBJHit(0).GetExternalTimestamp()*8/1e8,1000,-2000,2000,obj_sci->GetOBJHit(0).GetExternalTimestamp()*8-gretina->GetGretinaHit(0).Timestamp());
  }
  if(s800 && s800->GetTrigger().GetRegistr() <100 && s800->GetTrigger().GetRegistr()>0){
     TString dirname = Form("DAQ_Correlation_trigger_%d",s800->GetTrigger().GetRegistr());
     if(bank88&&s800){
       obj.FillHistogram(dirname,"tdiff_bank88_s800",3600,0,7200,s800->GetTimestamp()/1e8,1000,-2000,2000,s800->GetTimestamp()-bank88->GetTimestamp());
     }
     if(bank88&&gretina){
       obj.FillHistogram(dirname,"tdiff_bank88_gretina",3600,0,7200,bank88->GetTimestamp(),1000,-2000,2000,gretina->GetGretinaHit(0).Timestamp()-bank88->GetTimestamp());
     }
     if(obj_sci&&s800){
       obj.FillHistogram(dirname,"tdiff_ddas_s800",3600,0,7200,s800->GetTimestamp()/1e8,1000,-2000,2000,s800->GetTimestamp()-obj_sci->GetOBJHit(0).GetExternalTimestamp()*8);
     }
     if(s800&&gretina){
       obj.FillHistogram(dirname,"tdiff_s800_gretina",3600,0,7200,s800->GetTimestamp()/1e8,1000,-2000,2000,s800->GetTimestamp()-gretina->GetGretinaHit(0).Timestamp());
     }
     if(obj_sci&&gretina){
       obj.FillHistogram(dirname,"tdiff_gretina_ddas",3600,0,7200,obj_sci->GetOBJHit(0).GetExternalTimestamp()*8/1e8,1000,-2000,2000,obj_sci->GetOBJHit(0).GetExternalTimestamp()*8-gretina->GetGretinaHit(0).Timestamp());
     }
  }
}



/*

------------------------------
GRETINA
------------------------------

*/


bool HandleGretinaGated(TRuntimeObjects &obj, GCutG *outgoing) {

  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800    *s800    = obj.GetDetector<TS800>();
  TBank88  *bank88  = obj.GetDetector<TBank88>();
  TOBJ *obj_sci = obj.GetDetector<TOBJ>();

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

    for(unsigned int i=0;i<gretina->Size();i++) {
    TGretinaHit hit = gretina->GetGretinaHit(i);

 obj.FillHistogram(dirname,"DetNumber_vs_Timestamp",
     80,0,80,hit.GetCrystalId(),
     3600,0,3600,s800->Timestamp()*1e-8);

if(hit.GetPad()==0){
         histname="position_good_Decomp";
         obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
         360,0,360,hit.GetPhiDeg());}
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
 if(bank88) {
 histname = Form("Gretina_Bank88_TD");
      obj.FillHistogram(histname,600,-600,600,bank88->Timestamp()-hit.GetTime(),
                                 5000,0,10000,hit.GetCoreEnergy());

if(reg&2){
histname = Form("Gretina_Bank88_TD_Reg2");
      obj.FillHistogram(histname,600,-600,600,bank88->Timestamp()-hit.GetTime(),
                                 5000,0,10000,hit.GetCoreEnergy());
   
histname = Form("Gretina_Bank88_TD_Reg2_Cry%02i",hit.GetCrystalId());
      obj.FillHistogram(histname,600,-600,600,bank88->Timestamp()-hit.GetTime(),
                                 5000,0,10000,hit.GetCoreEnergy());

 }

if(reg&1){
histname = Form("Gretina_Bank88_TD_Reg1_Cry%02i",hit.GetCrystalId());
      obj.FillHistogram(histname,600,-600,600,bank88->Timestamp()-hit.GetTime(),
                                 5000,0,10000,hit.GetCoreEnergy());
    }

}

if(bank88 && s800){

if(reg&2){
histname = Form("S800_Bank88_TD-Energy_Reg2");
      obj.FillHistogram(histname,600,-600,600,bank88->Timestamp()-s800->Timestamp(),
                                 5000,0,10000,hit.GetCoreEnergy());}

if(reg&1){
histname = Form("S800_Bank88_TD-Energy_Reg1");
      obj.FillHistogram(histname,600,-600,600,bank88->Timestamp()-s800->Timestamp(),
                                 5000,0,10000,hit.GetCoreEnergy());
}}
*/

if(outgoing){

if(outgoing->IsInside(s800->GetCorrTOF_OBJTAC(),
s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()))){

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
s800->GetIonChamber().GetdE(s800->GetCrdc(0).GetDispersiveX(),s800->GetCrdc(0).GetNonDispersiveY()))){


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


if(outgoing->IsInside(time_diff,hit.GetCoreEnergy())){

      histname = "Gretina_S800_CoreEnergy_wCut_fakedtime";
      obj.FillHistogram(dirname,histname,2000,0,4000,hit.GetCoreEnergy());

      histname = "Gretina_S800_time_wfaketimecut";
      obj.FillHistogram(dirname,histname,1200,-600,600,time_diff,
                                 2000,0,4000,hit.GetCoreEnergy());


  if(s800->GetScint().GetEUp()>1400 && s800->GetScint().GetEUp()<1850){
      histname = "Gretina_S800_CoreEnergy_wCut_fakedtime_and_energy";
      obj.FillHistogram(dirname,histname,2000,0,4000,hit.GetCoreEnergy());

    }

  } //gate (faked time gate)

 }  //outgoing
   
//if(time_energy->IsInside(hit.GetCoreEnergy(),time_diff)){

//if(time_diff>260 && time_diff<300){

if(s800 && gretina){
      obj.FillHistogram(dirname,"E1Up_Singles_inGretina",2000,0,2000,s800->GetScint().GetEUp()) ;
}
 

}
  return true;
}


void MonitorOBJScintillator_DDAS(TRuntimeObjects &obj){
  TOBJ     *obj_sci = obj.GetDetector<TOBJ>();
  if(obj_sci){
    std::string dirname = "OBJSci";
    for(size_t x = 0; x<obj_sci->Size();x++){
      auto objhit = obj_sci->GetOBJHit(x);
      obj.FillHistogram(dirname,"Energy_of_OBJSci",1000,0,40000,objhit.GetEnergy());
      if(objhit.GetPileup()){
        obj.FillHistogram(dirname,"Energy_of_OBJSci_Pileup",1000,0,40000,objhit.GetEnergy());
      }else{
        obj.FillHistogram(dirname,"Energy_of_OBJSci_noPileup",1000,0,40000,objhit.GetEnergy());
      }

    }
  }
}

void MonitorCrdcs(TRuntimeObjects& obj){
  TS800 *s800 = obj.GetDetector<TS800>();
  if(!s800) return;
  
  std::string dirname = "CRDC_Det";
  auto crdc1 = s800->GetCrdc(0);
  auto crdc2 = s800->GetCrdc(1);
  Double_t crdc1_x = crdc1.GetDispersiveX();
  Double_t crdc1_y = crdc1.GetNonDispersiveY();
  Double_t crdc2_x = crdc2.GetDispersiveX();
  Double_t crdc2_y = crdc2.GetNonDispersiveY();

  obj.FillHistogram(dirname,"Crdc1_xy",800,-400,400,crdc1_x,800,-400,400,crdc1_y);
  obj.FillHistogram(dirname,"Crdc2_xy",800,-400,400,crdc2_x,800,-400,400,crdc2_y);

  obj.FillHistogram(dirname,"Crdc1_time_ts",3600,0,3600,s800->Timestamp()/1e8,500,-2000,2000,crdc1.GetTime());
  obj.FillHistogram(dirname,"Crdc2_time_ts",3600,0,3600,s800->Timestamp()/1e8,500,-2000,2000,crdc2.GetTime());
}


void MonitorICs(TRuntimeObjects &obj){
  TS800 *s800 = obj.GetDetector<TS800>();
  if(!s800) return;

  std::string dirname = "ION_Chamber";
  auto ic = s800->GetIonChamber();
  for(int x = 0; x<ic.Size(); x++){
    obj.FillHistogram(dirname,"Energy_Channel",32,0,32,ic.GetChannel(x),1000,0,4000,ic.GetData(x));
  }
  
  auto crdc1 = s800->GetCrdc(0);
  obj.FillHistogram(dirname,"ICEnergy_crdc1x",800,-400,400,crdc1.GetDispersiveX(),1000,0,4000,ic.GetSum());
  obj.FillHistogram(dirname,"ICAVEnergy_crdc1x",800,-400,400,crdc1.GetDispersiveX(),1000,0,4000,ic.GetAve());
  obj.FillHistogram(dirname,"ICEnergy_crdc1y",800,-400,400,crdc1.GetNonDispersiveY(),1000,0,4000,ic.GetSum());
  obj.FillHistogram(dirname,"ICAVEnergy_crdc1y",800,-400,400,crdc1.GetNonDispersiveY(),1000,0,4000,ic.GetAve());
}

void OutgoingPID(TRuntimeObjects &obj){
  TS800 *s800 = obj.GetDetector<TS800>();
  if(!s800) return;
  
  std::string dirname = "OUT_PID";
  auto ic = s800->GetIonChamber();
  auto crdc1 = s800->GetCrdc(0);
  obj.FillHistogram(dirname,"TOF-dE",2000,-4000,4000,s800->GetCorrTOF_OBJ_MESY(0),4000,0,4000,ic.GetdE(crdc1.GetDispersiveX(),crdc1.GetNonDispersiveY()));
  obj.FillHistogram(dirname,"dE-Crdc1x",800,-400,400,crdc1.GetDispersiveX(),4000,0,4000,ic.GetdE(crdc1.GetDispersiveX(),crdc1.GetNonDispersiveY()));

  auto obj_sci = obj.GetDetector<TOBJ>();
  if(!obj_sci) return;
  if(tmppidcuts.at(0)->IsInside(s800->GetRawOBJ_MESY(0)-s800->GetRawXF_MESY(0),obj_sci->GetOBJHit(0).GetEnergy())){
  obj.FillHistogram(dirname,"TOF-dE-gated-by-sci",2000,-4000,4000,s800->GetCorrTOF_OBJ_MESY(0),4000,0,4000,ic.GetdE(crdc1.GetDispersiveX(),crdc1.GetNonDispersiveY()));
  }
}

void IncomingPID(TRuntimeObjects &obj){
  TS800 *s800 = obj.GetDetector<TS800>();
  TOBJ *obj_sci = obj.GetDetector<TOBJ>();
  if(!s800 || !obj_sci) return;
  
  std::string dirname = "INC_PID";
  for(size_t x = 0; x<obj_sci->Size();x++)
  obj.FillHistogram(dirname,"TOF-dE-Sci",4000,-4000,4000,s800->GetCorrTOF_OBJ_MESY(0),10000,0,40000,obj_sci->GetOBJHit(x).GetEnergy());
  obj.FillHistogram(dirname,"TOF-dE-PIN",4000,-4000,4000,s800->GetCorrTOF_OBJ_MESY(0),1000,0,4000,s800->GetPinE());

  obj.FillHistogram(dirname,"TOF-dE-Sci-raw",4000,-10000,10000,s800->GetRawOBJ_MESY(0)-s800->GetRawXF_MESY(0),1000,0,40000,obj_sci->GetOBJHit(0).GetEnergy());
  obj.FillHistogram(dirname,"TOF-dE-PIN-raw",4000,-10000,10000,s800->GetRawOBJ_MESY(0)-s800->GetRawXF_MESY(0),1000,0,4000,s800->GetPinE());

  if(tmppidcuts.at(0)->IsInside(s800->GetRawOBJ_MESY(0)-s800->GetRawXF_MESY(0),obj_sci->GetOBJHit(0).GetEnergy())){
    obj.FillHistogram(dirname,"TOF-dE-PIN-raw-gated",4000,-10000,10000,s800->GetRawOBJ_MESY(0)-s800->GetRawXF_MESY(0),1000,0,4000,s800->GetPinE());
  }

}

void MonitorTOF(TRuntimeObjects &obj){
  TS800 *s800 = obj.GetDetector<TS800>();
  if(!s800) return;

  std::string dirname = "TOF_Packet";
  auto tof = s800->GetTof();
  obj.FillHistogram(dirname,"OBJ_TAC",4000,-2000,2000,tof.GetTacOBJ());
  obj.FillHistogram(dirname,"XFP_TAC",4000,-2000,2000,tof.GetTacXFP());
  obj.FillHistogram(dirname,"OBJ-XFP",4000,-2000,2000,tof.GetTacOBJ()-tof.GetTacXFP());
  obj.FillHistogram(dirname,"OBJ_vs_XFP",1000,-2000,2000,tof.GetTacOBJ(),1000,-2000,2000,tof.GetTacXFP());

  auto mtof = s800->GetMTof();
  for(int x = 0; x < mtof.XfpSize(); x++)
  for(int y = 0; y < mtof.E1UpSize();y++){
    obj.FillHistogram(dirname,"XFP-E1_Mesy",1000,-10000,10000,s800->GetRawXF_MESY(x)-s800->GetRawE1_MESY(y));
    obj.FillHistogram(dirname,"XFP-E1_Mesy_vs_AFP",3000,-4000,8000,s800->GetCorrTOF_XFP_MESY(x*y+y),1000,-0.1,0.1,s800->GetAFP());
    obj.FillHistogram(dirname,"XFP-E1_Mesy_vs_XFP",3000,-4000,8000,s800->GetCorrTOF_XFP_MESY(x*y+y),1000,-400,400,s800->GetXFP());
  }

  for(int x = 0; x < mtof.ObjSize(); x++)
  for(int y = 0; y < mtof.E1UpSize();y++){
    obj.FillHistogram(dirname,"OBJ-E1_Mesy",1000,-10000,10000,s800->GetRawOBJ_MESY(x)-s800->GetRawE1_MESY(y));
    obj.FillHistogram(dirname,"OBJ-E1_Mesy_vs_AFP",2000,-4000,4000,s800->GetCorrTOF_OBJ_MESY(x*y+y),1000,-0.1,0.1,s800->GetAFP());
    obj.FillHistogram(dirname,"OBJ-E1_Mesy_vs_XFP",2000,-4000,4000,s800->GetCorrTOF_OBJ_MESY(x*y+y),1000,-400,400,s800->GetXFP());
  }

  for(int x = 0; x < mtof.ObjSize(); x++)
  for(int y = 0; y < mtof.XfpSize();y++){
    obj.FillHistogram(dirname,"XFP-OBJ_Mesy",1000,-10000,10000,s800->GetRawOBJ_MESY(x)-s800->GetRawXF_MESY(y));
    obj.FillHistogram(dirname,"XFP-OBJ_Mesy_vs_AFP",2000,-10000,10000,s800->GetRawOBJ_MESY(x)-s800->GetRawXF_MESY(y),100,-0.1,0.1,s800->GetAFP());
    obj.FillHistogram(dirname,"XFP-OBJ_Mesy_vs_BFP",2000,-10000,10000,s800->GetRawOBJ_MESY(x)-s800->GetRawXF_MESY(y),100,-0.1,0.1,s800->GetBFP());
    obj.FillHistogram(dirname,"XFP-OBJ_Mesy_vs_XFP",2000,-10000,10000,s800->GetRawOBJ_MESY(x)-s800->GetRawXF_MESY(y),1000,-400,400,s800->GetXFP());
  }
  obj.FillHistogram(dirname,"XFP_vs_OBJ_MESY",2000,27000,31000,mtof.GetCorrelatedObj(),2000,28000,36000,mtof.GetCorrelatedXfp());
  
}

void CountOBJ(TRuntimeObjects &obj){
  if(pidcuts.size()==0) return;
  TS800 *s800 = obj.GetDetector<TS800>();
  TOBJ *obj_sci = obj.GetDetector<TOBJ>();
  if(!s800) return;
  if(!obj_sci) return;
  auto mtof = s800->GetMTof();
  if(tofcuts.size()==1 && !(tofcuts.at(0)->IsInside(mtof.GetCorrelatedObj(),mtof.GetCorrelatedXfp()))) return;

  std::string dirname="Count_OBJ";
  std::string dirname2="Check_Mdis";
  auto ic = s800->GetIonChamber();
  auto crdc1 = s800->GetCrdc(0);
  double tof = s800->GetCorrTOF_OBJ_MESY(0);
  double dE = ic.GetdE(crdc1.GetDispersiveX(),crdc1.GetNonDispersiveY());
  obj.FillHistogram(dirname,"tof-dE",3000,-4000,8000,tof,4000,0,4000,dE);
  for(size_t x = 0; x<pidcuts.size();x++){
    std::string histname;
    if(pidcuts.at(x)->IsInside(tof,dE)){
      histname = Form("tof-dE_%s",pidcuts.at(x)->GetName());
      obj.FillHistogram(dirname,histname,3000,-4000,8000,tof,4000,0,4000,dE);
      histname = Form("CRDC1_x_%s",pidcuts.at(x)->GetName());
      obj.FillHistogram(dirname2,histname,800,-400,400,crdc1.GetDispersiveX());
      dirname2 = "INC_PID";
      histname = Form("TOF_dE_Sci_Raw_%s",pidcuts.at(x)->GetName());
      obj.FillHistogram(dirname,histname,4000,-10000,10000,s800->GetRawOBJ_MESY(0)-s800->GetRawXF_MESY(0),1000,0,40000,obj_sci->GetOBJHit(0).GetEnergy());
      histname = Form("TOF_dE_Pin_Raw_%s",pidcuts.at(x)->GetName());
      obj.FillHistogram(dirname,histname,4000,-10000,10000,s800->GetRawOBJ_MESY(0)-s800->GetRawXF_MESY(0),1000,0,4000,s800->GetPinE());
    }

  }
  if(!obj_sci) return;
  for(size_t y = 0; y < pidcuts.size(); y++){
    
    std::string histname;
    if(pidcuts.at(y)->IsInside(tof,dE)){
    histname = Form("Energy_if_OBJSci_%s",pidcuts.at(y)->GetName());
    for(size_t x = 0; x<obj_sci->Size();x++)
      obj.FillHistogram(dirname,histname,1000,0,40000,obj_sci->GetOBJHit(x).GetEnergy());
    }
  }

}

void MonitorPin(TRuntimeObjects &obj){

  TS800 *s800 = obj.GetDetector<TS800>();
  if(!s800) return;

  std::string dirname = "PIN";
  obj.FillHistogram(dirname,"Pin_energy",4000,0,4000,s800->GetPinE());
}

void LoadGates(TRuntimeObjects &obj){
  TList *gates = &(obj.GetGates());
  if(!gates||gates->GetSize()==0) return;
  
  if(gates_loaded!=gates->GetSize()){
    TIter iter(gates);
    while(TObject *obj = iter.Next()){
      GCutG *gate = (GCutG*)obj;
      std::string tag = gate->GetTag();
      std::cout<<"tag : "<<tag<<std::endl;
      if(!tag.compare("pid")){
        pidcuts.push_back(gate);
      }else if(!tag.compare("tof")){
        tofcuts.push_back(gate);
      }else if(!tag.compare("temppid")){
        tmppidcuts.push_back(gate);
      }
      gates_loaded++;
    }
  }
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
  TBank88  *bank88  = obj.GetDetector<TBank88>();
  TS800    *s800    = obj.GetDetector<TS800>();
  TOBJ     *obj_sci = obj.GetDetector<TOBJ>();


  LoadGates(obj);
  CheckDAQCorrelation(obj);

  MonitorOBJScintillator_DDAS(obj);
  MonitorCrdcs(obj);
  MonitorICs(obj);
  MonitorTOF(obj);
  MonitorPin(obj);

  OutgoingPID(obj);
  IncomingPID(obj);
  CountOBJ(obj);
  

  return;

  TList *gates = &(obj.GetGates());
  TList    *list    = &(obj.GetObjects());
  int numobj        = list->GetSize();



  if(gates_loaded!=gates->GetSize()) {
    TIter iter(gates);
    while(TObject *obj = iter.Next()) {
      GCutG *gate = (GCutG*)obj;
      std::string tag = gate->GetTag();
      if(!tag.compare("time_energy")) {
//        time_energy_cuts.push_back(gate);
      } 
        else if(!tag.compare("outgoing")) {
        outgoing_cuts.push_back(gate);
      } 
        else if(!tag.compare("incoming")) {
        incoming_cuts.push_back(gate);
      } 
       // else if(!tag.compare("additional")){
       // additional_cuts.push_back(gate);
      //}
      gates_loaded++;
    }
  }


/////////////////////////


  std::string histname = "";
  std::string dirname  = "";

 unsigned int reg = s800->GetReg();

//if(bank88) {
//    for(unsigned int x=0;x<bank88->Size();x++) {
  //    TMode3Hit &hit = (TMode3Hit&)bank88->GetHit(x);
    //  std::string histname = Form("bank88_%i",hit.GetChannel());
      //obj.FillHistogram(histname,16000,0,64000,hit.Charge());
   // }
    if(s800&&bank88) {
     // std::string histname = "S800_Bank88_timestamp_difference";
      obj.FillHistogram("S800_Bank88_timestamp_difference",800,-400,400,bank88->Timestamp()-s800->Timestamp());}

if(!s800 && bank88){
      obj.FillHistogram("S800_Bank88_timestamp_difference",400,-400,400,110);}

if(!bank88 && s800){
      obj.FillHistogram("S800_Bank88_timestamp_difference",400,-400,400,100);}

if(!bank88 && !s800){
      obj.FillHistogram("S800_Bank88_timestamp_difference",400,-400,400,120);}


if(s800 && bank88){

      histname = "S800_Bank88_timestamp_difference_vs_bank88time";
      obj.FillHistogram(histname,7200,0,7200,bank88->Timestamp()*1e-8,400,-400,400,bank88->Timestamp()-s800->Timestamp());

      histname = "S800_Bank88_timestamp_difference_vs_s800time";
      obj.FillHistogram(histname,7200,0,7200,s800->Timestamp()*1e-8,400,-400,400,bank88->Timestamp()-s800->Timestamp());
    }
//  }

if(s800){
      //obj.FillHistogram("E1Up_s800",2000,0,2000,s800->GetScint().GetEUp()) ;
}

/*
  if(!gretina)
    return;
  
  if(gretina) {
    double sumsegener = 0 ;
    for(int i=0;i<gretina->Size();i++) {
      TGretinaHit hit = gretina->GetGretinaHit(i);


      int hole = HoleQMap[hit.GetHoleNumber()];
      obj.FillHistogram("summary",2000,0,2000,hit.GetCoreEnergy(),
                                  200,0,200,hit.GetCrystalId());
      obj.FillHistogram("sum",4000,0,4000,hit.GetCoreEnergy());

         histname="position";
         obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
         360,0,360,hit.GetPhiDeg());
*/

/*
dirname = "Summary_Segments";
double previous = 0 ;


    for(int z=0;z<hit.NumberOfInteractions();z++) {

histname = Form("GretinaSummary_SegEnerTry_X%02i",hit.GetCrystalId());
      obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(z),
      500,0,4000,hit.GetSegmentEner(z));



if(hit.GetPad()==0){
         histname="position_good_Decomp_in_NumberofInteractions_loop";
         obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
         360,0,360,hit.GetPhiDeg());

      // histname = Form("GretinaSummary_goodDecomp_X%02i",hit.GetCrystalId());
      //obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(z),
      //500,0,4000,hit.GetSegmentEng(z));

         histname="position_good_Decomp_with_CC_gate_in_NumberofInteractions_loop";
         if(hit.GetCoreEnergy()>500){
         obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
         360,0,360,hit.GetPhiDeg());}}

}


if(hit.GetPad()==0){
         histname="position_good_Decomp";
         obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
         360,0,360,hit.GetPhiDeg());


         //histname="position_good_Decomp_with_CC_gate_bt_1000keV";
         //if(hit.GetCoreEnergy()>1000){
         //obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
         //360,0,360,hit.GetPhiDeg());}

         //histname="position_good_Decomp_with_CC_gate_bt_500keV";
         //if(hit.GetCoreEnergy()>500){
         //obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
         //360,0,360,hit.GetPhiDeg());}

         //histname="position_good_Decomp_with_CC_gate_lt_500keV";
         //if(hit.GetCoreEnergy()<500){
         //obj.FillHistogram(histname,360,0,360,hit.GetThetaDeg(),
         //360,0,360,hit.GetPhiDeg());}
}
 

if(hole!=0){
 dirname = Form("Det%i",hole);
 histname = Form("Gretina_X_Y_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
         obj.FillHistogram(dirname,histname,720,-360,360,hit.GetX(),
         720,-360,360,hit.GetY());

 for(int j=0;j<4;j++){
 histname = Form("Cores_charge_Cry%02i",hit.GetCrystalId());
      obj.FillHistogram(dirname,histname,4,0,4,j,16000,0,64000,hit.GetCoreCharge(j));
}


 histname = Form("Gretina_GetPad_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
         obj.FillHistogram(dirname,histname,360,0,360,hit.GetPad());

 histname = Form("Gretina_Position_Det%02i",hole);
         obj.FillHistogram(dirname,histname,360,0,360,hit.GetThetaDeg(),
         360,0,360,hit.GetPhiDeg());
 }



 
*/

/*
if(hole!=0){
 dirname = Form("Det%i",hole);
    
 if(bank88) {

if(reg&2){
 histname = Form("Gretina_Bank88_TD_Reg2_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
      obj.FillHistogram(histname,600,-600,600,bank88->Timestamp()-hit.GetTime(),
                                 2000,0,4000,hit.GetCoreEnergy());

 histname = Form("Gretina_Bank88_TDvsBank88TS_Reg2_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
      obj.FillHistogram(histname,7200,0,7200,bank88->Timestamp()*1e-8,700,-400,1000,bank88->Timestamp()-hit.GetTime());
    }

if(reg&1){
 histname = Form("Gretina_Bank88_TD_Reg1_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
      obj.FillHistogram(histname,600,-600,600,bank88->Timestamp()-hit.GetTime(),
                                 2000,0,4000,hit.GetCoreEnergy());

 histname = Form("Gretina_Bank88_TDvsBank88TS_Reg1_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
      obj.FillHistogram(histname,7200,0,7200,bank88->Timestamp()*1e-8,700,-400,1000,bank88->Timestamp()-hit.GetTime());
    }}

    if(s800) {

    //if(s800->GetScint().GetEUp()>1560 && s800->GetScint().GetEUp()<1750){

      //histname = "Gretina_S800_coincidence_gateon_1332keV";
      //obj.FillHistogram(histname,2000,0,4000,hit.GetCoreEnergy());}

 histname = Form("Gretina_S800_TD_Cry%02i_Det%02i",hit.GetCrystalId(),hole);
      obj.FillHistogram(histname,1200,-600,600,s800->Timestamp()-hit.GetTime(),
                                 2000,0,4000,hit.GetCoreEnergy());

      //obj.FillHistogram("E1_Up_coincidence_with_Gretina",2000,0,2000,s800->GetScint().GetEUp());


   }}

 if(bank88) {
 histname = Form("Gretina_Bank88_TD");
      obj.FillHistogram(histname,600,-600,600,bank88->Timestamp()-hit.GetTime(),
                                 5000,0,10000,hit.GetCoreEnergy());

if(reg&2){
histname = Form("Gretina_Bank88_TD_Reg2");
      obj.FillHistogram(histname,600,-600,600,bank88->Timestamp()-hit.GetTime(),
                                 5000,0,10000,hit.GetCoreEnergy());
    }

if(reg&1){
histname = Form("Gretina_Bank88_TD_Reg1");
      obj.FillHistogram(histname,600,-600,600,bank88->Timestamp()-hit.GetTime(),
                                 5000,0,10000,hit.GetCoreEnergy());
    }

}

  if(s800 && bank88){
      std::string histname = "GretinaEnergy_Bank88-S800_timestamp_difference";
      obj.FillHistogram(histname,400,-400,400,bank88->Timestamp()-s800->Timestamp(),    2000,0,4000,hit.GetCoreEnergy());}

  }
 }
*/

/////////////////////////



// Outgoing beam with additional cuts

int incoming_passed=-1;
int outgoing_passed=-1;


  for(unsigned int x=0;x<incoming_cuts.size();x++) {
    bool passed = OutgoingBeam(obj,incoming_cuts.at(x));
   OutgoingBeam(obj,incoming_cuts.at(x)); 
    if(x!=0 && passed) {
      incoming_passed = x;
      break;
    }
 }



 for(unsigned int x=0;x<outgoing_cuts.size();x++) {
    bool passed = false;
    if(x==0 || (x!=0 && incoming_passed>0)) {
      passed = IncomingBeam(obj,outgoing_cuts.at(x)); 
               HandleGretinaGated(obj,outgoing_cuts.at(x));
    }
    if(x!=0 && passed) {
      outgoing_passed = x;
      break;
    }
}
// int incoming_passed=-1;
 //int incoming2_passed=-1;
// int outgoing_passed=-1;
// int break_condition=-1;

//int time_passed=-1;



   
     for(size_t j=0;j<outgoing_cuts.size();j++){
      HandleGretinaGated(obj,outgoing_cuts.at(j));}
      


  if(numobj!=list->GetSize())
    list->Sort();
}


