#include "TUnpackingLoop.h"

#include <chrono>
#include <thread>

//#include "RawDataQueue.h"
//#include "RawDataQueue.h"
//#include "TDetectorEnv.h"
#include "TGEBEvent.h"
#include "TGRUTOptions.h"
#include "TNSCLEvent.h"

#include "TDetectorEnv.h"
#include "TUnpackedEvent.h"

//#include "TMode3.h"

ClassImp(TUnpackingLoop)

TUnpackingLoop *TUnpackingLoop::Get(std::string name) {
  if(name.length() == 0) {
    name = "unpack_loop";
  }
  TUnpackingLoop *loop = (TUnpackingLoop*)StoppableThread::Get(name);
  if(!loop) {
    loop = new TUnpackingLoop(name);
  }
  return loop;
}

TUnpackingLoop::TUnpackingLoop(std::string name)
  : StoppableThread(name),
    fOutputEvent(NULL), fRunStart(0),
    input_queue(std::make_shared<ThreadsafeQueue<std::vector<TRawEvent> > >()),
    output_queue(std::make_shared<ThreadsafeQueue<TUnpackedEvent*> >()) { }

TUnpackingLoop::~TUnpackingLoop() { }

bool TUnpackingLoop::Iteration(){
  std::vector<TRawEvent> event;

  int error = input_queue->Pop(event);
  if(error < 0){
    if(input_queue->IsFinished()) {
      output_queue->SetFinished();
      return false;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      return true;
    }
  }

  fOutputEvent = new TUnpackedEvent;
  for(unsigned int i=0;i<event.size();i++) {
    TRawEvent& raw_event = event[i];
    switch(raw_event.GetFileType()){
      case kFileType::NSCL_EVT:
      {
        TNSCLEvent nscl_event(raw_event);
        HandleNSCLData(nscl_event);
      }
      break;

      case kFileType::ANL_RAW:
      case kFileType::GRETINA_MODE2:
      case kFileType::GRETINA_MODE3:
      {
        TGEBEvent geb_event(raw_event);
        HandleGEBData(geb_event);
      }
      break;
      case kFileType::RCNP_BLD:
      {
        //TRCNPEvent& evt = (TRCNPEvent&)raw_event;
        fOutputEvent->AddRawData(raw_event, kDetectorSystems::GRAND_RAIDEN);
        //consider adding HandleRCNPData
      }
      break;
      case kFileType::ROOT_DATA:
      {
        fOutputEvent->AddRawData(raw_event, kDetectorSystems::GRAND_RAIDEN);
      }
      break;
      default:
        break;
    }
  }

  fOutputEvent->Build();
  fOutputEvent->SetRunStart(fRunStart);

  if(fOutputEvent->GetDetectors().size() != 0){
    output_queue->Push(fOutputEvent);
    fOutputEvent = NULL;
  }
  return true;
}

void TUnpackingLoop::ClearQueue() {
  std::vector<TRawEvent> raw_event;
  while(input_queue->Size()) {
    input_queue->Pop(raw_event);
  }

  while(output_queue->Size()){
    TUnpackedEvent* event = NULL;
    output_queue->Pop(event);
    if(event){
      delete event;
    }
  }
}

void TUnpackingLoop::HandleNSCLData(TNSCLEvent& event) {
  switch(event.GetEventType()) {
    case kNSCLEventType::BEGIN_RUN:            // 0x0001
    {
      TRawEvent::TNSCLBeginRun* begin = (TRawEvent::TNSCLBeginRun*)event.GetPayload();
      fRunStart = begin->unix_time;
    }
      break;
    case kNSCLEventType::END_RUN:              // 0x0002
    case kNSCLEventType::PAUSE_RUN:            // 0x0003
    case kNSCLEventType::RESUME_RUN:           // 0x0004
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
      HandleNSCLPeriodicScalers(event);
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


void TUnpackingLoop::HandleBuiltNSCLData(TNSCLEvent& event){
  TNSCLBuiltRingItem built(event);
  for(unsigned int i=0; i<built.NumFragments(); i++){
    TNSCLFragment& fragment = built.GetFragment(i);
    int source_id = fragment.GetFragmentSourceID();
    kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(source_id);
    TRawEvent frag_event = fragment.GetNSCLEvent();
    // S800 events in CAESAR/S800 event have no body header.
    // This grabs the timestamp from the fragment header so it can be used later.
    if(frag_event.GetTimestamp() == -1){
      frag_event.SetFragmentTimestamp(fragment.GetFragmentTimestamp());
    }
    fOutputEvent->AddRawData(frag_event, detector);
  }
}

void TUnpackingLoop::HandleUnbuiltNSCLData(TNSCLEvent& event){
  kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(event);
  fOutputEvent->AddRawData(event, detector);
}

void TUnpackingLoop::HandleGEBMode3(TGEBEvent& event, kDetectorSystems system){
  TGEBMode3Event built(event);
  for(unsigned int i=0; i<built.NumFragments(); i++){
    TGEBEvent& fragment = built.GetFragment(i);
    fOutputEvent->AddRawData(fragment, system);
  }
}

void TUnpackingLoop::HandleNSCLPeriodicScalers(TNSCLEvent& event){
  TUnpackedEvent* scaler_event = new TUnpackedEvent;
  scaler_event->AddRawData(event, kDetectorSystems::NSCLSCALERS);
  scaler_event->Build();
  scaler_event->SetRunStart(fRunStart);
  output_queue->Push(scaler_event);
}

void TUnpackingLoop::HandleS800Scaler(TGEBEvent& event){
  TUnpackedEvent* scaler_event = new TUnpackedEvent;
  scaler_event->AddRawData(event, kDetectorSystems::S800SCALER);
  scaler_event->Build();
  scaler_event->SetRunStart(fRunStart);
  output_queue->Push(scaler_event);
}

void TUnpackingLoop::HandleGEBData(TGEBEvent& event){
  int type = event.GetEventType();

  switch(type) {
    case 1: // Gretina decomp data.
      fOutputEvent->AddRawData(event, kDetectorSystems::GRETINA);
      break;
    case 2: // Gretina Mode3 data.
      HandleGEBMode3(event, kDetectorSystems::MODE3);
      break;
    case 5: // S800 Mode2 equvilant.
      fOutputEvent->AddRawData(event, kDetectorSystems::S800);
      break;
    case 8: // Gretina diag. data.
      HandleGEBMode3(event, kDetectorSystems::BANK29);
      break;
    case 9:
      //Simulated S800 data,
      fOutputEvent->AddRawData(event, kDetectorSystems::S800_SIM);
      //event.Print("all");
      //std::cout << " In simulated S800 packet" << std::endl;
      break;
    case 10: // S800 scaler data....
      HandleS800Scaler(event);
      break;
    case 11:
      //GEANT4 gretina simulated data.
      fOutputEvent->AddRawData(event,kDetectorSystems::GRETINA_SIM);
      //event.Print("all");
      //exit(1);
      break;
    case 14:
      fOutputEvent->AddRawData(event, kDetectorSystems::ANL);
      break;
    case 17: //PWall Mode2 equivlant.
      fOutputEvent->AddRawData(event, kDetectorSystems::PHOSWALL);
      break;
    case 29: // Something.
      fOutputEvent->AddRawData(event, kDetectorSystems::BANK29);
      break;
    default:
      std::cout << "Dance Party EventType: " << type << std::endl;
      //dance party.
      break;
  }
}
