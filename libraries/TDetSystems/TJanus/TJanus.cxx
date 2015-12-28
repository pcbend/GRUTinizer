#include "TJanus.h"

#include <cassert>
#include <iostream>

#include "TMath.h"

#include "JanusDataFormat.h"
#include "TNSCLEvent.h"

TJanus::TJanus() { }

TJanus::~TJanus(){ }

void TJanus::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TJanus& janus = (TJanus&)obj;
  janus.janus_hits = janus_hits;
  janus.raw_data.clear();
}

void TJanus::Clear(Option_t* opt){
  TDetector::Clear(opt);

  janus_hits.clear();
}

int TJanus::BuildHits(){
  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    Build_VMUSB_Read(nscl.GetPayloadBuffer());
  }
  return janus_hits.size();
}

TJanusHit& TJanus::GetJanusHit(int i){
  return janus_hits.at(i);
}

TDetectorHit& TJanus::GetHit(int i){
  return janus_hits.at(i);
}


void TJanus::Build_VMUSB_Read(TSmartBuffer buf){
  const char* data = buf.GetData();

  const VMUSB_Header* vmusb_header = (VMUSB_Header*)data;
  data += sizeof(VMUSB_Header);

  // vmusb_header.size() returns the number of 16-bit words in the payload.
  // Each adc entry is a 32-bit word.
  // 3 additional 16-bit words for the timestamp
  int num_packets = vmusb_header->size()/2 - 3;

  const VME_Timestamp* vme_timestamp = (VME_Timestamp*)(data + num_packets*sizeof(CAEN_DataPacket));
  long timestamp = vme_timestamp->ts1() * 20;

  std::map<unsigned int,TJanusHit> front_hits;
  std::map<unsigned int,TJanusHit> back_hits;
  for(int i=0; i<num_packets; i++){
    const CAEN_DataPacket* packet = (CAEN_DataPacket*)data;
    data += sizeof(CAEN_DataPacket);

    if(!packet->IsValid()){
      continue;
    }

    // ADCs are in slots 5-8, TDCs in slots 9-12
    bool is_tdc = packet->card_num() >= 9;
    unsigned int adc_cardnum = packet->card_num();
    if(is_tdc){
      adc_cardnum -= 4;
    }
    unsigned int address =
      (2<<24) + //system id
      (4<<16) + //crate id
      (adc_cardnum<<8) +
      packet->channel_num();

    TChannel* chan = TChannel::GetChannel(address);
    // Bad stuff, tell somebody to fix it
    static int lines_displayed = 0;
    if(!chan){
      if(lines_displayed < 1000) {
        std::cout << "Unknown analog (slot, channel): ("
                  << adc_cardnum << ", " << packet->channel_num()
                  << ")" << std::endl;
      } else if(lines_displayed==1000){
        std::cout << "I'm going to stop telling you that the channel was unknown,"
                  << " you should probably stop the program." << std::endl;
      }
      lines_displayed++;
      continue;
    }

    TJanusHit* hit = NULL;
    if(*chan->GetArraySubposition() == 'F'){
      hit = &front_hits[address];
    } else {
      hit = &back_hits[address];
    }

    hit->SetAddress(address);
    hit->SetTimestamp(timestamp);

    if(is_tdc){
      hit->SetTDCOverflowBit(packet->overflow());
      hit->SetTDCUnderflowBit(packet->underflow());
      hit->SetTime(packet->adcvalue());
    } else {
      hit->SetADCOverflowBit(packet->overflow());
      hit->SetADCUnderflowBit(packet->underflow());
      hit->SetCharge(packet->adcvalue());
    }
  }

  // //DANGER, take this out later
  // for(auto& elem : front_hits){
  //   TJanusHit& hit = elem.second;
  //   janus_hits.emplace_back(hit);
  //   fSize++;
  // }
  // for(auto& elem : back_hits){
  //   TJanusHit& hit = elem.second;
  //   janus_hits.emplace_back(hit);
  //   fSize++;
  // }
  // return;
  // //END OF DANGER SECTION

  // Find all fronts with a reasonable TDC value
  std::vector<unsigned int> good_fronts;
  for(auto& elem : front_hits){
    TJanusHit& hit = elem.second;
    if(hit.Time() > 200 && hit.Time() < 3900){
      good_fronts.push_back(elem.first);
    }
  }

  // Find all backs with a reasonable TDC value
  std::vector<unsigned int> good_backs;
  for(auto& elem : back_hits){
    TJanusHit& hit = elem.second;
    if(hit.Time() > 200 && hit.Time() < 3900){
      good_backs.push_back(elem.first);
    }
  }

  if(good_fronts.size()==1 && good_backs.size()==1){
    //Copy most parameters from the front
    TJanusHit& front = front_hits[good_fronts[0]];
    janus_hits.emplace_back(front);
    fSize++;
    TJanusHit& hit = janus_hits.back();

    //Copy more parameters from the back
    TJanusHit& back  = back_hits[good_backs[0]];
    hit.GetBackHit().SetAddress(back.Address());
    hit.GetBackHit().SetCharge(back.Charge());
    hit.GetBackHit().SetTime(back.Time());
    hit.GetBackHit().SetTimestamp(back.Timestamp());
  } else {
    static bool message_displayed = false;
    if(!message_displayed){
      std::cout << "Abnormal JANUS Event: " << good_fronts.size()
                << ", " << good_backs.size() << std::endl;
      message_displayed = true;
    }
  }



  data += sizeof(VME_Timestamp);

  //assert(data == buf.GetData() + buf.GetSize());
  if(data != buf.GetData() + buf.GetSize()){
    std::cerr << "End of janus read not equal to size of buffer given:\n"
              << "\tBuffer Start: " << (void*)buf.GetData() << "\tBuffer Size: " << buf.GetSize()
              << "\n\tBuffer End: " << (void*)(buf.GetData() + buf.GetSize())
              << "\n\tNum ADC chan: " << num_packets
              << "\n\tPtr at end of read: " << (void*)(data)
              << "\n\tDiff: " << (buf.GetData() + buf.GetSize()) - data
              << std::endl;

    buf.Print("all");
  }
}

TVector3 TJanus::GetPosition(int detnum, int ring_num, int sector_num){
  if(detnum<0 || detnum>1 ||
     ring_num<1 || ring_num>24 ||
     sector_num<1 || sector_num>32){
    // batman vector, nan-nan-nan
    return TVector3(std::sqrt(-1),std::sqrt(-1),std::sqrt(-1));
  }

  TVector3 origin = TVector3(0,0,3);
  // Phi of sector 1 of downstream detector
  double phi_offset = 2*3.1415926535*(0.25);

  // Winding direction of sectors.
  bool clockwise = true;

  double janus_outer_radius = 3.5;
  double janus_inner_radius = 1.1;

  TVector3 position(1,0,0);  // Not (0,0,0), because otherwise SetPerp and SetPhi fail.
  double rad_slope = (janus_outer_radius - janus_inner_radius) /24;
  double rad_offset = janus_inner_radius;
  double perp_num = ring_num - 0.5; // Shift to 0.5-23.5
  position.SetPerp(perp_num*rad_slope + rad_offset);
  double phi_num = sector_num;
  double phi =
    phi_offset +
    (clockwise ? -1 : 1) * 2*3.1415926/32 * (phi_num - 1);
  position.SetPhi(phi);

  position += origin;

  if(detnum==0){
    position.RotateY(TMath::Pi());
  }

  return position;
}

void TJanus::InsertHit(const TDetectorHit& hit) {
  janus_hits.emplace_back((TJanusHit&)hit);
  fSize++;
}
