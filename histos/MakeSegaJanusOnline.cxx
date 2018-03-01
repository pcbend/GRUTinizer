#include "TRuntimeObjects.h"

#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TObject.h"
#include "TRandom.h"

#include "GValue.h"
#include "TJanusDDAS.h"
#include "TNSCLScalers.h"
#include "TSega.h"
#include "GCutG.h"

void MakeSega(TRuntimeObjects& obj) {
  TSega* sega = obj.GetDetector<TSega>();
  for(size_t x=0;x<sega->Size();x++) {
    TSegaHit &hit = sega->GetSegaHit(x);
    obj.FillHistogram("sega_summary",16,0,16,hit.GetDetnum(),
                                     3000,0,3000,hit.GetEnergy());
     obj.FillHistogram("segauptime",20,0,20,hit.GetDetnum(),
                                    3600,0,7200,hit.Timestamp()/1e9);
     //obj.FillHistogram(Form("Sega%02i",hit.GetDetnum()),"energy/time",3600,0,7200,hit.Timestamp()/1e9,
     //                                                                 1000,0,4000,hit.GetEnergy());
     obj.FillHistogram(Form("energy_time_%02i",hit.GetDetnum()),3600,0,7200,hit.Timestamp()/1e9,
                                                             100,1400,1500,hit.GetEnergy());
                                                                     
  }
  return; 
}

void MakeRawJanus(TRuntimeObjects& obj) {
  TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();
  std::vector<double> charge;
  for(size_t x=0; x<janus->GetAllChannels().size();x++) {
     TJanusDDASHit hit = janus->GetJanusChannel(x);
     obj.FillHistogram("multi",100,0,100,janus->GetAllChannels().size(),
                               1500,0,15000,hit.Charge());
     obj.FillHistogram("janusuptime",200,0,200,hit.GetFrontChannel(),
                              3600,0,3600,hit.Timestamp()/1e9);



     if(hit.Charge()>150)
         charge.push_back(hit.Charge());
  }

  for(size_t x=0;x<charge.size();x++) {
     obj.FillHistogram("multi_150gated",100,0,100,charge.size(),
                               1500,0,15000,charge.at(x));
  }

  return;
}

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {

  TSega* sega = obj.GetDetector<TSega>();
  TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();

  if(janus) 
    MakeRawJanus(obj);
  if(sega) 
    MakeSega(obj);



}
