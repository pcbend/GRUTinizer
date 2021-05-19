
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

GCutG *incoming_s44=0;

GCutG *outgoing_s44=0;

GCutG *gt_time=0;

bool OutgoingS800(TRuntimeObjects &obj, TS800 *s800, GCutG *outgoing) {

   if(!s800)
     return false;
   
   std::string dirname = "S800";
   std::string histname;
   if(outgoing) {
     //     if(!outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
     //			    s800->GetIonChamber().GetSum()) )
     if( !outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
			     s800->GetIonChamber().Charge()) )
       return false;
     histname = Form("incoming_%s", outgoing->GetName());
   } else {
     histname = "incoming";
   }
   obj.FillHistogram(dirname, histname,
		     1000, -3000, -1600,
		     s800->GetOBJ_E1Raw_MESY(),
		     1000,  1500, 2700,
		     s800->GetXF_E1Raw_MESY());

   return true;
}


bool IncomingS800(TRuntimeObjects &obj,TS800 *s800,GCutG *incoming) {
 
   if(!s800)
     return false;
   
   std::string dirname = "S800";
   std::string histname;
   if(incoming) {
     if( !incoming->IsInside(s800->GetOBJ_E1Raw_MESY(),
			     s800->GetXF_E1Raw_MESY()) )
       return false;
     histname = Form("outgoing_%s", incoming->GetName());
   } else {
     histname = "outgoing";
   }
   obj.FillHistogram(dirname, histname,
		     1000, -3000, -2100,
		     s800->GetCorrTOF_OBJ_MESY(),
		     1000, 32000, 60000,
   		     s800->GetIonChamber().Charge());
   //		     s800->GetIonChamber().GetSum());
   
   //   TMTof &mtof = s800->GetMTof();
   //histname = "RefSize";
   //obj.FillHistogram(dirname,histname,20,0,0,mtof.RefSize(),
   //                                   20,0,0,mtof.E1UpSize());
   
   //for(int i=0;i<mtof.E1UpSize();i++) {
   //  if(i<mtof.RefSize()) {
   //  //for(int j=0;j<mtof.E1UpSize();j++) {
   //    histname = "E1up";
   //    obj.FillHistogram(dirname,histname,2000,0,0,mtof.fE1Up.at(i)-mtof.fRef.at(i));
   //  }
   //  if(i<mtof.RfSize()) {
   //  //for(int j=0;j<mtof.RfSize();j++) {
   //    histname = "Rf";
   //    obj.FillHistogram(dirname,histname,5000,0,0,mtof.fE1Up.at(i)-mtof.fRf.at(i));
   //  }
   //  if(i<mtof.ObjSize()) {
   //  //for(int j=0;j<mtof.ObjSize();j++) {
   //    histname = "Obj";
   //    obj.FillHistogram(dirname,histname,5000,0,5000,mtof.fE1Up.at(i)-mtof.fObj.at(i));
   //  }
   //}

// Time corrections
//  histname = "time_x";
//  obj.FillHistogram(dirname,histname,2000,0,0,s800->GetCorrTOF_OBJ_MESY(),
//                                     2000,0,0,s800->GetCrdc(0).GetDispersiveX());
//
//  histname = "time_afp";
//  obj.FillHistogram(dirname,histname,2000,0,0,s800->GetCorrTOF_OBJ_MESY(),
//                                     2000,0,0,s800->GetAFP());
//
//  histname = "time_charge";
//  obj.FillHistogram(dirname,histname,2000,0,0,s800->GetCorrTOF_OBJ_MESY(),
//                                     2000,0,0,s800->GetIonChamber().Charge());

  return true;

}

bool DTA(TRuntimeObjects &obj, GCutG *incoming, GCutG *outgoing){

  TS800       *s800 = obj.GetDetector<TS800>();
  
  if(!s800)
    return false;
  
  if(!incoming || !outgoing)
    return false;

  if(!incoming->IsInside(s800->GetOBJ_E1Raw_MESY(),
			 s800->GetXF_E1Raw_MESY()) )
     return false;

  //  if(!outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
  //			 s800->GetIonChamber().GetSum()) )
  if( !outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
			  s800->GetIonChamber().Charge()) )
    return false;

  std::string dirname = "S800";
  std::string histname = Form("dta_%s_%s",
			      incoming->GetName(),
			      outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    200, -0.1, 0.1,
		    s800->GetDta());
  
  return true;
}

bool TriggerRegister(TRuntimeObjects &obj, GCutG *incoming, GCutG *outgoing){

  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800       *s800 = obj.GetDetector<TS800>();

  if(!s800)
    return false;
  
  std::string dirname = "S800";
  std::string histname;

  histname = "trigger_bit";
  unsigned short bits = s800->GetTrigger().GetRegistr();
  for(int j=0;j<16;j++) {
    if(((bits>>j)&0x0001))
      obj.FillHistogram(dirname, histname, 20, 0, 20, j);
  }
  histname = "trigger_raw";
  obj.FillHistogram(dirname, histname,
		    20, 0, 20,
		    s800->GetTrigger().GetRegistr());

  if(!incoming || !outgoing)
    return false;

  if(!incoming->IsInside(s800->GetOBJ_E1Raw_MESY(),
			 s800->GetXF_E1Raw_MESY()) )
     return false;

  //  if(!outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
  //			 s800->GetIonChamber().GetSum()) )
  if( !outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
			  s800->GetIonChamber().Charge()) )
    return false;

  histname = Form("trigger_bit_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  for(int j=0;j<16;j++) {
    if(((bits>>j)&0x0001))
      obj.FillHistogram(dirname, histname, 20, 0, 20, j);
  }
  histname = Form("trigger_raw_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    20, 0, 20,
		    s800->GetTrigger().GetRegistr());
  return true;
  
}

extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  //std::cout << "---------------------------------" <<std::endl;
  //std::cout << " At the beginning" << std::endl;
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank29  *bank29  = obj.GetDetector<TBank29>();
  TS800 *s800       = obj.GetDetector<TS800>();
  //std::cout << " Dets Gotten" << std::endl;
  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  TList *gates = &(obj.GetGates());
  if(!incoming_s44) {
    incoming_s44 = (GCutG*)gates->FindObject("in44S");
  }
  
  if(!outgoing_s44) {
    outgoing_s44 = (GCutG*)gates->FindObject("out44S");
  }

  if(!gt_time) {
    gt_time = (GCutG*)gates->FindObject("gt_time");
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
    }
  }

  if(s800) {

    dirname = "S800";

    TriggerRegister(obj, incoming_s44, outgoing_s44);

    IncomingS800(obj, s800, 0);
    IncomingS800(obj, s800, incoming_s44);

    OutgoingS800(obj, s800, 0);
    OutgoingS800(obj, s800, outgoing_s44);

    DTA(obj, incoming_s44, outgoing_s44);

  }

  if(numobj!=list->GetSize())
    list->Sort();

}

