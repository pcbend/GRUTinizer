
#include "TRuntimeObjects.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <string>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>

#include "TCagra.h"
#include "TGrandRaiden.h"
#include "TANLEvent.h"

#define BAD_NUM -441441

//#include "TChannel.h"
//#include "GValue.h"

#define PRINT(x) std::cout << #x" = " << x << std::endl
#define STR(x) #x << " = " <<() x

using namespace std;


string name;
stringstream stream;
ULong_t nevent = 0;

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {



  TCagra* cagra = obj.GetDetector<TCagra>();
  TGrandRaiden* gr = obj.GetDetector<TGrandRaiden>();

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  if (cagra && gr) {
    for (auto& hit : *gr) {

      auto& rcnp = hit.GR();
      auto grtime = hit.GetTimestamp();

      // coincidence rate
      static ULong_t first_timestamp = grtime;
      if (first_timestamp) {
        auto rate = (grtime-first_timestamp)/1e8;
        //cout << grtime << " " << first_timestamp << endl;
        obj.FillHistogram("CoinRate",1000,0,10000, rate);
      }

      // coincidence time difference
      for (auto& caghit : *cagra) {

        ULong_t cagratime = caghit.Timestamp();
        Double_t cagra_cfd = caghit.GetDiscTime();

        ULong_t ts_diff = cagratime-grtime;
        Double_t cfd_diff = cagra_cfd-(Double_t)grtime;

        obj.FillHistogram("CoinDiff",2000,-1000,1000,ts_diff);
        stream.str("");
        stream << "CoinDiff_" << caghit.GetBoardID() << "_" <<caghit.GetChannel();
        obj.FillHistogram(stream.str().c_str(),2000,-1000,1000,ts_diff);

        if (cagratime - grtime > 238 && cagratime -grtime < 246) {
          stream.str(""); stream << caghit.GetBoardID() << "Cal" << caghit.GetChannel() << "_CoinGated";
          obj.FillHistogram(stream.str().c_str(),10000,0,10000,caghit.GetEnergy());
          obj.FillHistogram("GATETEST_CoinDiff",2000,-1000,1000,ts_diff);
        }
        obj.FillHistogram("CoinDiff_CFD",2000,-1000,1000,cfd_diff);
        stream.str("");
        stream << "CoinDiff_CFD_" << caghit.GetBoardID() << "_" <<caghit.GetChannel();
        obj.FillHistogram(stream.str().c_str(),2000,-1000,1000,cfd_diff);

        if (cagra_cfd - grtime > 238 && cagra_cfd -grtime < 246) {
          stream.str(""); stream << caghit.GetBoardID() << "Cal" << caghit.GetChannel() << "_CoinGated";
          obj.FillHistogram(stream.str().c_str(),10000,0,10000,caghit.GetEnergy());
          obj.FillHistogram("GATETEST_CoinDiff_CFD",2000,-1000,1000,cfd_diff);
        }


      }



    }

  }


  if (gr) {

    for (auto& hit : *gr) {

      auto& rcnp = hit.GR();
      auto adc = rcnp.GR_ADC();


      if (rcnp.GR_MYRIAD(0) != BAD_NUM) {
        obj.FillHistogram("MyriadTimestamp",10000,1e9,5e12,hit.GetTimestamp());
      }

      static ULong_t prev_ts = 0;
      if (prev_ts) {
        obj.FillHistogram("GR_EventPeriod",5000,100,50000,hit.GetTimestamp()-prev_ts);
      }
      prev_ts = hit.GetTimestamp();


      if (rcnp.GR_ADC()) {
        auto& adc = *rcnp.GR_ADC();
        for (int i=0; i<4; i++) {
          stream.str(""); stream << "GR_ADC" << i;
          obj.FillHistogram(stream.str().c_str(), 1000,0,2000, adc[i]);
        }
        obj.FillHistogram("MeanPlastE1", 2000,0,2000, hit.GetMeanPlastE1());
        obj.FillHistogram("MeanPlastE2", 2000,0,2000, hit.GetMeanPlastE2());
      }
      if (rcnp.GR_TDC()) {
        auto& tdc = *rcnp.GR_TDC();
        for (int i=0; i<4; i++) {
          stream.str(""); stream << "GR_TDC" << i;
          obj.FillHistogram(stream.str().c_str(), 1000,-40000,40000, tdc[i]);
        }
        obj.FillHistogram("MeanPlastPos1", 1000, 0, 40000, hit.GetMeanPlastPos1());
        obj.FillHistogram("MeanPlastPos2", 1000, 0, 40000, hit.GetMeanPlastPos2());
      }
      if (rcnp.QTC_LEADING_TDC()) {
        auto& qtc_leading = *rcnp.QTC_LEADING_TDC();
        auto& qtc_leading_chan = *rcnp.QTC_LEADING_CH();
        auto x = rcnp.GR_X(0);

        for (int i=0; i< qtc_leading_chan.size(); i++) {
          int channum = qtc_leading_chan[i];
          stream.str(""); stream << "LaBrLeading" << channum;
          obj.FillHistogram(stream.str().c_str(), 10000,-40000, 40000, qtc_leading[i]);
          // gate on gr_x
          if (x < 100 && x > 0) {
            stream.str(""); stream << "LaBrLead"<< channum << "_GateX";
            obj.FillHistogram(stream.str().c_str(), 10000,-40000, 40000, qtc_leading[i]);
          }



          if (qtc_leading[i]>=-5100 && qtc_leading[i] <=-4400) {


            obj.FillHistogram("RayID_LEGate",64,-16,48, rcnp.GR_RAYID(0));
            if (rcnp.GR_RAYID(0) == 0) { // if track reconstruction successfull
              obj.FillHistogram("GR_X_LEGate",1200,-600,600, rcnp.GR_X(0));
              obj.FillHistogram("GR_Y_LEGate",200,-100,100, rcnp.GR_Y(0));
              obj.FillHistogram("GR_Theta_LEGate",100,-1,1, rcnp.GR_TH(0)); // need to learn
              obj.FillHistogram("GR_Phi_LEGate",100,-1,1, rcnp.GR_PH(0)); // from hist.def
              obj.FillHistogram("X_TH_LEGate",1200,-600,600,rcnp.GR_X(0),1000,-1,1,rcnp.GR_TH(0));
            }

            for (auto const& labr_hit : hit.GetLaBr()) {
              int channum = labr_hit.channel;
              stream.str(""); stream << "LaBrWidth_LEGate" << channum;
              obj.FillHistogram(stream.str().c_str(), 10000, -5000, 15000, labr_hit.width);

              if (rcnp.GR_X(0) != BAD_NUM) {
                obj.FillHistogram("X_LaBr_LEGate",1200,-600,600,rcnp.GR_X(0),10000,-5000,15000,labr_hit.width);
              }
            }


          }


        }
      }
      for (auto const& labr_hit : hit.GetLaBr()) {
        int channum = labr_hit.channel;
        stream.str(""); stream << "LaBrWidth" << channum;
        obj.FillHistogram(stream.str().c_str(), 10000, -5000, 15000, labr_hit.width);

        if (rcnp.GR_X(0) != BAD_NUM) {
          obj.FillHistogram("X_LaBr",1200,-600,600,rcnp.GR_X(0),10000,-5000,15000,labr_hit.width);
        }

      }
      obj.FillHistogram("RayID",64,-16,48, rcnp.GR_RAYID(0));
      if (rcnp.GR_RAYID(0) == 0) { // if track reconstruction successfull
        obj.FillHistogram("GR_X",1200,-600,600, rcnp.GR_X(0));
        obj.FillHistogram("GR_Y",200,-100,100, rcnp.GR_Y(0));
        obj.FillHistogram("GR_Theta",100,-1,1, rcnp.GR_TH(0)); // need to learn
        obj.FillHistogram("GR_Phi",100,-1,1, rcnp.GR_PH(0)); // from hist.def
        obj.FillHistogram("X_TH",1200,-600,600,rcnp.GR_X(0),1000,-1,1,rcnp.GR_TH(0));
      }
      auto rf = rcnp.GR_RF(0);
      if (rf != BAD_NUM) {
        obj.FillHistogram("GR_RF",1000,0,0,rf);
      }

      //GR timestamps check - sometimes the myriad timestamp is missing...
      //static int n_gr = 0;
      auto time = rcnp.GR_MYRIAD(0);
      if (time != BAD_NUM) {
        obj.FillHistogram("GR_LiveTimestamps",10,0,11,7);
      } else {
        obj.FillHistogram("GR_LiveTimestamps",10,0,11,3);
      }

      //   obj.FillHistogram("GR_RF",1000,0,0,rf);
      //   auto first = TMath::Sqrt(adc[0]*adc[1]);
      //   auto second = TMath::Sqrt(adc[2]*adc[3]);
      //   obj.FillHistogram("pid_1",500,0,0,rf,500,0,0,first);
      //   obj.FillHistogram("pid_2",500,0,0,rf,500,0,0,second);
      // }
    }
  }

  if(cagra) {

    for (auto& hit : *cagra) {

      auto boardid = hit.GetBoardID();
      auto chan = hit.GetChannel();

      stream.str("");
      stream << "Ge_" << boardid << "_" << chan;
      obj.FillHistogram("Raw", stream.str().c_str(),10000,0,10000,hit.Charge());

      stream.str("");
      stream << "Ge_" << boardid << "_" << chan;
      if (boardid == 102) {
        auto labr_E = hit.GetTraceEnergy(0,57,60,60+57);
        //if (nevent % 10000 == 0) { cout << labr_E << endl; }
        obj.FillHistogram("Calibrated",stream.str().c_str(),10000,0,10000,labr_E);
      } else {
        obj.FillHistogram("Calibrated",stream.str().c_str(),10000,0,10000,hit.GetEnergy());
      }

      stream.str("");
      stream << "Ge_PZ_" << boardid << "_" << chan;
      obj.FillHistogram("Corrected", stream.str().c_str(),10000,0,10000,hit.GetCorrectedEnergy(0));
      stream.str("");
      stream << "Ge_PZ_AsymBL_" << boardid << "Cal" << chan;
      obj.FillHistogram("Corrected", stream.str().c_str(),10000,0,10000,hit.GetCorrectedEnergy(hit.GetBaseSample()));

      Double_t prerise_base = hit.GetPreRise()/TANLEvent::GetShapingTime();

      stream.str("");
      stream << "E_BL" << boardid << "_" << chan;
      obj.FillHistogram("Baseline", stream.str().c_str(),1000,0,10000,hit.Charge(),1000,0,3000,prerise_base);

      stream.str("");
      stream << "E_BL_scale" << boardid << "_" << chan;
      obj.FillHistogram("Baseline", stream.str().c_str(),1000,0,10000,hit.Charge()-(1.0/-11.21)*prerise_base,1000,0,3000,prerise_base);

      stream.str("");
      stream << "E_cor_BL" << boardid << "_" << chan;
      obj.FillHistogram("Baseline", stream.str().c_str(),1000,0,10000,hit.GetCorrectedEnergy(hit.GetBaseSample()),1000,0,3000,prerise_base);



    }

  }


  if(numobj!=list->GetSize())
    list->Sort();

  nevent++;
}
