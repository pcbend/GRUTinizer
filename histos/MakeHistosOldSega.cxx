#include "TRuntimeObjects.h"

#include "TObject.h"
#include "TS800.h"
#include "TOldSega.h"

#include "GCutG.h"


std::vector<GCutG*> incoming_cuts   = {0};
std::vector<GCutG*> outgoing_cuts   = {0};
std::vector<GCutG*> timeenergy_cuts; // = {0};
int gates_loaded=0;

bool OutgoingBeam(TRuntimeObjects& obj,GCutG *incoming) {
  TS800    *s800    = obj.GetDetector<TS800>();
  if(!s800)
    return false;

  std::string histname;
  std::string dirname;
  if(incoming)
    dirname = Form("outgoing_%s",incoming->GetName());
  else
    dirname = "outgoing";


  double objtime = s800->GetTof().GetOBJ();
  double xfptime = s800->GetTof().GetXFP();
  if(incoming) {
    if(!incoming->IsInside(objtime,xfptime))
      return false;
  }

  double obj_corr   = s800->GetCorrTOF_OBJ(); 
  double ic_sum     = s800->GetIonChamber().GetAve();
  double afp        = s800->GetAFP();
  double xfp        = s800->GetXFP(0);

  histname = "AFP_vs_OBJTOF";
  obj.FillHistogram(dirname,histname,
  		    1000,-1000,1000,obj_corr,
        	    1000,-0.5,0.5,afp); // check units of AFP
 
  histname = "XFP_vs_OBJTOF";
  obj.FillHistogram(dirname,histname,
        	    1000,-1000,1000,obj_corr,
        	    350,-50,300,xfp);
 
  histname = "IC_vs_OBJTOF_PID";
  obj.FillHistogram(dirname,histname,
        	    1000,-1000,1000,obj_corr,
        	    1000,0,2000,ic_sum);

  obj.FillHistogram(dirname,"CRDC1Y",5000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
  obj.FillHistogram(dirname,"CRDC2Y",5000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
  obj.FillHistogram(dirname,"CRDC1X",400,-400,400,s800->GetCrdc(0).GetDispersiveX());
  obj.FillHistogram(dirname,"CRDC2X",400,-400,400,s800->GetCrdc(1).GetDispersiveX());
  obj.FillHistogram(dirname,"TrigBit",30,0,30,s800->GetTrigger().GetRegistr());
  obj.FillHistogram(dirname,"S800_YTA",1000,-50,50,s800->GetYta());
  obj.FillHistogram(dirname,"S800_DTA",1000,-0.2,0.2,s800->GetDta());
  obj.FillHistogram(dirname,"ATA_vs_BTA",1000,-0.2,0.2,s800->GetAta(),
			                 1000,-0.2,0.2,s800->GetBta());

  return true;
}

bool IncomingBeam(TRuntimeObjects& obj,GCutG *outgoing) {
  TS800    *s800    = obj.GetDetector<TS800>();
  if(!s800)
    return false;

  std::string histname;
  std::string dirname;
  if(outgoing)
    dirname = Form("incoming_%s",outgoing->GetName());
  else
    dirname = "incoming";
  
  double ic_sum      = s800->GetIonChamber().GetAve();
  double obj_corr    = s800->GetCorrTOF_OBJ();
  if(outgoing) {
    if(!outgoing->IsInside(obj_corr,ic_sum))
      return false;
  }

  double objtime = s800->GetTof().GetOBJ();
  double xfptime = s800->GetTof().GetXFP();
  histname = "IncomingPID";
  obj.FillHistogram(dirname,histname,
                     1000,-1000,5000,objtime,
                     1000,-1000,5000,xfptime);
  return true;
}


int HandleOldSega(TRuntimeObjects& obj,GCutG *incoming,GCutG *outgoing) {

  TS800     *s800 = obj.GetDetector<TS800>();
  TOldSega  *sega = obj.GetDetector<TOldSega>();
  if(!s800 || !sega)
    return false;

  for(unsigned int i=0;i<sega->Size();i++) {
    TOldSegaHit hit = sega->GetSegaHit(i);

    obj.FillHistogram("Gated","Charge",
                    4096,0,4096,hit.Charge(),
                    30,0,30,hit.GetDetId());
  } 

  return 0;
}

int HandleUngatedOldSega(TRuntimeObjects& obj) {

  //TS800     *s800 = obj.GetDetector<TS800>();
  TOldSega  *sega = obj.GetDetector<TOldSega>();
  //if(!s800 || !sega)
    //return false;

  std::string histname;

  for(unsigned int i=0;i<sega->Size();i++) {
    TOldSegaHit hit = sega->GetSegaHit(i);

    obj.FillHistogram("Ungated","Charge_Det",
                    4096,0,4096,hit.Charge(),
                    30,0,30,hit.GetDetId());
    for(int j=0; j<hit.Size(); j++) {
      histname = Form("SegCharge_Det%02i",hit.GetDetId());
      obj.FillHistogram("Ungated",histname,
                        4096,0,4096,hit.GetSegChg(j),
                        40,0,40,hit.GetSegId(j));
    } 
  }

  return 0;
}

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
 
  TList *list  = &(obj.GetObjects());
  TList *gates = &(obj.GetGates());
  int numobj = list->GetSize();

  HandleUngatedOldSega(obj); 

  if(gates_loaded!=gates->GetSize()) {
    TIter iter(gates);
    while(TObject *obj = iter.Next()) {
      GCutG *gate = (GCutG*)obj;
      std::string tag = gate->GetTag();
      if(!tag.compare("incoming")) {
        incoming_cuts.push_back(gate);
      } else if(!tag.compare("outgoing")) {
        outgoing_cuts.push_back(gate);
      } else if(!tag.compare("timeenergy")){
        timeenergy_cuts.push_back(gate);
      }
      gates_loaded++;
    }
  }

  //printf("incoming.size() == %i\n",incoming_cuts.size());
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
    bool passed = IncomingBeam(obj,outgoing_cuts.at(x)); 
    if(x!=0 && passed) {
      outgoing_passed = x;
      break;
    }
  } 


  if(incoming_passed>0 && outgoing_passed>0) {
    HandleOldSega(obj,incoming_cuts.at(incoming_passed),outgoing_cuts.at(outgoing_passed));
  }



  if(numobj!=list->GetSize())
    list->Sort();

}//end MakeHistograms
