

#include "TRuntimeObjects.h"



#include <iostream>
#include <map>



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

#define BETA .37

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








#define INTEGRATION 128.0


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank29  *bank29  = obj.GetDetector<TBank29>();
  TS800    *s800    = obj.GetDetector<TS800>();

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();


  //double MAFP_COEF = obj.GetVariable("MAFP_COEF");
  //double MCRDCX_COEF = obj.GetVariable("MCRDCX_COEF");
  double AFP_COEF = obj.GetVariable("AFP_COEF");
  double CRDCX_COEF = obj.GetVariable("CRDCX_COEF");
  //double BETA = obj.GetVariable("BETA");
  double E1_TDC_low = obj.GetVariable("E1_TDC_low");
  double E1_TDC_high = obj.GetVariable("E1_TDC_high");



  if(bank29) {
    for(int x=0;x<bank29->Size();x++) {
      TMode3Hit &hit = (TMode3Hit&)bank29->GetHit(x);
      std::string histname = Form("bank29_%i",hit.GetChannel());
      TH1 *hist = (TH1*)list->FindObject(histname.c_str());
      if(!hist) {
        hist= new TH1F(histname.c_str(),histname.c_str(),16000,0,64000);
        list->Add(hist);
      }
      //printf("%i\t\t%i\n",hit.Charge(),hit.Charge()/INTEGRATION);
      //std::cout << "In makehistos "  << hit.Charge() << "\t\t" << hit.GetChannel() << std::endl;
      //hist->Fill(hit.Charge()/INTEGRATION);
      hist->Fill(hit.Charge());
    }
    if(s800) {
      std::string histname = "S800_Bank29_time";
      TH1 *s829time = (TH1*)list->FindObject(histname.c_str());
      if(!s829time) {
        s829time = new TH1F(histname.c_str(),histname.c_str(),200,-200,200);
        list->Add(s829time);
      }
      s829time->Fill(bank29->Timestamp()-s800->Timestamp());
      histname = "Gretina_t0_Bank29_time";
    }
  }





  if(!gretina)
    return;


  double gsum = 0.0;
  for(int y=0;y<gretina->Size();y++) {
    //printf("I am Here %i\n",2+y); fflush(stdout);
    TGretinaHit hit = gretina->GetGretinaHit(y);
    std::string histname;
    gsum += hit.GetCoreEnergy();

    for(int z=y+1;z<gretina->Size();z++) {
      TGretinaHit hit2 = gretina->GetGretinaHit(z);
      histname = "Gamma_Gamma";
      TH2 *energy_mat = (TH2*)list->FindObject(histname.c_str());
      if(!energy_mat) {
        energy_mat = new TH2F(histname.c_str(),histname.c_str(),2000,0,4000,2000,0,4000);
        list->Add(energy_mat);
      }
      energy_mat->Fill(hit.GetCoreEnergy(),hit2.GetCoreEnergy());
      energy_mat->Fill(hit2.GetCoreEnergy(),hit.GetCoreEnergy());
      histname = "Gamma_Gamma_Time";
      TH2 *energy_time = (TH2*)list->FindObject(histname.c_str());
      if(!energy_time) {
        energy_time= new TH2F(histname.c_str(),"delta time vs. energy of low gamma"
                                                ,800,-400,400,2000,0,4000);
        list->Add(energy_time);
      }
      //if(hit.GetCoreEnergy()>hit2.GetCoreEnergy()) {
        energy_time->Fill(hit2.GetTime()-hit.GetTime(),hit2.GetCoreEnergy());
      //} else {
        energy_time->Fill(hit.GetTime()-hit2.GetTime(),hit.GetCoreEnergy());
      //}
    }


    if(bank29) {
      histname = "Gretina_Bank29_time";
      TH2 *g29time = (TH2*)list->FindObject(histname.c_str());
      if(!g29time) {
        g29time = new TH2F(histname.c_str(),histname.c_str(),1200,-600,600,2000,0,4000);
        list->Add(g29time);
      }
      g29time->Fill(bank29->Timestamp()-hit.GetTimestamp(),hit.GetCoreEnergy());
      histname = "Gretina_t0_Bank29_time";
      TH2 *gw29time = (TH2*)list->FindObject(histname.c_str());
      if(!gw29time) {
        gw29time = new TH2F(histname.c_str(),histname.c_str(),1200,-600,600,2000,0,4000);
        list->Add(gw29time);
      }
      //printf("%.02f\t%.02f\n",bank29->Timestamp()-hit.GetTime(),hit.GetCoreEnergy());
      gw29time->Fill(bank29->Timestamp()-hit.GetTime(),hit.GetCoreEnergy());
    }
    if(s800) {
      histname = "Gretina_S800_time";
      TH2 *g800time = (TH2*)list->FindObject(histname.c_str());
      if(!g800time) {
        g800time = new TH2F(histname.c_str(),histname.c_str(),1200,-600,600,2000,0,4000);
        list->Add(g800time);
      }
      g800time->Fill(s800->GetTimestamp()-hit.GetTimestamp(),hit.GetCoreEnergy());
      histname = "Gretina_t0_S800_time";
      TH2 *gw800time = (TH2*)list->FindObject(histname.c_str());
      if(!gw800time) {
        gw800time = new TH2F(histname.c_str(),histname.c_str(),1200,-600,600,2000,0,4000);
        list->Add(gw800time);
      }
      //printf("%.02f\t%.02f\n",s800->Timestamp()-hit.GetTime(),hit.GetCoreEnergy());
      gw800time->Fill(s800->Timestamp()-hit.GetTime(),hit.GetCoreEnergy());
    
    
      //TH2 *mtdc_vs_dispx = GetMatrix(list,"MTDC_vs_DispX",2000,-5000,5000,600,-300,300);
      //mtdc_vs_dispx->Fill(s800->GetTofE1_MTDC(MAFP_COEF,MCRDCX_COEF),s800->GetCrdc(0).GetDispersiveX());
      TH2 *tdc_vs_dispx = GetMatrix(list,"TDC_vs_DispX",2000,-2000,00,600,-300,300);
      tdc_vs_dispx->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetCrdc(0).GetDispersiveX());


      //TH2 *mtdc_vs_afp= GetMatrix(list,"MTDC_vs_AFP",1000,-5000,5000,600,-0.1,0.1);
      //mtdc_vs_afp->Fill(s800->GetTofE1_MTDC(MAFP_COEF,MCRDCX_COEF),s800->GetAFP());
      TH2 *tdc_vs_afp= GetMatrix(list,"TDC_vs_AFP",2000,-2000,00,600,-0.1,0.1);
      tdc_vs_afp->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetAFP());
      





      double delta_t = s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ();

      //TH2 *mtdc_vs_ic= GetMatrix(list,"PID_MTDC",2000,-2000,0,2000,20000,40000);
      //mtdc_vs_ic->Fill(s800->GetTofE1_MTDC(MAFP_COEF,MCRDCX_COEF),s800->GetIonChamber().Charge());
      TH2 *tdc_vs_ic= GetMatrix(list,"PID_TDC",2000,-2000,0,2000,10000,40000);
      tdc_vs_ic->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetIonChamber().Charge());

      if(delta_t>E1_TDC_low && delta_t<E1_TDC_high){
	TH2 *tdc_vs_ic_Prompt= GetMatrix(list,"PID_TDC_Prompt",4000,-8000,8000,2000,20000,40000);
	tdc_vs_ic_Prompt->Fill(s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),s800->GetIonChamber().Charge());
      }

      TH1 *E1_m_TDC = GetHistogram(list,"E1_m_TDC",8000,-8000,8000);
      E1_m_TDC->Fill(s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ());

      TH1 *hist1d = GetHistogram(list,"E1Raw",2000,0,8000);
      hist1d->Fill(s800->GetScint().GetTimeUp());
      
      TH1 *crdc1x = GetHistogram(list,"CRDC1_X",800,-400,400);
      crdc1x->Fill(s800->GetCrdc(0).GetDispersiveX());
      
      TH1 *crdc2x = GetHistogram(list,"CRDC2_X",800,-400,400);
      crdc2x->Fill(s800->GetCrdc(1).GetDispersiveX());
      
      TH1 *ion_sum = GetHistogram(list,"Ion Chamber Sum",8000,0,64000);
      ion_sum->Fill(s800->GetIonChamber().Charge());
    }
    
    


    histname = Form("GretinaEnergyTheta");
    TH2 *gret_energy_theta = (TH2*)list->FindObject(histname.c_str());
    if(!gret_energy_theta) {
      gret_energy_theta = new TH2F(histname.c_str(),histname.c_str(),4000,0,4000,314,0,3.14);
      list->Add(gret_energy_theta);
    }
    gret_energy_theta->Fill(hit.GetCoreEnergy(),hit.GetTheta());



    histname = Form("GretinaOverview");
    TH2 *gret_summary = (TH2*)list->FindObject(histname.c_str());
    if(!gret_summary) {
      gret_summary = new TH2F(histname.c_str(),histname.c_str(),4000,0,4000,60,20,80);
      list->Add(gret_summary);
    }
    gret_summary->Fill(hit.GetCoreEnergy(),hit.GetCrystalId());



    histname = Form("GretinaSummaryX%02i",hit.GetCrystalId());
    TH2 *seg_summary = (TH2*)list->FindObject(histname.c_str());
    if(!seg_summary) {
      seg_summary = new TH2F(histname.c_str(),histname.c_str(),40,0,40,2000,0,4000);
      list->Add(seg_summary);
    }

    //seg_summary->Fill(36,hit.GetCoreCharge(0)/INTEGRATION);
    //seg_summary->Fill(37,hit.GetCoreCharge(1)/INTEGRATION);
    //seg_summary->Fill(38,hit.GetCoreCharge(2)/INTEGRATION);
    //seg_summary->Fill(39,hit.GetCoreCharge(3)/INTEGRATION);
    for(int z=0;z<hit.NumberOfInteractions();z++) {
      seg_summary->Fill(hit.GetSegmentId(z),hit.GetSegmentEng(z));
      int layer  = hit.GetSegmentId(z)/6;
      histname = Form("GretinaPosition_%s",LayerMap[layer].c_str());
      TH2 *position = (TH2*)list->FindObject(histname.c_str());
      if(!position) {
        position = new TH2F(histname.c_str(),histname.c_str(),628,0,6.28,314,0,3.14);
        list->Add(position);
      }
      double theta = hit.GetInteractionPosition(z).Theta();
      double phi   = hit.GetInteractionPosition(z).Phi();
      if(phi<0)
        phi = TMath::TwoPi()+phi;
      position->Fill(phi,theta);

    }
    //for(int z=0;z<hit.NumberOfInteractions();z++) {
    if(hit.NumberOfInteractions()>0){
      if(hit.GetSegmentEng(0)>990) {
        seg_summary->Fill(hit.GetSegmentId(0),hit.GetSegmentEng(0));
        int layer  = hit.GetSegmentId(0)/6;
        histname = Form("GretinaPositionLargestPosition_%s",LayerMap[layer].c_str());
        TH2 *position = (TH2*)list->FindObject(histname.c_str());
        if(!position) {
          position = new TH2F(histname.c_str(),histname.c_str(),628,0,6.28,314,0,3.14);
          list->Add(position);
        }
        double theta = hit.GetInteractionPosition(0).Theta();
        double phi   = hit.GetInteractionPosition(0).Phi();
        if(phi<0)
          phi = TMath::TwoPi()+phi;
        position->Fill(phi,theta);
      }
    }
    //}
    histname = Form("GretinaPosition");
    TH2 *total_position = (TH2*)list->FindObject(histname.c_str());
    if(!total_position) {
      total_position = new TH2F(histname.c_str(),histname.c_str(),628,0,6.28,314,0,3.14);
      list->Add(total_position);
    }
    total_position->Fill(hit.GetPhi(),hit.GetTheta());

    histname = "GretinaEnergySum";
    TH1 *hist1d = (TH1*)list->FindObject(histname.c_str());
    if(!hist1d) {
      hist1d = new TH1F(histname.c_str(),histname.c_str(),8000,0,4000);
      list->Add(hist1d);
    }
    hist1d->Fill(hit.GetCoreEnergy());

    histname = "GretinaDopplerSum";
    hist1d = (TH1*)list->FindObject(histname.c_str());
    if(!hist1d) {
      hist1d = new TH1F(histname.c_str(),histname.c_str(),8000,0,4000);
      list->Add(hist1d);
    }
    hist1d->Fill(hit.GetDoppler(BETA));

    histname = "GretinaDopplerBeta";
    TH2 *doppler_beta = (TH2*)list->FindObject(histname.c_str());
    if(!doppler_beta) {
      doppler_beta = new TH2F(histname.c_str(),histname.c_str(),2000,0,4000,101,.2,.5);
      list->Add(doppler_beta);
    }
    double beta = 0.2;
    for(int z=0;z<100;z++) {
      beta += .3/100.0;
      doppler_beta->Fill(hit.GetDoppler(beta),beta);
    }


    histname = Form("Q%iCoreCharge",HoleQMap[hit.GetCrystalId()/4]);
    //if(HoleQMap[hit.GetHoleNumber()]==0)
    //  printf("HoleQMap[hit.GetHoleNumber() = 0 is %i\n",hit.GetHoleNumber());

    TH2 *qchg = (TH2*)list->FindObject(histname.c_str());
    if(!qchg) {
      qchg = new TH2F(histname.c_str(),histname.c_str(),16,0,16,8000,0,32000);
      list->Add(qchg);
    }
    qchg->Fill(4*hit.GetCrystalNumber()+0.,((double)hit.GetCoreCharge(0)));
    qchg->Fill(4*hit.GetCrystalNumber()+1.,((double)hit.GetCoreCharge(1)));
    qchg->Fill(4*hit.GetCrystalNumber()+2.,((double)hit.GetCoreCharge(2)));
    qchg->Fill(4*hit.GetCrystalNumber()+3.,((double)hit.GetCoreCharge(3)));

    histname = "SegmentId_hitpattern";
    TH1 *seghit = (TH1*)list->FindObject(histname.c_str());
    if(!seghit) {
      seghit = new TH1F(histname.c_str(),histname.c_str(),100,0,100);
      list->Add(seghit);
    }
    seghit->Fill(hit.GetCrystalId());



  }

  std::string histname = "GCalorimeter";
  TH1 *gcal = (TH1*)list->FindObject(histname.c_str());
  if(!gcal) {
    gcal = new TH1F(histname.c_str(),histname.c_str(),16000,0,8000);
    list->Add(gcal);
  }
  if(gsum>1.0) {
    gcal->Fill(gsum);
  }




  if(numobj!=list->GetSize())
    list->Sort();


}
