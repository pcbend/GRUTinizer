
#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>

#include "TGretina.h"
#include "TS800.h"
#include "TBank29.h"

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
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank29  *bank29  = obj.GetDetector<TBank29>();
  TS800    *s800    = obj.GetDetector<TS800>();

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  //std::cout << "obj pointer = 0x" << std::hex << &obj << std::dec << std::endl;
  //std::cout << "obj::Get()  = 0x" << std::hex << TRuntimeObjects::Get() << std::dec << std::endl;
  //std::cout << "---------------------------------------------" << std::endl;
  //std::cout << "---------------------------------------------" << std::endl;


  //double MAFP_COEF = obj.GetVariable("MAFP_COEF");
  //double MCRDCX_COEF = obj.GetVariable("MCRDCX_COEF");
  //double AFP_COEF    = GValue::Value("AFP_COEF");
  //double CRDCX_COEF  = GValue::Value("CRDCX_COEF");
  double E1_TDC_low  = GValue::Value("E1_TDC_low");
  double E1_TDC_high = GValue::Value("E1_TDC_high");
  double BETA        = GValue::Value("BETA");

  TList *gates = &(obj.GetGates());
  bool haspids = gates->GetSize();

  if(bank29) {
    for(unsigned int x=0;x<bank29->Size();x++) {
      TMode3Hit &hit = (TMode3Hit&)bank29->GetHit(x);
      std::string histname = Form("bank29_%i",hit.GetChannel());
      obj.FillHistogram(histname,16000,0,64000,hit.Charge());
    }
    if(s800) {
      std::string histname = "S800_Bank29_time";
      obj.FillHistogram(histname,200,-200,200,bank29->Timestamp()-s800->Timestamp());
    }
  }
  if(!gretina)
    return;


  //if(s800) {
    //std::string histname = "S800_DTA";
    //obj.FillHistogram(histname,200,-10,10,s800->GetDta());

    //histname = "S800_YTA";
    //obj.FillHistogram(histname,200,-10,10,s800->GetYta());

  //}

  double gsum = 0.0;
  for(unsigned int y=0;y<gretina->Size();y++) {
    TGretinaHit hit = gretina->GetGretinaHit(y);
    std::string histname;
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



    if(bank29) {
      histname = "Gretina_Bank29_time";
      obj.FillHistogram(histname,600,-600,600,bank29->Timestamp()-hit.GetTimestamp(),
                                 2000,0,4000,hit.GetCoreEnergy());
      histname = "Gretina_t0_Bank29_time";
      obj.FillHistogram(histname,600,-600,600,bank29->Timestamp()-hit.GetTime(),
                                 2000,0,4000,hit.GetCoreEnergy());
    }

    if(s800) {
      histname = "Gretina_S800_time";
      obj.FillHistogram(histname,1200,-600,600,s800->Timestamp()-hit.GetTimestamp(),
                                 2000,0,4000,hit.GetCoreEnergy());
      histname = "Gretina_t0_S800_time";
      obj.FillHistogram(histname,1200,-600,600,s800->Timestamp()-hit.GetTime(),
                                 2000,0,4000,hit.GetCoreEnergy());

      histname = "pidtune1_TDC_vs_DispX";
      obj.FillHistogram(histname,4000,-4000,4000,s800->GetCorrTOF_OBJ(),    //s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),
                                 600,-300,300,s800->GetCrdc(0).GetDispersiveX());

      histname = "pidtune2_TDC_vs_AFP";
      obj.FillHistogram(histname,4000,-4000,4000,s800->GetCorrTOF_OBJ(),    //s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),
                                 600,-0.1,0.1,s800->GetAFP());

      double delta_t = s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ();
      histname ="pidtune3_PID_TDC";
      obj.FillHistogram(histname,4000,-4000,4000,s800->GetCorrTOF_OBJ(),    //s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),
                                 2000,0,50000,s800->GetIonChamber().Charge());

      if(delta_t>E1_TDC_low && delta_t<E1_TDC_high){
        histname ="pidtune3_PID_TDC_Prompt";
        obj.FillHistogram(histname,8000,-4000,4000,s800->GetCorrTOF_OBJ(),  //s800->GetTofE1_TDC(AFP_COEF,CRDCX_COEF),
                                   2000,0,50000,s800->GetIonChamber().Charge());
      }

      if(haspids) {
        TIter it(gates);
        while(TObject *itobj = it.Next()) {
          if(!itobj->InheritsFrom(TCutG::Class()))
            continue;
          TCutG *mypid = (TCutG*)itobj;
          if(mypid->IsInside(s800->GetCorrTOF_OBJ(),s800->GetIonChamber().Charge())) {
            histname = "GretinaDoppler_%s",mypid->GetName();
            obj.FillHistogram(histname,2000,0,4000,hit.GetDoppler(BETA));
          }
        }
      }

      histname = "E1_m_TDC";
      obj.FillHistogram(histname,8000,-8000,8000,s800->GetScint().GetTimeUp()-s800->GetTof().GetOBJ());

      histname = "E1Raw";
      obj.FillHistogram(histname,2000,0,8000,s800->GetScint().GetTimeUp());

      histname = "M_E1Raw";
      for(int z=0;z<s800->GetMTof().E1UpSize();z++)
        obj.FillHistogram(histname,2000,0,8000,s800->GetMTof().fE1Up[z]);

      histname = "CRDC1_X";
      obj.FillHistogram(histname,800,-400,400,s800->GetCrdc(0).GetDispersiveX());

      histname = "CRDC2_X";
      obj.FillHistogram(histname,800,-400,400,s800->GetCrdc(1).GetDispersiveX());

      histname = "IonChamberSum";
      obj.FillHistogram(histname,4000,0,32000,s800->GetIonChamber().GetSum());




    }
    histname = Form("GretinaEnergyTheta");
    obj.FillHistogram(histname,4000,0,4000,hit.GetCoreEnergy(),
                               314,0,3.14,hit.GetTheta());

    histname = Form("GretinaOverview");
    obj.FillHistogram(histname,4000,0,4000,hit.GetCoreEnergy(),
                               100,0,100,hit.GetCrystalId());

    histname = Form("GretinaSummaryX%02i",hit.GetCrystalId());
    for(int z=0;z<hit.NumberOfInteractions();z++) {
      obj.FillHistogram(histname,40,0,40,hit.GetSegmentId(z),
                                 2000,0,4000,hit.GetSegmentEng(z));
      if(hit.GetSegmentId(z)>36)
        break;
      int layer  = hit.GetSegmentId(z)/6;
      histname = Form("GretinaPosition_%s",LayerMap[layer].c_str());
      double phi = hit.GetInteractionPosition(z).Phi();
      if(phi<0) phi +=TMath::Pi()*2;
      obj.FillHistogram(histname,628,0.0,6.28,phi,
                                 314,0,3.14,hit.GetInteractionPosition(z).Theta());
    }
/*
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
*/

    histname = Form("GretinaPosition");
    obj.FillHistogram(histname,628,0,6.28,hit.GetPhi(),
                               314,0,3.14,hit.GetTheta());

    histname = "GretinaEnergyTotal";
    obj.FillHistogram(histname,8000,0,4000,hit.GetCoreEnergy());

    histname = "GretinaDopplerTotal";
    obj.FillHistogram(histname,8000,0,4000,hit.GetDoppler(BETA));

    histname = "GretinaDopplerBeta";
    double beta = 0.2;
    for(int z=0;z<100;z++) {
      beta += .3/100.0;
      obj.FillHistogram(histname,8000,0,4000,hit.GetCoreEnergy(beta),
                                 101,0.2,0.5,beta);
    }

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

  if(gsum>1.0) {
    std::string histname = "GCalorimeter";
    obj.FillHistogram(histname,16000,0,8000,gsum);
  }

  if(numobj!=list->GetSize())
    list->Sort();

}
