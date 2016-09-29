
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
  TVector3 trackvect_top(0,0,1);
  TVector3 trackvect_Spec(0,0,1);

  double E1_TDC_low  = GValue::Value("E1_TDC_low");
  double E1_TDC_high = GValue::Value("E1_TDC_high");
  double BETA        = GValue::Value("BETA");

  TList *gates = &(obj.GetGates());
  bool haspids = gates->GetSize();
  std::string histname = "";
  std::string dirname  = "";
  //std::cout << " Before Bank29" << std::endl;
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
  //std::cout << " After Bank29 Before bad gretina RETURN" << std::endl;

  if(!gretina || gretina->Size()<1 || gretina->Size()>(7*4))
    return;

  //std::cout << " Passed GRET Check, Before S800-1" << std::endl;
  if(s800) {
    //std::cout << " Before ExitTargetVect_Spec " << std::endl;
    trackvect_Spec=s800->ExitTargetVect(6);

    dirname = "S800";
    histname = "S800_DTA";
    //std::cout << " Before first DTA " << std::endl;
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetDta());
    histname = "S800_ATA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetAta());
    histname = "S800_BTA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetBta());
    histname = "S800_YTA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetYta());
    histname = "ATA_vs_BTA_Spec";
    obj.FillHistogram(dirname,histname,
		      300,-0.2,0.2,s800->GetAta(),
		      300,-0.2,0.2,s800->GetBta());

    dirname = "";
  }

  //std::cout << " After S800-1" << std::endl;

  //if(s800) {
    //std::string histname = "S800_DTA";
    //obj.FillHistogram(histname,200,-10,10,s800->GetDta());

    //histname = "S800_YTA";
    //obj.FillHistogram(histname,200,-10,10,s800->GetYta());

  //}

  double gsum = 0.0;
  for(unsigned int y=0;y<gretina->Size();y++) {
    TGretinaHit hit = gretina->GetGretinaHit(y);
    gsum += hit.GetCoreEnergy();

    for(unsigned int z=y+1;z<gretina->Size();z++) {
      TGretinaHit hit2 = gretina->GetGretinaHit(z);
      histname = "Gamma_Gamma";
      obj.FillHistogramSym(histname,2000,0,4000,hit.GetCoreEnergy(),
                                    2000,0,4000,hit2.GetCoreEnergy());
      histname = "Gamma_Gamma_Time";
      obj.FillHistogram(histname,800,-400,400,hit2.GetTime()-hit.GetTime(),
                                 2000,0,4000,hit2.GetCoreEnergy());
      obj.FillHistogram(histname,800,-400,400,hit.GetTime()-hit2.GetTime(),
                                  2000,0,4000,hit.GetCoreEnergy());
    }
    //std::cout << " After gretina hit2 loop" << std::endl;
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
    //std::cout << " In gret loop after bank29" << std::endl;
    if(s800) {
      TVector3 trackvect=s800->ExitTargetVect();
      TVector3 zAxis(0,0,1);
      TVector3 NormResPlane = trackvect.Cross(zAxis);
      TVector3 NormGammaPlane = hit.GetPosition().Cross(zAxis);
      TVector3 NormDiff = NormResPlane-NormGammaPlane;
      TVector3 NormCrossNorm = NormResPlane.Cross(NormGammaPlane);
      double openang = NormResPlane.Angle(NormGammaPlane);

      double openang_Cos = TMath::ACos(NormResPlane.Dot(NormGammaPlane)/(NormResPlane.Mag()*NormGammaPlane.Mag()));
      if(NormResPlane.Dot(NormGammaPlane)>0 && (NormResPlane.Mag()*NormGammaPlane.Mag())<0)
	openang_Cos = 2.0*TMath::Pi() - openang_Cos;
      if(NormResPlane.Dot(NormGammaPlane)<0 && (NormResPlane.Mag()*NormGammaPlane.Mag())<0)
	openang_Cos = 2.0*TMath::Pi() - openang_Cos;

      double openang_Dirk = (2.0*TMath::Pi() - s800->Azita())  - TMath::Abs(hit.GetPosition().Phi());

      if(openang_Dirk<0) openang_Dirk += 2.0*TMath::Pi();
      dirname = "Checking_OpenAngCalc";
      histname = "GretinaDopplerVsOpenAng_CheckCalc";
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler(BETA),
			1000,0,6.3,openang);

      histname = "GretinaDopplerVsOpenAng_CalcUsingCos";
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler(BETA),
			2000,-6.3,6.3,openang_Cos);

      histname = "GretinaDopplerVspenAng_Dirk";
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler(BETA),
			1000,0,6.3,openang_Dirk);


      histname = "GretinaDopplerVsOpenAng_TrackCorr_CheckCalc";
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler(BETA,&trackvect),
			1000,0,6.3,openang);

      histname = "GretinaDopplerVsOpenAng_TrackCorr_CalcUsingCos";
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler(BETA,&trackvect),
			2000,-6.3,6.3,openang_Cos);

      histname = "GretinaDopplerVspenAng_TrackCorr_Dirk";
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler(BETA,&trackvect),
			1000,0,6.3,openang_Dirk);

      histname = Form("GretinaVecCorr");
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler(BETA,&trackvect));


      histname = Form("GretinaDopplerCorr");
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler(BETA));





      dirname = "GretSummTime";
      histname = "Gretina_S800_time";
      obj.FillHistogram(dirname,histname,
			1200,-600,600,s800->Timestamp()-hit.GetTimestamp(),
			2000,0,4000,hit.GetCoreEnergy());
      histname = "Gretina_t0_S800_time";
      obj.FillHistogram(dirname,histname,
			1200,-600,600,s800->Timestamp()-hit.GetTime(),
			2000,0,4000,hit.GetCoreEnergy());

      dirname = "PID";
      histname = "TDC_vs_DispX";
      obj.FillHistogram(dirname,histname,
			4000,-4000,4000,s800->GetCorrTOF_OBJ(),
			600,-300,300,s800->GetCrdc(0).GetDispersiveX());

      histname = "TDC_vs_AFP";
      obj.FillHistogram(dirname,histname,
			4000,-4000,4000,s800->GetCorrTOF_OBJ(),
			600,-0.1,0.1,s800->GetAFP());

      histname ="PID_TDC";
      double low = GValue::Value("PID_LOW");
      double high = GValue::Value("PID_HIGH");
      obj.FillHistogram(dirname,histname,
			1000,-1000,000,s800->GetCorrTOF_OBJ(),
			1000,low,high,s800->GetIonChamber().Charge());

      double delta_t = s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ();
      if(delta_t>E1_TDC_low && delta_t<E1_TDC_high){
        histname ="PID_TDC_Prompt";
        obj.FillHistogram(dirname,histname,
			  8000,-4000,4000,s800->GetCorrTOF_OBJ(),
			  2000,10000,60000,s800->GetIonChamber().Charge());
      }

      //std::cout << " In gret loop + s800 before haspids" << std::endl;
      if(haspids) {
	dirname = "PID_GATED";
        TIter it(gates);
        while(TObject *itobj = it.Next()) {
          if(!itobj)
            continue;

          if(!itobj->InheritsFrom(TCutG::Class()))
            continue;
          TCutG *mypid = (TCutG*)itobj;
          if(mypid->IsInside(s800->GetCorrTOF_OBJ(),s800->GetIonChamber().Charge())) {
	    TVector3 trackvect=s800->ExitTargetVect();

	    histname = Form("GretinaDopplerBeta_%s",mypid->GetName());
	    double _beta = 0.2;
	    for(int z=0;z<100;z++){
	      _beta+=0.3/100.0;
	      obj.FillHistogram(dirname,histname,
				101,0.2,0.5,_beta,
				2000,0,4000,hit.GetDoppler(_beta));
	    }
	    //-----------------------------------------------------
	    histname = Form("GretinaDoppler_%s",mypid->GetName());
	    obj.FillHistogram(dirname,histname,
			      2000,0,4000,hit.GetDoppler(BETA));

	    histname = Form("GretinaDoppVsDTA_%s",mypid->GetName());
	    obj.FillHistogram(dirname,histname,
			      1000,-0.3,0.3,s800->GetDta(),
			      1000,0,4000,hit.GetDoppler(BETA));
	    //-----------------------------------------------------
	    histname = Form("GretinaVecCorr_%s",mypid->GetName());
	    obj.FillHistogram(dirname,histname,
			      2000,0,4000,hit.GetDoppler(BETA,&trackvect));
	    histname = Form("GretinaVecCorrVsDTA_%s",mypid->GetName());
	    obj.FillHistogram(dirname,histname,
			      1000,-0.3,0.3,s800->GetDta(),
			      1000,0,4000,hit.GetDoppler(BETA,&trackvect));
	    //-----------------------------------------------------
	    histname = Form("Gretina_dB_%s",mypid->GetName());
	    obj.FillHistogram(dirname,histname,
			      2000,0,4000,hit.GetDoppler_dB(BETA,&trackvect,s800->GetDta()));

	    histname = Form("Gretina_dB_VsDTA_%s",mypid->GetName());
	    obj.FillHistogram(dirname,histname,
			      1000,-0.3,0.3,s800->GetDta(),
			      1000,0,4000,hit.GetDoppler_dB(BETA,&trackvect,s800->GetDta()));
          }
        }
      }
      //std::cout << " In gret loop + s800 before haspids" << std::endl;
      histname = "E1_m_TDC";
      obj.FillHistogram(histname,8000,-8000,8000,s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ());

      histname = "E1Raw";
      obj.FillHistogram(histname,2000,0,8000,s800->GetScint().GetTimeUp());

      histname = "M_E1Raw";
      for(int z=0;z<s800->GetMTof().E1UpSize();z++)
        obj.FillHistogram(histname,2000,0,8000,s800->GetMTof().fE1Up[z]);

      histname = "CRDC1_X";
      dirname  = "CRDC";
      obj.FillHistogram(dirname,histname,800,-400,400,s800->GetCrdc(0).GetDispersiveX());

      histname = "CRDC2_X";
      obj.FillHistogram(dirname,histname,800,-400,400,s800->GetCrdc(1).GetDispersiveX());

      histname = "IonChamberSum";
      obj.FillHistogram(histname,4000,0,32000,s800->GetIonChamber().GetSum());
    }
    //std::cout << " In gret loop after s800" << std::endl;
    histname = Form("GretinaEnergyTheta");
    obj.FillHistogram(histname,4000,0,4000,hit.GetCoreEnergy(),
                               314,0,3.14,hit.GetTheta());
    //if(hit.GetPad()==0) {
    //  histname = Form("GretinaOverview_pad0");
    //  obj.FillHistogram(histname,4000,0,4000,hit.GetCoreEnergy(),
    //                             100,0,100,hit.GetCrystalId());
    //}
    histname = Form("GretinaOverview");
    obj.FillHistogram(histname,4000,0,4000,hit.GetCoreEnergy(),
                               100,0,100,hit.GetCrystalId());
    
    histname = Form("GretinaSum");
    obj.FillHistogram(histname,10000,0,10000,hit.GetCoreEnergy());

    histname = Form("GretinaDecompErrorCode");
    obj.FillHistogram(histname,150,0,150,hit.GetPad());       

    //std::cout << " In gret loop before Number of interactions loop" << std::endl;
    //
    //histname = Form("Q%i_detmap",HoleQMap[hit.GetCrystalId()/4]);
    //obj.FillHistogram(histname,160,0,160,hit.GetCrystalId()%4+9,
    //                           4000,0,4000,hit.GetCoreEnergy());
    
    
    for(int z=0;z<hit.NumberOfInteractions();z++) {
      dirname  = "GretSummCrystal";
      //if(hit.GetPad()==0) {
      //  histname = Form("GretinaSummaryX%02i_pad0",hit.GetCrystalId());
      //  obj.FillHistogram(dirname,histname,
   //			40,0,40,hit.GetSegmentId(z),
//			2000,0,4000,hit.GetSegmentEng(z));
      //}
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
      if(hit.GetPad()==0) {
        obj.FillHistogram(dirname,histname,
                          628,0.0,6.28,phi,
			  314,0,3.14,hit.GetInteractionPosition(z).Theta());
      }
    }
    //std::cout << " In gret loop after number interactions loop" << std::endl;
    dirname = "GretSummPos";
    if(hit.GetPad()==0) {
    histname = Form("GretinaPosition");
      obj.FillHistogram(dirname,histname,
    		        628,0,6.28,hit.GetPhi(),
 		        314,0,3.14,hit.GetTheta());
    }
    histname = "GretinaEnergyTotal";
    obj.FillHistogram(histname,
		      8000,0,4000,hit.GetCoreEnergy());
    //std::cout << "After GretEnergyTotal " << std::endl;
    //--------------------------------------------------------------------
    dirname = "DopplerGretResDiag";
    histname = "GretinaDopplerTotal";
    obj.FillHistogram(dirname,histname,
		      2000,0,4000,hit.GetDoppler(BETA));

    //std::cout << " Before Opening angle calculation " << std::endl;
    double openang = hit.GetPosition().Phi()-trackvect_top.Phi();
    double openang_Spec = hit.GetPosition().Phi()-trackvect_Spec.Phi();

    histname = "GretinaDopplerVsOpenAng";
    obj.FillHistogram(histname,
		      1000,-3.15,3.15,openang,
		      2000,0,4000,hit.GetDoppler(BETA));
    if(s800){
    histname = "GretinaDopplerVsDTA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetDta(),
		      2000,0,4000,hit.GetDoppler(BETA));

    }

    histname = "QuadDoppSummary";
    obj.FillHistogram(dirname,histname,
		      10,0,10,HoleQMap[hit.GetCrystalId()/4],
		      2000,0,4000,hit.GetDoppler(BETA));

    histname = "GretinaDopplerBeta";
    double beta = 0.00;
    for(int z=0;z<100;z++) {
      beta += .3/100.0;
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetCoreEnergy(beta),
			101,0.2,0.5,beta);
    }
    //std::cout << " After Beta stuff" << std::endl;

    //--------------------------------------------------------------------
    dirname = "VectCorrByXtal";
    histname = Form("Gret_VectCorr_Xtal");

    obj.FillHistogram(dirname,histname,
			100,0,100,hit.GetCrystalId(),
			2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));

    //std::cout << "After vector correction by crystal" << std::endl;
    //--------------------------------------------------------------------
    dirname  = "VectGretResDiag";
    histname = "GretinaDoppVectTotal";
    obj.FillHistogram(dirname,histname,
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_top));

    //std::cout << "After total (1d) plot made, before vs opening angle" << std::endl;
    histname = "GretinaDoppVectVsOpenAng";
    obj.FillHistogram(dirname,histname,
		      1000,-3.15,3.15,openang,
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_top));

    histname = "GretinaDoppVectTotal_Spec";
    obj.FillHistogram(dirname,histname,
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));

    //std::cout << "After one done with Spec" << std::endl;
    histname = "GretinaDoppVectVsOpenAng_Spec";
    obj.FillHistogram(dirname,histname,
		      1000,-3.15,3.15,openang_Spec,
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));

    if(s800){
    //std::cout << " Before dopp vs DTA" << std::endl;
    histname = "GretinaDoppVectVsDTA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetDta(),
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));
    }
    //std::cout << " After dopp vs DTA" << std::endl;
    histname = "QuadDoppVectSummary";
    obj.FillHistogram(dirname,histname,
		      10,0,10,HoleQMap[hit.GetCrystalId()/4],
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));
    //std::cout << "Before loop over z value" << std::endl;
    histname = "GretinaDoppVectPosLoopZ";
    double z_ = -5.0;
    //std::cout << "Find val : " << GValue::Value("GRETINA_Z_OFFSET") << std::endl;
    double zdef = GValue::Value("GRETINA_Z_OFFSET");
    //std::cout << "zdef = " << zdef << std::endl;
    if(!(std::isnan(zdef))){
      //std::cout << " In isnan condition" << std::endl;
      for(int z=0;z<100;z++) {
	z_ += 10.0/100.0;
	GValue::FindValue("GRETINA_Z_OFFSET")->SetValue(z_);
	obj.FillHistogram(dirname,histname,
			  2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec),
			  101,-5.0,5.0,z_);
      }
      GValue::FindValue("GRETINA_Z_OFFSET")->SetValue(zdef);
    }
    //std::cout << " After gretina z offset gvalue stuff" << std::endl;

    /* histname = "GretinaDoppVectPosLoopX";
    double x_ = -5.0;
    double xdef = GValue::FindValue("GRETINA_X_OFFSET")->GetValue();
    for(int z=0;z<100;z++) {
      x_ += 10.0/100.0;
      GValue::FindValue("GRETINA_X_OFFSET")->SetValue(x_);
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec),
			101,-5.0,5.0,x_);
    }
    GValue::FindValue("GRETINA_X_OFFSET")->SetValue(xdef);


    histname = "GretinaDoppVectPosLoopY";
    double y_ = -5.0;
    double ydef = GValue::FindValue("GRETINA_Y_OFFSET")->GetValue();
    for(int z=0;z<100;z++) {
      y_ += 10.0/100.0;
      GValue::FindValue("GRETINA_Y_OFFSET")->SetValue(y_);
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec),
			101,-5.0,5.0,y_);
    }
    GValue::FindValue("GRETINA_Y_OFFSET")->SetValue(ydef);*/
    //--------------------------------------------------------------------
    if(s800){
      dirname  = "MomentumGretResDiag_Spec";
      histname = "GretinaDoppMomTotal";
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler_dB(BETA,&trackvect_Spec,s800->GetDta()));

      histname = "GretinaDoppMomVsOpenAng";
      obj.FillHistogram(dirname,histname,
			1000,-3.15,3.15,openang_Spec,
			2000,0,4000,hit.GetDoppler_dB(BETA,&trackvect_Spec,s800->GetDta()));

      histname = "GretinaDoppMomVsDTA";
      obj.FillHistogram(dirname,histname,
			1000,-0.2,0.2,s800->GetDta(),
			2000,0,4000,hit.GetDoppler_dB(BETA,&trackvect_Spec,s800->GetDta()));

      histname = "QuadDoppMomSummary";
      obj.FillHistogram(dirname,histname,
			10,0,10,HoleQMap[hit.GetCrystalId()/4],
			2000,0,4000,hit.GetDoppler_dB(BETA,&trackvect_Spec,s800->GetDta()));
    }
    //std::cout << "After momentum gretina resolution diagnosis" << std::endl;
    //====================================================================
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

    //std::cout << "After gretina core charge summary" << std::endl;
    /*    histname = "GretinaDopplerBeta";
    double beta = 0.2;
    for(int z=0;z<100;z++) {
      beta += .3/100.0;
      obj.FillHistogram(histname,8000,0,4000,hit.GetCoreEnergy(beta),
                                 101,0.2,0.5,beta);
				 }*/

    histname = Form("Q%iCoreCharge",HoleQMap[hit.GetCrystalId()/4]);
    //if(HoleQMap[hit.GetHoleNumber()]==0)
    //  printf("HoleQMap[hit.GetHoleNumber() = 0 is %i\n",hit.GetHoleNumber());
    obj.FillHistogram(histname,16,0,16,4*hit.GetCrystalNumber()+0.,
                               4000,0,32000,((double)hit.GetCoreCharge(0)));
    obj.FillHistogram(histname,16,0,16,4*hit.GetCrystalNumber()+1.,
                               4000,0,32000,((double)hit.GetCoreCharge(1)));
    obj.FillHistogram(histname,16,0,16,4*hit.GetCrystalNumber()+2.,
                               4000,0,32000,((double)hit.GetCoreCharge(2)));
    obj.FillHistogram(histname,16,0,16,4*hit.GetCrystalNumber()+3.,
                               4000,0,32000,((double)hit.GetCoreCharge(3)));

    histname = "CrystalId_hitpattern";
    obj.FillHistogram(histname,100,0,100,hit.GetCrystalId());
  }
  //std::cout << " After gret loop " << std::endl;
  if(gsum>1.0) {

    histname = "GCalorimeter";
    obj.FillHistogram(histname,16000,0,8000,gsum);
    /* NOTE -> largesthit is not set anywhere
    TGretinaHit hit = gretina->GetGretinaHit(largesthit);
    histname = "GCalorimeter_Theta";
    obj.FillHistogram(histname,314,0,3.14,hit.GetTheta(),
    8000,0,8000,gsum);*/
  }
  //std::cout << " After Gret Calorimeter" << std::endl;
  if(numobj!=list->GetSize())
    list->Sort();
  //std::cout << " end" << std::endl;
}
