
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

#include "TArgonne.h"

//#include "TChannel.h"
//#include "GValue.h"

#define PRINT(x) std::cout << #x" = " << x << std::endl
#define STR(x) #x << " = " << x

using namespace std;


string name;

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TArgonne *cagra = obj.GetDetector<TArgonne>();

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  if(!cagra)
    return;

  for(int y=0;y<cagra->Size();y++) {
    TArgonneHit hit = cagra->GetArgonneHit(y);
    //stringstream stream; stream.str("");
    //stream << "Energy";
    name = "Energy";
    obj.FillHistogram(name,10000,0,20000,((hit.GetPostE() - hit.GetPreE())/350.0));
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
