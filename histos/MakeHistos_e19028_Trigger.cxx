
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
#include "TBank88.h"
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

GCutG *incoming_cut=0;

GCutG *outgoing_cut=0;

bool OutgoingS800(TRuntimeObjects &obj, TS800 *s800, GCutG *outgoing) {

   if(!s800)
     return false;
   
   std::string dirname = "S800";
   std::string histname;
   if(outgoing) {
     if( !outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
			     s800->GetIonChamber().Charge()) )
       return false;
     histname = Form("incoming_%s", outgoing->GetName());
   } else {
     histname = "incoming";
   }

   //LR (e19028 Run0022, 74Kr setting, emtpy cell)
   obj.FillHistogram(dirname, histname,
		     600, -1800, -1200,
		     s800->GetOBJ_E1Raw_MESY(),
		     800, 3000, 3800,
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
		     1000, -2000, -1000,
		     s800->GetCorrTOF_OBJ_MESY(),
		     2000, 30000, 50000,
		     s800->GetIonChamber().Charge());
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

  if( !outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
			  s800->GetIonChamber().Charge()) )
    return false;

  histname = "trigger_bit_PID";
  for(int j=0;j<16;j++) {
    if(((bits>>j)&0x0001))
      obj.FillHistogram(dirname, histname, 20, 0, 20, j);
  }
  histname = "trigger_raw_PID";
  obj.FillHistogram(dirname, histname,
		    20, 0, 20,
		    s800->GetTrigger().GetRegistr());
  return true;
  
}

extern "C"
void MakeHistograms(TRuntimeObjects& obj) {

  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800 *s800       = obj.GetDetector<TS800>();

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  TList *gates = &(obj.GetGates());

  TIter next(gates);
  GCutG* gate = 0;
  while ((gate = (GCutG*)next())) {
    TString cutname = gate->GetName();
    if(cutname.Contains("in") && !incoming_cut)
      incoming_cut = gate;

    if(cutname.Contains("out") && !outgoing_cut)
      outgoing_cut = gate;
  }

  std::string histname = "";
  std::string dirname  = "";

  if(s800) {

    TriggerRegister(obj, incoming_cut, outgoing_cut);

    dirname = "S800";

    IncomingS800(obj, s800, 0);
    IncomingS800(obj, s800, incoming_cut);

    OutgoingS800(obj, s800, 0);
    OutgoingS800(obj, s800, outgoing_cut);

    DTA(obj, incoming_cut, outgoing_cut);

  }

  if(numobj!=list->GetSize())
    list->Sort();

}

