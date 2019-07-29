#include "TRuntimeObjects.h"

#include <map>
#include <iostream>
#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>
#include <TLine.h>

#include "TGretina.h"
#include "TBank88.h"
#include "TUML.h"
//#include "TS800.h"
//#include "TFastScint.h"

#include "TChannel.h"
#include "GValue.h"
#include "GCutG.h"


//quads as of June 2019.
#define Q1 15
#define Q2 7
#define Q3 11
#define Q4 16 
#define Q5 8
#define Q6 14
#define Q7 12
#define Q8 17
#define Q9 19
#define Q10 6
#define Q11 9
//#define Q12 20

//#define BETA .37

std::map<int,int> HoleQMap;
std::map<int,std::string> LayerMap;

bool map_inited=false;

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
  HoleQMap[Q10] = 10;
  HoleQMap[Q11] = 11;
  //  HoleQMap[Q12] = 12;
  LayerMap[0] = "alpha";
  LayerMap[1] = "beta";
  LayerMap[2] = "gamma";
  LayerMap[3] = "delta";
  LayerMap[4] = "epsilon";
  LayerMap[5] = "phi";
}


static long first_timestamp = -1;

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  //InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank88  *bank88  = obj.GetDetector<TBank88>();
  TUML     *uml     = obj.GetDetector<TUML>();


  //TList    *list    = &(obj.GetObjects());
  //int numobj        = list->GetSize();

  std::string histname = "";
  std::string dirname  = "";
  /*
  if(bank88) {
    if(first_timestamp==-1) { first_timestamp = bank88->Timestamp(); }
    for(unsigned int x=0;x<bank88->Size();x++) {
      TMode3Hit &hit = (TMode3Hit&)bank88->GetHit(x);
      //std::cout<< hit.GetChannel() <<"\t"<< hit.Charge()<<"\t"<< hit.Timestamp() << std::endl;
      //std::cout << "charge0:  " <<  hit.GetCharge0() << std::endl;
      //std::cout << "charge1:  " <<  hit.GetCharge1() << std::endl;
      //std::cout << "charge2:  " <<  hit.GetCharge2() << std::endl;
      std::string histname = Form("bank88_%i",hit.GetChannel());
      obj.FillHistogram(histname,16000,0,64000,hit.Charge());
      obj.FillHistogram("bank88_time",900,0,900,(bank88->Timestamp())/1e8,
          1000,0,1000,hit.Charge()*0.261594);
      obj.FillHistogram("bank88_time_zeroed",900,0,900,(bank88->Timestamp()-first_timestamp)/1e8,
          1000,0,1000,hit.Charge()*0.261594);
      obj.FillHistogram(Form("%s_0",histname.c_str()),16000,0,64000,hit.GetCharge0());
      obj.FillHistogram(Form("%s_1",histname.c_str()),16000,0,64000,hit.GetCharge1());
      obj.FillHistogram(Form("%s_2",histname.c_str()),16000,0,64000,hit.GetCharge2());
      //hit.Print("long");

      //for(int y=0;y<hit.WaveSize();y++) {
      //  histname = Form("bank88_wave_%i",hit.GetChannel());
      //  obj.FillHistogram(histname,200,0,200,y,4000,-16000,16000,hit.GetWave().at(y));
      //}

    }
  }
  */
  if(uml) {
    if(first_timestamp==-1) { first_timestamp = uml->Timestamp(); }

    for(unsigned int x=0;x<uml->Size();x++) {
      //std::cout << "size: " << uml->Size() << std::endl;
      TUMLHit hit = uml->GetUMLHit(x);
      std::string histname = Form("uml_%i",hit.GetNumber());
      obj.FillHistogram(histname,16000,0,64000,hit.Charge());
      obj.FillHistogram("uml_time",900,0,900,(uml->Timestamp())/1e8,
          1000,0,1000,hit.Charge()*0.384744);
      //obj.FillHistogram("uml_time_zeroed",900,0,900,(uml->Timestamp()-first_timestamp)/1e8,
      //    1000,0,1000,hit.Charge()*0.384744);
    }

  
  }
  
  if(gretina) {
    if(first_timestamp==-1) { first_timestamp = gretina->Timestamp(); }
    for(unsigned int x=0;x<gretina->Size();x++) {
      TGretinaHit hit = gretina->GetGretinaHit(x);
      obj.FillHistogram("gretina_time",900,0,900,(gretina->Timestamp())/1e8,
          1000,0,1000,hit.GetEnergy());
      //obj.FillHistogram("gretina_time_zeroed",900,0,900,(gretina->Timestamp()-first_timestamp)/1e8,
      //    1000,0,1000,hit.GetEnergy());
      obj.FillHistogram("summary",10000,0,10000,hit.GetCoreEnergy(),
          200,0,200,hit.GetCrystalId());

    }



  }
  
  
  if(uml && gretina) {
    obj.FillHistogram("delta_time",1800,0,1800,(gretina->Timestamp())/1e8,
        500,-500,500,gretina->Timestamp()-  uml->Timestamp()  );
    //double chg = uml->GetUMLHit(0).Charge();
    //for(unsigned int x=0;x<gretina->Size();x++) {
    //  TGretinaHit hit = gretina->GetGretinaHit(x);
    //  obj.FillHistogram("matrix",2000,0,2000,hit.GetCoreEnergy(),
    //      2000,0,20000,chg);
     // if((chg>11400 && chg<11900)) {
     //   obj.FillHistogram("delta_time_energy",2000,0,2000,hit.GetCoreEnergy(),
     //       500,-500,500,gretina->Timestamp()-  uml->Timestamp()  );
     // }
    //}


  }
  

}



