
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

#include "TCAGRA.h"

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
  TCAGRA *cagra = obj.GetDetector<TCAGRA>();

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  if(!cagra)
    return;


  for(int y=0;y<cagra->Size();y++) {
    //auto hit = cagra->GetCAGRAHit(y);




    //stream.str("");
    //stream << "PostE_BoardID" << hit.GetBoardID()  << "Chan" << hit.GetChannel();
    //obj.FillHistogram(stream.str(),10000,0,0,hit.GetPostE());

    //stream.str("");
    //stream << "LED_BoardID" << hit.GetBoardID()  << "Chan" << hit.GetChannel();
    //obj.FillHistogram(stream.str(),10000,0,0,hit.GetLED());


    // if(hit.GetBoardID() == 113) {
    //   //cout << hit.GetLED() << endl;
    //   stream.str("");
    //   stream << "Crystal" << hit.GetChannel();
    //   float Energy = ((hit.GetPostE() - hit.GetPreE())/350.0);
    //   obj.FillHistogram(stream.str(),10000,0,20000,Energy);
    // }

    for (auto& hit : *cagra) {
      if (hit.GetBoardID() == 0x71) {
        /*
        cout << "Clover: " << hit.GetDetnum()
             << " Leaf: " << hit.GetLeaf()
             << " Segment: " << hit.GetMainSegnum()
             << " Theta: "<< hit.GetPosition().Theta()*180/TMath::Pi()
             << endl;
        */
        //hit.GetPosition().Print();
        stream.str("");
        stream << "Leaf" << hit.GetChannel();
        obj.FillHistogram(stream.str(),10000,0,20000,hit.Charge());
        stream.str("");
        stream << "CalLeaf" << hit.GetChannel();
        obj.FillHistogram(stream.str(),10000,0,20000,hit.GetEnergy());

      }
    }



    // PRINT(hit.GetBoardID());
    // if (hit.GetChannel() > 2) PRINT(hit.GetChannel());
    // PRINT(hit.GetLED());
    // PRINT(hit.GetPostE());
    // PRINT(hit.GetPreE());
    // PRINT((hit.GetPostE() - hit.GetPreE())/350.0);
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  }




  if(numobj!=list->GetSize())
    list->Sort();

}
