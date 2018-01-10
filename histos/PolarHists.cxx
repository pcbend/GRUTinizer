

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
#include "TBank29.h"

#include "GValue.h"

//std::map<int,int> HoleQMap;
std::map<int,std::string> LayerMap;

void InitMap() {
//  HoleQMap[Q1] = 1;
//  HoleQMap[Q2] = 2;
//  HoleQMap[Q3] = 3;
//  HoleQMap[Q4] = 4;
//  HoleQMap[Q5] = 5;
//  HoleQMap[Q6] = 6;
//  HoleQMap[Q7] = 7;
//  HoleQMap[Q8] = 8;

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
//

extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank29  *bank29  = obj.GetDetector<TBank29>();

  TList *list = &(obj.GetObjects());
  int numobj  = list->GetSize();

  static int entry = 0;

  double BETA = GValue::Value("BETA");

  if(bank29) {
//    for(int x=0;x<bank29->Size();x++) {
//      TMode3Hit &hit = (TMode3Hit&)bank29->GetHit(x);
//      std::string histname = Form("bank29_%i",hit.GetChannel());
//      TH1 *hist = (TH1*)list->FindObject(histname.c_str());
//      if(!hist) {
//        hist= new TH1F(histname.c_str(),histname.c_str(),16000,0,64000);
//        list->Add(hist);
//      }
//      hist->Fill(hit.Charge());
//    }
  }

  if(!gretina || gretina->Size()<1 ) { //|| gretina->Size()>(7*4)) {
    entry++;
    //printf("Here\n"); fflush(stdout);
    return;
  }

  static double first_time = ((double)gretina->Timestamp())/1e8;

  double gsum = 0.0;
  int    largesthit = -1;
  double largesteng = -1.0;
  for(unsigned int y=0;y<gretina->Size();y++) {
    TGretinaHit hit = gretina->GetGretinaHit(y);
    //if(hit.NumberOfInteractions()<1 || hit.GetSegmentId(0)<1 || hit.GetPad()>0)
    //  continue;

    std::string histname;
    gsum += hit.GetCoreEnergy(0);
    if(hit.GetCoreEnergy(0)>largesteng) {
      largesteng = hit.GetCoreEnergy(0);
      largesthit = y;
    }
    for(unsigned int z=y+1;z<gretina->Size();z++) {
      TGretinaHit hit2 = gretina->GetGretinaHit(z);
      //if(hit2.NumberOfInteractions()<1 || hit2.GetSegmentId(0)<1 || hit2.GetPad()>0)
      //  continue;
      histname = "Gamma_Gamma";
      obj.FillHistogramSym(histname,2000,0,4000,hit.GetCoreEnergy(0),
                                    2000,0,4000,hit2.GetCoreEnergy(0));
      histname = "Gamma_Gamma_Time";
      obj.FillHistogram(histname,800,-400,400,hit2.GetTime()-hit.GetTime(),
                                  2000,0,4000,hit2.GetCoreEnergy(0));
      obj.FillHistogram(histname,800,-400,400,hit.GetTime()-hit2.GetTime(),
                                  2000,0,4000,hit.GetCoreEnergy(0));
    }

    histname = "Gretina_Energy_Time";
    obj.FillHistogram(histname,1000,0,4000,hit.GetCoreEnergy(0),
                               5000,0,5000,gretina->Timestamp()/1e8-first_time);

    histname = Form("Gretina%03i_Energy_Time",hit.GetCrystalId());
    obj.FillHistogram(histname,1000,0,4000,hit.GetCoreEnergy(0),
                               5000,0,5000,gretina->Timestamp()/1e8-first_time);

    histname = "Gretina_Energy_Entry";
    obj.FillHistogram(histname,1000,0,4000,hit.GetCoreEnergy(0),
                               9000,0,9000000,entry);
    entry++;


    //histname = "GretinaDopplerBeta";
    //std::string histnamecal = "GretinaDopplerBetaCal";
    //double beta = 0.00;
    //double betamax = 0.005;
    //for(int z=0;z<100;z++) {
    //  beta += betamax/100.0;
      //printf("GetDoppler%.02f() = %.02f\n",beta,hit.GetDoppler(beta));
      //hit.GetPosition().Print();
    //  obj.FillHistogram(histname,8000,0,4000,hit.GetDoppler(beta),
    //                             101,0.00,betamax,beta);
    //  obj.FillHistogram(histnamecal,8000,0,4000,hit.GetDoppler(0,beta),
    //                             101,0.00,betamax,beta);
    //}

    histname = "GretinaDopplerPhi";
    obj.FillHistogram(histname,628,0,6.28,hit.GetPhi(),
                               8000,0,4000,hit.GetDoppler(BETA));

    histname = "GretinaDopplerXId";
    obj.FillHistogram(histname,200,0,200,hit.GetCrystalId(),
                               8000,0,4000,hit.GetDoppler(BETA));

    histname = "GretinaDopplerXIdCal";
    obj.FillHistogram(histname,200,0,200,hit.GetCrystalId(),
                               8000,0,4000,hit.GetDoppler(0,BETA));

    histname = "GretinaEnergyXId";
    obj.FillHistogram(histname,200,0,200,hit.GetCrystalId(),
                               8000,0,4000,hit.GetCoreEnergy());

    histname = "GretinaEnergyXIdCal";
    obj.FillHistogram(histname,200,0,200,hit.GetCrystalId(),
                               8000,0,4000,hit.GetCoreEnergy(0));

    histname = "GretinaCarge0XIdCal";
    obj.FillHistogram(histname,200,0,200,hit.GetCrystalId(),
                               8000,0,32000,hit.GetCoreCharge(0));
    histname = "GretinaCarge1XIdCal";
    obj.FillHistogram(histname,200,0,200,hit.GetCrystalId(),
                               8000,0,32000,hit.GetCoreCharge(1));
    histname = "GretinaCarge2XIdCal";
    obj.FillHistogram(histname,200,0,200,hit.GetCrystalId(),
                               8000,0,32000,hit.GetCoreCharge(2));
    histname = "GretinaCarge3XIdCal";
    obj.FillHistogram(histname,200,0,200,hit.GetCrystalId(),
                               8000,0,32000,hit.GetCoreCharge(3));











    histname = "GretinaEnergyTheta";
    obj.FillHistogram(histname,314,0,3.14,hit.GetTheta(),
                              4000,0,4000,hit.GetCoreEnergy(0));

    histname = "Gretina_Theta_bySegment";
    obj.FillHistogram(histname,314,0,3.14,hit.GetSegmentPosition().Theta(),
                              4000,0,4000,hit.GetCoreEnergy(0));

    histname = "Gretina_Theta_byCrystal";
    obj.FillHistogram(histname,314,0,3.14,hit.GetCrystalPosition().Theta(),
                              4000,0,4000,hit.GetCoreEnergy(0));

    histname = "Gretina_Theta_byDecomp";
    obj.FillHistogram(histname,314,0,3.14,hit.GetPosition().Theta(),
                              4000,0,4000,hit.GetCoreEnergy(0));

    histname = Form("GretinaPosition");
    obj.FillHistogram(histname,628,0,6.28,hit.GetPhi(),
                                314,0,3.14,hit.GetTheta());

    histname = "GretinaEnergySum";
    obj.FillHistogram(histname,8000,0,4000,hit.GetCoreEnergy(0));

    histname = "GretinaDopplerSum";
    obj.FillHistogram(histname,8000,0,4000,hit.GetDoppler(BETA));

    histname = "CrystalId_hitpattern";
    obj.FillHistogram(histname,200,0,200,hit.GetCrystalId());

    if(hit.NumberOfInteractions()<1 || hit.GetSegmentId(0)<1 || hit.GetPad()>0)
      continue;

    std::string layer;
    if(hit.GetSegmentId(0)<7)
      layer = "_1_alpha";
    else if(hit.GetSegmentId(0)<13)
      layer = "_2_beta";
    else if(hit.GetSegmentId(0)<19)
      layer = "_3_gamma";
    else if(hit.GetSegmentId(0)<25)
      layer = "_4_delta";
    else if(hit.GetSegmentId(0)<31)
      layer = "_5_epsilon";
    else if(hit.GetSegmentId(0)<37)
      layer = "_6_phi";
    else
      layer = "_7_unknown";

    histname = "Gretina_Theta_bySegment";
    histname.append(layer);
    obj.FillHistogram(histname,314,0,3.14,hit.GetSegmentPosition().Theta(),
                              4000,0,4000,hit.GetCoreEnergy(0));

    histname = "Gretina_Theta_byCrystal";
    histname.append(layer);
    obj.FillHistogram(histname,314,0,3.14,hit.GetCrystalPosition().Theta(),
                              4000,0,4000,hit.GetCoreEnergy(0));

    histname = "Gretina_Theta_byDecomp";
    histname.append(layer);
    obj.FillHistogram(histname,314,0,3.14,hit.GetPosition().Theta(),
                              4000,0,4000,hit.GetCoreEnergy(0));

    histname = "GretinaOverview";
    obj.FillHistogram(histname,4000,0,4000,hit.GetCoreEnergy(0),
                                  60,20,80,hit.GetCrystalId());


    /*
    histname = Form("GretinaSummaryX%02i",hit.GetCrystalId());
    for(int z=0;z<hit.NumberOfInteractions();z++) {
      obj.FillHistogram(histname,40,0,40,hit.GetSegmentId(z),
                              2000,0,4000,hit.GetSegmentEng(z));
      int layer  = hit.GetSegmentId(z)/6;
      std::string histname2 = Form("GretinaPosition_%s",LayerMap[layer].c_str());
      double theta = hit.GetInteractionPosition(z).Theta();
      double phi   = hit.GetInteractionPosition(z).Phi();
      if(phi<0)
        phi = TMath::TwoPi()+phi;
      obj.FillHistogram(histname2,628,0,6.28,phi,
                                  314,0,3.14,theta);
    }
    */
  }

  if(gsum>1) {
    std::string histname = "GCalorimeter";
    obj.FillHistogram(histname,16000,0,8000,gsum);

    TGretinaHit hit = gretina->GetGretinaHit(largesthit);
    histname = "GCalorimeter_Theta";
    obj.FillHistogram(histname,314,0,3.14,hit.GetTheta(),
                               8000,0,8000,gsum);
  }
  if(numobj!=list->GetSize())
    list->Sort();
}
