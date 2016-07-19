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

#include "TGretina.h"
#include "TS800.h"
#include "TBank29.h"
#include "TS800.h"
#include "GValue.h"


#include "TChannel.h"
#include "GValue.h"

#define Q1 15
#define Q2 7
#define Q3 8
#define Q4 16
#define Q5 9
#define Q6 14
#define Q7 17
#define Q8 6
#define Q9 19

std::map<int,int> HoleQMap;
std::map<int,std::string> LayerMap;

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
  //  TGretina *gretina = obj.GetDetector<TGretina>();
  //TBank29  *bank29  = obj.GetDetector<TBank29>();
  TS800 *s800       = obj.GetDetector<TS800>();
  
  std::string histname = "";
  std::string dirname  = "";


  if(s800){

    double ic_sum = s800->GetIonChamber().GetAve();

    // std::cout << "Dispersive X value: " << s800->GetCrdc(0).GetDispersiveX() << std::endl; 
    // std::cout << "NonDispersive Y value: " << s800->GetCrdc(0).GetNonDispersiveY() << std::endl;

    obj.FillHistogram("s800","CRDC1Y",10000,-5000,5000,s800->GetCrdc(0).GetNonDispersiveY());
    obj.FillHistogram("s800","CRDC2Y",10000,-5000,5000,s800->GetCrdc(1).GetNonDispersiveY());
    obj.FillHistogram("s800","CRDC1X",800,-400,400,s800->GetCrdc(0).GetDispersiveX());
    obj.FillHistogram("s800","CRDC2X",800,-400,400,s800->GetCrdc(1).GetDispersiveX());

    dirname = "MaskCal_gated";
    if(ic_sum > 200){
      histname = "CRDC1_Gated";
      obj.FillHistogram(dirname,histname,
			520,-10,250,s800->GetCrdc(0).GetDispersiveX(),
			500,0,4000,s800->GetCrdc(0).GetNonDispersiveY());
      
      histname = "CRDC2_Gated";
      obj.FillHistogram(dirname,histname,
			520,-10,250,s800->GetCrdc(1).GetDispersiveX(),
			500,0,4000,s800->GetCrdc(1).GetNonDispersiveY());

      histname = "CRDC1_Cal_Gated";
      obj.FillHistogram(dirname,histname,
		        800,-400,400,s800->GetCrdc(0).GetDispersiveX(),
		        3000,-200,200,s800->GetCrdc(0).GetNonDispersiveY());

      histname = "CRDC2_Cal_Gated";
      obj.FillHistogram(dirname,histname,
		        800,-400,400,s800->GetCrdc(1).GetDispersiveX(),
		        3000,-200,200,s800->GetCrdc(1).GetNonDispersiveY());
    }

    histname = "IC_Energy";   
    obj.FillHistogram(histname,1000,-100,4000,ic_sum);               

    dirname = "MaskCal";
    histname = "CRDC1";
    obj.FillHistogram(dirname,histname,
		      520,-10,250,s800->GetCrdc(0).GetDispersiveX(),
		      500,0,4000,s800->GetCrdc(0).GetNonDispersiveY());
      
    histname = "CRDC2";
    obj.FillHistogram(dirname,histname,
		      520,-10,250,s800->GetCrdc(1).GetDispersiveX(),
		      500,0,4000,s800->GetCrdc(1).GetNonDispersiveY());

    histname = "CRDC1_Cal";
    obj.FillHistogram(dirname,histname,
		      520,-400,400,s800->GetCrdc(0).GetDispersiveX(),
		      3000,-200,200,s800->GetCrdc(0).GetNonDispersiveY());

    histname = "CRDC2_Cal";
    obj.FillHistogram(dirname,histname,
		      800,-400,400,s800->GetCrdc(1).GetDispersiveX(),
		      3000,-200,200,s800->GetCrdc(1).GetNonDispersiveY());

    dirname = "GetYOffset";
    histname = "CRDC1_Y_vs_S800Timestamp";
    obj.FillHistogram(dirname,histname,
		      10000,0,6000,s800->GetTimestamp()/1e8,
		      800,-400,400,s800->GetCrdc(0).GetNonDispersiveY());
    
    histname = "CRDC2_Y_vs_S800Timestamp";
    obj.FillHistogram(dirname,histname,
		      10000,0,6000,s800->GetTimestamp()/1e8,
		      800,-400,400,s800->GetCrdc(1).GetNonDispersiveY());
  
    histname = "CRDC1_Y_vs_S800Timestamp_UnCal";
    obj.FillHistogram(dirname,histname,
		      10000,0,6000,s800->GetTimestamp()/1e8,
		      800,-4000,4000,s800->GetCrdc(0).GetNonDispersiveY());
    
    histname = "CRDC2_Y_vs_S800Timestamp_UnCal";
    obj.FillHistogram(dirname,histname,
		      10000,0,6000,s800->GetTimestamp()/1e8,
		      800,-4000,4000,s800->GetCrdc(1).GetNonDispersiveY());
    
      dirname = "InverseMap";
     
      histname = "S800_YTA";
      obj.FillHistogram(dirname,histname,
			1000,-50,50,s800->GetYta());
      
      histname = "S800_DTA";
      obj.FillHistogram(dirname,histname,
			1000,-0.2,0.2,s800->GetDta());
      
      histname = "ATA";
      obj.FillHistogram(dirname,histname,
                        1000,-0.2,0.2,s800->GetAta());
      
      histname = "BTA";
      obj.FillHistogram(dirname,histname,
                        1000,-0.2,0.2,s800->GetBta());
      
      histname = "ATA_vs_BTA";
      obj.FillHistogram(dirname,histname,
			1000,-0.2,0.2,s800->GetAta(),
			1000,-0.2,0.2,s800->GetBta());
	    
    
  }


}
