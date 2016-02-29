
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
    for(int x=0;x<bank29->Size();x++) {
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
    trackvect_Spec=s800->ExitTargetVect_Spec(6);
    
    dirname = "S800";
    histname = "S800_DTA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetDta_Spec());
    histname = "S800_ATA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetAta_Spec());
    histname = "S800_BTA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetBta_Spec());
    histname = "S800_YTA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetYta_Spec());
    histname = "ATA_vs_BTA_Spec";
    obj.FillHistogram(dirname,histname,
		      300,-0.2,0.2,s800->GetAta_Spec(),
		      300,-0.2,0.2,s800->GetBta_Spec());
    
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
  for(int y=0;y<gretina->Size();y++) {
    TGretinaHit hit = gretina->GetGretinaHit(y);
    gsum += hit.GetCoreEnergy();

    for(int z=y+1;z<gretina->Size();z++) {
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
      obj.FillHistogram(dirname,histname,
			1000,-1000,000,s800->GetCorrTOF_OBJ(),
			1000,0,40000,s800->GetIonChamber().Charge());
      
      double delta_t = s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ();
      if(delta_t>E1_TDC_low && delta_t<E1_TDC_high){
        histname ="PID_TDC_Prompt";
        obj.FillHistogram(dirname,histname,
			  8000,-4000,4000,s800->GetCorrTOF_OBJ(),
			  2000,0,50000,s800->GetIonChamber().Charge());
      }
      
      //std::cout << " In gret loop + s800 before haspids" << std::endl;
      if(haspids) {
	dirname = "PID_GATED";
        TIter it(gates);
        while(TObject *itobj = it.Next()) {
          if(!itobj->InheritsFrom(TCutG::Class()))
            continue;
          TCutG *mypid = (TCutG*)itobj;
          if(mypid->IsInside(s800->GetCorrTOF_OBJ(),s800->GetIonChamber().Charge())) {
	    TVector3 trackvect=s800->ExitTargetVect_Spec();

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
			      1000,-0.3,0.3,s800->GetDta_Spec(),
			      1000,0,4000,hit.GetDoppler(BETA));
	    //-----------------------------------------------------
	    histname = Form("GretinaVecCorr_%s",mypid->GetName());
	    obj.FillHistogram(dirname,histname,
			      2000,0,4000,hit.GetDoppler(BETA,&trackvect));
	    histname = Form("GretinaVecCorrVsDTA_%s",mypid->GetName());
	    obj.FillHistogram(dirname,histname,
			      1000,-0.3,0.3,s800->GetDta_Spec(),
			      1000,0,4000,hit.GetDoppler(BETA,&trackvect));
	    //-----------------------------------------------------
	    histname = Form("Gretina_dB_%s",mypid->GetName());
	    obj.FillHistogram(dirname,histname,
			      2000,0,4000,hit.GetDoppler_dB(BETA,&trackvect,s800->GetDta_Spec()));

	    histname = Form("Gretina_dB_VsDTA_%s",mypid->GetName());
	    obj.FillHistogram(dirname,histname,
			      1000,-0.3,0.3,s800->GetDta_Spec(),
			      1000,0,4000,hit.GetDoppler_dB(BETA,&trackvect,s800->GetDta_Spec()));
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
    
    histname = Form("GretinaOverview");
    obj.FillHistogram(histname,4000,0,4000,hit.GetCoreEnergy(),
                               100,0,100,hit.GetCrystalId());
    
    //std::cout << " In gret loop before Number of interactions loop" << std::endl;
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

    histname = "GretinaEnergyTotal";
    obj.FillHistogram(histname,
		      8000,0,4000,hit.GetCoreEnergy());   
    //--------------------------------------------------------------------
    dirname = "DopplerGretResDiag";
    histname = "GretinaDopplerTotal";
    obj.FillHistogram(dirname,histname,
		      2000,0,4000,hit.GetDoppler(BETA));

    double openang = hit.GetPosition().Phi()-trackvect_top.Phi();
    double openang_Spec = hit.GetPosition().Phi()-trackvect_Spec.Phi();

    histname = "GretinaDopplerVsOpenAng";
    obj.FillHistogram(dirname,histname,
		      1000,-3.15,3.15,openang,
		      2000,0,4000,hit.GetDoppler(BETA));

    histname = "GretinaDopplerVsDTA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetDta_Spec(),
		      2000,0,4000,hit.GetDoppler(BETA));

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
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_top));

    histname = "GretinaDoppVectVsOpenAng";
    obj.FillHistogram(dirname,histname,
		      1000,-3.15,3.15,openang,
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_top));

    histname = "GretinaDoppVectTotal_Spec";
    obj.FillHistogram(dirname,histname,
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));

    histname = "GretinaDoppVectVsOpenAng_Spec";
    obj.FillHistogram(dirname,histname,
		      1000,-3.15,3.15,openang_Spec,
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));
    
    histname = "GretinaDoppVectVsDTA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetDta_Spec(),
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));
    
    histname = "QuadDoppVectSummary";
    obj.FillHistogram(dirname,histname,
		      10,0,10,HoleQMap[hit.GetCrystalId()/4],
		      2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec));
    
    histname = "GretinaDoppVectPosLoopZ";
    double z_ = -5.0;
    double zdef = GValue::FindValue("GRETINA_Z_OFFSET")->GetValue();
    for(int z=0;z<100;z++) {
      z_ += 10.0/100.0;
      GValue::FindValue("GRETINA_Z_OFFSET")->SetValue(z_);
      obj.FillHistogram(dirname,histname,
			2000,0,4000,hit.GetDoppler(BETA,&trackvect_Spec),
			101,-5.0,5.0,z_);
    }      
    GValue::FindValue("GRETINA_Z_OFFSET")->SetValue(zdef);


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
    dirname  = "MomentumGretResDiag_Spec";
    histname = "GretinaDoppMomTotal";
    obj.FillHistogram(dirname,histname,
		      2000,0,4000,hit.GetDoppler_dB(BETA,&trackvect_Spec,s800->GetDta_Spec()));

    histname = "GretinaDoppMomVsOpenAng";
    obj.FillHistogram(dirname,histname,
		      1000,-3.15,3.15,openang_Spec,
		      2000,0,4000,hit.GetDoppler_dB(BETA,&trackvect_Spec,s800->GetDta_Spec()));

    histname = "GretinaDoppMomVsDTA";
    obj.FillHistogram(dirname,histname,
		      1000,-0.2,0.2,s800->GetDta_Spec(),
		      2000,0,4000,hit.GetDoppler_dB(BETA,&trackvect_Spec,s800->GetDta_Spec()));

    histname = "QuadDoppMomSummary";
    obj.FillHistogram(dirname,histname,
		      10,0,10,HoleQMap[hit.GetCrystalId()/4],
		      2000,0,4000,hit.GetDoppler_dB(BETA,&trackvect_Spec,s800->GetDta_Spec()));

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
