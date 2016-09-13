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
#include "GValue.h"


#include "TChannel.h"
#include "GValue.h"

#define Q1 15
#define Q2 7
#define Q3 8
#define Q4 16
#define Q5 9
#define Q6 14
#define Q7 17
#define Q8 6
#define Q9 19

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
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank29  *bank29  = obj.GetDetector<TBank29>();
  TS800 *s800       = obj.GetDetector<TS800>();
  double BETA        = GValue::Value("BETA");
  TVector3 trackvect_Spec(0,0,1);

  TList *gates = &(obj.GetGates());
  bool haspids = gates->GetSize();
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
  }// end if BANK29


  /*=============================*\
  | If you dont have gretina, or  |
  | if the gretina size is less   |
  | than 1, get out of here.      |
  \*=============================*/
  if(!gretina || gretina->Size()<1)
    return;

  double gsum = 0.0;
  for(unsigned int y=0;y<gretina->Size();y++) {
    TGretinaHit hit = gretina->GetGretinaHit(y);
    gsum += hit.GetCoreEnergy();
    if(bank29) {
      dirname = "GretSummTime";
      histname = "Gretina_Bank29_time";
      obj.FillHistogram(dirname,histname,
			600,-600,600,bank29->Timestamp()-hit.GetTimestamp(),
			2000,0,4000,hit.GetCoreEnergy());
      histname = "Gretina_t0_Bank29_time";
      obj.FillHistogram(dirname,histname,
			600,-600,600,bank29->Timestamp()-hit.GetTime(),
			2000,0,4000,hit.GetCoreEnergy());
    }

    if(s800) {
      TVector3 trackvect=s800->ExitTargetVect();
      TVector3 trackvect_WithTC=s800->ExitTargetVect();
      TVector3 trackvect_WithNegTC=s800->ExitTargetVect();
      double targx = GValue::Value("GRETINA_X_OFFSET");
      double targy = GValue::Value("GRETINA_Y_OFFSET");
      double targz = GValue::Value("GRETINA_Z_OFFSET");
      TVector3 TargetShift;
      TargetShift.SetXYZ(targx,targy,targz);
      trackvect_WithTC = trackvect_WithTC+TargetShift;
      trackvect_WithNegTC = trackvect_WithNegTC-TargetShift;
      /*      TVector3 zAxis(0,0,1);
      TVector3 NormResPlane = trackvect.Cross(zAxis);
      TVector3 NormGammaPlane = hit.GetPosition().Cross(zAxis);
      TVector3 NormDiff = NormResPlane-NormGammaPlane;
      TVector3 NormCrossNorm = NormResPlane.Cross(NormGammaPlane);
      double SinHolder = TMath::ASin(NormCrossNorm.Mag()/(NormGammaPlane.Mag()*NormResPlane.Mag()));
      double openang = NormResPlane.Angle(NormGammaPlane);
      double CosHolder = NormResPlane.Angle(NormGammaPlane);
      double openang2 = TMath::ATan2(SinHolder,CosHolder);*/
      //if(Stuff.Phi()<0) openang =6.28 - openang;

      double openang = (2.0*TMath::Pi() - s800->Azita())  - TMath::Abs(hit.GetPosition().Phi());
      if(openang<0) openang += 2.0*TMath::Pi();




      dirname = "GretSummTime";
      histname = "Gretina_S800_time";
      obj.FillHistogram(dirname,histname,
			1200,-600,600,s800->Timestamp()-hit.GetTimestamp(),
			2000,0,4000,hit.GetCoreEnergy());
      histname = "Gretina_t0_S800_time";
      obj.FillHistogram(dirname,histname,
			1200,-600,600,s800->Timestamp()-hit.GetTime(),
			3000,0,6000,hit.GetCoreEnergy());

    float AllCorrOBJ_E1Val = s800->MCorrelatedOBJ_E1(false);
    float AllCorrXFP_E1Val = s800->MCorrelatedXFP_E1(false);
    dirname = "CorrelatedMESY";
    if(AllCorrOBJ_E1Val && AllCorrXFP_E1Val){
      //--------------------------------------------------------------------
      histname = "1D_OBJvsE1";
      obj.FillHistogram(dirname,histname,
			3000,-50000,50000,AllCorrOBJ_E1Val);

      histname = "1D_XFPvsE1";
      obj.FillHistogram(dirname,histname,
			3000,-50000,50000,AllCorrXFP_E1Val);

      histname = "2D_OBJvsE1";
      obj.FillHistogram(dirname,histname,
			3000,0,70000,s800->MCorrelatedOBJ(),
			3000,0,70000,s800->MCorrelatedE1());

      histname = "2D_XFPvsE1";
      obj.FillHistogram(dirname,histname,
			3000,0,70000,s800->MCorrelatedXFP(),
			3000,0,70000,s800->MCorrelatedE1());

      histname = "2D_Incoming_PID";
      obj.FillHistogram(dirname,histname,
			2000,-3000,0,AllCorrOBJ_E1Val,
			2000,2000,5000,AllCorrXFP_E1Val);

      double AllCorrectedOBJ_E1Val = s800->MCorrelatedOBJ_E1(true);
      double AllCorrectedXFP_E1Val = s800->MCorrelatedXFP_E1(true);
      histname = "2D_MTOF_vs_DispX";
      obj.FillHistogram(dirname,histname,
			4000,-2000,0,AllCorrectedOBJ_E1Val,
			600,-300,300,s800->GetCrdc(0).GetDispersiveX());

      histname = "2D_MTOF_vs_AFP";
      obj.FillHistogram(dirname,histname,
			4000,-2000,0,AllCorrectedOBJ_E1Val,
			600,-0.1,0.1,s800->GetAFP());

      histname ="2D_MTOF_PID";
      obj.FillHistogram(dirname,histname,
			1000,-2000,0,AllCorrectedOBJ_E1Val,
			1000,0,40000,s800->GetIonChamber().Charge());

      histname ="2D_MTOF_ICCorr_PID";
      obj.FillHistogram(dirname,histname,
			1000,-2000,0,AllCorrectedOBJ_E1Val,
			1000,0,40000,s800->GetIonChamber().GetdECorr(&(s800->GetCrdc())));

      histname = "2D_Incoming_PID_BothCorrected";
      obj.FillHistogram(dirname,histname,
			2000,-3000,0,AllCorrectedOBJ_E1Val,
			2000,2000,5000,AllCorrectedXFP_E1Val);

      histname = "2D_Incoming_PID_XFPCorrected";
      obj.FillHistogram(dirname,histname,
			2000,-3000,0,AllCorrOBJ_E1Val,
			2000,2000,5000,AllCorrectedXFP_E1Val);

      histname = "2D_Incoming_PID_OBJCorrected";
      obj.FillHistogram(dirname,histname,
			2000,-3000,0,AllCorrectedOBJ_E1Val,
			2000,2000,5000,AllCorrXFP_E1Val);

      //--------------------------------------------------------------------

      dirname = "Gamma_Doppler";
      histname = "GretinaDoppler";
      obj.FillHistogram(dirname,histname,
			3000,0,6000,hit.GetDoppler(BETA));

      if(gretina->Size()==1){
	histname = "Singles_GretinaDoppler";
	obj.FillHistogram(dirname,histname,
			  3000,0,6000,hit.GetDoppler(BETA));
      }


      histname = "GretinaDopplerVsOpenAng";
      obj.FillHistogram(dirname,histname,
			1000,0,6.3,openang,
			2000,0,4000,hit.GetDoppler(BETA));
      /*      histname = "GretinaDopplerVsOpenAng_Dirk";
      obj.FillHistogram(dirname,histname,
			3000,-10,10,openang_Dirk,
			2000,0,4000,hit.GetDoppler(BETA));
      histname = "GretinaDopplerVsOpenAng_Check";
      obj.FillHistogram(dirname,histname,
			2000,-7,7,openang2,
			2000,0,4000,hit.GetDoppler(BETA));*/


      int QuadNumb = HoleQMap[hit.GetCrystalId()/4];
      int SummarySlot = 0;
      bool FWDet = false;
      switch(QuadNumb){
      case 1:
	FWDet = false;
	SummarySlot = 1;
	break;
      case 2:
	FWDet = true;
	SummarySlot = 1;
	break;
      case 3:
	FWDet = true;
	SummarySlot = 2;
	break;
      case 4:
	FWDet = false;
	SummarySlot = 2;
	break;
      case 5:
	FWDet = true;
	SummarySlot = 3;
	break;
      case 6:
	FWDet = false;
	SummarySlot = 3;
	break;
      case 7:
	SummarySlot = 4;
	FWDet = false;
	break;
      case 8:
	SummarySlot = 4;
	FWDet = true;
	break;
      case 9:
	SummarySlot = 5;
	FWDet = false;
	break;
      default:

	break;
      }

      histname = Form("GretinaDopplerVsDTA_Quad%02i",QuadNumb);;
      obj.FillHistogram(dirname,histname,
			1000,-0.06,0.02,s800->GetDta(),
			2000,0,4000,hit.GetDoppler(BETA));



      histname = "XtalDoppSummary";
      obj.FillHistogram(dirname,histname,
			70,20,90,hit.GetCrystalId(),
			1000,0,4000,hit.GetDoppler(BETA));



      histname = "QuadDoppSummary";
      obj.FillHistogram(dirname,histname,
			10,0,10,HoleQMap[hit.GetCrystalId()/4],
			2000,0,4000,hit.GetDoppler(BETA));


      //std::cout << " ---- First call to GetDoppler with TRACK --------" << std::endl;
      if(FWDet){
	histname = "QuadDoppSummary_FWRing";
	obj.FillHistogram(dirname,histname,
			  10,0,10,SummarySlot,
			  2000,0,4000,hit.GetDoppler(BETA,&trackvect));

	histname = "GretinaDopplerVsDTA_FWRing";
	obj.FillHistogram(dirname,histname,
			  1000,-0.06,0.02,s800->GetDta(),
			  2000,0,4000,hit.GetDoppler(BETA));
      } else{
	histname = "QuadDoppSummary_90DegRing";
	obj.FillHistogram(dirname,histname,
			  10,0,10,SummarySlot,
			  2000,0,4000,hit.GetDoppler(BETA,&trackvect));

	histname = "GretinaDopplerVsDTA_90DegRing";
	obj.FillHistogram(dirname,histname,
			  1000,-0.06,0.02,s800->GetDta(),
			  2000,0,4000,hit.GetDoppler(BETA));

      }

      for(unsigned int z2=y+1;z2<gretina->Size();z2++) {
	TGretinaHit hit_2 = gretina->GetGretinaHit(z2);

	histname = "Gamma_Gamma";
	obj.FillHistogram(dirname,histname,
			  3000,0,6000,hit.GetDoppler(BETA),
			  3000,0,6000,hit_2.GetDoppler(BETA));
	obj.FillHistogram(dirname,histname,
			  3000,0,6000,hit_2.GetDoppler(BETA),
			  3000,0,6000,hit.GetDoppler(BETA));

	histname = "Gamma_Gamma_Time";
	obj.FillHistogram(dirname,histname,
			  800,-400,400,hit.GetTime()-hit_2.GetTime(),
			  3000,0,6000,hit.GetDoppler(BETA));
	obj.FillHistogram(dirname,histname,
			  800,-400,400,hit_2.GetTime()-hit.GetTime(),
			  3000,0,6000,hit.GetDoppler(BETA));
      }

      //--------------------------------------------------------------------
      dirname = "Gamma_InvMapCorr";
      histname = "GretinaVecCorr";
      obj.FillHistogram(dirname,histname,
			3000,0,6000,hit.GetDoppler(BETA,&trackvect));

      if(gretina->Size()==1){
	histname = "Singles_GretinaInvMap";
	obj.FillHistogram(dirname,histname,
			  3000,0,6000,hit.GetDoppler(BETA,&trackvect));
      }


      histname = "GretinaVecCorrVsOpenAng";
      obj.FillHistogram(dirname,histname,
			1000,0,6.3,openang,
			2000,0,4000,hit.GetDoppler(BETA,&trackvect));

      histname = "GretinaVecCorrVsDTA";
      obj.FillHistogram(dirname,histname,
			1000,-0.2,0.2,s800->GetDta(),
			2000,0,4000,hit.GetDoppler(BETA,&trackvect));


      histname = Form("GretinaVectorVsDTA_Quad%02i",QuadNumb);;
      obj.FillHistogram(dirname,histname,
			1000,-0.06,0.02,s800->GetDta(),
			2000,0,4000,hit.GetDoppler(BETA,&trackvect));

      histname = "QuadVectSummary";
      obj.FillHistogram(dirname,histname,
			10,0,10,HoleQMap[hit.GetCrystalId()/4],
			2000,0,4000,hit.GetDoppler(BETA,&trackvect));
      // std::cout << " Before XtalVectSummary ================== " << std::endl;
      // std::cout << " Xtal     = " << hit.GetCrystalId() << std::endl;
      // std::cout << " X Offset = " << GValue::Value("GRETINA_X_OFFSET") << std::endl;
      // std::cout << " Y Offset = " << GValue::Value("GRETINA_Y_OFFSET") << std::endl;
      // std::cout << " Z Offset = " << GValue::Value("GRETINA_Z_OFFSET") << std::endl;

      histname = "XtalVectSummary";
      obj.FillHistogram(dirname,histname,
			70,20,90,hit.GetCrystalId(),
			1000,0,4000,hit.GetDoppler(BETA,&trackvect));
      //std::cout << " After XtalVectSummary ================== " << std::endl;
      TVector3 TV90 = trackvect;
      TV90.RotateZ(TMath::Pi()/2.);
      TVector3 TV270 = trackvect;
      TV270.RotateZ(3.0*TMath::Pi()/2.);
      TVector3 TV180 = trackvect;
      TV180.RotateZ(TMath::Pi());

      histname = "XtalVectSummary_90";
      obj.FillHistogram(dirname,histname,
			70,20,90,hit.GetCrystalId(),
			1000,0,4000,hit.GetDoppler(BETA,&TV90));

      histname = "XtalVectSummary_180";
      obj.FillHistogram(dirname,histname,
			70,20,90,hit.GetCrystalId(),
			1000,0,4000,hit.GetDoppler(BETA,&TV180));

      histname = "XtalVectSummary_270";
      obj.FillHistogram(dirname,histname,
			70,20,90,hit.GetCrystalId(),
			1000,0,4000,hit.GetDoppler(BETA,&TV270));


      histname = "XtalVectSummary_WithTC";
      obj.FillHistogram(dirname,histname,
			70,20,90,hit.GetCrystalId(),
			1000,0,4000,hit.GetDoppler(BETA,&trackvect_WithTC));

      histname = "XtalVectSummary_WithNegTC";
      obj.FillHistogram(dirname,histname,
			70,20,90,hit.GetCrystalId(),
			1000,0,4000,hit.GetDoppler(BETA,&trackvect_WithNegTC));


      if(FWDet){
	histname = "QuadVectSummary_FWRing";
	obj.FillHistogram(dirname,histname,
			  10,0,10,SummarySlot,
			  2000,0,4000,hit.GetDoppler(BETA,&trackvect));

	histname = "GretinaVectorVsDTA_FWRing";
	obj.FillHistogram(dirname,histname,
			  1000,-0.06,0.02,s800->GetDta(),
			  2000,0,4000,hit.GetDoppler(BETA,&trackvect));

      } else{
	histname = "QuadVectSummary_90DegRing";
	obj.FillHistogram(dirname,histname,
			  10,0,10,SummarySlot,
			  2000,0,4000,hit.GetDoppler(BETA,&trackvect));

	histname = "GretinaVectorVsDTA_90DegRing";
	obj.FillHistogram(dirname,histname,
			  1000,-0.06,0.02,s800->GetDta(),
			  2000,0,4000,hit.GetDoppler(BETA,&trackvect));
      }


      for(unsigned int z3=y+1;z3<gretina->Size();z3++) {
	TGretinaHit hit_3 = gretina->GetGretinaHit(z3);

	histname = "Gamma_Gamma";
	obj.FillHistogram(dirname,histname,
			  3000,0,6000,hit.GetDoppler(BETA,&trackvect),
			  3000,0,6000,hit_3.GetDoppler(BETA,&trackvect));
	obj.FillHistogram(dirname,histname,
			  3000,0,6000,hit_3.GetDoppler(BETA,&trackvect),
			  3000,0,6000,hit.GetDoppler(BETA,&trackvect));

	histname = "Gamma_Gamma_Time";
	obj.FillHistogram(dirname,histname,
			  800,-400,400,hit.GetTime()-hit_3.GetTime(),
			  3000,0,6000,hit.GetDoppler(BETA,&trackvect));
	obj.FillHistogram(dirname,histname,
			  800,-400,400,hit_3.GetTime()-hit.GetTime(),
			  3000,0,6000,hit.GetDoppler(BETA,&trackvect));
      }

      //--------------------------------------------------------------------

      dirname = "InverseMap";
      histname = "S800_YTA";
      obj.FillHistogram(dirname,histname,
			1000,-0.2,0.2,s800->GetYta());

      histname = "S800_DTA";
      obj.FillHistogram(dirname,histname,
			1000,-0.2,0.2,s800->GetDta());

      histname = "ATA_vs_BTA";
      obj.FillHistogram(dirname,histname,
			1000,-0.2,0.2,s800->GetAta(),
			1000,-0.2,0.2,s800->GetBta());

      //--------------------------------------------------------------------
      dirname = "PID";
      if(haspids) {
	dirname = "PID_GATED";
        TIter it(gates);
        while(TObject *itobj = it.Next()) {
          if(!itobj->InheritsFrom(TCutG::Class()))
            continue;
          TCutG *mypid = (TCutG*)itobj;
          if(mypid->IsInside(AllCorrectedOBJ_E1Val,s800->GetIonChamber().Charge())) {
          }// end inside PID for Residues
	  if(mypid->IsInside(AllCorrOBJ_E1Val,AllCorrXFP_E1Val)){
	  }
	  if(mypid->IsInside(AllCorrectedOBJ_E1Val,
			     s800->GetIonChamber().GetdECorr(&(s800->GetCrdc())))){
	    histname = Form("GammaDoppler_Beta_%s",mypid->GetName());
	    obj.FillHistogram(dirname,histname,
			      2000,0,4000,hit.GetDoppler(BETA));
	    for(unsigned int z2_pid=y+1;z2_pid<gretina->Size();z2_pid++) {
	      TGretinaHit hit_2_pid = gretina->GetGretinaHit(z2_pid);

	      histname = Form("Gamma_Gamma_Beta_%s",mypid->GetName());
	      obj.FillHistogram(dirname,histname,
				3000,0,6000,hit.GetDoppler(BETA),
				3000,0,6000,hit_2_pid.GetDoppler(BETA));
	      obj.FillHistogram(dirname,histname,
				3000,0,6000,hit_2_pid.GetDoppler(BETA),
				3000,0,6000,hit.GetDoppler(BETA));
	    }

	  }

	  // end inside PID for Incoming PID
	  // if(mypid->IsInside(AllCorrectedOBJ_E1Val,s800->GetCrdc(0).GetDispersiveX())){
	  //   dirname = Form("TOF_DispX_%s",mypid->GetName());
	  //   histname = Form("GretinaDoppler_%s",mypid->GetName());
	  //   obj.FillHistogram(dirname,histname,
	  // 		      3000,0,6000,hit.GetDoppler(BETA));

	  //   if(gretina->Size()==1){
	  //     histname = Form("Singles_GretinaDoppler_%s",mypid->GetName());
	  //     obj.FillHistogram(dirname,histname,
	  // 			3000,0,6000,hit.GetDoppler(BETA));
	  //   }


	  //   histname = Form("GretinaDopplerVsOpenAng_%s",mypid->GetName());
	  //   obj.FillHistogram(dirname,histname,
	  // 		      1000,-3.15,3.15,openang,
	  // 		      2000,0,4000,hit.GetDoppler(BETA));

	  //   histname = Form("GretinaDopplerVsDTA_%s",mypid->GetName());
	  //   obj.FillHistogram(dirname,histname,
	  // 		      1000,-0.2,0.2,s800->GetDta(),
	  // 		      2000,0,4000,hit.GetDoppler(BETA));

	  //   histname = Form("QuadDoppSummary_%s",mypid->GetName());
	  //   obj.FillHistogram(dirname,histname,
	  // 		      10,0,10,HoleQMap[hit.GetCrystalId()/4],
	  // 		      2000,0,4000,hit.GetDoppler(BETA));

	  //   for(int z2=y+1;z2<gretina->Size();z2++) {
	  //     TGretinaHit hit_2 = gretina->GetGretinaHit(z2);

	  //     histname = Form("Gamma_Gamma_%s",mypid->GetName());
	  //     obj.FillHistogram(dirname,histname,
	  // 			3000,0,6000,hit.GetDoppler(BETA),
	  // 			3000,0,6000,hit_2.GetDoppler(BETA));
	  //     obj.FillHistogram(dirname,histname,
	  // 			3000,0,6000,hit_2.GetDoppler(BETA),
	  // 			3000,0,6000,hit.GetDoppler(BETA));

	  //     histname = Form("Gamma_Gamma_Time_%s",mypid->GetName());
	  //     obj.FillHistogram(dirname,histname,
	  // 			800,-400,400,hit.GetTime()-hit_2.GetTime(),
	  // 			3000,0,6000,hit.GetDoppler(BETA));
	  //     obj.FillHistogram(dirname,histname,
	  // 			800,-400,400,hit_2.GetTime()-hit.GetTime(),
	  // 			3000,0,6000,hit.GetDoppler(BETA));
	  //   }

	  // }// end inside PID for Disp X
	  // if(mypid->IsInside(bank29->Timestamp()-hit.GetTime(),hit.GetCoreEnergy())){// Note GetTime has walk correction.
	  //   dirname = Form("Prompt_Timing_%s",mypid->GetName());
	  //   histname = Form("GretinaDoppler_%s",mypid->GetName());
	  //   obj.FillHistogram(dirname,histname,
	  // 		      3000,0,6000,hit.GetDoppler(BETA));

	  //   if(gretina->Size()==1){
	  //     histname = Form("Singles_GretinaDoppler_%s",mypid->GetName());
	  //     obj.FillHistogram(dirname,histname,
	  // 			3000,0,6000,hit.GetDoppler(BETA));
	  //   }


	  //   histname = Form("GretinaDopplerVsOpenAng_%s",mypid->GetName());
	  //   obj.FillHistogram(dirname,histname,
	  // 		      1000,-3.15,3.15,openang,
	  // 		      2000,0,4000,hit.GetDoppler(BETA));

	  //   histname = Form("GretinaDopplerVsDTA_%s",mypid->GetName());
	  //   obj.FillHistogram(dirname,histname,
	  // 		      1000,-0.2,0.2,s800->GetDta(),
	  // 		      2000,0,4000,hit.GetDoppler(BETA));

	  //   histname = Form("QuadDoppSummary_%s",mypid->GetName());
	  //   obj.FillHistogram(dirname,histname,
	  // 		      10,0,10,HoleQMap[hit.GetCrystalId()/4],
	  // 		      2000,0,4000,hit.GetDoppler(BETA));

	  //   for(int z2=y+1;z2<gretina->Size();z2++) {
	  //     TGretinaHit hit_2 = gretina->GetGretinaHit(z2);

	  //     histname = Form("Gamma_Gamma_%s",mypid->GetName());
	  //     obj.FillHistogram(dirname,histname,
	  // 			3000,0,6000,hit.GetDoppler(BETA),
	  // 			3000,0,6000,hit_2.GetDoppler(BETA));
	  //     obj.FillHistogram(dirname,histname,
	  // 			3000,0,6000,hit_2.GetDoppler(BETA),
	  // 			3000,0,6000,hit.GetDoppler(BETA));

	  //     histname = Form("Gamma_Gamma_Time_%s",mypid->GetName());
	  //     obj.FillHistogram(dirname,histname,
	  // 			800,-400,400,hit.GetTime()-hit_2.GetTime(),
	  // 			3000,0,6000,hit.GetDoppler(BETA));
	  //     obj.FillHistogram(dirname,histname,
	  // 			800,-400,400,hit_2.GetTime()-hit.GetTime(),
	  // 			3000,0,6000,hit.GetDoppler(BETA));
	  //   }
	  // }
	  // end insided prompt timing peak
        }// end iterate over cuts.
      } // end "HAS PIDS"

      dirname  = "CRDC";
      histname = "CRDC1_X";
      obj.FillHistogram(dirname,histname,
			800,-400,400,s800->GetCrdc(0).GetDispersiveX());

      histname = "CRDC2_X";
      obj.FillHistogram(dirname,histname,
			800,-400,400,s800->GetCrdc(1).GetDispersiveX());

      histname = "CRDC1_Y_vs_S800Timestamp";
      obj.FillHistogram(dirname,histname,
			10000,0,4000,s800->GetTimestamp()/1e8,
			800,-400,400,s800->GetCrdc(0).GetNonDispersiveY());

      histname = "CRDC2_Y_vs_S800Timestamp";
      obj.FillHistogram(dirname,histname,
			10000,0,4000,s800->GetTimestamp()/1e8,
			800,-400,400,s800->GetCrdc(1).GetNonDispersiveY());
    }// end good correlated events.

    for(int z=0;z<hit.NumberOfInteractions();z++) {
      dirname  = "GretSummCrystal";
      histname = Form("GretinaSummaryX%02i",hit.GetCrystalId());
      obj.FillHistogram(dirname,histname,
			40,0,40,hit.GetSegmentId(z),
			2000,0,4000,hit.GetSegmentEng(z));
      if(hit.GetSegmentId(z)>36)
        break;
      int layer  = hit.GetSegmentId(z)/6;
      dirname = "GretSummPos";
      histname = Form("GretinaPosition_%s",LayerMap[layer].c_str());
      double phi = hit.GetInteractionPosition(z).Phi();
      if(phi<0) phi +=TMath::Pi()*2;
      obj.FillHistogram(dirname,histname,
			628,0.0,6.28,phi,
			314,0,3.14,hit.GetInteractionPosition(z).Theta());
    }
    //std::cout << " In gret loop after number interactions loop" << std::endl;
    dirname = "GretSummPos";
    histname = Form("GretinaPosition");
    obj.FillHistogram(dirname,histname,
		      628,0,6.28,hit.GetPhi(),
		      314,0,3.14,hit.GetTheta());

    histname = Form("GretinaPosition_Quad_%i",HoleQMap[hit.GetCrystalId()/4]);
    obj.FillHistogram(dirname,histname,
		       628,0,6.28,hit.GetPhi(),
		       314,0,3.14,hit.GetTheta());

    histname = Form("GretinaPosition_Xtal_%i",hit.GetCrystalId());
    obj.FillHistogram(dirname,histname,
		       628,0,6.28,hit.GetPhi(),
		       314,0,3.14,hit.GetTheta());




    histname = "GretinaEnergyTotal";
    obj.FillHistogram(histname,
		      8000,0,4000,hit.GetCoreEnergy());
    //--------------------------------------------------------------------
    dirname = "DopplerGretResDiag";
    histname = "GretinaDopplerTotal";
    obj.FillHistogram(dirname,histname,
		      2000,0,4000,hit.GetDoppler(BETA));

    double openang_Spec = hit.GetPosition().Phi()-trackvect_Spec.Phi();

    histname = "GretinaDopplerVsOpenAng";
    obj.FillHistogram(dirname,histname,
		      1000,-3.15,3.15,openang_Spec,
		      2000,0,4000,hit.GetDoppler(BETA));

    histname = "GretinaDopplerVsDTA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetDta(),
		      2000,0,4000,hit.GetDoppler(BETA));

    histname = "QuadDoppSummary";
    obj.FillHistogram(dirname,histname,
		      10,0,10,HoleQMap[hit.GetCrystalId()/4],
		      2000,0,4000,hit.GetDoppler(BETA));

    histname = "GretinaDopplerBeta";
    double beta = 0.2;
    for(int z=0;z<100;z++) {
      beta += .3/100.0;
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetCoreEnergy(beta),
			101,0.2,0.5,beta);
    }

    //--------------------------------------------------------------------
    dirname = "VectCorrByXtal";
    histname = Form("Gret_VectCorr_Xtal");

    obj.FillHistogram(dirname,histname,
			100,0,100,hit.GetCrystalId(),
			2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));

    //--------------------------------------------------------------------
    dirname  = "VectGretResDiag";
    histname = "GretinaDoppVectTotal";
    obj.FillHistogram(dirname,histname,
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));

    histname = "GretinaDoppVectVsOpenAng";
    obj.FillHistogram(dirname,histname,
		      1000,-3.15,3.15,openang_Spec,
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));

    histname = "QuadDoppVectSummary";
    obj.FillHistogram(dirname,histname,
		      10,0,10,HoleQMap[hit.GetCrystalId()/4],
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));

    histname = "GretinaDoppVectPosLoopZ";
    // double z_ = -5.0;
    // double zdef = GValue::FindValue("GRETINA_Z_OFFSET")->GetValue();
    // for(int z=0;z<100;z++) {
    //   z_ += 10.0/100.0;
    //   GValue::FindValue("GRETINA_Z_OFFSET")->SetValue(z_);
    //   obj.FillHistogram(dirname,histname,
    // 			2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec),
    // 			101,-5.0,5.0,z_);
    // }
    // GValue::FindValue("GRETINA_Z_OFFSET")->SetValue(zdef);
    //--------------------------------------------------------------------
    dirname = "GretSummCoreCharge";
    histname = Form("Q%iCoreCharge",HoleQMap[hit.GetCrystalId()/4]);
    obj.FillHistogram(dirname,histname,
		      16,0,16,4*hit.GetCrystalNumber()+0.,
		      4000,0,32000,((double)hit.GetCoreCharge(0)));
    obj.FillHistogram(dirname,histname,
		      16,0,16,4*hit.GetCrystalNumber()+1.,
		      4000,0,32000,((double)hit.GetCoreCharge(1)));
    obj.FillHistogram(dirname,histname,
		      16,0,16,4*hit.GetCrystalNumber()+2.,
		      4000,0,32000,((double)hit.GetCoreCharge(2)));
    obj.FillHistogram(dirname,histname,
		      16,0,16,4*hit.GetCrystalNumber()+3.,
		      4000,0,32000,((double)hit.GetCoreCharge(3)));
    }// end if(S800)!!!!!!!!
  }// end of everything?
}
