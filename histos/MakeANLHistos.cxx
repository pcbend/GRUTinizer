
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

#define BAD_NUM -441441

//#include "TChannel.h"
//#include "GValue.h"

#define PRINT(x) std::cout << #x" = " << x << std::endl
#define STR(x) #x << " = " <<() x

using namespace std;


string name;
stringstream stream;


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
    int totalhits = 0;
    for (auto& hit : *gr) { totalhits++; }
    for (int i=0; i < totalhits; i++) {
      obj.FillHistogram("nCoin",4,0,1,0);
    }

    static int ncoin = 0;
    ncoin+=totalhits;
    //cout << "Coin: " << ncoin << endl;
  } else if (gr) {
    //for (auto& hit : *gr) {
      //cout <<"Single GR: " << hit.Timestamp << endl;
    //}
  }


  if (gr) {
    int totalhits = 0;
    for (auto& hit : *gr) { totalhits++; }
    static int ncoin = 0;
    ncoin+=totalhits;
    //cout << "GR only: " << ncoin << endl;
  }

  if (gr) {
    for (auto& hit : *gr) {

      auto& rcnp = hit.GR();


      auto adc = rcnp.GR_ADC();




      if (rcnp.GR_ADC()) {
        auto& adc = *rcnp.GR_ADC();
        for (int i=0; i<4; i++) {
          stream.str(""); stream << "GR_ADC" << i;
          obj.FillHistogram(stream.str().c_str(), 1000,0,1000, adc[i]);
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

        for (int i=0; i< qtc_leading_chan.size(); i++) {
          int channum = qtc_leading_chan[i];
          stream.str(""); stream << "LaBrLeading" << channum;
          obj.FillHistogram(stream.str().c_str(), 10000,-40000, 40000, qtc_leading[i]);
        }
      }
      for (auto const& labr_hit : hit.GetLaBr()) {
        int channum = labr_hit.channel;
        stream.str(""); stream << "LaBrWidth" << channum;
        obj.FillHistogram(stream.str().c_str(), 10000, -5000, 15000, labr_hit.width);
      }
      obj.FillHistogram("RayID",64,-16,48, rcnp.GR_RAYID(0));
      if (rcnp.GR_RAYID(0) == 0) { // if track reconstruction successfull
        obj.FillHistogram("GR_X",1200,-600,600, rcnp.GR_RAYID(0));
        obj.FillHistogram("GR_Y",200,-100,100, rcnp.GR_RAYID(0));
        obj.FillHistogram("GR_Theta",100,-1,1, rcnp.GR_RAYID(0)); // need to learn
        obj.FillHistogram("GR_Phi",100,-1,1, rcnp.GR_RAYID(0)); // from hist.def
      }
      auto rf = rcnp.GR_RF(0);
      if (rf != BAD_NUM) {
        obj.FillHistogram("GR_RF",1000,0,0,rf);
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

    //cout << "Size: " << cagra->Size() << endl;
    for (auto& hit : *cagra) {

      //cout << hit.Timestamp() << endl;
      stream.str("");
      stream << "PostE_BoardID" << hit.GetBoardID()  << "_Chan" << hit.GetChannel();
      obj.FillHistogram(stream.str(),10000,0,0,hit.Charge());

      if (hit.GetBoardID() == 0x71) {

        stream.str("");
        stream << "Leaf" << hit.GetChannel();
        obj.FillHistogram(stream.str(),10000,0,20000,hit.Charge());
        stream.str("");
        stream << "CalLeaf" << hit.GetChannel();
        obj.FillHistogram(stream.str(),10000,0,20000,hit.GetEnergy());
      }
    }
  }


  if(numobj!=list->GetSize())
    list->Sort();


}
