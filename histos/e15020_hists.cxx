
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
  
  if(!s800 || s800->GetMTof().E1UpSize() != 1) {return false;}

  int E1UpSize = s800->GetMTof().E1UpSize();
  int XfpSize = s800->GetMTof().XfpSize();
  int ObjSize = s800->GetMTof().ObjSize();
  std::string dirname = "S800";

  obj.FillHistogram("Timing Scints","E1UpSize",10,0,10,E1UpSize);

  obj.FillHistogram("Timing Scints","XfpSize",10,0,10,XfpSize);

  obj.FillHistogram("Timing Scints","ObjSize",10,0,10,ObjSize);
  
  for(int i=0; i < E1UpSize; i++) {
    obj.FillHistogram("Timing Scints","E1",8000,0,25000,s800->GetMTof().fE1Up.at(i));

    obj.FillHistogram("Timing Scints","E1_v_Register",6000,0,20000,s800->GetMTof().fE1Up.at(i)
                                             ,10,0,10,s800->GetReg());

    obj.FillHistogram("Timing Scints","E1_v_Xfp-E1",2000,-30000,30000,s800->GetMTOF_XfpE1()
		                                   ,2000,0,70000,s800->GetMTof().fE1Up.at(i));

    obj.FillHistogram("Timing Scints","E1_v_Obj-E1",2000,-30000,30000,s800->GetMTOF_ObjE1()
		                                   ,2000,0,70000,s800->GetMTof().fE1Up.at(i));
   }

  for(int i=0; i < ObjSize; i++) {
   obj.FillHistogram("Timing Scints","Obj", 6000,0,20000,s800->GetMTof().fObj.at(i));

   obj.FillHistogram("Timing Scints","Obj_v_Register",6000,0,20000,s800->GetMTof().fObj.at(i)
                                             ,10,0,10,s800->GetReg());
  }

  for(int i=0; i < XfpSize; i++) {
   obj.FillHistogram("Timing Scints","Xfp", 6000,0,20000,s800->GetMTof().fXfp.at(i));

   obj.FillHistogram("Timing Scints","Xfp_v_Register",6000,0,20000,s800->GetMTof().fXfp.at(i)
                                             ,10,0,10,s800->GetReg());
  }

  obj.FillHistogram("Timing Scints","Xfp-E1",6000,14000,20000,s800->GetMTOF_XfpE1());
  
  for(int k=0; k < E1UpSize; k++) {
   for(int i=0; i < XfpSize; i++) {

     obj.FillHistogram("Timing Scints","Xfp-E1_raw",4000,2000,6000,s800->GetMTof().fXfp.at(i)-s800->GetMTof().fE1Up.at(k));

     obj.FillHistogram("Timing Scints","E1_v_Xfp_raw",2000,0,70000,s800->GetMTof().fXfp.at(i)
		                                     ,2000,0,70000,s800->GetMTof().fE1Up.at(k));

     obj.FillHistogram("Timing Scints","E1_v_Xfp-E1_raw",4000,-70000,70000,s800->GetMTof().fXfp.at(i)-s800->GetMTof().fE1Up.at(k)
		                                        ,2000,0,70000,s800->GetMTof().fE1Up.at(k));
   } 
  } 

  obj.FillHistogram("Timing Scints","Obj-E1",4000,-2000,0,s800->GetMTOF_ObjE1());

  for(int k=0; k < E1UpSize; k++) {
   for(int i=0; i < ObjSize; i++) {

     obj.FillHistogram("Timing Scints","Obj-E1_raw",4000,-2000,0,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k));

     obj.FillHistogram("Timing Scints","E1_v_Obj_raw",2000,0,70000,s800->GetMTof().fObj.at(i)
		                                     ,2000,0,70000,s800->GetMTof().fE1Up.at(k));

     obj.FillHistogram("Timing Scints","E1_v_Obj-E1_raw",4000,-70000,70000,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k)
		                                        ,2000,0,70000,s800->GetMTof().fE1Up.at(k));
   } 
  }
 
  return true;
}

bool HandleS800_Gated(TRuntimeObjects &obj,TCutG *incoming, TCutG* outgoing) {
  TS800 *s800  = obj.GetDetector<TS800>();  

  if(!s800 || s800->GetMTof().E1UpSize() != 1) {return false;}
  int E1UpSize = s800->GetMTof().E1UpSize();
  int XfpSize = s800->GetMTof().XfpSize();
  int ObjSize = s800->GetMTof().ObjSize();
  std::vector<bool> FillFlag (XfpSize + ObjSize,false);
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

  
  //INCOMING GATES on Obj-E1 vs Xfp-E1 raw data
  if(incoming) {
    dirname=Form("S800_%s",incoming->GetName());
    bool passed = false;
    for(int k=0; k < E1UpSize; k++) {
      for(int j=0; j < XfpSize; j++) {
        for(int i=0; i < ObjSize; i++) {
          if(incoming->IsInside(s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k),
			        s800->GetMTof().fXfp.at(j)-s800->GetMTof().fE1Up.at(k))) {
            passed=true;
	    FillFlag.at(j) = true;
	    FillFlag.at(XfpSize + i) = true;
            //break;
          }
        }
      }
    }
    if(!passed)
      return false;
  }
  
                                  
  //PID Plots
  for(int j=0; j < XfpSize; j++) {
   for(int i=0; i < ObjSize; i++) {
     if(FillFlag.at(j) && FillFlag.at(XfpSize + i)) { 
     obj.FillHistogram(dirname,"Xfp_v_Obj_raw",4000,11000,15000,s800->GetMTof().fObj.at(i)
                                              ,3000,16000,19000,s800->GetMTof().fXfp.at(j));
    }  
   }
  }

  obj.FillHistogram(dirname,"Xfp-E1_v_Obj-E1",3000,-10000,0,s800->GetMTOF_ObjE1()
		                             ,3000,0,20000,s800->GetMTOF_XfpE1());

  obj.FillHistogram(dirname,"Xfp-E1_v_Obj-E1_zoom",1400,-1500,-600,s800->GetMTOF_ObjE1()
		                             ,3000,16000,19000,s800->GetMTOF_XfpE1());

  for(int k=0; k < E1UpSize; k++) {
   for(int j=0; j < XfpSize; j++) {
    for(int i=0; i < ObjSize; i++) {
     if((FillFlag.at(j) && FillFlag.at(XfpSize + i)) {
      obj.FillHistogram(dirname,"Xfp-E1_v_Obj-E1_raw",500,-1300,-800,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k)
                                                     ,600,3400,4000,s800->GetMTof().fXfp.at(j)-s800->GetMTof().fE1Up.at(k));
     }
    } 
   }
  }

  obj.FillHistogram(dirname,"Ion_v_Obj-E1",1400,-1500,-800,s800->GetMTOF_ObjE1()
                                          ,4000,20000,35000,s800->GetIonChamber().Charge());

  for(int k=0; k < E1UpSize; k++) {
   for(int i=0; i < ObjSize; i++) {
    if((FillFlag.at(i) && FillFlag.at(XfpSize + i)) {
     obj.FillHistogram(dirname,"Ion_v_Obj-E1_raw",700,-1500,-800,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k)
		                                 ,2000,20000,35000,s800->GetIonChamber().Charge());

   } 
  }

  
  //CRDC Positions
  obj.FillHistogram(dirname, "CRDC1X", 1200, -300, 300, s800->GetCrdc(0).GetDispersiveX());

  obj.FillHistogram(dirname, "CRDC1Y", 800, -200, 200, s800->GetCrdc(0).GetNonDispersiveY());

  obj.FillHistogram(dirname, "CRDC2X", 1200, -300, 300, s800->GetCrdc(1).GetDispersiveX());

  obj.FillHistogram(dirname, "CRDC2Y", 800, -200, 200, s800->GetCrdc(1).GetNonDispersiveY());

  obj.FillHistogram(dirname, "CRDC1Y - CRDC2Y", 800, -200, 200,
		    s800->GetCrdc(0).GetNonDispersiveY() - s800->GetCrdc(1).GetNonDispersiveY()); 

  obj.FillHistogram(dirname,"CRDC1Y_v_CRDC1X",1200,-300,300,s800->GetCrdc(0).GetDispersiveX()
                                             ,800,-200,200,s800->GetCrdc(0).GetNonDispersiveY());

  //obj.FillHistogram(dirname,"CRDC1X_v_CRDC1Y_Uncal",600,0,300,s800->GetCrdc(0).GetDispersiveX()
  //                                           ,2000,0,2000,s800->GetCrdc(0).GetNonDispersiveY());
   
  obj.FillHistogram(dirname,"CRDC2Y_v_CRDC2X",1200,-300,300,s800->GetCrdc(1).GetDispersiveX()
                                             ,800,-200,200,s800->GetCrdc(1).GetNonDispersiveY());

  //obj.FillHistogram(dirname,"CRDC2X_v_CRDC2Y_Uncal",600,0,300,s800->GetCrdc(1).GetDispersiveX()
  //                                           ,2000,0,2000,s800->GetCrdc(1).GetNonDispersiveY());

  
  //ATA, AFP, etc
  obj.FillHistogram(dirname,"AFP",1000,-0.1,0.1,s800->GetAFP());

  obj.FillHistogram(dirname,"BFP",1000,-0.15,0.15,s800->GetBFP());

  obj.FillHistogram(dirname,"ATA",1000,-0.1,0.1,s800->GetAta());

  obj.FillHistogram(dirname,"YTA",1000,-50,50,s800->GetYta());

  obj.FillHistogram(dirname,"BTA",1000,-0.1,0.1,s800->GetBta());

  obj.FillHistogram(dirname,"DTA",1000,-0.5,0.5,s800->GetDta());

  
  //Other
  obj.FillHistogram(dirname,"XFP_v_Obj-E1",1200,-1400,-800,s800->GetMTOF_ObjE1()
                                          ,1200,-300,300,s800->GetXFP());

  for(int k=0; k < E1UpSize; k++) {
   for(int i=0; i < ObjSize; i++) {
     obj.FillHistogram(dirname,"XFP_v_Obj-E1_raw",600,-1400,-800,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k)
		                                 ,1200,-300,300,s800->GetXFP());
   } 
  }

  obj.FillHistogram(dirname,"AFP_v_Obj-E1",400,-1300,-900,s800->GetMTOF_ObjE1()
                                          ,1000,-0.05,0.05,s800->GetAFP());

  for(int k=0; k < E1UpSize; k++) {
   for(int i=0; i < ObjSize; i++) {
     obj.FillHistogram(dirname,"AFP_v_Obj-E1_raw",500,-1300,-800,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k)
		                                 ,1000,-0.05,0.05,s800->GetAFP());
   } 
  }

  obj.FillHistogram(dirname,"YFP_v_BFP",1000,-0.1,0.1,s800->GetBFP()
                                       ,800,-200,200,s800->GetYFP());
  
  //OUTGOING GATES on Ion Chamber Charge vs Object - E1
  if(!outgoing) {return false;}
  if(!outgoing->IsInside(s800->GetMTOF_ObjE1(),s800->GetIonChamber().Charge())) {return false;} 
    
  //PID Plots
  for(int j=0; j < XfpSize; j++) {
   for(int i=0; i < ObjSize; i++) {
    obj.FillHistogram(dirname,Form("Xfp_v_Obj_raw_%s",outgoing->GetName()),4000,11000,15000,s800->GetMTof().fObj.at(i)
                                                                          ,3000,16000,19000,s800->GetMTof().fXfp.at(j));
   }
  }
  
  obj.FillHistogram(dirname, Form("Xfp-E1_v_Obj-E1_%s",outgoing->GetName()),1000,-1500,-600,s800->GetMTOF_ObjE1()
                                                                           ,3000,16000,19000,s800->GetMTOF_XfpE1());

  for(int k=0; k < E1UpSize; k++) { 
   for(int j=0; j < XfpSize; j++) {
    for(int i=0; i < ObjSize; i++) {
      obj.FillHistogram(dirname,Form("Xfp-E1_v_Obj-E1_raw_%s",outgoing->GetName())
			             ,500,-1300,-800,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k)
			             ,600,3400,4000,s800->GetMTof().fXfp.at(j)-s800->GetMTof().fE1Up.at(k));
    } 
   }
  }

 obj.FillHistogram(dirname,Form("Ion_v_Obj-E1_%s",outgoing->GetName()),1000,-1500,-800,s800->GetMTOF_ObjE1()
                                                                      ,4000,20000,35000,s800->GetIonChamber().Charge());


 //CRDCs
 obj.FillHistogram(dirname,Form("CRDC1X_%s",outgoing->GetName()), 1200, -300, 300, s800->GetCrdc(0).GetDispersiveX());

 obj.FillHistogram(dirname,Form("CRDC1Y_%s",outgoing->GetName()), 800, -200, 200, s800->GetCrdc(0).GetNonDispersiveY());

 obj.FillHistogram(dirname,Form("CRDC1Y_v_CRDC1X_%s",outgoing->GetName()),1200,-300,300,s800->GetCrdc(0).GetDispersiveX()
                                                                         ,800,-200,200,s800->GetCrdc(0).GetNonDispersiveY());

 //ATA, AFP, etc
 obj.FillHistogram(dirname,Form("AFP_%s",outgoing->GetName()),1000,-0.1,0.1,s800->GetAFP());

 obj.FillHistogram(dirname,Form("BFP_%s",outgoing->GetName()),1000,-0.15,0.15,s800->GetBFP());

 obj.FillHistogram(dirname,Form("ATA_%s",outgoing->GetName()),1000,-0.1,0.1,s800->GetAta());

 obj.FillHistogram(dirname,Form("YTA_%s",outgoing->GetName()),1000,-50,50,s800->GetYta());

 obj.FillHistogram(dirname,Form("BTA_%s",outgoing->GetName()),1000,-0.1,0.1,s800->GetBta());

 obj.FillHistogram(dirname,Form("DTA_%s",outgoing->GetName()),1000,-0.5,0.5,s800->GetDta());

 //Other
 obj.FillHistogram(dirname,Form("XFP_v_Obj-E1_%s",outgoing->GetName()),1200,-1400,-800,s800->GetMTOF_ObjE1()
                                                                      ,1200,-300,300,s800->GetXFP());

 obj.FillHistogram(dirname,Form("AFP_v_Obj-E1_%s",outgoing->GetName()),400,-1300,-900,s800->GetMTOF_ObjE1()
                                                                      ,1000,-0.05,0.05,s800->GetAFP());

 obj.FillHistogram(dirname,Form("YFP_v_BFP_%s",outgoing->GetName()),1000,-0.1,0.1,s800->GetBFP()
                                                                   ,800,-200,200,s800->GetYFP());

  return true;
  
}

bool HandleGretina(TRuntimeObjects &obj) {
  TGretina *gretina  = obj.GetDetector<TGretina>(); 
   
  if(!gretina) {return false;}
  std::string dirname = "Gretina";
  for(unsigned int i=0;i<gretina->Size();i++) {
      TGretinaHit hit = gretina->GetGretinaHit(i);

      obj.FillHistogram(dirname,"CoreEnergy",2000,0,4000,hit.GetCoreEnergy());

      obj.FillHistogram(dirname,"Summary_Spectrum",200,0,200,hit.GetCrystalId()
			                          ,2000,0,4000,hit.GetCoreEnergy());
                                                 

      obj.FillHistogram(dirname,"Position_Spectrum",226,0,3.2,hit.GetTheta()
                                                   ,452,0,6.3,hit.GetPhi());
      
      //plots with s800
      //TS800 *s800 = obj.GetDetector<TS800>();
      //if(!s800) {return false;}
      
      
    }


  return true;

}

bool HandleGretina_Gated(TRuntimeObjects &obj,TCutG *outgoing, TCutG* incoming) {
  TGretina *gretina  = obj.GetDetector<TGretina>(); 
  TS800 *s800 = obj.GetDetector<TS800>(); 
  if(!gretina || !s800 || s800->GetMTof().E1UpSize() != 1) {return false;}

  int E1UpSize = s800->GetMTof().E1UpSize();
  int XfpSize = s800->GetMTof().XfpSize();
  int ObjSize = s800->GetMTof().ObjSize();
  std::string dirname = "Gretina";
  
  if(incoming) {
     dirname=Form("Gretina_%s",incoming->GetName());
     bool passed = false;
     for(int k=0; k < E1UpSize; k++) {
       for(int j=0; j < XfpSize; j++) {
         for(int i=0; i < ObjSize; i++) {
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

  for(unsigned int i=0;i<gretina->Size();i++) {
    TGretinaHit hit = gretina->GetGretinaHit(i);
  
    //Plots with S800
    TVector3 track = s800->Track(); 
    obj.FillHistogram(dirname,"GammeEnergyTrack_v_DTA",1000,-0.5,0.5,s800->GetDta()
			                                ,2000,0,4000,hit.GetDoppler(0.39,&track));
  }
    
  //Outgoing Gates
  if(!outgoing) {return false;}
  
  if(!outgoing->IsInside(s800->GetMTOF_ObjE1(),s800->GetIonChamber().Charge())) {return false;}

  for(unsigned int i=0;i<gretina->Size();i++) {
    TGretinaHit hit = gretina->GetGretinaHit(i);

    obj.FillHistogram(dirname,Form("CoreEnergy_%s",outgoing->GetName()),2000,0,4000,hit.GetCoreEnergy());

    obj.FillHistogram(dirname,Form("GammaEnergy_%s",outgoing->GetName()),2000,0,4000,hit.GetDoppler(0.39));

    TVector3 track = s800->Track();
    obj.FillHistogram(dirname,Form("GammaEnergyTrack_%s",outgoing->GetName()),2000,0,4000,hit.GetDoppler(0.39,&track));

    obj.FillHistogram(dirname,Form("GammaEnergyTrack_v_DTA_%s",outgoing->GetName()),1000,-0.5,0.5,s800->GetDta()
                                                                                   ,2000,0,4000,hit.GetDoppler(0.39,&track)); 
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




