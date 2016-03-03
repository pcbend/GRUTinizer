
#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>

#include "TArgonne.h"

//#include "TChannel.h"
//#include "GValue.h"

#define PRINT(x) std::cout << #x"=" << x << std::endl
#define STR(x) #x << " = " << x

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
    //histname = "Energy";
    //obj.FillHistogram(histname,,0,100,hit.GetCrystalId());
    std::cout << STR(hit.GetLed()) << std::endl;
    std::cout << STR(hit.GetPostE()) << std::endl;
    std::cout << STR(hit.GetPreE()) << std::endl;
    std::cout << STR((hit.GetPostE() - hit.GetPreE())/350.0) << std::endl;
    std::cin.get();
  }



  if(numobj!=list->GetSize())
    list->Sort();

}
