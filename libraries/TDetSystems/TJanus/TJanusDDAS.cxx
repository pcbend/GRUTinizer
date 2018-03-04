#include "TJanusDDAS.h"


#include <cassert>
#include <iostream>
#include <memory>

#include "TMath.h"
#include "TRandom.h"

#include "DDASDataFormat.h"
#include "JanusDataFormat.h"
#include "TRawEvent.h"
#include "TChannel.h"
#include "GValue.h"


TJanusDDAS::TJanusDDAS() {
  Clear();
}

TJanusDDAS::~TJanusDDAS(){ }

void TJanusDDAS::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TJanusDDAS& janus = (TJanusDDAS&)obj;
  janus.janus_hits = janus_hits;
}

void TJanusDDAS::Clear(Option_t* opt){
  TDetector::Clear(opt);

  d0_rings.clear();  
  d0_sectors.clear();
  d1_rings.clear();  
  d1_sectors.clear();


  janus_channels.clear();
  janus_hits.clear();
}

int TJanusDDAS::BuildHits(std::vector<TRawEvent>& raw_data){
  UnpackChannels(raw_data);
  BuildCorrelatedHits();
  /*
     printf("**************************\n");
     printf("**************************\n");
     printf("**************************\n");
     Print("channels");
     Print();
     printf("**************************\n");
     printf("**************************\n");
     printf("**************************\n");
     */
  return janus_hits.size() + janus_channels.size();
}

void TJanusDDAS::UnpackChannels(std::vector<TRawEvent>& raw_data) {

  unsigned long smallest_timestamp = 0x7fffffffffffffff;
  for(auto& event : raw_data){
    //SetTimestamp(event.GetTimestamp());

    TSmartBuffer buf = event.GetPayloadBuffer();
    TDDASEvent<DDASHeader> ddas(buf);

    //std::cout << "In Janus Unpack Channels" << std::endl;

    unsigned int address = ( (5<<24) +
        (ddas.GetCrateID()<<16) +
        (ddas.GetSlotID()<<8) +
        ddas.GetChannelID() );
    TChannel* chan = TChannel::GetChannel(address);

    static int lines_displayed = 0;
    if(!chan){
      if(lines_displayed < 10) {
        std::cout << "Unknown JANUS (crate, slot, channel): ("
          << ddas.GetCrateID() << ", " << ddas.GetSlotID()
          << ", " << ddas.GetChannelID() << ")" << std::endl;
      }
      lines_displayed++;
      continue;
    }

    // bool is_ring = (*chan->GetArraySubposition() == 'F');
    // int detnum = chan->GetArrayPosition();
    // int segnum = chan->GetSegment();

    TJanusDDASHit janus_chan;
    janus_chan.SetCharge(ddas.GetEnergy());
    janus_chan.SetTime(ddas.GetCFDTime());
    janus_chan.SetTimestamp(ddas.GetTimestamp()*8); // this are more 
    if(janus_chan.Timestamp()<smallest_timestamp) { smallest_timestamp = janus_chan.Timestamp(); }
    janus_chan.SetAddress(address);

    janus_channels.push_back(janus_chan);


    // static int shown = 0;
    // if(shown < 10) {
    //   std::cout << "-------------------\n";
    //   std::cout << "Charge = " << ddas.GetEnergy() << "\n";

    //   for(int i=0; i<ddas.GetTraceLength(); i++) {
    //     std::cout << ddas.trace[i] << "\t";
    //     if(i%8 == 0) {
    //       std::cout << "\n";
    //     }
    //   }

    //   std::cout << "\n";

    //   std::cout << std::flush;
    //   shown++;
    // }
  }

  SetTimestamp(smallest_timestamp);
}



void TJanusDDAS::BuildRingSectors() { 
  for(size_t x=0;x<janus_channels.size();x++) {
    TJanusDDASHit hit = janus_channel[x];
    if(hit.IsRing()) {
      if(hit.GetDetnum()==0) {
        d0_rings.push_back(*hit);
      } else {    
        d1_rings.push_back(*hit);
      }
    } else {
      if(hit.GetDetnum()==0) {
        d0_sectors.push_back(*hit);
      } else {    
        d1_sectors.push_back(*hit);
      }
    }
  }
}

void CleanRingScetors(std::vector<TJanusDDASHit> rings,std::vector<TJanusDDASHit> sectors) {
  std::vector<bool> r_passed;
  std::vector<bool> s_passed;
  r_passed.resize(rings.size());
  std::fill(r_passed.begin(),r_passed.end(),false);
  s_passed.resize(sectors.size());
  std::fill(s_passed.begin(),s_passed.end(),false);
  
  for(size_t x=0;x<rings.size();x++) {
  for(size_t x=0;x<rings.size();x++) {
    for(size_t y=0;y<sectors.size();y++) {
      int tdiff = abs(rings.at(x).Timestamp() - sectors.at(y).Timestamp());
      int cdiff = abs(rings.at(x).GetEnergy() - sectors.at(y).GetEnergy()); // calfile dependent!!!
      if(tdiff<250 && cdiff<600) {
        r_passed[x] = true;
        s_passed[y] = true;
      }
    }
  }
  std::vector<TJanusDDASHit>::iterator it;
  int counter=0;
  for(it=rings.begin();it!=rings.end();) {
    if(r_passed.at(counter)==false) {
      it.erase();
    } else {
      it++;
    }
    counter++;
  }
  counter=0;
  for(it=sectors.begin();it!=sectors.end();) {
    if(s_passed.at(counter)==false) {
      it.erase();
    } else {
      it++;
    }
    counter++;
  }
}




void TJanusDDAS::BuildCorrelatedHits(double EDiff, double TDiff) {
  int minimum_charge = 149;
  int maximum_charge = 32738;

  ClearCorrelatedHits();

  static int counter1=0;
  static int counter2=0;

  
  std::vector<TJanusDDASHit> rings;
  std::vector<TJanusDDASHit> sectors;
  for(size_t x=0;x<janus_channels.size();x++){
    TJanusDDASHit chan(janus_channels.at(x));
    if(chan.Charge()<minimum_charge || chan.Charge()>=maximum_charge) continue; //was an and...
    if(chan.IsRing())
      rings.push_back(chan);
    else if(chan.IsSector())
      sectors.push_back(chan);
  }
  






  if(false){

    if(rings.size()>2 || sectors.size()>2) return;

    janus_channels.clear();
    if(rings.size()==1) {
      janus_channels.push_back(rings[0]);
    }else if(rings.size()==2 && abs(rings.at(0).GetRing()-rings.at(1).GetRing())==1){
      TJanusDDASHit hit;
      if(rings.at(0).Charge()>rings.at(1).Charge()){
        rings.at(0).Copy(hit);
        hit.SetCharge(hit.Charge() + rings.at(1).Charge());
      }else {
        rings.at(1).Copy(hit);
        hit.SetCharge(hit.Charge() + rings.at(0).Charge());
      }
      janus_channels.push_back(hit);
    }else if(rings.size()!=0){
      janus_channels.push_back(rings[0]);
      janus_channels.push_back(rings[1]);
    }

    if(sectors.size()==1) {
      janus_channels.push_back(sectors[0]);
    }else if(sectors.size()==2 && ((abs(sectors.at(0).GetRing()-sectors.at(1).GetRing())==1) || 
          (abs(sectors.at(0).GetRing()-sectors.at(1).GetRing())==31))){
      TJanusDDASHit hit;
      if(sectors.at(0).Charge()>sectors.at(1).Charge()){
        sectors.at(0).Copy(hit);
        hit.SetCharge(hit.Charge() + sectors.at(1).Charge());
      }else {
        sectors.at(1).Copy(hit);
        hit.SetCharge(hit.Charge() + sectors.at(0).Charge());
      }
      janus_channels.push_back(hit);
    } else if(sectors.size()!=0){
      janus_channels.push_back(sectors[0]);
      janus_channels.push_back(sectors[1]);
    } 

  }
  //printf("janus size() = %lu\n",janus_channels.size());
  std::vector<bool> used;
  used.resize(janus_channels.size());
  std::fill(used.begin(),used.end(),false);
  //printf("used size() = %lu\n",used.size());
  for(size_t x=0;x<janus_channels.size();x++) {
    //printf("x is %lu\n",x);
    //printf("here 1\n",x);
    if(used.at(x)) continue;
    //printf("here 2\n",x);
    TJanusDDASHit &chan_ring = GetJanusChannel(x);
    //printf("here 3\n",x);
    if(!chan_ring.IsRing()) continue;
    //check gross charge window
    if(chan_ring.Charge()<minimum_charge || chan_ring.Charge()>=maximum_charge) continue; //was an and...
    for(size_t y=0;y<janus_channels.size();y++) {
      if(x==y) continue;
      if(used.at(y)) continue;
      TJanusDDASHit &chan_sector = GetJanusChannel(y);
      if(!chan_sector.IsSector()) continue;
      if(chan_ring.GetDetnum() != chan_sector.GetDetnum()) continue;
      //check gross charge window
      if(chan_sector.Charge()<minimum_charge || chan_sector.Charge()>=maximum_charge) continue; //was an and...
      float ediff = fabs(chan_ring.GetEnergy() - chan_sector.GetEnergy());
      int tdiff = abs(chan_ring.Timestamp() - chan_sector.Timestamp());

      if(ediff < EDiff) { //build.....
        if(tdiff < TDiff)                  { //build.....
          MakeHit(chan_ring, chan_sector);

          //printf("x,y is %lu \t %lu \n",x,y);
          used[x] = true;
          used[y] = true;
          break;
        }
      }
    }

  }



  // for(const auto& chan_ring : janus_channels) {
  //   if(chan_ring.IsRing() && chan_ring.RawCharge() > 150) {
  //     for(const auto& chan_sector : janus_channels) {
  //       if(chan_sector.IsSector() && chan_sector.RawCharge() > 150) {
  //         if(chan_ring.GetDetnum() == chan_sector.GetDetnum()) {
  //           MakeHit(chan_ring, chan_sector);
  //         }
  //       }
  //     }
  //   }
  // }
}

void TJanusDDAS::MakeHit(const TJanusDDASHit& chan_ring,
    const TJanusDDASHit& chan_sector) {
  TJanusDDASHit correlated_hit(chan_ring);

  TDetectorHit& back = correlated_hit.GetBackHit();
  back.SetCharge(chan_sector.Charge());
  back.SetTime(chan_sector.Time());
  back.SetAddress(chan_sector.Address());
  back.SetTimestamp(chan_sector.Timestamp());

  janus_hits.push_back(correlated_hit);
  fSize++;
}

TJanusDDASHit& TJanusDDAS::GetJanusHit(int i){
  return janus_hits.at(i);
}

TJanusDDASHit& TJanusDDAS::GetJanusChannel(int i){
  return janus_channels.at(i);
}

TDetectorHit& TJanusDDAS::GetHit(int i){
  return janus_hits.at(i);
}


//void TJanusDDAS::Build_VMUSB_Read(TSmartBuffer buf){
// const char* data = buf.GetData();

//   const VMUSB_Header* vmusb_header = (VMUSB_Header*)data;
//   data += sizeof(VMUSB_Header);


//   std::map<unsigned int,TJanusDDASHit> front_hits;
//   std::map<unsigned int,TJanusDDASHit> back_hits;
//   for(int i=0; i<num_packets; i++){
//     const CAEN_DataPacket* packet = (CAEN_DataPacket*)data;
//     data += sizeof(CAEN_DataPacket);

//     if(!packet->IsValid()){
//       continue;
//     }

//     // ADCs are in slots 5-8, TDCs in slots 9-12
//     bool is_tdc = packet->card_num() >= 9;
//     unsigned int adc_cardnum = packet->card_num();
//     if(is_tdc){
//       adc_cardnum -= 4;
//     }
//     unsigned int address =
//       (2<<24) + //system id
//       (4<<16) + //crate id
//       (adc_cardnum<<8) +
//       packet->channel_num();

//     TChannel* chan = TChannel::GetChannel(address);
//     // Bad stuff, tell somebody to fix it
//     static int lines_displayed = 0;
//     if(!chan){
//       if(lines_displayed < 1000) {
//         std::cout << "Unknown analog (slot, channel): ("
//                   << adc_cardnum << ", " << packet->channel_num()
//                   << "), address = 0x"
//                   << std::hex << address << std::dec
//                   << std::endl;
//       } else if(lines_displayed==1000){
//         std::cout << "I'm going to stop telling you that the channel was unknown,"
//                   << " you should probably stop the program." << std::endl;
//       }
//       lines_displayed++;
//       continue;
//     }

//     TJanusDDASHit* hit = NULL;
//     if(*chan->GetArraySubposition() == 'F'){
//       hit = &front_hits[address];
//     } else {
//       hit = &back_hits[address];
//     }

//     hit->SetAddress(address);
//     //hit->SetTimestamp(timestamp);

//     if(is_tdc){
//       hit->SetTime(packet->adcvalue());
//     } else {
//       hit->SetCharge(packet->adcvalue());
//     }
//   }

//   for(auto& elem : front_hits){
//     TJanusDDASHit& hit = elem.second;
//     janus_channels.emplace_back(hit);
//   }
//   for(auto& elem : back_hits){
//     TJanusDDASHit& hit = elem.second;
//     janus_channels.emplace_back(hit);

//     // if(hit.Time() > 50 && hit.Time() < 4000) {
//     //   std::cout << "address: 0x" << std::hex << hit.Address() << std::dec
//     //             << "\tvalue: " << hit.Time()
//     //             << "\tvalue: " << janus_channels.back().Time()
//     //             << std::endl;
//     // }
//   }

//   // Find all fronts with a reasonable TDC value
//   int best_front = -1;
//   double max_charge = -1e9;
//   for(auto& elem : front_hits){
//     TJanusDDASHit& hit = elem.second;
//     // if(hit.Time() > 50 && hit.Time() < 3900 &&
//     //    hit.Charge() > max_charge){
//     if(hit.Charge() > max_charge &&
//        hit.GetDetnum() >= 0 &&
//        hit.GetDetnum() < 2){
//       best_front = elem.first;
//       max_charge = hit.Charge();
//     }
//   }

//   // Find all backs with a reasonable TDC value
//   int best_back = -1;
//   max_charge = -1e9;
//   for(auto& elem : back_hits){
//     TJanusDDASHit& hit = elem.second;
//     // if(hit.Time() > 50 && hit.Time() < 3900 &&
//     //    hit.Charge() > max_charge) {
//     if(hit.Charge() > max_charge &&
//        hit.GetDetnum() >= 0 &&
//        hit.GetDetnum() < 2){
//       best_back = elem.first;
//       max_charge = hit.Charge();
//     }
//   }


//   if(best_front != -1 && best_back != -1){
//     //Copy most parameters from the front
//     TJanusDDASHit& front = front_hits[best_front];
//     janus_hits.emplace_back(front);
//     fSize++;
//     TJanusDDASHit& hit = janus_hits.back();

//     //Copy more parameters from the back
//     TJanusDDASHit& back  = back_hits[best_back];
//     hit.GetBackHit().SetAddress(back.Address());
//     hit.GetBackHit().SetCharge(back.Charge());
//     hit.GetBackHit().SetTime(back.Time());
//     hit.GetBackHit().SetTimestamp(back.Timestamp());

//     // std::cout << "Front chan: " << hit.GetFrontChannel() << ", ADC=" << front.Charge() << ", TDC=" << front.Time()
//     //           << "\tBack chan: " << hit.GetBackChannel() << ", ADC=" << back.Charge() << ", TDC=" << back.Time()
//     //           << std::endl;

//     // std::cout << "Back hit is channel: " << hit.GetBackChannel()
//     //           << " with TDC = " << hit.GetBackHit().Time()
//     //           << ", ADC = " << hit.GetBackHit().Charge()
//     //           << std::endl;

//   } //else {
// //   static bool message_displayed = false;
// //   //    if(!message_displayed){
// //     std::cout << "Abnormal JANUS Event: " << good_fronts.size()
// //               << ", " << good_backs.size() << std::endl;
// //     for(auto good_front : good_fronts){
// //       std::cout << "\tRing: " << std::hex << front_hits[good_front].Address() << std::dec<< "\tCharge: " << front_hits[good_front].Charge() << "\tTime: " << front_hits[good_front].Time() << std::dec << std::endl;
// //     }
// //     for(auto good_back : good_backs){
// //       std::cout << "\tSector: " << std::hex << back_hits[good_back].Address() << std::dec << "\tCharge: " << back_hits[good_back].Charge() << "\tTime: " << back_hits[good_back].Time() << std::dec << std::endl;
// //     }
// //     message_displayed = true;
// //     //    }
// // }


//   data += sizeof(VME_Timestamp);

//   //assert(data == buf.GetData() + buf.GetSize());
//   if(data != buf.GetData() + buf.GetSize()){
//     std::cerr << "End of janus read not equal to size of buffer given:\n"
//               << "\tBuffer Start: " << (void*)buf.GetData() << "\tBuffer Size: " << buf.GetSize()
//               << "\n\tBuffer End: " << (void*)(buf.GetData() + buf.GetSize())
//               << "\n\tNum ADC chan: " << num_packets
//               << "\n\tPtr at end of read: " << (void*)(data)
//               << "\n\tDiff: " << (buf.GetData() + buf.GetSize()) - data
//               << std::endl;

//     buf.Print("all");
//   }
//}

TVector3 TJanusDDAS::GetPosition(int detnum, int ring_num, int sector_num){
  if(detnum<0 || detnum>1 ||
      ring_num<1 || ring_num>24 ||
      sector_num<1 || sector_num>32){
    // batman vector, nan-nan-nan
    return TVector3(std::sqrt(-1),std::sqrt(-1),std::sqrt(-1));
  }

  TVector3 origin = TVector3(0,0,3.);
  // Phi of sector 1 of downstream detector
  double phi_offset = 2.*3.1415926535*(0.25);

  // Winding direction of sectors.
  bool clockwise = true;

  double janus_outer_radius = 3.5;
  double janus_inner_radius = 1.1;

  TVector3 position(1.,0,0);  // Not (0,0,0), because otherwise SetPerp and SetPhi fail.
  double rad_slope = (janus_outer_radius - janus_inner_radius) /24.;
  double rad_offset = janus_inner_radius;
  double perp_num = ring_num - 0.5; // Shift to 0.5-23.5
  position.SetPerp(perp_num*rad_slope + rad_offset);
  double phi_num = sector_num;
  double phi =
    phi_offset +
    (clockwise ? -1 : 1) * 2.*3.1415926/32. * (phi_num - 1);
  position.SetPhi(phi);

  position += origin;

  if(detnum==0){
    position.RotateY(TMath::Pi());
  }

  return position;
}

void TJanusDDAS::InsertHit(const TDetectorHit& hit) {
  janus_hits.emplace_back((TJanusDDASHit&)hit);
  fSize++;
}

void TJanusDDAS::Print(Option_t *opt) const {
  TString sopt(opt);
  if(sopt.Contains("channels")){
    PrintChannels(opt); 
  } else {
    PrintHits(opt); 
  }
}


void TJanusDDAS::PrintHits(Option_t *opt) const {
  printf("TJanusDDAS @ %lu\n",Timestamp());
  printf(" Size: %i\n",janus_hits.size());
  for(unsigned int i=0;i<Size();i++) {
    printf("\t"); janus_hits.at(i).Print(); printf("\n");
  }
  printf("---------------------------\n");
  fflush(stdout);
}

void TJanusDDAS::PrintChannels(Option_t *opt) const {
  printf("JANUS Channels!\n");
  printf("TJanusDDAS @ %lu\n",Timestamp());
  printf(" Size: %i\n",janus_channels.size());
  for(unsigned int i=0;i<janus_channels.size();i++) {
    TJanusDDASHit hit = janus_channels.at(i);
    TChannel *c = TChannel::GetChannel(hit.Address());
    if(c) {
      printf("\t%s\tDet: %i \tChannel: %02i\tChg: %i\tTime: %lu\n",
          c->GetName(),hit.GetDetnum(),hit.GetFrontChannel(),hit.Charge(),hit.Timestamp());
    } else {
      printf("\tjanus_channel 0x%08x, no channel.\n",hit.Address());
    }
  }
  printf("---------------------------\n");
  fflush(stdout);
}






