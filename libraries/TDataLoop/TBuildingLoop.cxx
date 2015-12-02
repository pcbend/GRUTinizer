#include "TBuildingLoop.h"

#include <chrono>
#include <thread>

//#include "RawDataQueue.h"
//#include "RawDataQueue.h"
//#include "TDetectorEnv.h"
#include "TGEBEvent.h"
#include "TGRUTOptions.h"
#include "TNSCLEvent.h"

#include "TDataLoop.h"

//#include "TMode3.h"

ClassImp(TBuildingLoop);

TBuildingLoop *TBuildingLoop::Get(std::string name,TDataLoop *input) {
  if(name.length()==0 && input) {
    name = "build_loop";
  }
  TBuildingLoop *loop = (TBuildingLoop*)StoppableThread::Get(name);
  if(!loop && input) 
    loop = new TBuildingLoop(name,input);
  return loop;
}

TBuildingLoop::TBuildingLoop(std::string name,TDataLoop *input) :
  StoppableThread(name),input_source(input) {

  SetBuildWindow(1000); 
  event_start = 0;
  //next_event = new TUnpackedEvent;
}
    

TBuildingLoop::~TBuildingLoop(){
  //delete next_event;
}

bool TBuildingLoop::Iteration(){
  TRawEvent event;
  
  static int counter = 0;
  //if(counter++>10000)
  //  return true;

  int error = input_source->Pop(event);///input_queue.Pop(event);

  CheckBuildWindow(&event);

  next_event.push_back(event);

  //if(error){
  //  return true;
  //}
  /*
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
  */
  return true;
}
/*
void TBuildingLoop::HandleNSCLData(TNSCLEvent& event) {
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
*/

bool TBuildingLoop::CheckBuildWindow(TRawEvent *event) {
  long timestamp = event->GetTimestamp();
  if(timestamp > event_start + build_window) {
    //next_event->Build();
    output_queue.Push(next_event);
    next_event.clear(); // = new TUnpackedEvent;
    event_start = timestamp;
    return true;
  }
  return false;
}

/*
void TBuildingLoop::HandleBuiltNSCLData(TNSCLEvent& event){
  TNSCLBuiltRingItem built(event);
  for(unsigned int i=0; i<built.NumFragments(); i++){
    TNSCLFragment& fragment = built.GetFragment(i);
    kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(fragment.GetFragmentSourceID());
    CheckBuildWindow(fragment.GetFragmentTimestamp());
    next_event->AddRawData(fragment.GetNSCLEvent(), detector);
  }
}

void TBuildingLoop::HandleUnbuiltNSCLData(TNSCLEvent& event){
  kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(event);
  CheckBuildWindow(event.GetTimestamp());
  next_event->AddRawData(event, detector);
}

void TBuildingLoop::HandleGEBMode3(TGEBEvent& event, kDetectorSystems system){
  TGEBMode3Event built(event);
  for(unsigned int i=0; i<built.NumFragments(); i++){
    TGEBEvent& fragment = built.GetFragment(i);
    next_event->AddRawData(event, system);
  }
}

void TBuildingLoop::HandleS800Scaler(TGEBEvent& event){
  TUnpackedEvent* scaler_event = new TUnpackedEvent;
  scaler_event->AddRawData(event, kDetectorSystems::S800SCALER);
  scaler_event->Build();
  output_queue.Push(scaler_event);
}

void TBuildingLoop::HandleGEBData(TGEBEvent& event){
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

*/


