#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
//
// Note: from TRuntimeObjects:
//    TList& GetDetectors();
//    TList& GetObjects();
//

#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include "TRandom.h"

#include "TObject.h"
#include "TFastScint.h"



extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TFastScint *fast = obj.GetDetector<TFastScint>();

  if(!fast)
    return;

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  std::string histname;

  histname = "Multiplicity";
  obj.FillHistogram(histname,20,0,20,fast->Size());

  for(int i=0;i<fast->Size();i++) {
    TFastScintHit hit = fast->GetLaBrHit(i);
    
    histname = "Multiplicity_detId";
    obj.FillHistogram(histname,20,0,20,hit.GetChannel(),
                               20,0,20,fast->Size());
  
    histname = "ChannelCharge";
    obj.FillHistogram(histname,4000,0,4000,hit.Charge(),
        20,0,20,hit.GetChannel());

    histname = "ChannelTime-Uncalibrated";
    obj.FillHistogram(histname,64100,-100,64000,hit.Time(),
        20,0,20,hit.GetChannel());

    histname = "ChannelEnergy";
    obj.FillHistogram(histname,4000,0,4000,hit.GetEnergy(),
        20,0,20,hit.GetChannel());

    histname = Form("ChannelEnergy_multi%02i",fast->Size());
    obj.FillHistogram(histname,4000,0,4000,hit.GetEnergy(),
                                20,0,20,hit.GetChannel());

    histname = "ChannelTime-Calibrated";
    obj.FillHistogram(histname,64100,-100,64000,hit.GetTime(),
                               20,0,20,hit.GetChannel());
    
    histname = Form("ChannelTime-Calibrated_mult%02i",fast->Size());
    obj.FillHistogram(histname,64100,-100,64000,hit.GetTime(),
                               20,0,20,hit.GetChannel());
   


    for(int j=0;j<fast->Size();j++) {
      if(i==j) 
        continue;
      TFastScintHit hit2 = fast->GetLaBrHit(j);
      histname = "gamma_gamma";
      std::string dirname = "energy_mat";
      obj.FillHistogram(dirname,histname,4000,0,4000,hit.GetEnergy(),
                                 4000,0,4000,hit2.GetEnergy());
      
      histname = Form("gamma_gamma_mult%i",fast->Size());
      obj.FillHistogram(dirname,histname,4000,0,4000,hit.GetEnergy(),
                                 4000,0,4000,hit2.GetEnergy());

      dirname = "time_mat";
      histname = "time_time";
      obj.FillHistogram(dirname,histname,4000,0,4000,hit.GetTime(),
                                 4000,0,4000,hit2.GetTime());
      
      histname = Form("time_time_multi%i",fast->Size());
      obj.FillHistogram(dirname,histname,4000,0,4000,hit.GetTime(),
                                 4000,0,4000,hit2.GetTime());
      
      histname = "dtime_all";
      obj.FillHistogram(dirname,histname,4000,-2000,2000,hit.GetTime() - hit2.GetTime());

      histname = Form("dtime_%i_%i",hit.GetChannel(),hit2.GetChannel());
      obj.FillHistogram(dirname,histname,4000,-2000,2000,hit.GetTime() - hit2.GetTime());

      dirname = "energy_mat";
      histname = Form("eng%i_eng%i",hit.GetChannel(),hit2.GetChannel());    
      obj.FillHistogram(dirname,histname,4000,0,4000,hit.GetEnergy(),
                                 4000,0,4000,hit2.GetEnergy());

    }
  }
  if(numobj!=list->GetSize())
    list->Sort();
}
