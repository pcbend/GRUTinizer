#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
//
// Note: from TRuntimeObjects:
//    TList& GetDetectors();
//    TList& GetObjects();
//

#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include "TRandom.h"

#include "TObject.h"
#include "TGretina.h"
#include "TS800.h"
#include "TBank29.h"

#define Q1 15
#define Q2 7
#define Q3 8
#define Q4 16
#define Q5 9
#define Q6 14
#define Q7 17
#define Q8 6





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

  LayerMap[0] = "alpha";
  LayerMap[1] = "beta";
  LayerMap[2] = "gamma";
  LayerMap[3] = "delta";
  LayerMap[4] = "epsilon";
  LayerMap[5] = "phi";

}

#define INTEGRATION 128.0

TH1 *GetHistogram(TList *list, std::string histname,int xbins,double xlow,double xhigh) {
  //TList *list = &(obj.GetObjects());
  TH1   *hist = (TH1*)list->FindObject(histname.c_str());
  if(!hist) {
    hist= new TH1I(histname.c_str(),histname.c_str(),xbins,xlow,xhigh);
    list->Add(hist);
  }
  return hist; 
}

TH2 *GetMatrix(TList *list, std::string histname,int xbins, double xlow,double xhigh,
                                                 int ybins, double ylow,double yhigh) {
  //TList *list = &(obj.GetObjects());
  TH2   *mat  = (TH2*)list->FindObject(histname.c_str());
  if(!mat) {
    mat = new TH2I(histname.c_str(),histname.c_str(),xbins,xlow,xhigh,
                                                     ybins,ylow,yhigh);
    list->Add(mat);
  }
  return mat; 
}


extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  //printf("I am Here 1\n"); fflush(stdout);
  InitMap();
  TGretina *gretina = 0;
  TBank29  *bank29  = 0;
  TS800    *s800    = 0;
  TIter iter(&obj.GetDetectors());
  while(TObject *object = iter.Next()) {
    if(object->InheritsFrom(TGretina::Class()))
        gretina = (TGretina*)object;
    if(object->InheritsFrom(TS800::Class()))
        s800 = (TS800*)object;
    if(object->InheritsFrom(TBank29::Class()))
        bank29 = (TBank29*)object;
  }

  TList *list = &(obj.GetObjects());

  

  //if(cut) cut->Print();
  double MAFP_COEF = obj.GetVariable("MAFP_COEF");
  double MCRDCX_COEF = obj.GetVariable("MCRDCX_COEF");
  double AFP_COEF = obj.GetVariable("AFP_COEF");
  double CRDCX_COEF = obj.GetVariable("CRDCX_COEF");
  double BETA = obj.GetVariable("BETA");
  double E1_TDC_low = obj.GetVariable("E1_TDC_low");
  double E1_TDC_high = obj.GetVariable("E1_TDC_high");
 
  
  int numobj = list->GetSize();

  if(bank29) {
    for(int x=0;x<bank29->Size();x++) {
      TMode3Hit &hit = (TMode3Hit&)bank29->GetHit(x);
      TH1* hist = GetHistogram(list,Form("bank29_%i",hit.GetChannel()),
                               16000,0,64000);
      hist->Fill(hit.Charge());
    }
    if(s800) {
      TH1 *s829time = GetHistogram(list,"S800_Bank29_time",200,-200,200);
      s829time->Fill(bank29->Timestamp()-s800->Timestamp());
    }
  }

  if(!gretina)
    return;

  double gsum = 0.0;
  for(int y=0;y<gretina->Size();y++) {
    TGretinaHit hit = gretina->GetGretinaHit(y);
    std::string histname;
    gsum += hit.GetCoreEnergy();

    for(int z=y+1;z<gretina->Size();z++) {
      TGretinaHit hit2 = gretina->GetGretinaHit(z);
      TH2 *energy_mat = GetMatrix(list,"Gamma_Gamma",2000,0,4000,2000,0,4000);
      energy_mat->Fill(hit.GetCoreEnergy(),hit2.GetCoreEnergy());
      energy_mat->Fill(hit2.GetCoreEnergy(),hit.GetCoreEnergy());
      
      
      TH2 *energy_time = GetMatrix(list,"Gamma_Gamma_Time",800,-400,400,2000,0,4000);
      energy_time->Fill(hit2.GetTime()-hit.GetTime(),hit2.GetCoreEnergy());
      energy_time->Fill(hit.GetTime()-hit2.GetTime(),hit.GetCoreEnergy());
    }

    if(bank29) {
      TH2 *g29time = GetMatrix(list,"Gretina_Bank29_time",1200,-600,600,2000,0,4000);
      g29time->Fill(bank29->Timestamp()-hit.GetTimestamp(),hit.GetCoreEnergy());

      TH2 *gw29time = GetMatrix(list,"Gretina_t0_Bank29_time",1200,-600,600,2000,0,4000);
      gw29time->Fill(bank29->Timestamp()-hit.GetTime(),hit.GetCoreEnergy());
    }
    if(s800) {
      TH2 *g800time = GetMatrix(list,"Gretina_S800_time",1200,-600,600,2000,0,4000);
      g800time->Fill(s800->GetTimestamp()-hit.GetTimestamp(),hit.GetCoreEnergy());
      TH2 *gw800time = GetMatrix(list,"Gretina_t0_S800_time",1200,-600,600,2000,0,4000);
      gw800time->Fill(s800->Timestamp()-hit.GetTime(),hit.GetCoreEnergy());

      TH2 *mtdc_vs_dispx = GetMatrix(list,"MTDC_vs_DispX",1000,-5000,5000,600,-300,300);
      mtdc_vs_dispx->Fill(s800->GetTofE1_MTDC(MAFP_COEF,MCRDCX_COEF),s800->GetCrdc(0).GetDispersiveX());
      TH2 *tdc_vs_dispx = GetMatrix(list,"TDC_vs_DispX",1000,-800,00,600,-300,300);
      tdc_vs_dispx->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetCrdc(0).GetDispersiveX());
      
      
      TH2 *mtdc_vs_afp= GetMatrix(list,"MTDC_vs_AFP",1000,-5000,5000,600,-0.1,0.1);
      mtdc_vs_afp->Fill(s800->GetTofE1_MTDC(MAFP_COEF,MCRDCX_COEF),s800->GetAFP());
      TH2 *tdc_vs_afp= GetMatrix(list,"TDC_vs_AFP",1000,-800,00,600,-0.1,0.1);
      tdc_vs_afp->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetAFP());

      double delta_t = s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ();
      
      TH2 *mtdc_vs_ic= GetMatrix(list,"PID_MTDC",1000,-2000,-1000,1000,0,25000);
      mtdc_vs_ic->Fill(s800->GetTofE1_MTDC(MAFP_COEF,MCRDCX_COEF),s800->GetIonChamber().Charge());
      TH2 *tdc_vs_ic= GetMatrix(list,"PID_TDC",1000,-800,00,1000,0,25000);
      tdc_vs_ic->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetIonChamber().Charge());
      
      if(delta_t>E1_TDC_low && delta_t<E1_TDC_high){
	TH2 *tdc_vs_ic_Prompt= GetMatrix(list,"PID_TDC_Prompt",1000,-800,0,1000,0,25000);
	tdc_vs_ic_Prompt->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetIonChamber().Charge());
      }
      
      TH1 *E1_m_TDC = GetHistogram(list,"E1_m_TDC",8000,-8000,8000);
      E1_m_TDC->Fill(s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ());
      
      TH1 *hist1d = GetHistogram(list,"E1Raw",2000,0,8000);
      hist1d->Fill(s800->GetScint().GetTimeUp());
    }
    
    TH2 *gret_energy_theta = GetMatrix(list,"GretinaEnergyTheta",4000,0,4000,314,0,3.14);
    gret_energy_theta->Fill(hit.GetCoreEnergy(),hit.GetTheta());

    TH2 *gret_summary = GetMatrix(list,"GretinaOverview",4000,0,4000,60,20,80);
    gret_summary->Fill(hit.GetCoreEnergy(),hit.GetCrystalId());

    TH2 *seg_summary = GetMatrix(list,Form("GretinaSummaryX%02i",hit.GetCrystalId()),
                                 40,0,40,2000,0,4000);

   
    
    //seg_summary->Fill(36,hit.GetCoreCharge(0)/INTEGRATION);
    //seg_summary->Fill(37,hit.GetCoreCharge(1)/INTEGRATION);
    //seg_summary->Fill(38,hit.GetCoreCharge(2)/INTEGRATION);
    //seg_summary->Fill(39,hit.GetCoreCharge(3)/INTEGRATION);
    for(int z=0;z<hit.NumberOfInteractions();z++) {
      seg_summary->Fill(hit.GetSegmentId(z),hit.GetSegmentEng(z));
      int layer  = hit.GetSegmentId(z)/6;
      TH2 *position = GetMatrix(list,Form("GretinaPosition_%s",LayerMap[layer].c_str()),628,0,6.28,314,0,3.14);
      double theta = hit.GetInteractionPosition(z).Theta();
      double phi   = hit.GetInteractionPosition(z).Phi();
      if(phi<0)
        phi = TMath::TwoPi()+phi;
      position->Fill(phi,theta);

    }
    //for(int z=0;z<hit.NumberOfInteractions();z++) {
    
    TH2 *total_position = GetMatrix(list,"GretinaPosition",628,0,6.28,314,0,3.14);
    total_position->Fill(hit.GetPhi(),hit.GetTheta());

    TH1 *hist1d = GetHistogram(list,"GretinaEnergySum",8000,0,4000);
    hist1d->Fill(hit.GetCoreEnergy());
   
    hist1d = GetHistogram(list,"GretinaDopplerSum",1000,0,4000);
    hist1d->Fill(hit.GetDoppler(BETA));

    TCutG *blob   = obj.GetCut("blob");
    TCutG *blob_1 = obj.GetCut("blob_1");
    TCutG *blob_2 = obj.GetCut("blob_2");
    //TCutG *blob_Top = obj.GetCut("blob_Top");
    
    double pidx = s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF);
    double pidy = s800->GetIonChamber().Charge();

    if(s800 && blob && blob->IsInside(pidx,pidy)){
      hist1d = GetHistogram(list,"GretinaDopplerSum_blob",1000,0,4000);
      hist1d->Fill(hit.GetDoppler(BETA));
      TH2* doppler_beta = GetMatrix(list,"GretinaDopplerBeta_blob",2000,0,4000,101,.2,.5);
      double beta = 0.2;
      for(int z=0;z<100;z++) {
	beta += .3/100.0;
	doppler_beta->Fill(hit.GetDoppler(beta),beta);
      }
    
    }
    if(s800 && blob_1 && blob_1->IsInside(pidx,pidy)){
      hist1d = GetHistogram(list,"GretinaDopplerSum_blob_1",1000,0,4000);
      hist1d->Fill(hit.GetDoppler(BETA));
      TH2* doppler_beta = GetMatrix(list,"GretinaDopplerBeta_blob_1",2000,0,4000,101,.2,.5);
      double beta = 0.2;
      for(int z=0;z<100;z++) {
	beta += .3/100.0;
	doppler_beta->Fill(hit.GetDoppler(beta),beta);
      }
    }
    if(s800 && blob_2 && blob_2->IsInside(pidx,pidy)){
      hist1d = GetHistogram(list,"GretinaDopplerSum_blob_2",1000,0,4000);
      hist1d->Fill(hit.GetDoppler(BETA));
      TH2* doppler_beta = GetMatrix(list,"GretinaDopplerBeta_blob_2",2000,0,4000,101,.2,.5);
      double beta = 0.2;
      for(int z=0;z<100;z++) {
	beta += .3/100.0;
	doppler_beta->Fill(hit.GetDoppler(beta),beta);
      }
    }
//    if(s800 && blob_Top && blob_Top->IsInside(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetIonChamber().Charge())){
//      hist1d = GetHistogram(list,"GretinaDopplerSum_blobT",1000,0,4000);
//      hist1d->Fill(hit.GetDoppler(BETA));
//      TH2* doppler_beta = GetMatrix(list,"GretinaDopplerBeta_blobT",2000,0,4000,101,.2,.5);
//      double beta = 0.2;
//      for(int z=0;z<100;z++) {
//	beta += .3/100.0;
//	doppler_beta->Fill(hit.GetDoppler(beta),beta);
    //  }
    //}
   
    TH2* doppler_beta = GetMatrix(list,"GretinaDopplerBeta",2000,0,4000,101,.2,.5);
    double beta = 0.2;
    for(int z=0;z<100;z++) {
      beta += .3/100.0;
      doppler_beta->Fill(hit.GetDoppler(beta),beta);
    }

    
    TH2 *qchg = GetMatrix(list,Form("Q%iCoreCharge",HoleQMap[hit.GetCrystalId()/4]),
                          16,0,16,8000,0,32000);
    qchg->Fill(4*hit.GetCrystalNumber()+0.,((double)hit.GetCoreCharge(0)));
    qchg->Fill(4*hit.GetCrystalNumber()+1.,((double)hit.GetCoreCharge(1)));
    qchg->Fill(4*hit.GetCrystalNumber()+2.,((double)hit.GetCoreCharge(2)));
    qchg->Fill(4*hit.GetCrystalNumber()+3.,((double)hit.GetCoreCharge(3)));

    TH1 *seghit = GetHistogram(list,"SegmentId_hitpattern",100,0,100);
    seghit->Fill(hit.GetCrystalId());
  }

  TH1 *gcal = GetHistogram(list,"GCalorimeter",16000,0,8000);
  if(gsum>1.0) {
    gcal->Fill(gsum);
  }

  if(numobj!=list->GetSize())
    list->Sort();
}
