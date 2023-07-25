
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

GCutG *gt_time=0;

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
		     1500, -5000, -2000,
		     s800->GetOBJ_E1Raw_MESY(),
		     1500, 500, 3500,
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
		     1000, -4600, -3600,
		     s800->GetCorrTOF_OBJ_MESY(),
		     1200, 6000, 20000,
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

  std::string histname = Form("crdc1_x_%s_%s",
			      incoming->GetName(),
			      outgoing->GetName());
  obj.FillHistogram(dirname,histname,800,-400,400,
		    s800->GetCrdc(0).GetDispersiveX());

  histname = Form("crdc2_x_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname,histname,
		    800, -400, 400,
		    s800->GetCrdc(1).GetDispersiveX());

  histname = Form("crdc1_x_TOF_OBJ_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname,histname,
		    1000, -4600, -3600,
		    s800->GetCorrTOF_OBJ_MESY(),
		    800, -400, 400,
		    s800->GetCrdc(0).GetDispersiveX());

  histname = Form("AFP_TOF_OBJ_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname,histname,
		    1000, -4600, -3600,
		    s800->GetCorrTOF_OBJ_MESY(),
		    100, -0.05, 0.05,
		    s800->GetAFP());
  
  histname = Form("dta_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    200, -0.1, 0.1,
		    s800->GetDta());

  Double_t ata = s800->GetAta()*TMath::RadToDeg();
  Double_t bta = s800->GetBta()*TMath::RadToDeg();
  histname = Form("ata_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    1200, -6.0, 6.0,
		    ata);
  histname = Form("bta_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    1200, -6.0, 6.0,
		    bta);
  histname = Form("ata_bta_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    1200, -6.0, 6.0,
		    bta,
		    1200, -6.0, 6.0,
		    ata);
  histname = Form("yta_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  obj.FillHistogram(dirname, histname,
		    200, -20, 20,
		    s800->GetYta());
  histname = Form("scatter_%s_%s",
		  incoming->GetName(),
		  outgoing->GetName());
  Double_t scatter = sqrt(ata*ata + bta*bta);
  obj.FillHistogram(dirname, histname,
		    600, 0, 6,
		    scatter);

  return true;
}

bool TriggerRegister(TRuntimeObjects &obj, GCutG *incoming, GCutG *outgoing){

  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800       *s800 = obj.GetDetector<TS800>();

  if(!gretina || !s800)
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

bool HandleGretina(TRuntimeObjects &obj,GCutG *incoming,
		   GCutG *outgoing, GCutG *gt_time) {

   if(!incoming || !outgoing)
     return false;

   TGretina *gretina = obj.GetDetector<TGretina>();
   TS800 *s800       = obj.GetDetector<TS800>();

   if(!gretina || !s800)
     return false;
   
   //   if(!((s800->GetTrigger().GetRegistr()&0x0002)>>1))
   //     return false;

   if(!incoming->IsInside(s800->GetOBJ_E1Raw_MESY(),
			  s800->GetXF_E1Raw_MESY()) )
     return false;

   if(!outgoing->IsInside(s800->GetCorrTOF_OBJ_MESY(),
   			  s800->GetIonChamber().GetSum()) )
     return false;
   //			  s800->GetIonChamber().Charge()) )   
   
   std::string dirname = "gretina";

   Int_t    energyNChannels = 2000;
   Double_t energyLlim = 0.;
   Double_t energyUlim = 8000.;
   
   double beta = GValue::Value("BETA");
   if(std::isnan(beta))
     beta=0.00;
   double xoffset = GValue::Value("GRETINA_X_OFFSET");
   if(std::isnan(xoffset))
     xoffset=0.00;
   double yoffset = GValue::Value("GRETINA_Y_OFFSET");
   if(std::isnan(yoffset))
     yoffset=0.00;
   double zoffset = GValue::Value("GRETINA_Z_OFFSET");
   if(std::isnan(zoffset))
     zoffset=0.00;
   TVector3 targetOffset(xoffset, yoffset, zoffset);

   for(int x=0;x<gretina->Size();x++) {
     TGretinaHit hit = gretina->GetGretinaHit(x);
     std::string histname = "energy";
     obj.FillHistogram(dirname, histname,
		       energyNChannels*4, energyLlim, energyUlim,
		       hit.GetCoreEnergy());

     histname = "overview";
     obj.FillHistogram(dirname, histname,
		       energyNChannels*4, energyLlim, energyUlim,
		       hit.GetCoreEnergy(),
		       120, 0, 120,
		       hit.GetCrystalId());
       
     histname = Form("doppler_%s_%s",incoming->GetName(), outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       energyNChannels, energyLlim, energyUlim,
		       hit.GetDoppler(beta, 0));

     histname = Form("doppler_s800_%s_%s",incoming->GetName(),
		     outgoing->GetName());
     TVector3 track = s800->Track();
     obj.FillHistogram(dirname, histname,
		       energyNChannels, energyLlim, energyUlim,
		       hit.GetDoppler(beta, &track));

     histname = Form("doppler_theta_%s_%s",incoming->GetName(),
		     outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       100, 0, TMath::Pi(),
		       hit.GetTheta(),
		       energyNChannels, energyLlim, energyUlim,
		       hit.GetDoppler(beta, 0));

     histname = Form("theta_phi_%s_%s",incoming->GetName(),
		     outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       500, 0, TMath::Pi(),
		       hit.GetPhi(),
		       500, 0, TMath::Pi(),
		       hit.GetTheta());
     histname = Form("theta_%s_%s",incoming->GetName(), outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       500, 0, TMath::Pi(),
		       hit.GetTheta());
     histname = Form("phi_%s",outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       500, 0, TMath::Pi(),
		       hit.GetPhi());
       
     if( hit.GetCrystalPosition().Theta() < TMath::Pi()/2. ){
       histname = Form("theta_phi_fw_%s_%s",incoming->GetName(),
		       outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetPhi(),
			 500, 0, TMath::Pi(),
			 hit.GetTheta());
       histname = Form("theta_fw_%s_%s",incoming->GetName(),
		       outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetTheta());
       histname = Form("phi_fw_%s_%s",incoming->GetName(),
		       outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetPhi());
     } else {
       histname = Form("theta_phi_bw_%s_%s",incoming->GetName(),
		       outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetPhi(),
			 500, 0, TMath::Pi(),
			 hit.GetTheta());
       histname = Form("theta_bw_%s_%s",incoming->GetName(),
		       outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetTheta());
       histname = Form("phi_bw_%s_%s",incoming->GetName(),
		       outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 500, 0, TMath::Pi(),
			 hit.GetPhi());
     }

     // Time cut
     if(!gt_time)
       return false;

     if( !gt_time->IsInside(s800->GetTimestamp()-hit.GetTime(),
			    hit.GetCoreEnergy()) ){
       histname = Form("background_%s_%s",incoming->GetName(),
		       outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 energyNChannels*4, energyLlim, energyUlim,
			 hit.GetCoreEnergy());
       histname = Form("background_dop_%s_%s",incoming->GetName(),
		       outgoing->GetName());
       obj.FillHistogram(dirname, histname,
			 energyNChannels*4, energyLlim, energyUlim,
			 hit.GetDoppler(beta, 0));
       return false;
     }
  
     histname = Form("doppler_%s_%s_t",incoming->GetName(),
		     outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       energyNChannels, energyLlim, energyUlim,
		       hit.GetDoppler(beta, 0));

     histname = Form("doppler_s800_%s_%s_t",incoming->GetName(),
		     outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       energyNChannels, energyLlim, energyUlim,
		       hit.GetDoppler(beta, &track));

     histname = Form("doppler_theta_%s_%s_t",incoming->GetName(),
		     outgoing->GetName());
     obj.FillHistogram(dirname, histname,
		       100, 0, TMath::Pi(),
		       hit.GetTheta(),
		       energyNChannels, energyLlim, energyUlim,
		       hit.GetDoppler(beta, 0));

   }


  return true;
}


extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  //std::cout << "---------------------------------" <<std::endl;
  //std::cout << " At the beginning" << std::endl;
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TS800 *s800       = obj.GetDetector<TS800>();
  //std::cout << " Dets Gotten" << std::endl;
  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  TList *gates = &(obj.GetGates());

  TIter next(gates);
  GCutG* gate = 0;
  while ((gate = (GCutG*)next())) {
    TString cutname = gate->GetName();
    if(!incoming_cut && cutname.Contains("in"))
      incoming_cut = gate;

    if(!outgoing_cut && cutname.Contains("out"))
      outgoing_cut = gate;
  }

  if(!gt_time) {
    gt_time = (GCutG*)gates->FindObject("gt_time");
  }

  std::string histname = "";
  std::string dirname  = "";


  if(s800) {
    dirname = "S800";

    IncomingS800(obj, s800, 0);
    IncomingS800(obj, s800, incoming_cut);

    OutgoingS800(obj, s800, 0);
    OutgoingS800(obj, s800, outgoing_cut);

    DTA(obj, incoming_cut, outgoing_cut);

    if(gretina) {
      
      HandleGretina(obj, incoming_cut, outgoing_cut, gt_time);

      dirname = "gretina";
      for(unsigned int i=0;i<gretina->Size();i++) {
	TGretinaHit hit = gretina->GetGretinaHit(i);
	histname = "dtime_all";
	// obj.FillHistogram(dirname,histname,
	// 		  500, -250, 250,
	// 		  s800->GetTimestamp()-hit.GetTimestamp(),
	// 		  2000, 0, 8000,
	// 		  hit.GetCoreEnergy());
	histname = "dtimet0_all";
	obj.FillHistogram(dirname,histname,
			  500, 0, 500,
			  s800->GetTimestamp()-hit.GetTime(),
			  2000, 0, 8000,
			  hit.GetCoreEnergy());
	unsigned short bits = s800->GetTrigger().GetRegistr();
	for(int j=0;j<5;j++) {
	  if((bits>>j)&0x0001) {
	    // histname = Form("dtime_all_reg%i",j);
	    // obj.FillHistogram(dirname,histname,
	    // 		      500, -250, 250,
	    // 		      s800->GetTimestamp()-hit.GetTimestamp(),
	    // 		      1000, 0, 8000,
	    // 		      hit.GetCoreEnergy());
	    histname = Form("dtimet0_reg%i",j);
	    obj.FillHistogram(dirname,histname,
			      500, 0, 500,
			      s800->GetTimestamp()-hit.GetTime(),
			      1000, 0, 8000,
			      hit.GetCoreEnergy());
	    if(incoming_cut && !outgoing_cut &&
	       incoming_cut->IsInside(s800->GetOBJ_E1Raw_MESY(),
				      s800->GetXF_E1Raw_MESY()) &&
	       outgoing_cut->IsInside(s800->GetCorrTOF_OBJ_MESY(),
				      s800->GetIonChamber().GetSum()) ){
	      //		      s800->GetIonChamber().Charge()) ){

	      // histname = Form("dtime_all_reg%i_%s_%s",j,
	      // 		      incoming_cut->GetName(),
	      // 		      outgoing_cut->GetName());
	      // obj.FillHistogram(dirname,histname,
	      // 			500, -250, 250,
	      // 			s800->GetTimestamp()-hit.GetTimestamp(),
	      // 			1000, 0, 8000,
	      // 			hit.GetCoreEnergy());
	      histname = Form("dtimet0_reg%i_%s_%s",j,
			      incoming_cut->GetName(),
			      outgoing_cut->GetName());
	      obj.FillHistogram(dirname,histname,
				500, 0, 500,
				s800->GetTimestamp()-hit.GetTime(),
				1000, 0, 8000,
				hit.GetCoreEnergy());
	    }
	  }
	}
      }
    }
  }

  if(numobj!=list->GetSize())
    list->Sort();

}

