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

TJanus::TJanus() {
  Clear();
}

TJanus::~TJanus(){ }

void TJanus::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TJanus& janus = (TJanus&)obj;
  janus.janus_hits = janus_hits;
}

void TJanus::Clear(Option_t* opt){
  TDetector::Clear(opt);

  janus_hits.clear();
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

TJanusHit& TJanus::GetJanusHit(int i){
  return janus_hits.at(i);
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
                  << "), address = 0x"
                  << std::hex << address << std::dec
                  << std::endl;
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

      // //if(packet->adcvalue() > 150 && packet->adcvalue() < 4000) {
      //   int channel_number = (adc_cardnum-5)*32 + packet->channel_num();
      //   std::cout << "TDC, channel=" << channel_number << "\tvalue=" << packet->adcvalue()
      //             << std::endl;
      //   //}
    } else {
      hit->SetADCOverflowBit(packet->overflow());
      hit->SetADCUnderflowBit(packet->underflow());
      hit->SetCharge(packet->adcvalue());

      // //if(packet->adcvalue() > 150 && packet->adcvalue() < 4000) {
      //   int channel_number = (adc_cardnum-5)*32 + packet->channel_num();
      //   std::cout << "ADC, channel=" << channel_number << "\tvalue=" << packet->adcvalue()
      //             << std::endl;
      //   //}
    }
  }

  for(auto& elem : front_hits){
    TJanusHit& hit = elem.second;
    janus_channels.emplace_back(hit);
  }
  for(auto& elem : back_hits){
    TJanusHit& hit = elem.second;
    janus_channels.emplace_back(hit);

    // if(hit.Time() > 50 && hit.Time() < 4000) {
    //   std::cout << "address: 0x" << std::hex << hit.Address() << std::dec
    //             << "\tvalue: " << hit.Time()
    //             << "\tvalue: " << janus_channels.back().Time()
    //             << std::endl;
    // }
  }

  // Find all fronts with a reasonable TDC value
  int best_front = -1;
  double max_energy = -1e9;
  for(auto& elem : front_hits){
    TJanusHit& hit = elem.second;
    // if(hit.Time() > 50 && hit.Time() < 3900 &&
    //    hit.Charge() > max_charge){
    if(hit.GetEnergy() > max_energy &&
       hit.GetDetnum() >= 0 &&
       hit.GetDetnum() < 2){
      best_front = elem.first;
      max_energy = hit.Charge();
    }
  }

  // Find all backs with a reasonable TDC value
  int best_back = -1;
  max_energy = -1e9;
  for(auto& elem : back_hits){
    TJanusHit& hit = elem.second;
    // if(hit.Time() > 50 && hit.Time() < 3900 &&
    //    hit.Charge() > max_charge) {
    if(hit.GetEnergy() > max_energy &&
       hit.GetDetnum() >= 0 &&
       hit.GetDetnum() < 2){
      best_back = elem.first;
      max_energy = hit.Charge();
    }
  }


  if(best_front != -1 && best_back != -1){
    //Copy most parameters from the front
    TJanusHit& front = front_hits[best_front];
    janus_hits.emplace_back(front);
    fSize++;
    TJanusHit& hit = janus_hits.back();

    //Copy more parameters from the back
    TJanusHit& back  = back_hits[best_back];
    hit.GetBackHit().SetAddress(back.Address());
    hit.GetBackHit().SetCharge(back.Charge());
    hit.GetBackHit().SetTime(back.Time());
    hit.GetBackHit().SetTimestamp(back.Timestamp());

    // std::cout << "Front chan: " << hit.GetFrontChannel() << ", ADC=" << front.Charge() << ", TDC=" << front.Time()
    //           << "\tBack chan: " << hit.GetBackChannel() << ", ADC=" << back.Charge() << ", TDC=" << back.Time()
    //           << std::endl;

    // std::cout << "Back hit is channel: " << hit.GetBackChannel()
    //           << " with TDC = " << hit.GetBackHit().Time()
    //           << ", ADC = " << hit.GetBackHit().Charge()
    //           << std::endl;

  } //else {
//   static bool message_displayed = false;
//   //    if(!message_displayed){
//     std::cout << "Abnormal JANUS Event: " << good_fronts.size()
//               << ", " << good_backs.size() << std::endl;
//     for(auto good_front : good_fronts){
//       std::cout << "\tRing: " << std::hex << front_hits[good_front].Address() << std::dec<< "\tCharge: " << front_hits[good_front].Charge() << "\tTime: " << front_hits[good_front].Time() << std::dec << std::endl;
//     }
//     for(auto good_back : good_backs){
//       std::cout << "\tSector: " << std::hex << back_hits[good_back].Address() << std::dec << "\tCharge: " << back_hits[good_back].Charge() << "\tTime: " << back_hits[good_back].Time() << std::dec << std::endl;
//     }
//     message_displayed = true;
//     //    }
// }


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

void TJanus::Print(Option_t *opt) const {
  printf("TJanus @ %lu\n",Timestamp());
  printf(" Size: %i\n",Size());
  for(unsigned int i=0;i<Size();i++) {
    printf("\t"); janus_hits.at(i).Print(); printf("\n");
  }
  printf("---------------------------\n");

}

void TJanus::SetRunStart(unsigned int unix_time) {
  // Wed Jan 27 22:57:09 2016
  unsigned int previous = fRunStart==0 ? 1453953429 : fRunStart;
  int tdiff = unix_time - previous;
  long timestamp_diff = (1e9) * tdiff;

  fTimestamp += timestamp_diff;
  for(auto& hit : janus_hits) {
    hit.SetTimestamp(timestamp_diff + hit.Timestamp());
  }
}

double TJanus::GetBeta(double betamax, double kr_angle_rad, bool energy_loss, double collision_pos) {
  // Factors of 1e3 are because TNucleus and TReaction use MeV, while TSRIM uses keV.

  static auto kr = std::make_shared<TNucleus>("78Kr");
  static auto pb = std::make_shared<TNucleus>("208Pb");
  static TSRIM srim("kr78_in_pb208");

  double thickness = (0.75 / 11342.0) * 1e4; // (0.75 mg/cm^2) / (11342 mg/cm^3) * (10^4 um/cm)

  double pre_collision_energy_MeV = kr->GetEnergyFromBeta(betamax);
  if(energy_loss) {
    pre_collision_energy_MeV = srim.GetAdjustedEnergy(pre_collision_energy_MeV*1e3, thickness*collision_pos)/1e3;
  }

  TReaction reac(kr, pb, kr, pb, pre_collision_energy_MeV);

  double post_collision_energy_MeV = reac.GetTLab(kr_angle_rad, 2);

  if(energy_loss) {
    double distance_travelled;
    if(kr_angle_rad < TMath::Pi()/2) {
      // Forward scattering, must make it out the front of the target.
      distance_travelled = thickness*(1-collision_pos)/std::abs(std::cos(kr_angle_rad));
    } else {
      // Backward scattering, must make it out the back of the target.
      distance_travelled = thickness*collision_pos/std::abs(std::cos(kr_angle_rad));
    }
    post_collision_energy_MeV = srim.GetAdjustedEnergy(post_collision_energy_MeV*1e3, distance_travelled)/1e3;
  }

  double beta = kr->GetBetaFromEnergy(post_collision_energy_MeV);
  return beta;
}

double TJanus::SimAngle() {
  static auto kr = std::make_shared<TNucleus>("78Kr");
  static auto pb = std::make_shared<TNucleus>("208Pb");
  static TSRIM srim("kr78_in_pb208");
  double thickness = (0.75 / 11342.0) * 1e4; // (0.75 mg/cm^2) / (11342 mg/cm^3) * (10^4 um/cm)

  double collision_pos = gRandom->Uniform();

  double collision_energy = srim.GetAdjustedEnergy(3.9*78*1e3, thickness*collision_pos)/1e3;
  double energy_mid = srim.GetAdjustedEnergy(3.9*78*1e3, thickness*0.5)/1e3;

  TReaction reac(kr, pb, kr, pb, collision_energy);
  TReaction reac_mid(kr, pb, kr, pb, energy_mid);

  double pb_angle_rad = reac.ConvertThetaLab(90 * (3.1415926/180), 2, 3);
  double kr_angle_rad_recon = reac_mid.ConvertThetaLab(pb_angle_rad, 3, 2);
  return kr_angle_rad_recon;
}
