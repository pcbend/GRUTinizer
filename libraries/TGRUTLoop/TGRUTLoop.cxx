#include "TGRUTLoop.h"

#include "RawDataQueue.h"
#include "TNSCLEvent.h"
#include "TRootOutfile.h"

ClassImp(TGRUTLoop);

TGRUTLoop::TGRUTLoop(){
  queue = new RawDataQueue;
  outfile = NULL;
}

TGRUTLoop::~TGRUTLoop(){
  delete queue;
}

TGRUTLoop* TGRUTLoop::Get(){
  return (TGRUTLoop*) Instance();
}

void TGRUTLoop::PrintQueue(){
  queue->Print();
}

void TGRUTLoop::StatusQueue(){
  queue->Status();
}

TRawEvent TGRUTLoop::GetEvent() {
  return queue->Pop();
}

int TGRUTLoop::ProcessEvent(TRawEvent& event){
  queue->Push(event);

  if(event.GetFileType()==kFileType::NSCL_EVT) {
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
        //HandleNSCLPhysics(event);
        break;
    };
  }else if(event.GetFileType()==kFileType::GRETINA_MODE2) {
    //switch(event->GetType()) {
    //};
  }
}

void TGRUTLoop::Initialize(){
  outfile = new TRootOutfile();
  outfile->Init("my_output.root");
  write_thread = std::thread(&TGRUTLoop::WriteLoop, this);
}

void TGRUTLoop::WriteLoop(){
  std::cout << "Write loop starting" << std::endl;
  while(running || queue->Size()){
    TRawEvent event = queue->Pop();
    outfile->AddRawData(event);
  }
  std::cout << "Write loop ending" << std::endl;
}

void TGRUTLoop::Finalize(){
  std::cout << "Waiting for write_thread to finish" << std::endl;
  write_thread.join();
  std::cout << "About to finalize file" << std::endl;
  outfile->FinalizeFile();
  std::cout << "Finalizing file" << std::endl;
}

void TGRUTLoop::PrintOutfile(){
  if(outfile){
    outfile->Print();
  } else {
    std::cout << "No output file yet" << std::endl;
  }
}
