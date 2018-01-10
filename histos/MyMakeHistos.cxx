
#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>
#include <TLine.h>

#include "TGretina.h"
#include "TS800.h"
#include "TBank29.h"
#include "TS800.h"
#include "GCutG.h"

#include "TChannel.h"
#include "GValue.h"

//#define Q1 15
//#define Q2 7
//#define Q3 8
//#define Q4 16
//#define Q5 9
//#define Q6 14
//#define Q7 17
//#define Q8 6
//#define Q9 19

#define Q1 15
#define Q2 7
#define Q3 11
#define Q4 1
#define Q5 22
#define Q6 14
#define Q7 12
#define Q8 6
#define Q9 21

//#define BETA .37

std::map<int,int> HoleQMap;
std::map<int,std::string> LayerMap;

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

  LayerMap[0] = "alpha";
  LayerMap[1] = "beta";
  LayerMap[2] = "gamma";
  LayerMap[3] = "delta";
  LayerMap[4] = "epsilon";
  LayerMap[5] = "phi";

}

#define INTEGRATION 128.0

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.

std::vector<GCutG*> incoming_cuts = {0};
std::vector<GCutG*> outgoing_cuts = {0};

int gates_loaded=0;

//for(unsigned int j=0;j<outgoing.size();j++) {

bool OutgoingBeam(TRuntimeObjects& obj,GCutG *incoming) {  //TRuntimeObjects &obj,TS800 *s800,GCutG *outgoing) {
  TS800    *s800    = obj.GetDetector<TS800>();
  if(!s800)
    return false;
  
  std::string dirname;
  if(incoming)
    dirname = Form("outgoing_%s",incoming->GetName());
  else 
    dirname = "outgoing";

  if(incoming) { 
    if(!incoming->IsInside(s800->GetMTOF_ObjE1(),s800->GetMTOF_RfE1())) 
      return false;
  }
 
  std::string histname;
  
  TIonChamber ion = s800->GetIonChamber();
  histname = "ion_summary";
  for(int i=0;i<ion.Size();i++) {
      obj.FillHistogram(dirname,histname,16,0,16,ion.GetChannel(i),
                                         300,0,6000,ion.GetCalData(i));
  }
  histname = "CRDC1_X";
  obj.FillHistogram(dirname,histname,800,-400,400,s800->GetCrdc(0).GetDispersiveX());

  histname = "CRDC2_X";
  obj.FillHistogram(dirname,histname,800,-400,400,s800->GetCrdc(1).GetDispersiveX());
  
  TMTof &mtof = s800->GetMTof();
  
  for(int i=0;i<mtof.E1UpSize();i++) {
    if(i<mtof.RefSize()) {
    //for(int j=0;j<mtof.E1UpSize();j++) {
      histname = "E1up";
      obj.FillHistogram(dirname,histname,8000,0,64000,mtof.fE1Up.at(i)-mtof.fRef.at(i));
    }
    if(i<mtof.RfSize()) {
      histname = "Rf";
      obj.FillHistogram(dirname,histname,8000,0,64000,std::abs(s800->GetMTOF_RfE1(i))); //mtof.fE1Up.at(i)-mtof.fRf.at(i)));
    }
    if(i<mtof.ObjSize()) {
      histname = "Obj";
      obj.FillHistogram(dirname,histname,8000,0,64000,std::abs(s800->GetMTOF_ObjE1(i)));//mtof.fE1Up.at(i)-mtof.fObj.at(i));
    }
  }
  
  histname = "time_x";
  obj.FillHistogram(dirname,histname,600,-6000,-3000,s800->GetCorrTOF_OBJ_MESY(),
                                     600,-300,300,s800->GetCrdc(0).GetDispersiveX());
  
  histname = "time_afp";
  obj.FillHistogram(dirname,histname,600,-6000,-3000,s800->GetCorrTOF_OBJ_MESY(),
                                     100,-.1,.1,s800->GetAFP());
  
  histname = "time_charge";
  obj.FillHistogram(dirname,histname,1200,-6000,-3000,s800->GetCorrTOF_OBJ_MESY(),
                                   6000,22000,45000,s800->GetIonChamber().Charge());
  
  return true;
}


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
  TMTof &mtof = s800->GetMTof();


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
  //histname = "obj_xfp_2d";
  //obj.FillHistogram(dirname,histname,800,0,64000,s800->GetMTOF_ObjE1(),
  //                                   800,0,64000,s800->GetMTOF_XfpE1());
 
  histname = "trigger_bit";
  unsigned short bits = s800->GetTrigger().GetRegistr();
  for(int j=0;j<16;j++) {
    if(((bits>>j)&0x0001))
      obj.FillHistogram(dirname,histname,20,0,20,j);
  }
  histname = "trigger_raw";
  obj.FillHistogram(dirname,histname,20,0,20,s800->GetTrigger().GetRegistr());

  return true;
}



bool HandleGretina(TRuntimeObjects &obj,GCutG *outgoing=0) {

  TGretina *gretina = obj.GetDetector<TGretina>();
  if(!gretina)
    return false;
 
  std::string dirname = "gretina";
  std::string histname;

  for(unsigned int x=0;x<gretina->Size();x++) {
    histname="summary";
    obj.FillHistogram(dirname,histname,4000,0,4000,gretina->GetGretinaHit(x).GetCoreEnergy(),
                                       120,0,120,gretina->GetGretinaHit(x).GetCrystalId());
    histname="singles";
    obj.FillHistogram(dirname,histname,12000,0,6000,gretina->GetGretinaHit(x).GetCoreEnergy());
  
    histname="singles";
    obj.FillHistogram(dirname,histname,12000,0,6000,gretina->GetGretinaHit(x).GetCoreEnergy());
  
    histname="position";
    obj.FillHistogram(dirname,histname,360,0,360,gretina->GetGretinaHit(x).GetThetaDeg(),
                                       180,0,180,gretina->GetGretinaHit(x).GetPhiDeg());
  
  }
  return true;
}

bool HandleGretinaGated(TRuntimeObjects &obj,GCutG *outgoing=0) {

  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800    *s800    = obj.GetDetector<TS800>();

  if(!gretina || !s800) 
    return false;

  std::string dirname = "gretina";
  std::string histname;

  
  if(outgoing &&
    outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),s800->GetIonChamber().Charge())) {
    histname = Form("gretina_%s",outgoing->GetName());
    for(unsigned int x=0;x<gretina->Size();x++) {
      obj.FillHistogram(dirname,histname,1000,0,4000,
                        gretina->GetGretinaHit(x).GetDoppler(GValue::Value("BETA")));
    }
  }
 
  return true;
}


extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank29  *bank29  = obj.GetDetector<TBank29>();
  TS800    *s800    = obj.GetDetector<TS800>();
  TList    *list    = &(obj.GetObjects());
  int numobj = list->GetSize();

  TList *gates = &(obj.GetGates());

  if(gates_loaded!=gates->GetSize()) {
    TIter iter(gates);
    while(TObject *obj = iter.Next()) {
      GCutG *gate = (GCutG*)obj;
      std::string tag = gate->GetTag();
      if(!tag.compare("incoming")) {
        incoming_cuts.push_back(gate);
      } else if(!tag.compare("outgoing")) {
        outgoing_cuts.push_back(gate);
      }
      gates_loaded++;
    }
  }

  std::string histname = "";
  std::string dirname  = "";

  if(bank29) {
    for(unsigned int x=0;x<bank29->Size();x++) {
      TMode3Hit &hit = (TMode3Hit&)bank29->GetHit(x);
      dirname = "Bank29";
      histname = Form("bank29_%i",hit.GetChannel());
      obj.FillHistogram(dirname,histname,
          16000,0,64000,hit.Charge());
    }
    if(s800) {
      histname = "S800_Bank29_time";
      dirname  = "Bank29";
      obj.FillHistogram(dirname,histname,
          200,-200,200,bank29->Timestamp()-s800->Timestamp());

      histname = "S800_Bank29_runtime";
      obj.FillHistogram(dirname,histname,
                        3600,0,3600,bank29->Timestamp()*1e-8,
                        1000,-1000,1000,bank29->Timestamp()-s800->Timestamp());
      histname = Form("S800_Bank29_runtime_Reg%i",s800->GetTrigger().GetRegistr());
      obj.FillHistogram(dirname,histname,
                        3600,0,3600,bank29->Timestamp()*1e-8,
                        1000,-1000,1000,bank29->Timestamp()-s800->Timestamp());
    }
  }

  int incoming_passed=-1;
  int outgoing_passed=-1;
  
  for(unsigned int x=0;x<incoming_cuts.size();x++) {
    bool passed = OutgoingBeam(obj,incoming_cuts.at(x)); 
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
  

  HandleGretina(obj);


  if(numobj!=list->GetSize())
    list->Sort();
}




