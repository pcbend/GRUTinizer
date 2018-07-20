
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

#include "TFSU.h"
#include "GCutG.h"

#include "TChannel.h"
#include "GValue.h"

TCutG *triton = 0;
TCutG *alpha = 0;
TCutG *deuteron = 0;
TCutG *proton = 0;
TCutG* timing=0;
TCutG* timing2=0;


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
	TFSU  *fsu = obj.GetDetector<TFSU>();
	TList *list  = &(obj.GetObjects());
	TList *gates = &(obj.GetGates());
	int numobj = list->GetSize();
	if(numobj!=list->GetSize())
		list->Sort();

	if(!fsu) {
		return;
	}

	if(!triton || !alpha || !deuteron || !proton) {
		TIter it(gates);
		while(TCutG* cut = (TCutG*)it.Next()) {
			if(!strcmp(cut->GetName(),"triton")) {
				triton = cut;		
			}else if(!strcmp(cut->GetName(),"alpha")) {
                                alpha = cut;
			}
			 else if(!strcmp(cut->GetName(),"deuteron")) {
                                deuteron = cut;
			}
			 else if(!strcmp(cut->GetName(),"proton")) {
                                proton = cut;
			}
			//else if(!strcmp(cut->GetName(),"timing")) {
                          //      timing = cut;
			//}
		}
	}


	obj.FillHistogram("DeltaE",4000,0,16000,fsu->GetDeltaE().Charge());
	obj.FillHistogram("Energy",4000,0,16000,fsu->GetE().Charge());

 
	//if(strcmp(TFSUHit().Address(),"0x20")==0)
	//	obj.FillHistogram("Gammas",4000,0,16000,TFSUHit().Charge());
        //for (unsigned int x = 0; x<fsu->Size();x++){
	//TFSUHit hit = fsu->GetFSUHit(x);
	

	obj.FillHistogram("pid",1000,0,16000,fsu->GetE().Charge(),
			1000,0,16000,fsu->GetDeltaE().Charge());


	for (unsigned int x = 0; x<fsu->Size();x++){
		TFSUHit hit = fsu->GetFSUHit(x);
                obj.FillHistogram("summary",4000,0,32000,hit.Charge(),
                                            200,0,200,hit.Id());
		if(strncmp(hit.GetName(),"Monitor",7)==0)
			obj.FillHistogram("Monitor",4000,0,16000,hit.Charge());
   	        if(strncmp(hit.GetName(),"FSU7",4)==0)
			obj.FillHistogram("Gammas",4000,0,16000,hit.Charge());
	         
	}

	if(fsu->GetE().Charge()>1){
		double totalcharge = fsu->GetE().Charge()+fsu->GetDeltaE().Charge();
		obj.FillHistogram("pidtotal",1000,0,16000,totalcharge,
				1000,0,16000,fsu->GetDeltaE().Charge());
		double scaledcharge = fsu->GetE().Charge()*GValue::Value("scale")+fsu->GetDeltaE().Charge();
		obj.FillHistogram("pidcorrected",4000,0,64000,totalcharge,
				4000,0,16000,fsu->GetDeltaE().Charge());

		double totalenergy=scaledcharge*GValue::Value("gain")+GValue::Value("offset");

		obj.FillHistogram("pidenergy",1000,0,35,totalenergy,
				1000,0,25000,fsu->GetDeltaE().Charge());
		double totaltime = fsu->GetE().Timestamp()-fsu->GetDeltaE().Timestamp();
		obj.FillHistogram("times",1000,-500,500,totaltime);
		obj.FillHistogram("2Dtimes",1000,-500,500,totaltime,8000,0,35,totalenergy);


		if(triton && triton->IsInside(totalenergy,fsu->GetDeltaE().Charge())) {
			obj.FillHistogram("tritons",8000,0,32000,totalcharge);
			obj.FillHistogram("tritoncorrected",1000,0,35,totalenergy);
			obj.FillHistogram("tritontimes",1000,-500,500,totaltime,8000,0,35,totalenergy);
		}
		if(alpha && alpha->IsInside(totalenergy,fsu->GetDeltaE().Charge())) {
			obj.FillHistogram("alphas",8000,0,32000,totalcharge);
			obj.FillHistogram("alphacorrected",1000,0,35,totalenergy);
			obj.FillHistogram("alphatimes",1000,-500,500,totaltime,8000,0,35,totalenergy);
		}
		if(deuteron && deuteron->IsInside(totalenergy,fsu->GetDeltaE().Charge())) {
			obj.FillHistogram("deuterons",20,0,20,totalcharge);
			obj.FillHistogram("deuteroncorrected",1000,0,35,totalenergy);
			obj.FillHistogram("deuterontimes",1000,-500,500,totaltime,8000,0,35,totalenergy);
		}
		if(proton && proton->IsInside(totalenergy,fsu->GetDeltaE().Charge())) {
			obj.FillHistogram("protons",20,0,20,totalcharge);
			obj.FillHistogram("protoncorrected",1000,0,35,totalenergy);
			obj.FillHistogram("protontimes",1000,-500,500,totaltime,8000,0,35,totalenergy);
		}
		if(timing && timing->IsInside(totaltime,totalenergy)) {
		obj.FillHistogram("pidproton",1000,0,35,totalenergy,
				1000,0,25000,fsu->GetDeltaE().Charge());
		}
		if(timing2 && timing2->IsInside(totaltime,totalenergy)) {
		obj.FillHistogram("timecut",1000,0,35,totalenergy,
				1000,0,25000,fsu->GetDeltaE().Charge());
		}
	}

	//TList *gates = &(obj.GetGates());
	 if(!timing) {
    TIter it(gates);
    while(TCutG *gate =(TCutG*)it.Next()) {
      if(strncmp(gate->GetName(),"time",4)==0) {
        timing = gate;
        break;
      }
    }
  }
if(!timing2) {
    TIter it(gates);
    while(TCutG *gate =(TCutG*)it.Next()) {
      if(strncmp(gate->GetName(),"time2",5)==0) {
        timing2 = gate;
        break;
      }
    }
  }
	// TIter it(gates);
  //while(TCutG *gate =(TCutG*)it.Next()) {
    //if(strncmp(gate->GetName(),"time",4)==0) continue;
    //HandleFSU(obj,gate);
    //if(timing) HandleFSU(obj,gate,timing);
  //}	

	
	
}
