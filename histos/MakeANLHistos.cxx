
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
    for (auto& hit : *gr) {
      //cout <<"Single GR: " << hit.Timestamp << endl;
    }
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
      auto adc = hit.GetADC();
      if (adc) {
        for (int i=0; i<4; i++) {
          stream.str(""); stream << "GR_ADC" << i;
          obj.FillHistogram(stream.str().c_str(), 1000,0,1000, adc[i]);
        }
        auto rf = hit.GetRF();
        if (rf != BAD_NUM) {
          obj.FillHistogram("GR_RF",1000,0,0,rf);

          auto first = TMath::Sqrt(adc[0]*adc[1]);
          auto second = TMath::Sqrt(adc[2]*adc[3]);
          obj.FillHistogram("pid_1",500,0,0,rf,500,0,0,first);
          obj.FillHistogram("pid_2",500,0,0,rf,500,0,0,second);
        }
      }
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
