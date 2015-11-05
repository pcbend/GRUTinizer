#include "TSega.h"

#include <iostream>

#include "TClonesArray.h"

#include "DDASDataFormat.h"
#include "TNSCLEvent.h"

TSega::TSega(){
  sega_hits = new TClonesArray("TSegaHit");
}

TSega::~TSega(){
  delete sega_hits;
}

void TSega::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TSega& sega = (TSega&)obj;
  sega_hits->Copy(*sega.sega_hits);
  sega.raw_data.clear();
}

void TSega::Clear(Option_t* opt){
  TDetector::Clear(opt);

  sega_hits->Clear(opt);
}

void TSega::InsertHit(const TDetectorHit& hit){
  TSegaHit* new_hit = (TSegaHit*)sega_hits->ConstructedAt(Size());
  hit.Copy(*new_hit);
  fSize++;
}

TSegaHit& TSega::GetSegaHit(int i){
  return *(TSegaHit*)sega_hits->At(i);
}

TDetectorHit& TSega::GetHit(int i){
  return *(TSegaHit*)sega_hits->At(i);
}

int TSega::BuildHits() {
  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;

    SetTimestamp(nscl.GetTimestamp());

    TDDASEvent ddas(nscl.GetPayloadBuffer());
    TSegaHit hit;

    hit.SetTimestamp(nscl.GetTimestamp());

    hit.SetChannel(ddas.GetChannelID());
    hit.SetSlot(ddas.GetSlotID());
    hit.SetCrate(ddas.GetCrateID());

    hit.SetCharge(ddas.GetEnergy());
    hit.SetTrace(ddas.GetTraceLength(), ddas.trace);

    InsertHit(hit);
  }
}
