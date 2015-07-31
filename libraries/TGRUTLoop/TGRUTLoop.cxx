#include "TGRUTLoop.h"

#include <chrono>
#include <thread>

#include "RawDataQueue.h"
#include "TDetectorEnv.h"
#include "TGEBEvent.h"
#include "TGRUTOptions.h"
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
}

void TGRUTLoop::ProcessFile(const char* input, const char* output){
  TDataLoop::ProcessFile(input);
  outfile = new TRootOutfile();
  outfile->Init(output);
}

void TGRUTLoop::ProcessFile(const std::vector<std::string>& input, const char* output){
  TDataLoop::ProcessFile(input);
  outfile = new TRootOutfile();
  outfile->Init(output);
}

void TGRUTLoop::Initialize(){
  if(!outfile){
    outfile = new TRootOutfile();
    outfile->Init("my_output.root");
  }
  write_thread = std::thread(&TGRUTLoop::WriteLoop, this);
}

void TGRUTLoop::WriteLoop(){
  std::cout << "Write loop starting" << std::endl;
  while(running || queue->Size()){
    TRawEvent event = queue->Pop();
    ProcessFromQueue(event);
  }
  std::cout << "Flushing last event" << std::endl;
  outfile->FillTree();
  std::cout << "Write loop ending" << std::endl;
}

void TGRUTLoop::ProcessFromQueue(TRawEvent& event){
  if(event.GetFileType()==kFileType::NSCL_EVT) {
    TNSCLEvent& nscl_event = (TNSCLEvent&)event;
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
      if(nscl_event.IsBuiltData()){
        HandleBuiltNSCLData(nscl_event);
      } else {
        HandleUnbuiltNSCLData(nscl_event);
      }
      break;
    };
  }else if(event.GetFileType()==kFileType::GRETINA_MODE2) {
    TGEBEvent& geb_event = (TGEBEvent&)event;
    //switch(event->GetType()) {
    HandleGEBData(geb_event);
  //};
  }
}

void TGRUTLoop::Finalize(){
  std::cout << "Waiting for write_thread to finish" << std::endl;
  if(std::this_thread::get_id() != write_thread.get_id()){
    write_thread.join();
  }
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

void TGRUTLoop::HandleBuiltNSCLData(TNSCLEvent& event){
  if(FillCondition(event)){
    outfile->FillTree();
  }

  TNSCLBuiltRingItem built(event);
  for(unsigned int i=0; i<built.NumFragments(); i++){
    TNSCLFragment& fragment = built.GetFragment(i);
    kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(fragment.GetFragmentSourceID());
    outfile->AddRawData(fragment.GetNSCLEvent(), detector);
  }
}

void TGRUTLoop::HandleUnbuiltNSCLData(TNSCLEvent& event){
  if(FillCondition(event)){
    outfile->FillTree();
  }

  kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(event);
  outfile->AddRawData(event, detector);
}

void TGRUTLoop::HandleGEBData(TGEBEvent& event){
  if(FillCondition(event)){
    outfile->FillTree();
  }

  kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(event);
  outfile->AddRawData(event, detector);
}

bool TGRUTLoop::FillCondition(TRawEvent& event){
  return true;
}

void TGRUTLoop::Status() {
  if(!GetInfile())  {
    std::cout << "Status: Not running" << std::endl;
  } else {
    std::cout << "Status: " << std::endl;
    while(!GetInfile()->IsFinished())  {
      std::cout << "\r" << GetInfile()->Status() << "             " << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }
}
