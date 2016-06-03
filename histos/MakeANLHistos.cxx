
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
      cout <<"Single GR: " << hit.Timestamp << endl;
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
      for (int i=0; i<4; i++) {
        stream.str(""); stream << "GR_ADC" << i;
        obj.FillHistogram(stream.str().c_str(), 1000,0,1000, adc[i]);
      }
    }
  }
  //   cout << "  GR TS: " << gr->Timestamp() << " " << gr->Size() << endl;
  //   cout << "  CAGRA TS: " << cagra->Timestamp() << " " << cagra->Size() << endl;
  //   debugctr=0;
  // } else {
  //   cout << "Single ";
  //   if (cagra) {
  //     cout << "Cagra" << endl;
  //     cout << cagra->Timestamp() << endl;
  //   } else if (gr) {
  //     cout << "Grand Raiden" << endl;
  //     cout << gr->Timestamp() << endl;
  //   }
  //   debugctr+=1;
  // }
  // if (debugctr>1e3){
  //   cout << totalctr << endl;
  //   exit(1);
  // }
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
