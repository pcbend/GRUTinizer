#include "TGenericDDAS.h"

#include <algorithm>
#include <iostream>
#include <fstream>

#include "DDASDataFormat.h"
#include "DDASBanks.h"
#include "TNSCLEvent.h"
#include "TRawEvent.h"
#include "TChannel.h"
#include "TGRUTOptions.h"


bool TGenericDDAS::fFileDetermined = false;
bool TGenericDDAS::fGEB = false;

TGenericDDAS::TGenericDDAS(){ }

TGenericDDAS::~TGenericDDAS(){ }

void TGenericDDAS::Copy(TObject& obj) const {
  TDetector::Copy(obj);
  TGenericDDAS& ddas= (TGenericDDAS&)obj;
  ddas.ddas_hits = ddas_hits;
}

void TGenericDDAS::Clear(Option_t* opt){
  TDetector::Clear(opt);
  ddas_hits.clear();
}

void TGenericDDAS::Draw(Option_t *opt) {

}

TGenericDDASHit& TGenericDDAS::GetDDASHit(int i){
  return ddas_hits.at(i);
}

TDetectorHit& TGenericDDAS::GetHit(int i){
  return ddas_hits.at(i);
}

int TGenericDDAS::BuildHits(std::vector<TRawEvent>& raw_data) {

  if(!fFileDetermined) {
    kFileType fFileType = raw_data.at(0).GetFileType();
    switch(fFileType) {
      case GRETINA_MODE2:
	fGEB = true;
        break;
      case NSCL_EVT:
	fGEB = false;
        break;
      default:
        break;
    }
  }

  for(auto& event : raw_data){
    TSmartBuffer buf = event.GetPayloadBuffer();
//    TDDASEvent<(fGEB==true ? DDASHeader:DDASGEBHeader)> ddasevt(buf);
    TDDASEvent<DDASHeader> ddasevt(buf);
    unsigned int address = ( (ddasevt.GetCrateID()<<16) + (ddasevt.GetSlotID()<<8) + ddasevt.GetChannelID() );
/*    TChannel* chan = TChannel::GetChannel(address);
    static int lines_displayed = 0;
    if(!chan){
      if(lines_displayed < 10) {
        std::cout << "Unknown DDAS (crate, slot, channel): (" << ddasevt.GetCrateID() << ", " << ddasevt.GetSlotID() << ", " << ddasevt.GetChannelID() << ")" << std::endl;
      }
      lines_displayed++;
      continue;
    }
*/
    // Get a hit, make it if it does not exist
    TGenericDDASHit hit;
    hit.SetAddress(address);
    hit.SetDetectorNumber(ddasevt.GetChannelID()+ddasevt.GetSlotID()*16);
//    hit.SetDetectorNumber(chan->GetArrayPosition());
    hit.SetCharge(ddasevt.GetEnergy());
    hit.SetTimestamp(ddasevt.GetTimestamp()); // this is now in ns pcb!!
    hit.SetTimeFull(ddasevt.GetTime()); // Timestamp + CFD
    hit.SetCFDTime(ddasevt.GetCFDTime()); // CFD ONLY
//    hit.SetTrace(ddasevt.GetTraceLength(), ddasevt.trace);
    ddas_hits.push_back(hit);
  }
  return Size();
}

void TGenericDDAS::InsertHit(const TDetectorHit& hit) {
  ddas_hits.emplace_back((TGenericDDASHit&)hit);
  fSize++;
}

void TGenericDDAS::SortHitsByTimestamp() {
  std::sort(ddas_hits.begin(), ddas_hits.end(), [](const TGenericDDASHit& a, const TGenericDDASHit& b) {
    return a.Timestamp() < b.Timestamp();
  });
}

