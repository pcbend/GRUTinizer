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

float gamma1 = 1173;
float gamma2 = 1332;
float tol1 = 30;
float tol2 = 30;
int   eventnum = 0;

extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TFastScint *fast = obj.GetDetector<TFastScint>();

  if(!fast)
    return;

  eventnum++;
  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  std::string histname;

  histname = "Multiplicity";
  obj.FillHistogram(histname,20,0,20,fast->Size());

  for(unsigned int i=0;i<fast->Size();i++) {
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



    for(unsigned int j=0;j<fast->Size();j++) {
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

      //histname = Form("dtime_%i_%i",hit.GetChannel(),hit2.GetChannel());
      //obj.FillHistogram(dirname,histname,4000,-2000,2000,hit.GetTime() - hit2.GetTime());

      //dirname = "energy_mat";
      //histname = Form("eng%i_eng%i",hit.GetChannel(),hit2.GetChannel());    
      //obj.FillHistogram(dirname,histname,4000,0,4000,hit.GetEnergy(),
      //                           4000,0,4000,hit2.GetEnergy());

      if(abs(hit.GetEnergy() - gamma1) < tol1){
         dirname = "coincidence_energy_gated";
         histname = Form("%5.1f_in_channel_%i__dtime_vs_energy_in_channel_%i",gamma1,hit.GetChannel(),hit2.GetChannel());
         obj.FillHistogram(dirname,histname,4000,-2000,2000,hit.GetTime() - hit2.GetTime(),
                                            4000,0,4000,hit2.GetEnergy());

      }//end if correct energy in first detector

      if(abs(hit2.GetEnergy() - gamma1) < tol1){
         dirname = "coincidence_energy_gated";
         histname = Form("%5.1f_in_channel_%i__dtime_vs_energy_in_channel_%i",gamma1,hit2.GetChannel(),hit.GetChannel());
         obj.FillHistogram(dirname,histname,4000,-2000,2000,hit2.GetTime() - hit.GetTime(),
                                            4000,0,4000,hit.GetEnergy());

      }//end if correct energy in second detector

      if((abs(hit.GetEnergy() - gamma1) < tol1) && (abs(hit2.GetEnergy() - gamma2) < tol2)){

         dirname = "coincidence_energy_gated";
         histname = Form("dtime_energies_%5.1f_and_%5.1f___channels_%i_%i",gamma1,gamma2,hit.GetChannel(),hit2.GetChannel());
         obj.FillHistogram(dirname,histname,4000,-2000,2000,hit.GetTime() - hit2.GetTime());

         histname = "dtime_vs_eventnum";
         obj.FillHistogram(dirname,histname,4000,-2000,2000,hit.GetTime() - hit2.GetTime(),
                                            20000000/200000,0,20000000, eventnum);


      }//end if correct energies in two coincident detectros   

    }//end for loop over j
  }//end for loop over i
  if(numobj!=list->GetSize())
    list->Sort();
}
