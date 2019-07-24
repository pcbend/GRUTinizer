
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
#include "TFastScint.h"
#include "GCutG.h"

#include "TChannel.h"
#include "GValue.h"


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.

std::vector<GCutG*> incoming_cuts = {0};
std::vector<GCutG*> outgoing_cuts = {0};
GCutG* time_energy_cut = NULL;

int gates_loaded=0;

bool HandleTiming(TRuntimeObjects &obj) {
  TS800 *s800  = obj.GetDetector<TS800>();
  //TBank88 *bank29  = obj.GetDetector<TBank88>();
  
  if(!s800) //|| s800->GetMTof().E1UpSize() != 1)
    {return false;}

  int E1UpSize = s800->GetMTof().E1UpSize();
  int XfpSize = s800->GetMTof().XfpSize();
  int ObjSize = s800->GetMTof().ObjSize();
  std::string dirname = "Timing";

  obj.FillHistogram(dirname,"E1UpSize",10,0,10,E1UpSize);

  obj.FillHistogram(dirname,"XfpSize",10,0,10,XfpSize);

  obj.FillHistogram(dirname,"ObjSize",10,0,10,ObjSize);
  
  
  obj.FillHistogram(dirname,"E1",4000,0,64000,s800->GetMTof().fE1Up);
 
  obj.FillHistogram(dirname,"XfpE1",4000,-10000,10000,s800->GetMTof().GetCorrelatedXfpE1());
      
  obj.FillHistogram(dirname,"Obj", 4000,0,64000,s800->GetMTof().fObj);

  obj.FillHistogram(dirname,"ObjE1",4000,-10000,10000,s800->GetMTof().GetCorrelatedObjE1());

  obj.FillHistogram(dirname,"Xfp", 4000,0,64000,s800->GetMTof().fXfp);


  
  for(int k=0; k < E1UpSize; k++) {
   for(int i=0; i < XfpSize; i++) {

     obj.FillHistogram(dirname,"Xfp-E1_raw",4000,2000,6000,s800->GetMTof().fXfp.at(i)-s800->GetMTof().fE1Up.at(k));

     obj.FillHistogram(dirname,"E1_v_Xfp_raw",2000,0,70000,s800->GetMTof().fXfp.at(i)
		                             ,2000,0,70000,s800->GetMTof().fE1Up.at(k));

     obj.FillHistogram(dirname,"E1_v_Xfp-E1_raw",4000,-70000,70000,s800->GetMTof().fXfp.at(i)-s800->GetMTof().fE1Up.at(k)
		                                ,2000,0,70000,s800->GetMTof().fE1Up.at(k));
   } 
  } 

  for(int k=0; k < E1UpSize; k++) {
   for(int i=0; i < ObjSize; i++) {

     obj.FillHistogram(dirname,"Obj-E1_raw",4000,-2000,0,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k));

     obj.FillHistogram(dirname,"E1_v_Obj_raw",2000,0,70000,s800->GetMTof().fObj.at(i)
		                                    ,2000,0,70000,s800->GetMTof().fE1Up.at(k));

     obj.FillHistogram(dirname,"E1_v_Obj-E1_raw",4000,-70000,70000,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k)
		                                ,2000,0,70000,s800->GetMTof().fE1Up.at(k));
   } 
  }

  //if(!bank29)
  //{return false;}

  //obj.FillHistogram(dirname,"bank29TimeStamp",10000,0,3000000,bank29->Timestamp());
 
  return true;
}

bool HandleS800(TRuntimeObjects &obj) {
  TS800 *s800  = obj.GetDetector<TS800>();
  
  if(!s800 || s800->GetMTof().E1UpSize() != 1)
    {return false;}
  
  return true;
}

bool HandleS800_Gated(TRuntimeObjects &obj,TCutG *incoming, TCutG* outgoing) {
  TS800 *s800  = obj.GetDetector<TS800>();  

  if(!s800 || s800->GetMTof().E1UpSize() != 1) {return false;}
  int E1UpSize = s800->GetMTof().E1UpSize();
  int XfpSize = s800->GetMTof().XfpSize();
  int ObjSize = s800->GetMTof().ObjSize();
  std::string dirname = "S800";

  
  //incoming gates on Xfp vs. OBJ -E1 (CorrTarget)
  if(incoming) {
    bool passed = false;  
    if(incoming->IsInside(s800->GetMTof().GetCorrelatedObjE1(),s800->GetMTof().GetCorrelatedObjE1())) {
          dirname=Form("S800_%s",incoming->GetName());
          passed=true;
        }
    if(!passed)
      return false;
  }

  
  //INCOMING GATES on Obj-E1 vs Xfp-E1 raw data
  /*
  if(incoming) {
    dirname=Form("S800_%s",incoming->GetName());
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
  }*/

  /*
  if(incoming) {
    dirname=Form("S800_%s",incoming->GetName());
    bool passed = false;
    if(incoming->IsInside(s800->MCorrelatedOBJ_E1(true),s800->MCorrelatedXFP_E1(true)))
      {passed=true;} 
    
    if(!passed)
      return false;
  }*/
  
                                  
  //Incoming PID Plots
  if(incoming)
    {dirname=Form("IncPID_%s",incoming->GetName());}
  
  for(int j=0; j < XfpSize; j++) {
   for(int i=0; i < ObjSize; i++) {
     
     obj.FillHistogram(dirname,"Xfp_v_Obj_raw",4000,11000,15000,s800->GetMTof().fObj.at(i)
                                              ,3000,16000,19000,s800->GetMTof().fXfp.at(j));
      
   }
  }
  
  //obj.FillHistogram(dirname,"Xfp_v_Obj-E1_CorrTarget_wide",2000,-10000,0,s800->GetMTOF_ObjE1(0,true)
  //		                                          ,2000,0,20000,s800->GetMTOF_XfpE1(0,true));

  //obj.FillHistogram(dirname,"Xfp_v_Obj-E1_CorrTarget",1400,-1500,-700,s800->GetMTof().GetCorrelatedObjE1()
  //      	                                     ,3000,16000,19000,s800->GetMTof().GetCorrelatedXfpE1());
  //
  //obj.FillHistogram(dirname,"Xfp-E1_v_Obj-E1_CorrTarget",1000,-1500,-600,s800->GetMTOF_ObjE1(0,true)
  //      	                                        ,1600,3000,4600,s800->GetMTOF_XfpE1(0,true)
  //      	                                                              -s800->GetMTof().fE1Up.at(0));
  //
  //obj.FillHistogram(dirname,"Xfp-E1_v_Obj-E1_CorrGate",800,-1500,-700,s800->MCorrelatedOBJ_E1(true)
  //      	                                      ,1600,3000,4600,s800->MCorrelatedXFP_E1(true));
  //
  //obj.FillHistogram(dirname,"Xfp-E1_v_Obj-E1_CorrGate_Uncorrected",800,-1500,-700,s800->MCorrelatedOBJ_E1(false)
  //      	                                                  ,1600,3000,4600,s800->MCorrelatedXFP_E1(false));
  //
  //for(int k=0; k < E1UpSize; k++) {
  // for(int j=0; j < XfpSize; j++) {
  //  for(int i=0; i < ObjSize; i++) {
  //    
  //    obj.FillHistogram(dirname,"Xfp-E1_v_Obj-E1_raw",800,-1500,-700,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k)
  //                                                   ,1600,3000,4600,s800->GetMTof().fXfp.at(j)-s800->GetMTof().fE1Up.at(k));
  //   
  //  } 
  // }
  //}

  //Outgoing PID Plots
  if(incoming)
    {dirname=Form("OutPID_%s",incoming->GetName());}
  
  obj.FillHistogram(dirname,"Ion_v_Obj-E1_CorrTarget",1400,-1500,-800,s800->GetMTofObjE1()
                                                     ,2000,20000,35000,s800->GetIonChamber().Charge());


  
  //CRDC Positions
  if(incoming)
    {dirname=Form("CRDCX/Y_%s",incoming->GetName());}
  
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

  obj.FillHistogram(dirname,"CRDC1X_v_Ion",2000,20000,35000,s800->GetIonChamber().Charge()
		                          ,1200,-300,300,s800->GetCrdc(0).GetDispersiveX());

  
  //Focal Plane, Target
  if(incoming)
    {dirname=Form("FP/TA_%s",incoming->GetName());}
  
  obj.FillHistogram(dirname,"AFP",1000,-0.1,0.1,s800->GetAFP());

  obj.FillHistogram(dirname,"BFP",1000,-0.15,0.15,s800->GetBFP());

  obj.FillHistogram(dirname,"ATA",1000,-0.1,0.1,s800->GetAta());

  obj.FillHistogram(dirname,"YTA",1000,-50,50,s800->GetYta());

  obj.FillHistogram(dirname,"BTA",1000,-0.1,0.1,s800->GetBta());

  obj.FillHistogram(dirname,"DTA",1000,-0.5,0.5,s800->GetDta());

  obj.FillHistogram(dirname,"YFP_v_BFP",2000,-0.1,0.1,s800->GetBFP()
                                       ,1600,-200,200,s800->GetYFP());

  
  //Other
  if(incoming)
    {dirname=Form("FPvToF_%s",incoming->GetName());}
  
  obj.FillHistogram(dirname,"XFP_v_Obj-E1_CorrTarget",1200,-1400,-800,s800->GetMTofObjE1()
                                                     ,1200,-300,300,s800->GetXFP());



  obj.FillHistogram(dirname,"AFP_v_Obj-E1_CorrTarget",800,-1300,-900,s800->GetMTofObjE1()
                                                     ,1000,-0.05,0.05,s800->GetAFP());

  
  ////////////////////////////////////////////////////////////////////
  //OUTGOING GATES on Ion Chamber Charge vs Object - E1 (CorrTarget)//
  ////////////////////////////////////////////////////////////////////
 
  if(!outgoing)
    {return false;}
  if(!outgoing->IsInside(s800->GetMTofObjE1(),s800->GetIonChamber().Charge()))
    {return false;} 
    
  //Incoming PID Plots
  if(incoming)
    {dirname=Form("IncPID_%s",incoming->GetName());}
  
  for(int j=0; j < XfpSize; j++) {
   for(int i=0; i < ObjSize; i++) {
    obj.FillHistogram(dirname,Form("Xfp_v_Obj_raw_%s",outgoing->GetName()),4000,11000,15000,s800->GetMTof().fObj.at(i)
                                                                          ,3000,16000,19000,s800->GetMTof().fXfp.at(j));
   }
  }
  
  //obj.FillHistogram(dirname, Form("Xfp_v_Obj-E1_CorrTarget_%s",outgoing->GetName())
  //      	                                              ,1000,-1500,-600,s800->GetMTOF_ObjE1(0,true)
  //      	                                              ,3000,16000,19000,s800->GetMTOF_XfpE1(0,true));

  //obj.FillHistogram(dirname, Form("Xfp-E1_v_Obj-E1_CorrTarget_%s",outgoing->GetName())
  //      	                                                 ,1000,-1500,-600,s800->GetMTOF_ObjE1(0,true)
  //      	                                                 ,1600,3000,4600,s800->GetMTOF_XfpE1(0,true)
  //      	                                                                   - s800->GetMTof().fE1Up.at(0));


  //obj.FillHistogram(dirname, Form("Xfp-E1_v_Obj-E1_CorrGate_%s",outgoing->GetName())
  //      	                                               ,800,-1500,-700,s800->MCorrelatedOBJ_E1(true)
  //      	                                               ,2000,3000,4600,s800->MCorrelatedXFP_E1(true));

  //obj.FillHistogram(dirname, Form("Xfp-E1_v_Obj-E1_CorrGate_Uncorrected_%s",outgoing->GetName())
  //      	                                                           ,800,-1500,-700,s800->MCorrelatedOBJ_E1(false)
  //      	                                                           ,2000,3000,4600,s800->MCorrelatedXFP_E1(false));

  for(int k=0; k < E1UpSize; k++) { 
   for(int j=0; j < XfpSize; j++) {
    for(int i=0; i < ObjSize; i++) {
      obj.FillHistogram(dirname,Form("Xfp-E1_v_Obj-E1_raw_%s",outgoing->GetName())
			             ,800,-1500,-700,s800->GetMTof().fObj.at(i)-s800->GetMTof().fE1Up.at(k)
			             ,600,3400,4000,s800->GetMTof().fXfp.at(j)-s800->GetMTof().fE1Up.at(k));
    } 
   }
  }

  //Outgoing PID Plots
  if(incoming)
    {dirname=Form("OutPID_%s",incoming->GetName());}
  
 //obj.FillHistogram(dirname,Form("Ion_v_Obj-E1_CorrTarget_%s",outgoing->GetName())
 //      	                                             ,1000,-1500,-800,s800->GetMTOF_ObjE1(0,true)
 //                                                           ,2000,20000,35000,s800->GetIonChamber().Charge());

 //obj.FillHistogram(dirname,Form("Ion_v_Obj-E1_CorrGate_%s",outgoing->GetName())
 //      	                                           ,800,-1500,-700,s800->MCorrelatedOBJ_E1(true)
 //                                                         ,2000,20000,35000,s800->GetIonChamber().Charge());

 //obj.FillHistogram(dirname,Form("Ion_v_Obj-E1_CorrGate_Uncorrected_%s",outgoing->GetName())
 //		                                                       ,800,-1500,-700,s800->MCorrelatedOBJ_E1(false)
  //                                                                    ,2000,20000,35000,s800->GetIonChamber().Charge());


 //CRDC Positions
 if(incoming)
    {dirname=Form("CRDCX/Y_%s",incoming->GetName());}
  
 obj.FillHistogram(dirname,Form("CRDC1X_%s",outgoing->GetName()), 1200, -300, 300, s800->GetCrdc(0).GetDispersiveX());

 obj.FillHistogram(dirname,Form("CRDC1Y_%s",outgoing->GetName()), 800, -200, 200, s800->GetCrdc(0).GetNonDispersiveY());

 obj.FillHistogram(dirname,Form("CRDC1Y_v_CRDC1X_%s",outgoing->GetName()),1200,-300,300,s800->GetCrdc(0).GetDispersiveX()
                                                                         ,800,-200,200,s800->GetCrdc(0).GetNonDispersiveY());

 obj.FillHistogram(dirname,Form("CRDC1X_v_Ion_%s",outgoing->GetName()),2000,20000,35000,s800->GetIonChamber().Charge()
		                                                      ,1200,-300,300,s800->GetCrdc(0).GetDispersiveX());

 //Focal Plane, Target
 if(incoming)
    {dirname=Form("FP/TA_%s",incoming->GetName());}
 
 obj.FillHistogram(dirname,Form("AFP_%s",outgoing->GetName()),1000,-0.1,0.1,s800->GetAFP());

 obj.FillHistogram(dirname,Form("BFP_%s",outgoing->GetName()),1000,-0.15,0.15,s800->GetBFP());

 obj.FillHistogram(dirname,Form("ATA_%s",outgoing->GetName()),1000,-0.1,0.1,s800->GetAta());

 obj.FillHistogram(dirname,Form("YTA_%s",outgoing->GetName()),1000,-50,50,s800->GetYta());

 obj.FillHistogram(dirname,Form("BTA_%s",outgoing->GetName()),1000,-0.1,0.1,s800->GetBta());

 obj.FillHistogram(dirname,Form("DTA_%s",outgoing->GetName()),1000,-0.5,0.5,s800->GetDta());

 obj.FillHistogram(dirname,Form("YFP_v_BFP_%s",outgoing->GetName()),1000,-0.1,0.1,s800->GetBFP()
                                                                   ,800,-200,200,s800->GetYFP());

 //Other
 if(incoming)
    {dirname=Form("FPvToF_%s",incoming->GetName());}
  //
  //obj.FillHistogram(dirname,Form("XFP_v_Obj-E1_CorrTarget_%s",outgoing->GetName())
  //       	                                            ,1200,-1400,-800,s800->GetMTOF_ObjE1(0,true)
  //                                                           ,1200,-300,300,s800->GetXFP());
  //
  //obj.FillHistogram(dirname,Form("XFP_v_Obj-E1_CorrGate_%s",outgoing->GetName())
  //       	                                          ,800,-1500,-700,s800->MCorrelatedOBJ_E1(true)
  //       	                                          ,1200,-300,300,s800->GetXFP());
  //
  //obj.FillHistogram(dirname,Form("XFP_v_Obj-E1_CorrGate_Uncorrected_%s",outgoing->GetName())
  //       	                                                      ,800,-1500,-700,s800->MCorrelatedOBJ_E1(false)
  //       	                                                      ,1200,-300,300,s800->GetXFP());
  //
  //obj.FillHistogram(dirname,Form("AFP_v_Obj-E1_CorrTarget_%s",outgoing->GetName())
  //       	                                            ,800,-1300,-900,s800->GetMTOF_ObjE1(0,true)
  //                                                           ,1000,-0.05,0.05,s800->GetAFP());
  //
  //obj.FillHistogram(dirname,Form("AFP_v_Obj-E1_CorrGate_%s",outgoing->GetName())
  //       	                                          ,800,-1500,-700,s800->MCorrelatedOBJ_E1(true)
  //                                                         ,1000,-0.05,0.05,s800->GetAFP());
  //
  //obj.FillHistogram(dirname,Form("AFP_v_Obj-E1_CorrGate_Uncorrected_%s",outgoing->GetName())
  //       	                                                      ,800,-1500,-700,s800->MCorrelatedOBJ_E1(false)
  //                                                                     ,1000,-0.05,0.05,s800->GetAFP());
  //
  return true;
    
}

bool HandleGretina(TRuntimeObjects &obj) {
  TGretina *gretina  = obj.GetDetector<TGretina>(); 
   
  if(!gretina)
    {return false;}
  std::string dirname = "Gretina";
  for(unsigned int i=0;i<gretina->Size();i++) {
      TGretinaHit hit = gretina->GetGretinaHit(i);

      if(hit.GetCoreEnergy() > 90)
	{

         obj.FillHistogram(dirname,"GretinaCoreEnergy",2000,0,4000,hit.GetCoreEnergy());

         obj.FillHistogram(dirname,"GretinaSummarySpectrum",200,0,200,hit.GetCrystalId()
			                                   ,2000,0,4000,hit.GetCoreEnergy());
                                                 

         obj.FillHistogram(dirname,"GretinaPositionSpectrum",226,0,3.2,hit.GetTheta()
                                                            ,452,0,6.3,hit.GetPhi());
      
         //plots with s800
         //TS800 *s800 = obj.GetDetector<TS800>();
         //if(!s800) {return false;}
      
        } 
    }


  return true;

}

bool HandleGretina_Gated(TRuntimeObjects &obj,TCutG *outgoing, TCutG* incoming, TCutG* time_energy = NULL) {
  TGretina *gretina  = obj.GetDetector<TGretina>(); 
  TS800 *s800 = obj.GetDetector<TS800>();
  TBank88 *bank29 = obj.GetDetector<TBank88>();
  
  if(!gretina || !s800 || s800->GetMTof().E1UpSize() != 1)
    {return false;}

  //int E1UpSize = s800->GetMTof().E1UpSize();
  //int XfpSize = s800->GetMTof().XfpSize();
  //int ObjSize = s800->GetMTof().ObjSize();
  
  std::string dirname = "Gretina";
  
  //Incoming gate on Xfp - E1 vs OBJ - E1 (raw) 
  /*
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
  }*/

  //Incoming gate on Xfp - E1 vs OBJ - E1 (CorrGate)
  /*
  if(incoming) {
    dirname=Form("Gretina_%s",incoming->GetName());
    bool passed = false;
    if(incoming->IsInside(s800->MCorrelatedOBJ_E1(true),s800->MCorrelatedXFP_E1(true)))
      {passed=true;} 
    
    if(!passed)
      return false;
  }*/

  //incoming gates on Xfp vs. OBJ -E1 (CorrTarget)
  if(incoming) {
    bool passed = false;  
    //if(incoming->IsInside(s800->GetMTofObjE1(),s800->GetMTofXfpE1())) {  //!!! this requires both obj AND xfp corrections to be present!
    if(incoming->IsInside(s800->GetMTof().GetCorrelatedObjE1(),s800->GetMTof().GetCorrelatedXfpE1())) {  
          dirname=Form("Gretina_%s",incoming->GetName());
          passed=true;
        }
    if(!passed)
      return false;
  }

   for(unsigned int i=0;i<gretina->Size();i++) {
     TGretinaHit hit = gretina->GetGretinaHit(i);

     if(hit.GetCoreEnergy() > 90) {

     obj.FillHistogram(dirname,"Gamma(Beta)_v_Phi",452,0,6.3,hit.GetPhi()
		                                  ,2000,0,4000,hit.GetDoppler(0.39));
                                                  
     obj.FillHistogram(dirname,"Gamma(Beta)_v_Theta",452,0,6.3,hit.GetTheta()
                                                    ,2000,0,4000,hit.GetDoppler(0.39));

     TVector3 track = s800->Track();
     obj.FillHistogram(dirname,"Gamma(Beta&Track)_v_Phi",452,0,6.3,hit.GetPhi()
                                                        ,2000,0,4000,hit.GetDoppler(0.39,&track));

     obj.FillHistogram(dirname,"Gamma(Beta&Track)_v_Theta",452,0,6.3,hit.GetTheta()
                                                          ,2000,0,4000,hit.GetDoppler(0.39,&track));

     //obj.FillHistogram(dirname,"GretinaTime",10000,0,3000000,hit.GetTime());
   }
  }

   if(!bank29)
     {return false;}

   for(unsigned int i=0;i<gretina->Size();i++) {
     TGretinaHit hit = gretina->GetGretinaHit(i);
     
     TVector3 track = s800->Track();
     obj.FillHistogram(dirname,"GretinaTime_v_Bank88TimeStamp",1200,-400,800,bank29->Timestamp() - hit.GetTime()
                                                              ,2000,0,4000,hit.GetDoppler(0.39,&track));

   }
    
  ////////////////////////////////////////////////////////////////////
  //OUTGOING GATES on Ion Chamber Charge vs Object - E1 (CorrTarget)//
  ////////////////////////////////////////////////////////////////////
  
  if(!outgoing)
    {return false;}
  
  if(!outgoing->IsInside(s800->GetMTofObjE1(),s800->GetIonChamber().Charge()))
    {return false;}

  for(unsigned int i=0;i<gretina->Size();i++) {
    TGretinaHit hit = gretina->GetGretinaHit(i);

    if(hit.GetCoreEnergy() > 90) {   
	
      obj.FillHistogram(dirname,Form("Gamma(Beta)_%s",outgoing->GetName()),2000,0,4000,hit.GetDoppler(0.39));

      TVector3 track = s800->Track();
      obj.FillHistogram(dirname,Form("Gamma(Beta&Track)_%s",outgoing->GetName()),2000,0,4000,hit.GetDoppler(0.39,&track));

      obj.FillHistogram(dirname,Form("Gamma(Beta&Track)_v_DTA_%s",outgoing->GetName()),1000,-0.5,0.5,s800->GetDta()
                                                                                      ,2000,0,4000,hit.GetDoppler(0.39,&track));

      //obj.FillHistogram(dirname,Form("GretinaTime_%s",outgoing->GetName()),10000,0,3000000,hit.GetTime());
    }
  }

  
  if(!bank29)
     {return false;}

   for(unsigned int i=0;i<gretina->Size();i++) {
     TGretinaHit hit = gretina->GetGretinaHit(i);
     
     TVector3 track = s800->Track();
     obj.FillHistogram(dirname,Form("GretinaTime_v_Bank88TimeStamp_%s",outgoing->GetName())
		                                                      ,1200,-400,800,bank29->Timestamp() - hit.GetTime()
                                                                      ,2000,0,4000,hit.GetDoppler(0.39,&track));
  }

  ///////////////////////////////////////////////////////////////
  //Time-Energy Gate on Gamma Enery vs s800 Time - Gretina Time//
  ///////////////////////////////////////////////////////////////

  /*
  if(!time_energy)
    {return false;}

  for(unsigned int i=0;i<gretina->Size();i++) {
    TGretinaHit hit = gretina->GetGretinaHit(i);
    TVector3 track = s800->Track();
    
    if(!time_energy->IsInside(s800->GetTimestamp() - hit.GetTime(),hit.GetDoppler(0.39,&track)))
      {return false;}


  }*/
  
  return true;

}

extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  //TGretina *gretina = obj.GetDetector<TGretina>();
  //TBank88  *bank29  = obj.GetDetector<TBank88>();
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
      } else if(!tag.compare("time_energy")) {
        time_energy_cut = gate;
        std::cout << "time_energy: << " << gate->GetName() << std::endl;
      }
      gates_loaded++;
    }
  }
   
  
  HandleGretina(obj);

  //HandleS800(obj);

  HandleTiming(obj);

  for(size_t i=0;i<incoming_cuts.size();i++) {
   for(size_t j=0;j<outgoing_cuts.size();j++) {
     HandleS800_Gated(obj,incoming_cuts.at(i),outgoing_cuts.at(j));
     HandleGretina_Gated(obj,outgoing_cuts.at(j),incoming_cuts.at(i),time_energy_cut);
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
//      histname = "Gretina_Bank88_time";
//      obj.FillHistogram(dirname,histname,
//          600,-600,600,bank29->Timestamp()-hit.Timestamp(),
//          2000,0,4000,hit.GetCoreEnergy());
//      histname = "Gretina_t0_Bank88_time";
//      obj.FillHistogram(dirname,histname,
//          600,-600,600,bank29->Timestamp()-hit.GetTime(),
//          2000,0,4000,hit.GetCoreEnergy());
//    }
//  }

  if(numobj!=list->GetSize())
    list->Sort();
}




