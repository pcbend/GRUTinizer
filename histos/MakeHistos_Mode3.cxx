#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>
#include <TLine.h>

#include "TMode3.h"
#include "TBank88.h"
#include "GValue.h"


#include "TChannel.h"
#include "GValue.h"


//#define Q1 15
//#define Q2 7
//#define Q3 8
//#define Q4 16
//#define Q5 9
//#define Q6 14
//#define Q7 17
//#define Q8 6
//#define Q9 19

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
#define Q12 20



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
  HoleQMap[Q12] = 12;

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
  //TGretina *gretina = obj.GetDetector<TGretina>();
  TMode3 *mode3     = obj.GetDetector<TMode3>();
  //TBank88  *bank29  = obj.GetDetector<TBank88>();
  TList *list = &(obj.GetObjects());

  int numobj = list->GetSize();
  if(!mode3)
    return;

  for(unsigned int x=0;x<mode3->Size();x++) {
    TMode3Hit hit = mode3->GetMode3Hit(x);
    int hole = HoleQMap[hit.GetHole()];
    std::string dirname  = Form("SummaryAll");
    std::string histname = Form("Q%i_detmap",hole); ////;channel;Charge/128.",hole);
    std::string histname2 = Form("Q%i_detmap_led_2D",hole); ////;channel;Charge/128.",hole);
    obj.FillHistogram(dirname,histname,
                      160,0,160,hit.GetAbsSegId(),
                      8000,0,64000,hit.Charge());  // int division done in the mode3hit class.

// was
                    //  8000,0,32000,hit.Charge());  // int division done in the mode3hit class.

    //obj.FillHistogram(dirname,histname2,
      //                160,0,160,hit.GetAbsSegId(),
        //              1e6,1000,2000,hit.GetLed()/(1e9)); // previously 1e12


      //histname2 = Form("Q%i_detmap_led",hole); ////;channel;Charge/128.",hole);
      //obj.FillHistogram(dirname,histname2,
        //                20,0,20,hit.GetCrystal()*4 + hit.GetVME(),
//                        8000,0,32000,hit.GetLed());

    if(hit.GetChannel()%9==0) {
      dirname  = Form("SummaryCore");
      histname = Form("Q%i_cores",hole); ////;channel;Charge/128.",hole);
      obj.FillHistogram(dirname,histname,
                        20,0,20,hit.GetCrystal()*4 + hit.GetVME(),
                        16000,0,64000,hit.Charge());

     // histname2 = Form("Q%i_cores_led",hole); ////;channel;Charge/128.",hole);
     // obj.FillHistogram(dirname,histname2,
       //                 20,0,20,hit.GetCrystal()*4 + hit.GetVME(),
         //               2500,100000,200000,hit.GetLed());

      histname2 = Form("Q%i_cores_led_2D",hole); ////;channel;Charge/128.",hole);
     //obj.FillHistogram(dirname,histname2,
       //                 0.0000002,0,1000,hit.GetLed()/(1e12),
         //               20,0,20,hit.GetCrystal()*4 + hit.GetVME());

    // histname2 = Form("Q%i_cores_led_1D",hole); ////;channel;Charge/128.",hole);
    // obj.FillHistogram(dirname,histname2,
      //                 200,34800,54800,hit.GetLed()/(1e7),64000,0,64000,hit.Charge());

    //std::cout << "LED: " << hit.GetLed()/(1e12) << std::endl ;

        //obj.FillHistogram("DT1",
          //                10000,0,10000,hit.GetDt1());

        //obj.FillHistogram("DT2",
          //                10000,0,10000,hit.GetDt2());
    }


    if(TChannel::Size()) {
      histname = Form("Q%i_cal",hole); ////;channel;Charge/128.",hole);
      obj.FillHistogram(dirname,histname,
                        160,0,160,hit.GetAbsSegId(),
                        5000,0,10000,hit.GetEnergy());
      if(hit.GetChannel()%9==0) {
        histname = Form("Q%i_cores",hole); ////;channel;Charge/128.",hole);
        obj.FillHistogram(histname,
                          20,0,20,hit.GetCrystal()*4 + hit.GetVME(),
                          10000,0,10000,hit.GetEnergy());


        //if(hit.GetVME()==2) {  //2.5 MeV channel
         // if(hit.GetDt1()>0) {
          //  histname = Form("Q%i_2MeV_Energy_vs_d1",hole); ////;channel;Charge/128.",hole);
           // obj.FillHistogram(dirname,histname,
            //                  2000,0,2000,hit.GetEnergy(),
       
/*                       2000,0,2000,hit.GetDt1());}}
        if(hit.GetVME()==1) {  //check1
          if(hit.GetDt1()>0) {
            histname = Form("Q%i_check1_Energy_vs_d1",hole); ////;channel;Charge/128.",hole);
            obj.FillHistogram(dirname,histname,
                              2000,0,2000,hit.GetEnergy(),
                              2000,0,2000,hit.GetDt1());}}

        if(hit.GetVME()==3) {  //check2
          if(hit.GetDt1()>0) {
            histname = Form("Q%i_check2_Energy_vs_d1",hole); ////;channel;Charge/128.",hole);
            obj.FillHistogram(dirname,histname,
                              2000,0,2000,hit.GetEnergy(),
                              2000,0,2000,hit.GetDt1());}}

        if(hit.GetVME()==4) {  //check3
          if(hit.GetDt1()>0) {
            histname = Form("Q%i_check3_Energy_vs_d1",hole); ////;channel;Charge/128.",hole);
            obj.FillHistogram(dirname,histname,
                              2000,0,2000,hit.GetEnergy(),
                             2000,0,2000,hit.GetDt1());}}
*/

          //}
        //  if(hit.GetDt2()>0) {
        //    histname = Form("Q%i_2MeV_Energy_vs_d2",hole); ////;channel;Charge/128.",hole);
        //    obj.FillHistogram(dirname,histname,
        //                      2000,0,2000,hit.GetEnergy(),
        //                      2000,0,2000,hit.GetDt2());
        //  }
        //}


      }

    }
  }
  if(numobj!=list->GetSize())
    list->Sort();
}
