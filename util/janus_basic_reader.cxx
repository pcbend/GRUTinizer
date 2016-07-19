#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "Globals.h"
#include "TRawEvent.h"
#include "TRawSource.h"
#include "JanusDataFormat.h"
#include "TRawBanks.h"
#include "TNSCLEvent.h"
#include "TJanus.h"
#include "TSmartBuffer.h"

using namespace std;

void HandleJanusBuf(TNSCLEvent& event) {
  TSmartBuffer buf = event.GetPayloadBuffer();
  const char* data = buf.GetData();

  std::cout << GREEN
            << "-------- Size = " << buf.GetSize() << ", TS = " << event.GetTimestamp() << "-----------"
            << RESET_COLOR << std::endl;

  const VMUSB_Header* vmusb_header = (VMUSB_Header*)data;
  data += sizeof(VMUSB_Header);

  // std::cout << *vmusb_header << std::endl;

  int num_packets = vmusb_header->size()/2 - 3;
  //std::cout << "num_packets: " << num_packets << std::endl;

  int total_invalid = 0;
  int num_per_module = 0;

  for(int i=0; i<num_packets; i++) {
    const CAEN_DataPacket* packet = (CAEN_DataPacket*)data;
    data += sizeof(CAEN_DataPacket);

    std::cout << *packet << std::endl;

    switch(packet->entry_type()) {
      case CAEN_DataPacket::Invalid:
        total_invalid++;
        break;

      case CAEN_DataPacket::Begin:
        num_per_module = 0;
        break;

      case CAEN_DataPacket::End:
        std::cout << "Num channels: " << num_per_module << std::endl;
        num_per_module = 0;
        //std::cout << *packet << std::endl;
        break;

      case CAEN_DataPacket::Event:
        num_per_module++;
        break;

      default:
        break;
    }


    // if(!packet->IsValid()){

    // }

    // int channel;
    // if(packet->card_num() < 9) {
    //   channel = (packet->card_num()-5)*32 + packet->channel_num();
    // } else {
    //   channel = (packet->card_num()-9)*32 + packet->channel_num();
    // }


  }


  if(num_per_module) {
    std::cout << "Num channels: " << num_per_module << std::endl;
  }

  std::cout << "Total invalid: " << total_invalid << std::endl;
}

int main(int argc, char** argv) {
  if(argc < 3) {
    std::cerr << "Usage: " << argv[0] << " INPUT_FILE OUTPUT_FILE" << std::endl;
    return 1;
  }

  std::string input_filename = argv[1];
  std::string output_filename = argv[2];

  TRawFile filein(input_filename.c_str());

  TRawEvent  raw_event;

  while(filein.Read(&raw_event)>0) {
    TNSCLEvent& event = (TNSCLEvent&)raw_event;
    if(event.GetEventType() == kNSCLEventType::PHYSICS_EVENT) {
      if(event.IsBuiltData()) {
        TNSCLBuiltRingItem built(event);
        for(unsigned int i=0; i<built.NumFragments(); i++) {
          TNSCLFragment& fragment = built.GetFragment(i);
          int source_id = fragment.GetFragmentSourceID();
          if(source_id == 4) {
            HandleJanusBuf(fragment.GetNSCLEvent());
          }
        }
      } else {
        if(event.GetSourceID() == 4) {
          HandleJanusBuf(event);
        }
      }
    }
  }

  return 0;
}
