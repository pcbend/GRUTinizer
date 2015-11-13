#include "TSega.h"

#include <iostream>

#include "DDASDataFormat.h"
#include "TNSCLEvent.h"
#include "TChannel.h"

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

TSegaHit& TSega::GetSegaHit(int i){
  return sega_hits.at(i);
}

TDetectorHit& TSega::GetHit(int i){
  return sega_hits.at(i);
}

TSegaHit& GetOrMakeHit(int detnum) {

}

int TSega::BuildHits() {
  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;

    SetTimestamp(nscl.GetTimestamp());

    TDDASEvent ddas(nscl.GetPayloadBuffer());

    unsigned int address = ( (1<<24) +
                             (ddas.GetCrateID()<<16) +
                             (ddas.GetSlotID()<<8) +
                             ddas.GetChannelID() );
    TChannel* chan = TChannel::GetChannel(address);
    if(!chan){
      std::cout << "Unknown (crate, slot, channel): ("
                << ddas.GetCrateID() << ", " << ddas.GetSlotID()
                << ", " << ddas.GetChannelID() << std::endl;
      continue;
    }

    int detnum = chan->GetArrayPosition();
    int segnum = chan->GetSegment();

    // Get a hit, make it if it does not exist
    TSegaHit* hit = NULL;
    for(auto& ihit : sega_hits){
      if(ihit.GetDetnum() == detnum){
        hit = &ihit;
        break;
      }
    }
    if(hit == NULL){
      sega_hits.emplace_back();
      hit = &sega_hits.back();
      fSize++;
    }

    if(segnum==0){
      hit->SetAddress(address);
      hit->SetTimestamp(nscl.GetTimestamp());
      hit->SetCharge(ddas.GetEnergy());
      hit->SetTrace(ddas.GetTraceLength(), ddas.trace);
    } else {
      TSegaSegmentHit& seg = hit->MakeSegmentByAddress(address);
      seg.SetCharge(ddas.GetEnergy());
      seg.SetTimestamp(ddas.GetTimestamp());
      seg.SetTrace(ddas.GetTraceLength(), ddas.trace);
    }


  }
}
