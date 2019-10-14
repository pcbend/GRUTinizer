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

void CheckDAQCorrelation(TRuntimeObjects& obj){

  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank88  *bank88  = obj.GetDetector<TBank88>();
  TS800    *s800    = obj.GetDetector<TS800>();
  TOBJ     *obj_sci = obj.GetDetector<TOBJ>();

  std::string dirname = "DAQ_Correlation";
  
  // Check tdiff between different DAQsys
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

  // Check tdiff between differnent DAQ sys with different triggers
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

  obj.FillHistogram(dirname,"Crdc1_time_ts",3600,0,7200,s800->Timestamp()/1e8,500,-2000,5000,crdc1.GetTime());
  obj.FillHistogram(dirname,"Crdc2_time_ts",3600,0,7200,s800->Timestamp()/1e8,500,-2000,5000,crdc2.GetTime());
  obj.FillHistogram(dirname,"Crdc1_anode_ts",3600,0,7200,s800->Timestamp()/1e8,500,-2000,5000,crdc1.GetAnode());
  obj.FillHistogram(dirname,"Crdc2_anode_ts",3600,0,7200,s800->Timestamp()/1e8,500,-2000,5000,crdc2.GetAnode());
  obj.FillHistogram(dirname,"Crdc1_anode_time",500,-2000,5000,crdc1.GetAnode(),500,-2000,5000,crdc1.GetTime());
  obj.FillHistogram(dirname,"Crdc2_anode_time",500,-2000,5000,crdc2.GetAnode(),500,-2000,5000,crdc2.GetTime());

  obj.FillHistogram(dirname,"E1_ts",3600,0,7200,s800->Timestamp()/1e8,1000,0,70000,s800->GetMTof().GetCorrelatedE1Up());

  if(s800 && s800->GetTrigger().GetRegistr() <100 && s800->GetTrigger().GetRegistr()>0){
    TString histname = Form("crdc1_anode_time_%d",s800->GetTrigger().GetRegistr());
    obj.FillHistogram(dirname,histname.Data(),500,-2000,5000,crdc1.GetAnode(),500,-2000,5000,crdc1.GetTime());
    histname = Form("crdc2_anode_time_%d",s800->GetTrigger().GetRegistr());
    obj.FillHistogram(dirname,histname.Data(),500,-2000,5000,crdc2.GetAnode(),500,-2000,5000,crdc2.GetTime());
  }
  
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
  obj.FillHistogram(dirname,"ICAVEnergy_ts",3600,0,7200,s800->Timestamp()/1e8,1000,0,4000,ic.GetAve());
}

void OutgoingPID(TRuntimeObjects &obj){
  TS800 *s800 = obj.GetDetector<TS800>();
  if(!s800) return;
  
  std::string dirname = "OUT_PID";
  auto ic = s800->GetIonChamber();
  auto crdc1 = s800->GetCrdc(0);
  obj.FillHistogram(dirname,"TOF-dE",2000,-2000,0,s800->GetCorrTOF_OBJ_MESY(0),4000,0,4000,ic.GetdE(crdc1.GetDispersiveX(),crdc1.GetNonDispersiveY()));
  obj.FillHistogram(dirname,"dE-Crdc1x",800,-400,400,crdc1.GetDispersiveX(),4000,0,4000,ic.GetdE(crdc1.GetDispersiveX(),crdc1.GetNonDispersiveY()));

  /*
  auto obj_sci = obj.GetDetector<TOBJ>();
  if(!obj_sci) return;
  if(tmppidcuts.at(0)->IsInside(s800->GetRawOBJ_MESY(0)-s800->GetRawXF_MESY(0),obj_sci->GetOBJHit(0).GetEnergy())){
  obj.FillHistogram(dirname,"TOF-dE-gated-by-sci",2000,-4000,4000,s800->GetCorrTOF_OBJ_MESY(0),4000,0,4000,ic.GetdE(crdc1.GetDispersiveX(),crdc1.GetNonDispersiveY()));
  }
  */
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
  obj.FillHistogram(dirname,"TOF-Obj-Rf-dE-Sci-raw",4000,-10000,10000,s800->GetRawOBJ_MESY(0)-s800->GetMRf(0),1000,0,40000,obj_sci->GetOBJHit(0).GetEnergy());
  obj.FillHistogram(dirname,"TOF-xfp-Rf-dE-Sci-raw",4000,-10000,10000,s800->GetRawXF_MESY(0)-s800->GetMRf(0),1000,0,40000,obj_sci->GetOBJHit(0).GetEnergy());
  obj.FillHistogram(dirname,"TOF-dE-PIN-raw",4000,-10000,10000,s800->GetRawOBJ_MESY(0)-s800->GetRawXF_MESY(0),1000,0,4000,s800->GetPinE());
/*
  if(tmppidcuts.at(0)->IsInside(s800->GetRawOBJ_MESY(0)-s800->GetRawXF_MESY(0),obj_sci->GetOBJHit(0).GetEnergy())){
    obj.FillHistogram(dirname,"TOF-dE-PIN-raw-gated",4000,-10000,10000,s800->GetRawOBJ_MESY(0)-s800->GetRawXF_MESY(0),1000,0,4000,s800->GetPinE());
  }
*/
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
 
  
  if(s800 && s800->GetTrigger().GetRegistr() <100 && s800->GetTrigger().GetRegistr()>0){
     TString histname = Form("XFP-OBJ_%d",s800->GetTrigger().GetRegistr());
     for(int x = 0; x < mtof.ObjSize(); x++)
     for(int y = 0; y < mtof.XfpSize();y++){
       obj.FillHistogram(dirname,histname.Data(),1000,-10000,10000,s800->GetRawOBJ_MESY(x)-s800->GetRawXF_MESY(y));
     }
  }
}

void CountOBJ(TRuntimeObjects &obj){
  if(pidcuts.size()==0) return;
  TS800 *s800 = obj.GetDetector<TS800>();
  TOBJ *obj_sci = obj.GetDetector<TOBJ>();
  if(!s800) return;
  auto ic = s800->GetIonChamber();
  auto crdc1 = s800->GetCrdc(0);
  double tof = s800->GetCorrTOF_OBJ_MESY(0);
  double dE = ic.GetdE(crdc1.GetDispersiveX(),crdc1.GetNonDispersiveY());
  std::string dirname="Count_OBJ";
  for(size_t x = 0; x<pidcuts.size();x++){
    std::string histname;
    if(pidcuts.at(x)->IsInside(tof,dE)){
      histname = Form("tof-dE_%s",pidcuts.at(x)->GetName());
      obj.FillHistogram(dirname,histname,3000,-4000,8000,tof,4000,0,4000,dE);
    }
  }
  if(!obj_sci) return;
  auto mtof = s800->GetMTof();
  if(tofcuts.size()==1 && !(tofcuts.at(0)->IsInside(mtof.GetCorrelatedObj(),mtof.GetCorrelatedXfp()))) return;

  obj.FillHistogram(dirname,"tof-dE",3000,-4000,8000,tof,4000,0,4000,dE);

  std::string dirname2="Check_Mdis";
  for(size_t x = 0; x<pidcuts.size();x++){
    std::string histname;
    if(pidcuts.at(x)->IsInside(tof,dE)){
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
    histname = Form("Energy_of_OBJSci_%s",pidcuts.at(y)->GetName());
      for(size_t x = 0; x<obj_sci->Size();x++){
        double tdiff = s800->GetTimestamp()-obj_sci->GetOBJHit(x).GetExternalTimestamp()*8;
        if(tdiff>20&&tdiff<60)
        obj.FillHistogram(dirname,histname,1000,0,40000,obj_sci->GetOBJHit(x).GetEnergy());
      }
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

  LoadGates(obj);
  CheckDAQCorrelation(obj);

  // Quite important for online monitoring
  MonitorOBJScintillator_DDAS(obj);
  MonitorCrdcs(obj);
  MonitorICs(obj);
  MonitorTOF(obj);
//  MonitorPin(obj);

  OutgoingPID(obj);
  IncomingPID(obj);
  CountOBJ(obj);
  TS800 *s800 = obj.GetDetector<TS800>();
  if(s800)
  obj.FillHistogram("triggers",10,0,10,s800->GetTrigger().GetRegistr());
  

  return;


/////////////////////////

}
