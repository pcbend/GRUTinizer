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

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {

  TSega* sega = obj.GetDetector<TSega>();
  TJanusDDAS* janus = obj.GetDetector<TJanusDDAS>();

  if(!janus) return;

  std::vector<double> charge;
  for(size_t x=0; x<janus->GetAllChannels().size();x++) {
     TJanusDDASHit hit = janus->GetAllChannels().at(x);
     obj.FillHistogram("multi",100,0,100,janus->GetAllChannels().size(),
                               1500,0,15000,hit.Charge());
     if(hit.Charge()>150)
         charge.push_back(hit.Charge());
  }

  for(size_t x=0;x<charge.size();x++) {
     obj.FillHistogram("multi_150gated",100,0,100,charge.size(),
                               1500,0,15000,charge.at(x));
  }
}
