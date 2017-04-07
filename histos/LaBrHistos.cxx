
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
#include "TFastScint.h"
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

//#define Q1 15
//#define Q2 7
//#define Q3 11
//#define Q4 1
//#define Q5 22
//#define Q6 14
//#define Q7 12
//#define Q8 6
//#define Q9 21

#define Q1 15
#define Q2 7
#define Q3 11
#define Q4 16
#define Q5 8
#define Q6 14
#define Q7 12
#define Q8 6
#define Q9 17
#define Q10 9

//#define BETA .37

std::map<int,int> HoleQMap;
std::map<int,std::string> LayerMap;

//hole_list="15 7 11 16 8 14 12 6 17 9";

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
    if(!incoming->IsInside(s800->GetMTOF_ObjE1(),s800->GetMTOF_XfpE1())) 
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
      obj.FillHistogram(dirname,histname,8000,0,64000,std::abs(s800->GetMTOF_ObjE1(i,false)));//mtof.fE1Up.at(i)-mtof.fObj.at(i));
    }
  }
  
  histname = "time_x";
  obj.FillHistogram(dirname,histname,600,-300,300,s800->GetCrdc(0).GetDispersiveX(),
                                     600,-3000,-1500,s800->GetMTOF_ObjE1());
                                     
  
  histname = "time_afp";
  obj.FillHistogram(dirname,histname,600,-3000,-1500,s800->GetMTOF_ObjE1(),
                                     100,-.1,.1,s800->GetAFP());
  
  histname = "time_charge";
  obj.FillHistogram(dirname,histname,1200,-3000,-1500,s800->GetMTOF_ObjE1(),
                                   6000,0,30000,s800->GetIonChamber().Charge());
  
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
  histname = "obj_xfp_2d";
  obj.FillHistogram(dirname,histname,2000,-5000,-1000,s800->GetMTOF_ObjE1(),
                                     1600,-3200,3200,s800->GetMTOF_XfpE1());
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
    obj.FillHistogram(dirname,histname,180,0,180,gretina->GetGretinaHit(x).GetThetaDeg(),
                                       360,0,360,gretina->GetGretinaHit(x).GetPhiDeg());
    TGretinaHit hit = gretina->GetGretinaHit(x);
    for(int y=0;y<hit.Size();y++) {
      if(hit.GetSegmentId(y)>36)
        continue;
      histname = Form("GretinaSummaryX%02i",hit.GetCrystalId());
      obj.FillHistogram(dirname,histname,40,0,40,hit.GetSegmentId(y),
                                         2000,0,4000,hit.GetSegmentEng(y));

      int layer  = hit.GetSegmentId(y)/6;
      histname = Form("position_%s",LayerMap[layer].c_str());
      double phi = hit.GetIntPosition(y).Phi();
      if(phi<0) phi +=TMath::Pi()*2;
      if(hit.GetPad()==0) {
        obj.FillHistogram(dirname,histname,
                          628,0.0,6.28,phi,
        		  314,0,3.14,hit.GetIntPosition(y).Theta());
      }
    }
  }
  return true;
}


bool HandleLaBr(TRuntimeObjects &obj,GCutG *outgoing=0) {

  TFastScint *labr = obj.GetDetector<TFastScint>();
  TS800 *s800      = obj.GetDetector<TS800>();

  if(!labr)
    return false;
 
  std::string dirname = "labr";
  std::string histname;

  for(unsigned int x=0;x<labr->Size();x++) {
    if(labr->GetLaBrHit(x).GetChannel()==15) {
      histname="chan_15";
      obj.FillHistogram(dirname,histname,4000,0,4000,labr->GetLaBrHit(x).Charge());
      
      histname="chan_15_time";
      obj.FillHistogram(dirname,histname,130,0,3600,labr->Timestamp()/(double)1e8,
                                         4000,0,4000,labr->GetLaBrHit(x).Charge());

     for(unsigned int z=0;z<labr->Size();z++) {
      if(labr->GetLaBrHit(z).GetChannel()>=15)
         continue;
      histname=Form("chan_15_charge_chan_%i_charge",labr->GetLaBrHit(z).GetChannel());
      obj.FillHistogram(dirname,histname,1000,0,4000,labr->GetLaBrHit(x).Charge(),
                                         1000,0,4000,labr->GetLaBrHit(z).Charge());
     }

     TGretina *gretina = obj.GetDetector<TGretina>();
     if(gretina) {
       std::string gdirname = "gretina";
       histname = "labr_gretina";
       for(unsigned int y=0;y<gretina->Size();y++) {
         obj.FillHistogram(gdirname,histname,4000,0,8000,gretina->GetGretinaHit(y).GetCoreEnergy(),
                                             1000,0,4000,labr->GetLaBrHit(x).Charge());


       }
     }
    }
    if(labr->GetLaBrHit(x).GetChannel()==0) {
      histname="chan_0";
      obj.FillHistogram(dirname,histname,4000,0,4000,labr->GetLaBrHit(x).Charge());
      
      histname="chan_0_time";
      obj.FillHistogram(dirname,histname,130,0,3600,labr->Timestamp()/(double)1e8,
                                         4000,0,4000,labr->GetLaBrHit(x).Charge());
     
    }

    histname="summary_charge";
    obj.FillHistogram(dirname,histname,4000,0,4000,labr->GetLaBrHit(x).Charge(),
                                       32,0,32,labr->GetLaBrHit(x).GetChannel());
    histname="summary_energy";
    obj.FillHistogram(dirname,histname,4000,0,4000,labr->GetLaBrHit(x).GetEnergy(),
                                       32,0,32,labr->GetLaBrHit(x).GetChannel());
  
    histname="summary_time";
    obj.FillHistogram(dirname,histname,8000,0,64000,labr->GetLaBrHit(x).GetTime(),
                                       32,0,32,labr->GetLaBrHit(x).GetChannel());

    if(labr->GetLaBrHit(x).GetTime() >100 &&  labr->GetLaBrHit(x).Charge() >100) {
      histname="summary_timeref";
      obj.FillHistogram(dirname,histname,16000,0,64000,labr->GetRefTime() - labr->GetLaBrHit(x).GetTime(),
                                         32,0,32,labr->GetLaBrHit(x).GetChannel());

    histname = Form("chan_%i_time_charge",labr->GetLaBrHit(x).GetChannel());
    obj.FillHistogram(dirname,histname,3000,10000,13000,labr->GetRefTime() - labr->GetLaBrHit(x).GetTime(),
                                       1000,0,4000,labr->GetLaBrHit(x).Charge());

    //histname = Form("chan_%i_time_energy",labr->GetLaBrHit(x).GetChannel());
    //obj.FillHistogram(dirname,histname,3000,10000,13000,labr->GetRefTime() - labr->GetLaBrHit(x).GetTime(),
    //                                   1000,0,4000,labr->GetLaBrHit(x).GetEnergy());   

    }

    if(labr->GetLaBrHit(x).GetTime() >100 &&  labr->GetLaBrHit(x).Charge() >100 && s800->GetReg()==1) {
      histname="summary_timeref_reg1";
      obj.FillHistogram(dirname,histname,16000,0,64000,labr->GetRefTime() - labr->GetLaBrHit(x).GetTime(),
                                         32,0,32,labr->GetLaBrHit(x).GetChannel());

    histname = Form("chan_%i_time_charge_reg1",labr->GetLaBrHit(x).GetChannel());
    obj.FillHistogram(dirname,histname,3000,10000,13000,labr->GetRefTime() - labr->GetLaBrHit(x).GetTime(),
                                       1000,0,4000,labr->GetLaBrHit(x).Charge());

    //histname = Form("chan_%i_time_energy",labr->GetLaBrHit(x).GetChannel());
    //obj.FillHistogram(dirname,histname,3000,10000,13000,labr->GetRefTime() - labr->GetLaBrHit(x).GetTime(),
    //                                   1000,0,4000,labr->GetLaBrHit(x).GetEnergy());   

    }
  }


  return true;
}


bool HandleS800(TRuntimeObjects &obj,GCutG *outgoing=0) {

  TS800 *s800 = obj.GetDetector<TS800>();
  if(!s800)
    return false;
 
  std::string dirname = "s800";
  std::string histname;

  histname="e1_charge";
  obj.FillHistogram(dirname,histname,2000,0,2000,s800->GetScint().GetEUp());

  TGretina *gretina = obj.GetDetector<TGretina>();
  if(gretina) {
    std::string gdirname = "gretina";
    histname = "e1_gretina";
    for(unsigned int y=0;y<gretina->Size();y++) {
      obj.FillHistogram(gdirname,histname,4000,0,8000,gretina->GetGretinaHit(y).GetCoreEnergy(),
                                          2000,0,2000,s800->GetScint().GetEUp());


    }
 }



}


bool HandleS800Gated(TRuntimeObjects &obj,GCutG *incoming=0, GCutG *outgoing=0) {
  TS800    *s800    = obj.GetDetector<TS800>();

  if( !s800) 
    return false;

  std::string dirname = "s800_gated";
  std::string histname;

  histname = Form("CRDC1_X_%s_%s",incoming->GetName(),outgoing->GetName());
  obj.FillHistogram(dirname,histname,800,-400,400,s800->GetCrdc(0).GetDispersiveX());

  histname = Form("CRDC2_X_%s_%s",incoming->GetName(),outgoing->GetName());
  obj.FillHistogram(dirname,histname,800,-400,400,s800->GetCrdc(1).GetDispersiveX());
  return true;
}
  
bool HandleGretinaGated(TRuntimeObjects &obj,GCutG *incoming=0, GCutG *outgoing=0) {

  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800    *s800    = obj.GetDetector<TS800>();

  if(!gretina || !s800) 
    return false;

  std::string dirname = "gretina";
  std::string histname;

  
  //if(!outgoing || !outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),s800->GetIonChamber().Charge())) 
  if(!outgoing || !outgoing->IsInside(s800->GetMTOF_ObjE1(),s800->GetIonChamber().Charge())) 
    return false;
  //if(!incoming || !incoming->IsInside(s800->GetMTOF_ObjE1(),s800->GetMTOF_XfpE1())) 
  //  return false;

  //histname = Form("gretina_%s_%s",incoming->GetName(),outgoing->GetName());
  histname = Form("gretina__%s",outgoing->GetName());
  for(unsigned int x=0;x<gretina->Size();x++) {
    obj.FillHistogram(dirname,histname,1000,0,4000,
                      gretina->GetGretinaHit(x).GetDoppler(GValue::Value("BETA")));
  }
  return true;
}

bool HandleLaBrGated(TRuntimeObjects &obj,GCutG *incoming=0,GCutG *outgoing=0) {

  TFastScint *labr = obj.GetDetector<TFastScint>();
  TS800    *s800    = obj.GetDetector<TS800>();
  if(!labr || !s800)
    return false;
 
  std::string dirname = "labr";
  std::string histname;
  
  //if(!outgoing || !outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),s800->GetIonChamber().Charge())) 
  if(!outgoing || !outgoing->IsInside(s800->GetMTOF_ObjE1(),s800->GetIonChamber().Charge())) 
    return false;
  //if(!incoming || !incoming->IsInside(s800->GetMTOF_ObjE1(),s800->GetMTOF_XfpE1())) 
  //  return false;

  //histname = Form("summary_%s_%s",incoming->GetName(),outgoing->GetName());
  //histname = Form("summary_%s",outgoing->GetName());
  //for(unsigned int x=0;x<labr->Size();x++) {
  //  obj.FillHistogram(dirname,histname,4000,0,4000,labr->GetLaBrHit(x).GetDoppler(GValue::Value("BETA")),
  //                                     20,0,20,labr->GetLaBrHit(x).GetChannel());
  //}
  return true;
}


extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank29  *bank29  = obj.GetDetector<TBank29>();
  TS800    *s800    = obj.GetDetector<TS800>();
  TFastScint *labr  = obj.GetDetector<TFastScint>();
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
        std::cout << "incoming: << " << gate->GetName() << std::endl;
      } else if(!tag.compare("outgoing")) {
        outgoing_cuts.push_back(gate);
        std::cout << "outgoing: << " << gate->GetName() << std::endl;
      } else {
        std::cout << "unknown: << " << gate->GetName() << std::endl;
      }
      gates_loaded++;
    }
    std::cout << "outgoing size: " << outgoing_cuts.size() << std::endl;
  }

  std::string histname = "";
  std::string dirname  = "";

  if(bank29) {
    dirname  = "Bank29";
    for(unsigned int x=0;x<bank29->Size();x++) {
      TMode3Hit &hit = (TMode3Hit&)bank29->GetHit(x);
      dirname = "Bank29";
      histname = Form("bank29_%i",hit.GetChannel());
      obj.FillHistogram(dirname,histname,
          16000,0,64000,hit.Charge());
    }
    if(s800) {
      histname = "S800_Bank29_time";
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
    if(gretina) {
      for(unsigned int i=0;i<gretina->Size();i++) {
        TGretinaHit hit = gretina->GetGretinaHit(i);
        histname = "Gretina_Bank29_time";
        obj.FillHistogram(dirname,histname,
                          600,-600,600,bank29->Timestamp()-hit.Timestamp(),
                          2000,0,4000,hit.GetCoreEnergy());
        histname = "Gretina_t0_Bank29_time";
        obj.FillHistogram(dirname,histname,
                          600,-600,600,bank29->Timestamp()-hit.GetTime(),
                          2000,0,4000,hit.GetCoreEnergy());
      }
    }
    if(labr) {
      histname = "labr_Bank29_time";
      obj.FillHistogram(dirname,histname,
                        600,-600,600,bank29->Timestamp()-labr->Timestamp());

    }
  }

  std::vector<int> incoming_passed;
  std::vector<int> outgoing_passed;
  
  for(unsigned int x=0;x<incoming_cuts.size();x++) {
    bool passed = OutgoingBeam(obj,incoming_cuts.at(x)); 
    if(x!=0 && passed) {
      incoming_passed.push_back(x);
      break;
    }
  }

  for(unsigned int x=0;x<outgoing_cuts.size();x++) {
    bool passed = true; //false;
    //if(x==0 || (x!=0)) { //&& incoming_passed>0)) {
    //  passed = IncomingBeam(obj,outgoing_cuts.at(x)); 
    //}
    if(x!=0 && passed) {
      outgoing_passed.push_back(x);
      //break;
    }
  }
  

  HandleGretina(obj);
  HandleLaBr(obj);
  HandleS800(obj);

  //for(unsigned int i=0; i<incoming_passed.size(); i++) {
    for(unsigned int j=0; j<outgoing_passed.size(); j++) {
      //HandleGretinaGated(obj,incoming_cuts.at(incoming_passed.at(i)),outgoing_cuts.at(outgoing_passed.at(j)));
      //HandleLaBrGated(obj,incoming_cuts.at(incoming_passed.at(i)),outgoing_cuts.at(outgoing_passed.at(j)));
      //HandleS800Gated(obj,incoming_cuts.at(incoming_passed.at(i)),outgoing_cuts.at(outgoing_passed.at(j)));
      //std::cout << outgoing_cuts.at(outgoing_passed.at(j))->GetName() << std::endl;
      HandleGretinaGated(obj,0,outgoing_cuts.at(outgoing_passed.at(j)));
      HandleLaBrGated(obj,0,outgoing_cuts.at(outgoing_passed.at(j)));
      //HandleS800Gated(obj,0,outgoing_cuts.at(outgoing_passed.at(j)));
    }
  //}

  if(numobj!=list->GetSize())
    list->Sort();
}




