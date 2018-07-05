
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

double beta =0.02;

TCutG* timing=0;
TCutG* pp_timing=0;


void HandleFSU(TRuntimeObjects &obj,TCutG *cut) {
  TFSU *fsu = obj.GetDetector<TFSU>();
 
  //std::cout << "IN MAKE HISTOS!! " << std::endl; 
  if(!fsu) return;
  //fsu->Print("all");


  std::string dirname ="";
  if(cut) dirname=cut->GetName();

  if(!fsu->GoodParticle()) return;
  obj.FillHistogram(dirname, "pid_all_clean",4000,0,24000,fsu->GetDeltaE().Charge(),
                                             4000,0,16000,fsu->GetE().Charge());
 
  //fsu->Print("addback");

 
  if(!cut->IsInside(fsu->GetDeltaE().Charge(),fsu->GetE().Charge())) { 
    //std::cout << "failed " << cut->GetName() << " pid cut!" << std::endl;
    return;
  } 
  //std::cout << "passed " << cut->GetName() << " pid cut!" << std::endl;

  //for(size_t i=0;i<fsu->Size();i++) {
  //  TFSUHit hit = fsu->GetFSUHit(i);
  for(size_t i=0;i<fsu->AddbackSize();i++) {
    TFSUHit hit = fsu->GetAddbackHit(i);

    if(!TChannel::GetChannel(hit.Address())) continue;
    if(hit.Charge()<0 || hit.Charge()>30000) continue;   // if the gamma hit has good charge
    
    double time = fsu->GetDeltaE().Timestamp()-hit.Timestamp();

    obj.FillHistogram(Form("dop_%s",cut->GetName()),10000,0,5000,hit.GetDoppler(beta));

    obj.FillHistogram(dirname,"summary_cal",10000,0,5000,hit.GetEnergy(),
                                               200,0,200,hit.Id());
    
    obj.FillHistogram(dirname,"summary_dop",10000,0,5000,hit.GetDoppler(beta),
                                200,0,200,hit.Id());
    
    obj.FillHistogram(dirname,"angle_dop",180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
                                          10000,0,5000,hit.GetDoppler(beta));
    obj.FillHistogram(dirname,"angle_cal",180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
                                          10000,0,5000,hit.GetEnergy());
    obj.FillHistogram(dirname,"DeltaT",1000,-500,500,time,
                                2000,0,5000,hit.GetEnergy());
    
    obj.FillHistogram(dirname,"DeltaT",1000,-500,500,time,
                               2000,0,5000,hit.GetEnergy());

    //obj.FillHistogram("Angle",180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
    //                          5000,0,5000,hit.GetEnergy());

    //for(size_t j=i+1;j<fsu->Size();j++) {
    //  TFSUHit hit2 = fsu->GetFSUHit(j);
    for(size_t j=i+1;j<fsu->AddbackSize();j++) {
      TFSUHit hit2 = fsu->GetAddbackHit(j);
      if(!TChannel::GetChannel(hit2.Address())) continue;
      obj.FillHistogramSym(Form("ggmat_%s",cut->GetName()),5000,0,5000,hit.GetEnergy(),
                                                           5000,0,5000,hit2.GetEnergy());
    }
  }

  obj.FillHistogram(dirname,"pid",4000,0,16000,fsu->GetDeltaE().Charge(),
                          4000,0,16000,fsu->GetE().Charge());
  obj.FillHistogram("pid",4000,0,24000,fsu->GetDeltaE().Charge(),
                          4000,0,16000,fsu->GetE().Charge());

    
  double time = fsu->GetDeltaE().Timestamp()-fsu->GetE().Timestamp();
  obj.FillHistogram("pp_time",1000,-500,500,time,
                              4000,0,32000,fsu->GetDeltaE().Charge());
 

}




void HandleFSU(TRuntimeObjects &obj) {
  TFSU *fsu = obj.GetDetector<TFSU>();
  if(!fsu) return;

  for(size_t i=0;i<fsu->Size();i++) {
    TFSUHit hit = fsu->GetFSUHit(i);

    if(!TChannel::GetChannel(hit.Address())) continue;
    obj.FillHistogram("summary_cal",10000,0,5000,hit.GetEnergy(),
                                200,0,200,hit.Id());
    
    obj.FillHistogram("summary_dop",10000,0,5000,hit.GetDoppler(beta),
                                200,0,200,hit.Id());
    
    obj.FillHistogram("summary_raw",8000,0,16000,hit.Charge(),
                                200,0,200,hit.Id());
    
    //obj.FillHistogram("angle_dop",180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
    //                              10000,0,5000,hit.GetDoppler());
    
    obj.FillHistogram("angle_cal",180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
                                          10000,0,5000,hit.GetEnergy());
//    for(size_t j=i+1;j<fsu->Size();j++) {
//      TFSUHit hit2 = fsu->GetFSUHit(j);
//      if(!TChannel::GetChannel(hit2.Address())) continue;
//      obj.FillHistogramSym("ggmat",5000,0,5000,hit.GetEnergy(),
//                                   5000,0,5000,hit2.GetEnergy());
//    }
//    obj.FillHistogram("Angle",180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
//                              5000,0,5000,hit.GetEnergy());
  }

  obj.FillHistogram("pid_all_dirty",4000,0,24000,fsu->GetDeltaE().Charge(),
                                    4000,0,16000,fsu->GetE().Charge());

  

}










// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  TFSU  *fsu = obj.GetDetector<TFSU>();
  TList *list  = &(obj.GetObjects());
  TList *gates = &(obj.GetGates());
  int numobj = list->GetSize();

  if(!fsu || !fsu->Size()) {
    return;
  } 
  if(!timing) {
    TIter it(gates);
    while(TCutG *gate =(TCutG*)it.Next()) {
      if(strncmp(gate->GetName(),"timing",6)==0) {
        printf("loaded timing\n"); fflush(stdout);
        timing = gate;
        //break;
      }else if(strncmp(gate->GetName(),"pp_timing",9)==0) {
        printf("loaded pp_timing\n"); fflush(stdout);
        pp_timing = gate;
      }
    }
  }
  
  HandleFSU(obj); 
  //fsu->WriteToEv2("blah");
 
  fsu->CleanHits(timing,pp_timing);
  if(!fsu->Size()) return;
  fsu->MakeAddbackHits();
 
  TIter it(gates);
  while(TCutG *gate =(TCutG*)it.Next()) {
    if(strncmp(((GCutG*)gate)->GetTag(),"pid",3)!=0) continue;
    HandleFSU(obj,gate);
    //if(timing) HandleFSU(obj,gate,timing);
  }
  

  if(numobj!=list->GetSize())
    list->Sort();
}
