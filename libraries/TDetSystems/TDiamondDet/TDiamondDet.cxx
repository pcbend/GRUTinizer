#include "TDiamondDet.h"

#include <algorithm>
#include <iostream>
#include <fstream>

#include "DDASDataFormat.h"
#include "TNSCLEvent.h"
#include "TChannel.h"
#include "TRawEvent.h"

TDiamondDet::TDiamondDet(){ }

TDiamondDet::~TDiamondDet(){ }

void TDiamondDet::Copy(TObject& obj) const {
  TDetector::Copy(obj);
  TDiamondDet& diamond = (TDiamondDet&)obj;
  diamond.diamond_hits = diamond_hits;
}

void TDiamondDet::Clear(Option_t* opt){
  TDetector::Clear(opt);
  diamond_hits.clear();
}

void TDiamondDet::Draw(Option_t *opt) {

}

TDiamondDetHit& TDiamondDet::GetDiamondDetHit(int i){
  return diamond_hits.at(i);
}

TDetectorHit& TDiamondDet::GetHit(int i){
  return diamond_hits.at(i);
}

int TDiamondDet::BuildHits(std::vector<TRawEvent>& raw_data) {

  unsigned long smallest_timestamp = 0x7fffffffffffffff;
  for(auto& event : raw_data){
    TSmartBuffer buf = event.GetPayloadBuffer();
    TDDASEvent<DDASHeader> ddas(buf);
    unsigned int address = ( (5<<24) + (ddas.GetCrateID()<<16) + (ddas.GetSlotID()<<8) + ddas.GetChannelID() );
    TChannel* chan = TChannel::GetChannel(address);
    static int lines_displayed = 0;
    if(!chan){
      if(lines_displayed < 10) {
        std::cout << "Unknown Diamond (crate, slot, channel): (" << ddas.GetCrateID() << ", " << ddas.GetSlotID() << ", " << ddas.GetChannelID() << ")" << std::endl;
      }
      lines_displayed++;
      continue;
    }
    // Get a hit, make it if it does not exist
    TDiamondDetHit hit;
    hit.SetAddress(address);
    hit.SetTimestamp(ddas.GetTimestamp()*10); // this is now in ns pcb!!
    if(hit.Timestamp()<smallest_timestamp) { smallest_timestamp = hit.Timestamp(); }
    hit.SetCharge(ddas.GetEnergy());

    diamond_hits.push_back(hit);
  }
  //set the TDiamondDet  time....
  SetTimestamp(smallest_timestamp);  //fix me pcb
  return Size();
}

void TDiamondDet::InsertHit(const TDetectorHit& hit) {
  diamond_hits.emplace_back((TDiamondDetHit&)hit);
  fSize++;
}

void TDiamondDet::SortHitsByTimestamp() {
  std::sort(diamond_hits.begin(), diamond_hits.end(), [](const TDiamondDetHit& a, const TDiamondDetHit& b) {
    return a.Timestamp() < b.Timestamp();
  });
}

