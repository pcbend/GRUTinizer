#include "TSega.h"

#include <iostream>

#include "DDASDataFormat.h"
#include "TNSCLEvent.h"

TSega::TSega(){ }

TSega::~TSega(){ }

void TSega::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TSega& sega = (TSega&)obj;
  sega.sega_hits = sega_hits;
  sega.raw_data.clear();
}

void TSega::Clear(Option_t* opt){
  TDetector::Clear(opt);

  sega_hits.clear();
}

void TSega::InsertHit(const TDetectorHit& hit){
  sega_hits.push_back((const TSegaHit&)hit);
  fSize++;
}

TSegaHit& TSega::GetSegaHit(int i){
  return sega_hits.at(i);
}

TDetectorHit& TSega::GetHit(int i){
  return sega_hits.at(i);
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
