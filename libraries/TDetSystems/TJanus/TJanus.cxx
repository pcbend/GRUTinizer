#include "TJanus.h"

#include <cassert>
#include <iostream>
#include <memory>

#include "TMath.h"
#include "TRandom.h"

#include "JanusDataFormat.h"
#include "TNSCLEvent.h"
#include "TNucleus.h"
#include "TReaction.h"
#include "TSRIM.h"

int TJanus::NRing = 24;
int TJanus::NSector = 24;

double TJanus::PhiOffset = 2*TMath::Pi()*0.25;
double TJanus::OuterDiameter  = 70.;
double TJanus::InnerDiameter  = 22.;
double TJanus::TargetDistance = 31.;

double TJanus::TDiff = 1000;
double TJanus::EWin = 0.9;
double TJanus::FrontBackOffset = 0;

bool TJanus::multhit = false;

TJanus::TJanus() {
  Clear();
}

TJanus::~TJanus(){ }

void TJanus::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TJanus& janus = (TJanus&)obj;
  janus.janus_hits = janus_hits;
  janus.ring_hits = ring_hits;
  janus.sector_hits = sector_hits;
}

void TJanus::Clear(Option_t* opt){
  TDetector::Clear(opt);

  janus_hits.clear();
  ring_hits.clear();
  sector_hits.clear();
  stack_triggered = -1;
  num_packets = -1;
  total_bytes = -1;
}

int TJanus::BuildHits(std::vector<TRawEvent>& raw_data){
  //assert(raw_data.size() == 1);

  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    Build_VMUSB_Read(nscl.GetPayloadBuffer());
  }
  return janus_hits.size() + janus_channels.size();
}

TDetectorHit& TJanus::GetHit(int i){
  return janus_hits.at(i);
}


void TJanus::Build_VMUSB_Read(TSmartBuffer buf){
  total_bytes = buf.GetSize();
  // Events with all ADCs have 1166 bytes without zero-suppression, about 850 with zero-suppression
  // Events with missing ADCs have 690 bytes (first event of run), 418 bytes (other events),
  //    or ~200 bytes (zero suppression).
  // Very large events are caused when the VM-USB reads incorrectly.
  //   These are very rare, but Jeromy doesn't know what causes it.
  if(total_bytes < 750 ||
     total_bytes > 1300) {
    return;
  }

  const char* data = buf.GetData();

  const VMUSB_Header* vmusb_header = (VMUSB_Header*)data;
  data += sizeof(VMUSB_Header);


  // This value should ALWAYS be zero, because that corresponds to the i1 trigger of the VMUSB.
  // If it is not, it is a malformed event.
  stack_triggered = vmusb_header->stack();

  // vmusb_header.size() returns the number of 16-bit words in the payload.
  // Each adc entry is a 32-bit word.
  // 6 additional 16-bit words for the timestamp (2 48-bit numbers)
  num_packets = vmusb_header->size()/2 - 3;

  const VME_Timestamp* vme_timestamp = (VME_Timestamp*)(data + num_packets*sizeof(CAEN_DataPacket));
  long timestamp = vme_timestamp->ts1() * 20;

  std::map<unsigned int,TJanusHit> front_hits;
  std::map<unsigned int,TJanusHit> back_hits;
  //std::cout << "NP = " << num_packets << std::endl;
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
        std::cout << "Unknown analog (slot, channel): (" << adc_cardnum << ", " << packet->channel_num() << "), address = 0x" << std::hex << address << std::dec << std::endl;
      } else if(lines_displayed==1000){
        std::cout << "I'm going to stop telling you that the channel was unknown you should probably stop the program." << std::endl;
      }
      lines_displayed++;
      continue;
    }

    if(strcmp(chan->GetSystem(),"DEL") == 0) {
//      std::cout << "Not A Janus channel" << std::endl;
      continue;
    }
//    std::cout << i << "\t" << chan->GetSystem() << "\t" << chan->GetSegment() << "\t" << is_tdc << "\t" << packet->adcvalue() << "\t0x" << std::hex << address << std::dec << std::endl;
    TJanusHit hit;
    //Match a TDC hit to ADC Hit do so for rings/sectors
    if(is_tdc) {
      if(*chan->GetArraySubposition() == 'F'){
        for(unsigned int r = 0; r < ring_hits.size(); r++) {
	  if(ring_hits.at(r).GetRing() == chan->GetSegment()) {
	    ring_hits.at(r).SetTDCOverflowBit(packet->overflow());
	    ring_hits.at(r).SetTDCUnderflowBit(packet->underflow());
            ring_hits.at(r).SetTime(packet->adcvalue());
	    break;
	  }
	}
      } else {
        for(unsigned int s = 0; s < sector_hits.size(); s++) {
          if(sector_hits.at(s).GetSector() == chan->GetSegment()) {
	    sector_hits.at(s).SetTDCOverflowBit(packet->overflow());
            sector_hits.at(s).SetTDCUnderflowBit(packet->underflow());
            sector_hits.at(s).SetTime(packet->adcvalue());
            break;
          }
        }
      }
    } else {
//      if(packet->adcvalue() < 100) continue;
      hit.SetADCOverflowBit(packet->overflow());
      hit.SetADCUnderflowBit(packet->underflow());
      hit.SetCharge(packet->adcvalue());
      hit.SetAddress(address);
      hit.SetTimestamp(timestamp);
      hit.SetDetectorNumber(chan->GetArrayPosition());
      if(*chan->GetArraySubposition() == 'F'){
	hit.SetRingNumber(chan->GetSegment());
	hit.SetSectorNumber(-1);
	ring_hits.push_back(std::move(hit));
      } else {
        hit.SetRingNumber(-1);
        hit.SetSectorNumber(chan->GetSegment());
        sector_hits.push_back(std::move(hit));
      }
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

Int_t TJanus::GetJanusSize() {
  BuildJanusHit();
  return janus_hits.size();
}

void TJanus::BuildJanusHit() {
  //Constructs front/back coincidences, energy and time differences can be changed with
  //SetFrontBackEn and SetFrontBackTime
  janus_hits.clear();
  //std::cout << "Begin Hit" << std::endl;
  if(ring_hits.size() == 0 || sector_hits.size() == 0) return;

  //For storage of energies
  std::vector<double> EnR, EnS;
  std::vector<bool> UR, US;

  for(size_t i = 0; i < ring_hits.size(); i++) {
    EnR.push_back(ring_hits.at(i).GetEnergy());
    UR.push_back(false);
  }

  for(size_t i = 0; i < sector_hits.size(); i++) {
    EnS.push_back(sector_hits.at(i).GetEnergy());
    US.push_back(false);
  }


  for(size_t i = 0; i < ring_hits.size(); i++) {
    for(size_t j = 0; j < sector_hits.size(); j++) {
      if(ring_hits.at(i).GetDetnum() == sector_hits.at(j).GetDetnum()) {
	//Check time between events is good
	if(abs(ring_hits.at(i).Timestamp() - sector_hits.at(j).Timestamp()) < TDiff) {
	  //Check energy is good
	  if((EnR.at(i)*EWin) < EnS.at(j) && (EnS.at(j)*EWin) < EnR.at(i)) {
	    //Use rings for all data, sector for position only
	    TJanusHit dhit = ring_hits.at(i);
	    dhit.SetSectorNumber(sector_hits.at(j).GetSector());
	    janus_hits.push_back(dhit);
	    //For multihit events
	    UR.at(i) = true;
	    US.at(j) = true;
	  }
	}
      }
    }
  }
  if(multhit) {
    std::cout << "Do something" << std::endl;
    int ringCount = 0;
    int secCount = 0;
    //Check for hits without segments, charge may be shared between multiple strips
    for(size_t i = 0; i < UR.size(); i++) {
      if(!UR.at(i)) ringCount++;
    }
    for(size_t i = 0; i < US.size(); i++) {
      if(!US.at(i)) secCount++;
    }
    // Check for single ring hit + multiple sectors
    for(size_t i = 0; i < UR.size(); i++) {
      if(UR.at(i)) continue;
      for(size_t j = 0; j < US.size(); j++) {
        if(US.at(j)) continue;
        if(ring_hits.at(i).GetDetnum() != sector_hits.at(j).GetDetnum()) continue;
        for(size_t k = j+1; k < US.size(); k++) {
          if(US.at(k)) continue;
          if(ring_hits.at(i).GetDetnum() != sector_hits.at(k).GetDetnum()) continue;
	  //Check time of hits
  	  if(abs(ring_hits.at(i).Timestamp() - sector_hits.at(j).Timestamp()) < TDiff &&
             abs(ring_hits.at(i).Timestamp() - sector_hits.at(k).Timestamp()) < TDiff) {
	    //check ring energy is equal to sum of sector energy
	    if((EnR.at(i)*EWin) < EnS.at(j)+EnS.at(k) && ((EnS.at(j)+EnS.at(k))*EWin) < EnR.at(i)) {
	      int dSec = abs(sector_hits.at(j).GetSector() - sector_hits.at(k).GetSector());
	      if( dSec == 1 || dSec == NSector) {  //Check for neighbouring Sectors
		//Likely a charge sharing event, create a new hit
		TJanusHit dhit = ring_hits.at(i);
		//Assign sector number as highest energy strip
	        if(sector_hits.at(j) < sector_hits.at(k)) {
		  dhit.SetSectorNumber(sector_hits.at(k).GetSector());
		} else {
                  dhit.SetSectorNumber(sector_hits.at(j).GetSector());
		}
		janus_hits.push_back(dhit);
	      } else {
		//Two different hits with the same ring
		//Have to use sector information for hit, ring gives position only
		TJanusHit dhit0 = sector_hits.at(j);
		dhit0.SetRingNumber(ring_hits.at(i).GetRing());
		janus_hits.push_back(dhit0);

		TJanusHit dhit1 = sector_hits.at(k);
		dhit1.SetRingNumber(ring_hits.at(i).GetRing());
		janus_hits.push_back(dhit1);
	      } //Sector difference
	      UR.at(i) = true;
	      US.at(j) = true;
	      US.at(k) = true;
	    } //Energy
	  } //Time
	} //2nd sector
      } //1st sector
    }  //Shared Ring


    ringCount = 0;
    secCount = 0;
    //Check for hits without segments, charge may be shared between multiple strips
    for(size_t i = 0; i < UR.size(); i++) {
      if(!UR.at(i)) ringCount++;
    }
    for(size_t i = 0; i < US.size(); i++) {
      if(!US.at(i)) secCount++;
    }
    // Check for single sector hit + multiple rings
    for(size_t i = 0; i < US.size(); i++) {
      if(US.at(i)) continue;
      for(size_t j = 0; j < UR.size(); j++) {
        if(UR.at(j)) continue;
        if(sector_hits.at(i).GetDetnum() != ring_hits.at(j).GetDetnum()) continue;
        for(size_t k = j+1; k < UR.size(); k++) {
          if(UR.at(k)) continue;
          if(sector_hits.at(i).GetDetnum() != ring_hits.at(k).GetDetnum()) continue;
	  //Check time of hits
  	  if(abs(sector_hits.at(i).Timestamp() - ring_hits.at(j).Timestamp()) < TDiff &&
             abs(sector_hits.at(i).Timestamp() - ring_hits.at(k).Timestamp()) < TDiff) {
	    //check sector energy is equal to sum of ring energy
	    if((EnS.at(i)*EWin) < EnR.at(j)+EnR.at(k) && ((EnR.at(j)+EnR.at(k))*EWin) < EnS.at(i)) {
	      int dRing = abs(ring_hits.at(j).GetRing() - ring_hits.at(k).GetRing());
	      if( dRing == 1 || dRing == NRing) {  //Check for neighbouring rings
		//Likely a charge sharing event, create a new hit
		//Use sector information for hit, ring gives position only
		TJanusHit dhit = sector_hits.at(i);
		//Assign ring number as highest energy strip
	        if(ring_hits.at(j) < ring_hits.at(k)) {
		  dhit.SetRingNumber(ring_hits.at(k).GetRing());
		} else {
                  dhit.SetRingNumber(ring_hits.at(j).GetRing());
		}
		janus_hits.push_back(dhit);
	      } else {
		//Two different hits with the same sector
		//Use ring information for hit, sector gives position only
		TJanusHit dhit0 = ring_hits.at(j);
		dhit0.SetSectorNumber(sector_hits.at(i).GetSector());
		janus_hits.push_back(dhit0);

		TJanusHit dhit1 = ring_hits.at(k);
		dhit1.SetSectorNumber(sector_hits.at(i).GetRing());
		janus_hits.push_back(dhit1);
	      } //Ring difference
	      US.at(i) = true;
	      UR.at(j) = true;
	      UR.at(k) = true;
	    } //Energy
	  } //Time
	} //2nd ring
      } //1st ring
    }  //Shared sector
  }  //Multihit
}

TVector3 TJanus::GetPosition(int ring, int sector, double zoffset, bool sectorsdownstream, bool smear) {
  double ring_width = (OuterDiameter - InnerDiameter) * 0.5 / NRing;
  double inner_radius = (InnerDiameter)/2;
  double radius = inner_radius + ring_width * (ring + 0.5);

  double phi_width = 2 * TMath::Pi() / NSector;
  double phi = phi_width * sector;

  if(sectorsdownstream) {
    phi = -phi;
  }
  phi += PhiOffset;

  //Uniform distrubution over S3 pixel
  if(smear) {
    double sep = ring_width * 0.025;
    double r1 = radius - ring_width * 0.5 + sep;
    double r2 = radius + ring_width * 0.5 - sep;
    radius = sqrt(gRandom->Uniform(r1 * r1, r2 * r2));
    double sepphi = sep / radius;
    phi = gRandom->Uniform(phi - phi_width * 0.5 + sepphi, phi + phi_width * 0.5 - sepphi);
  }
  return TVector3(cos(phi) * radius, sin(phi) * radius, zoffset);
}

void TJanus::InsertHit(const TDetectorHit& hit) {
  janus_hits.emplace_back((TJanusHit&)hit);
  fSize++;
}

TJanusHit* TJanus::GetRingHit(const int&i) {
  if(i < GetRingSize()) {
    return &ring_hits.at(i);
  }
  return nullptr;
}

TJanusHit* TJanus::GetSectorHit(const int&i) {
  if(i < GetSectorSize()) {
    return &sector_hits.at(i);
  }
  return nullptr;
}

TJanusHit* TJanus::GetJanusHit(const int&i) {
  if(i < (int)janus_hits.size()) {
    return &janus_hits.at(i);
  }
  return nullptr;
}

void TJanus::Print(Option_t *opt) const {
  std::cout << "TJanus @ " << Timestamp() << std::endl;
  std::cout << "Size: " << Size() << std::endl;
  for(unsigned int i=0;i<Size();i++) {
    janus_hits.at(i).Print();
  }
  std::cout << "---------------------------" << std::endl;

}
