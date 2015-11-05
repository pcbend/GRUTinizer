#include "TUnpackLoop.h"

#include <chrono>
#include <thread>

#include "RawDataQueue.h"
#include "TDetectorEnv.h"
#include "TGEBEvent.h"
#include "TGRUTOptions.h"
#include "TNSCLEvent.h"

#include "TMode3.h"

ClassImp(TUnpackLoop);

TUnpackLoop::TUnpackLoop(ThreadsafeQueue<TRawEvent>& input_queue,
                         ThreadsafeQueue<TUnpackedEvent*>& output_queue)
  : input_queue(input_queue), output_queue(output_queue),
    event_start(0), build_window(1000) {
  next_event = new TUnpackedEvent;
}

TUnpackLoop::~TUnpackLoop(){
  delete next_event;
}

bool TUnpackLoop::Iteration(){
  TRawEvent event;
  int error = input_queue.Pop(event);

  if(error){
    return true;
  }

  switch(event.GetFileType()){
    case kFileType::NSCL_EVT:
    {
      TNSCLEvent nscl_event(event);
      HandleNSCLData(nscl_event);
    }
      break;

    case kFileType::GRETINA_MODE2:
    case kFileType::GRETINA_MODE3:
    {
      TGEBEvent geb_event(event);
      HandleGEBData(geb_event);
    }
      break;
  }

  return true;
}

void TUnpackLoop::HandleNSCLData(TNSCLEvent& event) {
  switch(event.GetEventType()) {
    case kNSCLEventType::BEGIN_RUN:            // 0x0001
    case kNSCLEventType::END_RUN:              // 0x0002
    case kNSCLEventType::PAUSE_RUN:            // 0x0003
    case kNSCLEventType::RESUME_RUN:           // 0x0004
      break;
    case kNSCLEventType::PACKET_TYPES:         // 0x000A
    case kNSCLEventType::MONITORED_VARIABLES:  // 0x000B
    case kNSCLEventType::RING_FORMAT:          // 0x000C
    case kNSCLEventType::PHYSICS_EVENT_COUNT:  // 0x001F
    case kNSCLEventType::EVB_FRAGMENT:         // 0x0028
    case kNSCLEventType::EVB_UNKNOWN_PAYLOAD:  // 0x0029
    case kNSCLEventType::EVB_GLOM_INFO:        // 0x002A
    case kNSCLEventType::FIRST_USER_ITEM_CODE: // 0x8000
      break;
    case kNSCLEventType::PERIODIC_SCALERS:     // 0x0014
      //HandleNSCLPeriodicScalers(event);
      break;
    case kNSCLEventType::PHYSICS_EVENT:        // 0x001E
      if(event.IsBuiltData()){
        HandleBuiltNSCLData(event);
      } else {
        HandleUnbuiltNSCLData(event);
      }
      break;
  }
}

void TUnpackLoop::CheckBuildWindow(long timestamp) {
  if(timestamp > event_start + build_window) {
    next_event->Build();
    output_queue.Push(next_event);
    next_event = new TUnpackedEvent;
    event_start = timestamp;
  }
}

void TUnpackLoop::HandleBuiltNSCLData(TNSCLEvent& event){
  TNSCLBuiltRingItem built(event);
  for(unsigned int i=0; i<built.NumFragments(); i++){
    TNSCLFragment& fragment = built.GetFragment(i);
    kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(fragment.GetFragmentSourceID());
    CheckBuildWindow(fragment.GetFragmentTimestamp());
    next_event->AddRawData(fragment.GetNSCLEvent(), detector);
  }
}

void TUnpackLoop::HandleUnbuiltNSCLData(TNSCLEvent& event){
  kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(event);
  CheckBuildWindow(event.GetTimestamp());
  next_event->AddRawData(event, detector);
}

void TUnpackLoop::HandleGEBMode3(TGEBEvent& event, kDetectorSystems system){
  TGEBMode3Event built(event);
  for(unsigned int i=0; i<built.NumFragments(); i++){
    TGEBEvent& fragment = built.GetFragment(i);
    next_event->AddRawData(event, system);
  }
}

void TUnpackLoop::HandleS800Scaler(TGEBEvent& event){
  TUnpackedEvent* scaler_event = new TUnpackedEvent;
  scaler_event->AddRawData(event, kDetectorSystems::S800SCALER);
  scaler_event->Build();
  output_queue.Push(scaler_event);
}

void TUnpackLoop::HandleGEBData(TGEBEvent& event){
  int type = event.GetEventType();

  // Fill an event, if it needs to be filled.
  switch(type) {
    case 1: // Gretina decomp data.
    case 2: // Gretina Mode3 data.
    case 5: // S800 Mode2 equvilant.
    case 8: // Gretina diag. data.
      CheckBuildWindow(event.GetTimestamp());
      break;
  }

  switch(type) {
    case 1: // Gretina decomp data.
      next_event->AddRawData(event, kDetectorSystems::GRETINA);
      break;
    case 2: // Gretina Mode3 data.
      // TODO, move this check somewhere else
      if(!TGRUTOptions::Get()->IgnoreMode3()) {
        HandleGEBMode3(event, kDetectorSystems::MODE3);
      }
      break;
    case 5: // S800 Mode2 equvilant.
      next_event->AddRawData(event, kDetectorSystems::S800);
      break;
    case 8: // Gretina diag. data.
      HandleGEBMode3(event, kDetectorSystems::BANK29);
      break;
    case 10: // S800 scaler data....
      HandleS800Scaler(event);
      break;
    case 17: //PWall Mode2 equivlant.
      break;
    case 29: // Something.
      break;
    default:
      std::cout << "Dance Party EventType: " << type << std::endl;
      //dance party.
      break;
  }
}
