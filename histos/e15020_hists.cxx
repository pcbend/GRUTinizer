
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


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.

std::vector<GCutG*> incoming_cuts = {0};
std::vector<GCutG*> outgoing_cuts = {0};

int gates_loaded=0;

bool HandleS800(TRuntimeObjects &obj) {
  TS800 *s800  = obj.GetDetector<TS800>();  
  if(!s800) return false;
  std::string dirname = "S800";

  //obj.FillHistogram(dirname,"XFP (CRDC1X)", 600,-300,300,s800->GetXFP());

  //obj.FillHistogram(dirname,"YFP", 550,-50,500,s800->GetYFP());
  
  for(int i=0; i < s800->GetMTof().E1UpSize(); i++) {
    obj.FillHistogram(dirname,"E1",6400,0,64000,s800->GetMTof().fE1Up.at(i));

    obj.FillHistogram(dirname,"E1_v_Register",6000,0,20000,s800->GetMTof().fE1Up.at(i)
                                             ,10,0,10,s800->GetReg());
   }

  for(int i=0; i < s800->GetMTof().ObjSize(); i++) {
   obj.FillHistogram(dirname,"Obj", 6000,0,20000,s800->GetMTof().fObj.at(i));

   obj.FillHistogram(dirname,"Obj_v_Register",6000,0,20000,s800->GetMTof().fObj.at(i)
                                             ,10,0,10,s800->GetReg());
  }
 
  return true;
}

bool HandleS800_Gated(TRuntimeObjects &obj,TCutG *incoming, TCutG* outgoing) {
  TS800 *s800  = obj.GetDetector<TS800>();  
  if(!s800) return false;

  std::string dirname = "S800";

  /*
  //incoming gates on 0bj-E1 vs Xfp-E1
  if(incoming) {
    bool passed = false;
    for(int j=0; j < s800->GetMTof().XfpSize(); j++) {
      for(int i=0; i < s800->GetMTof().ObjSize(); i++) {
        if(incoming->IsInside(s800->GetMTof().fObj.at(i),s800->GetMTof().fXfp.at(j))) {
          dirname=Form("S800_%s",incoming->GetName());
          passed=true;
          break;
        }
      }
    }
    if(!passed)
      return false;
  }*/

    //incoming gates on Obj-E1 vs Xfp-E1 raw data
  if(incoming) {
    dirname=Form("S800_%s",incoming->GetName());
    bool passed = false;
    for(int k=0; k < s800->GetMTof().E1UpSize(); k++) {
      for(int j=0; j < s800->GetMTof().XfpSize(); j++) {
        for(int i=0; i < s800->GetMTof().ObjSize(); i++) {
          if(incoming->IsInside(s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k),
			      s800->GetMTof().fXfp.at(j)-s800->GetMTof().fE1Up.at(k))) {
            passed=true;
            break;
          }
        }
      }
    }
    if(!passed)
      return false;
  }  

  obj.FillHistogram(dirname,"Xfp-E1",4000,0,8000,s800->GetMTOF_XfpE1());
  
  for(int k=0; k < s800->GetMTof().E1UpSize(); k++) {
   for(int i=0; i < s800->GetMTof().XfpSize(); i++) {
     obj.FillHistogram(dirname,"Xfp-E1_raw",4000,0,8000,s800->GetMTof().fXfp.at(i)-s800->GetMTof().fE1Up.at(k));
   } 
  } 

  obj.FillHistogram(dirname,"Obj-E1",5000,-5000,0,s800->GetMTOF_ObjE1());

  for(int k=0; k < s800->GetMTof().E1UpSize(); k++) {
   for(int i=0; i < s800->GetMTof().ObjSize(); i++) {
     obj.FillHistogram(dirname,"Obj-E1_raw",4000,-4000,4000,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k));
   } 
  }
 
  obj.FillHistogram(dirname, "Obj-E1_v_Xfp-E1",320,-1260,-940,s800->GetMTOF_ObjE1()
                                              ,420,3480,3900,s800->GetMTOF_XfpE1());

  for(int k=0; k < s800->GetMTof().E1UpSize(); k++) {
   for(int j=0; j < s800->GetMTof().XfpSize(); j++) {
    for(int i=0; i < s800->GetMTof().ObjSize(); i++) {
     obj.FillHistogram(dirname,"Obj-E1_v_Xfp-E1_raw",600,-1400,-800,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k)
                                                    ,600,3400,4000,s800->GetMTof().fXfp.at(j)-s800->GetMTof().fE1Up.at(k));
    } 
   }
  }

  obj.FillHistogram(dirname,"Ion_v_Obj-E1",1000,-1500,-800,s800->GetMTOF_ObjE1()
                                          ,2000,20000,35000,s800->GetIonChamber().Charge());

  obj.FillHistogram(dirname,"XFP_v_Obj-E1",1000,-1120,-1020,s800->GetMTOF_ObjE1()
                                          ,1200,-300,300,s800->GetXFP());

  obj.FillHistogram(dirname,"AFP_v_Obj-E1",300,-1200,-900,s800->GetMTOF_ObjE1()
                                          ,1000,-0.05,0.05,s800->GetAFP());

  obj.FillHistogram(dirname,"YTA",4000,-1000,1000,s800->GetYta());

  obj.FillHistogram(dirname,"DTA",1000,-0.5,0.5,s800->GetDta());

  for(int j=0; j < s800->GetMTof().XfpSize(); j++) {
   for(int i=0; i < s800->GetMTof().ObjSize(); i++) {
    obj.FillHistogram(dirname,"Obj_v_Xfp",2000,0,30000,s800->GetMTof().fObj.at(i)
                                         ,2000,0,30000,s800->GetMTof().fXfp.at(j));
   }
  }

  

  obj.FillHistogram(dirname,"CRDC1X_v_CRDC1Y",1200,-300,300,s800->GetCrdc(0).GetDispersiveX()
                                             ,800,-200,200,s800->GetCrdc(0).GetNonDispersiveY());

  obj.FillHistogram(dirname,"CRDC1X_v_CRDC1Y_Uncal",600,0,300,s800->GetCrdc(0).GetDispersiveX()
                                             ,2000,0,2000,s800->GetCrdc(0).GetNonDispersiveY());
   
  obj.FillHistogram(dirname,"CRDC2X_v_CRDC2Y",1200,-300,300,s800->GetCrdc(1).GetDispersiveX()
                                             ,800,-200,200,s800->GetCrdc(1).GetNonDispersiveY());

  obj.FillHistogram(dirname,"CRDC2X_v_CRDC2Y_Uncal",600,0,300,s800->GetCrdc(1).GetDispersiveX()
                                             ,2000,0,2000,s800->GetCrdc(1).GetNonDispersiveY());

  
  obj.FillHistogram(dirname, "CRDC1X", 1200, -300, 300, s800->GetCrdc(0).GetDispersiveX());

  obj.FillHistogram(dirname, "CRDC1Y", 800, -200, 200, s800->GetCrdc(0).GetNonDispersiveY());

  obj.FillHistogram(dirname, "CRDC2X", 1200, -300, 300, s800->GetCrdc(1).GetDispersiveX());

  obj.FillHistogram(dirname, "CRDC2Y", 800, -200, 200, s800->GetCrdc(1).GetNonDispersiveY());

  obj.FillHistogram(dirname, "CRDC1Y - CRDC2Y", 800, -200, 200, s800->GetCrdc(0).GetNonDispersiveY() - s800->GetCrdc(1).GetNonDispersiveY());
  
  //outgoing gates
  if(!outgoing) {return false;}
  if(!outgoing->IsInside(s800->GetMTOF_ObjE1(),s800->GetIonChamber().Charge()))
    {return false;}

  obj.FillHistogram(dirname,Form("DTA_%s",outgoing->GetName()),1000,-0.5,0.5,s800->GetDta());
  
  obj.FillHistogram(dirname, Form("Obj-E1_v_Xfp-E1_%s",outgoing->GetName()),320,-1260,-940,s800->GetMTOF_ObjE1()
                                                                           ,420,3480,3900,s800->GetMTOF_XfpE1());

  for(int j=0; j < s800->GetMTof().XfpSize(); j++) {
   for(int i=0; i < s800->GetMTof().ObjSize(); i++) {
    obj.FillHistogram(dirname,Form("Obj_v_Xfp_%s",outgoing->GetName()),1050,11800,13900,s800->GetMTof().fObj.at(i)
                                                                      ,1075,16600,18750,s800->GetMTof().fXfp.at(j));
   }
  }

  obj.FillHistogram(dirname,Form("Ion_v_Obj-E1_%s",outgoing->GetName()),1000,-1500,-800,s800->GetMTOF_ObjE1()
                                          ,2000,20000,35000,s800->GetIonChamber().Charge());

   for(int k=0; k < s800->GetMTof().E1UpSize(); k++) { 
   for(int j=0; j < s800->GetMTof().XfpSize(); j++) {
    for(int i=0; i < s800->GetMTof().ObjSize(); i++) {
      obj.FillHistogram(dirname,Form("Obj-E1_v_Xfp-E1_raw_%s",outgoing->GetName()),320,-1260,-940,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k)
                                                                                  ,420,3480,3900,s800->GetMTof().fXfp.at(j)-s800->GetMTof().fE1Up.at(k));
    } 
   }
  }

 obj.FillHistogram(dirname,Form("CRDC1X_%s",outgoing->GetName()), 1200, -300, 300, s800->GetCrdc(0).GetDispersiveX());

 obj.FillHistogram(dirname, Form("CRDC1Y_%s",outgoing->GetName()), 800, -200, 200, s800->GetCrdc(0).GetNonDispersiveY());

 obj.FillHistogram(dirname,Form("CRDC1X_v_CRDC1Y_%s",outgoing->GetName()),1200,-300,300,s800->GetCrdc(0).GetDispersiveX()
                                                                         ,800,-200,200,s800->GetCrdc(0).GetNonDispersiveY());

  //obj.FillHistogram(dirname,Form("XFP_v_YFP_uncal_%s",outgoing->GetName()),600,0,300,s800->GetXFP()
  //		                                     ,2000,0,1000,s800->GetYFP());


  return true;
  
}

bool HandleGretina(TRuntimeObjects &obj) {
  TGretina *gretina  = obj.GetDetector<TGretina>(); 
  TS800 *s800 = obj.GetDetector<TS800>(); 
  if(!gretina) return false;
  std::string dirname = "Gretina";

  for(unsigned int i=0;i<gretina->Size();i++) {
      TGretinaHit hit = gretina->GetGretinaHit(i);

      obj.FillHistogram(dirname,"CoreEnergy",2000,0,4000,hit.GetCoreEnergy());

      obj.FillHistogram(dirname,"Summary_Spectrum",200,0,200,hit.GetCrystalId()
			                          ,2000,0,4000,hit.GetCoreEnergy());
                                                 

      obj.FillHistogram(dirname,"Position_Spectrum",226,0,3.2,hit.GetTheta()
                                                   ,452,0,6.3,hit.GetPhi());
      
      //plots with s800
      if(!s800) return false;
      TVector3 vec = s800->Track(); 
      obj.FillHistogram(dirname,"DTA_v_GammaEnergyTrack",1000,-0.5,0.5,s800->GetDta()
			                                ,2000,0,4000,hit.GetDoppler(0.39,&vec));
    }

  return true;

}

bool HandleGretina_Gated(TRuntimeObjects &obj,TCutG *outgoing, TCutG* incoming) {
  TGretina *gretina  = obj.GetDetector<TGretina>(); 
  TS800 *s800 = obj.GetDetector<TS800>(); 
  if(!gretina || !s800) {return false;}
  std::string dirname = "Gretina";

  if(incoming) {
    dirname=Form("Gretina_%s",incoming->GetName());
    bool passed = false;
    for(int j=0; j < s800->GetMTof().XfpSize(); j++) {
      for(int i=0; i < s800->GetMTof().ObjSize(); i++) {
        if(incoming->IsInside(s800->GetMTof().fObj.at(i),s800->GetMTof().fXfp.at(j))) {       
          passed=true;
          break;
        }
      }
    }
  
    if(!passed) {return false;}
  
  } 
  
  if(!outgoing) {return false;}
  
  if(!outgoing->IsInside(s800->GetMTOF_ObjE1(),s800->GetIonChamber().Charge())) {
    return false;
  }

  for(unsigned int i=0;i<gretina->Size();i++) {
    TGretinaHit hit = gretina->GetGretinaHit(i);

    obj.FillHistogram(dirname,Form("GretinaEnergy_%s",outgoing->GetName()),2000,0,4000,hit.GetCoreEnergy());

    obj.FillHistogram(dirname,Form("GammaEnergy_%s",outgoing->GetName()),2000,0,4000,hit.GetDoppler(0.39));

    TVector3 vec = s800->Track();
    obj.FillHistogram(dirname,Form("GammaEnergyTrack_%s",outgoing->GetName()),2000,0,4000,hit.GetDoppler(0.39,&vec));

    obj.FillHistogram(dirname,Form("DTA_v_GammaEnergyTrack_%s",outgoing->GetName()),1000,-0.5,0.5,s800->GetDta()
                                                                                   ,2000,0,4000,hit.GetDoppler(0.39,&vec)); 
  }
  
  return true;

}

extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  //TGretina *gretina = obj.GetDetector<TGretina>();
  //TBank29  *bank29  = obj.GetDetector<TBank29>();
  //TS800    *s800    = obj.GetDetector<TS800>();

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
      }
      gates_loaded++;
    }
  }
   
  
  HandleGretina(obj);

  HandleS800(obj);

  for(int i=0;i<incoming_cuts.size();i++) {
   for(int j=0;j<outgoing_cuts.size();j++) {
     HandleS800_Gated(obj,incoming_cuts.at(i),outgoing_cuts.at(j));
    HandleGretina_Gated(obj,outgoing_cuts.at(j),incoming_cuts.at(i));
   }
  }

  /*
  for(int i=0;i<incoming_cuts.size();i++) {
   for(int j=0;j<outgoing_cuts.size();j++) {
     HandleGretina_Gated(obj,outgoing_cuts.at(j),incoming_cuts.at(i));
   }
  }
  */


//      obj.FillHistogram(dirname,histname,
//          600,-600,600,bank29->Timestamp()-hit.GetTime(),
//          2000,0,4000,hit.GetCoreEnergy());


  

//  TList *gates = &(obj.GetGates());
//  if(gretina) {
//    for(unsigned int i=0;i<gretina->Size();i++) {
//      TGretinaHit hit = gretina->GetGretinaHit(i);
//      histname = "Gretina_Bank29_time";
//      obj.FillHistogram(dirname,histname,
//          600,-600,600,bank29->Timestamp()-hit.Timestamp(),
//          2000,0,4000,hit.GetCoreEnergy());
//      histname = "Gretina_t0_Bank29_time";
//      obj.FillHistogram(dirname,histname,
//          600,-600,600,bank29->Timestamp()-hit.GetTime(),
//          2000,0,4000,hit.GetCoreEnergy());
//    }
//  }

  if(numobj!=list->GetSize())
    list->Sort();
}




