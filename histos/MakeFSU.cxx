
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


void HandleFSU(TRuntimeObjects obj,TCutG *cut) {
  TFSU *fsu = obj.GetDetector<TFSU>();
  if(!fsu) return;
  if(!fsu->GoodParticle()) return;
  //fsu->Print("all");
  if(!cut->IsInside(fsu->GetDeltaE().Charge(),fsu->GetE().Charge())) return;

  std::string dirname ="";
  if(cut) dirname=cut->GetName();


  for(size_t i=0;i<fsu->Size();i++) {
    TFSUHit hit = fsu->GetFSUHit(i);
    if(!TChannel::GetChannel(hit.Address())) continue;
    obj.FillHistogram(dirname,"summary_cal",10000,0,5000,hit.GetEnergy(),
                                200,0,200,hit.Id());
    
    obj.FillHistogram(dirname,"summary_dop",10000,0,5000,hit.GetDoppler(),
                                200,0,200,hit.Id());
    
    obj.FillHistogram(dirname,"angle_dop",180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
                                          10000,0,5000,hit.GetDoppler());
    
    obj.FillHistogram(dirname,"angle_cal",180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
                                          10000,0,5000,hit.GetEnergy());
 
    obj.FillHistogram(dirname,"summary_raw",8000,0,16000,hit.Charge(),
                                200,0,200,hit.Id());
  }

  obj.FillHistogram(dirname,"pid",4000,0,16000,fsu->GetDeltaE().Charge(),
                          4000,0,16000,fsu->GetE().Charge());
}


void HandleFSU(TRuntimeObjects obj) {
  TFSU *fsu = obj.GetDetector<TFSU>();
  if(!fsu) return;

  for(size_t i=0;i<fsu->Size();i++) {
    TFSUHit hit = fsu->GetFSUHit(i);
    if(!TChannel::GetChannel(hit.Address())) continue;
    obj.FillHistogram("summary_cal",10000,0,5000,hit.GetEnergy(),
                                200,0,200,hit.Id());
    
    obj.FillHistogram("summary_raw",8000,0,16000,hit.Charge(),
                                200,0,200,hit.Id());
    
    obj.FillHistogram("angle_dop",180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
                                  10000,0,5000,hit.GetDoppler());
    
    obj.FillHistogram("angle_cal",180,0,180,hit.GetPosition().Theta()*TMath::RadToDeg(),
                                          10000,0,5000,hit.GetEnergy());

  }

  obj.FillHistogram("pid",4000,0,16000,fsu->GetDeltaE().Charge(),
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

  if(!fsu) {
    return;
  } 

  HandleFSU(obj); 
  TIter it(gates);
  while(TCutG *gate =(TCutG*)it.Next()) {
    HandleFSU(obj,gate);
  }
  

  if(numobj!=list->GetSize())
    list->Sort();
}
