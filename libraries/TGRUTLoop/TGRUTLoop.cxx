#include "TGRUTLoop.h"

#include <chrono>
#include <thread>

#include "RawDataQueue.h"
#include "TDetectorEnv.h"
#include "TGEBEvent.h"
#include "TGRUTOptions.h"
#include "TNSCLEvent.h"

#include "TRootOutfileNSCL.h"
#include "TRootOutfileGEB.h"

#include "TMode3.h"

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
  switch(TGRUTOptions::Get()->DetermineFileType(input)) {
    case kFileType::NSCL_EVT:
      outfile = new TRootOutfileNSCL();
      break;
    case kFileType::GRETINA_MODE2:
    case kFileType::GRETINA_MODE3:
      outfile = new TRootOutfileGEB();
      break;
    default:
      fprintf(stderr,"%s: trying to sort unknown filetype:%s\n",__PRETTY_FUNCTION__,input);
      exit(1);
  };
  outfile->Init(output);
}

void TGRUTLoop::ProcessFile(const std::vector<std::string>& input, const char* output){
  TDataLoop::ProcessFile(input);
  switch(TGRUTOptions::Get()->DetermineFileType(input.at(0))) {
    case kFileType::NSCL_EVT:
      outfile = new TRootOutfileNSCL();
      break;
    case kFileType::GRETINA_MODE2:
    case kFileType::GRETINA_MODE3:
      outfile = new TRootOutfileGEB();
      break;
    default:
      fprintf(stderr,"%s: trying to sort unknown filetype:%s\n",__PRETTY_FUNCTION__,input.at(0).c_str());
      exit(1);
  };
  //outfile = new TRootOutfile();
  outfile->Init(output);
}

void TGRUTLoop::Initialize(){
  //printf("%s called.\n",__PRETTY_FUNCTION__);

/*
  if(!outfile){
    switch(TGRUTOptions::Get()->DetermineFileType(input)) {
      case kFileType::NSCL_EVT:
        outfile = new TRootOutfileNSCL();
        break;
      case kFileType::GRETINA_MODE2:
      case kFileType::GRETINA_MODE3:
        outfile = new TRootOutfileGEB();
        break;
      default:
        fprintf(stderr,"%s: trying to sort unknown filetype:%s\n",__PRETTY_FUNCTION__,input);
        exit(1);
    };
  }
*/
  if(!outfile) // bad things are about to happen.
    exit(1);

  write_thread = std::thread(&TGRUTLoop::WriteLoop, this);
}

void TGRUTLoop::WriteLoop(){
  std::cout << "Write loop starting" << std::endl;
  //int counter = 0;
  while(running || queue->Size()){
    //printf("counter = %i\n",counter); fflush(stdout); counter++;
    TRawEvent event = queue->Pop();
    ProcessFromQueue(event);
  }
  std::cout << "Flushing last event" << std::endl;
  outfile->FillAllTrees();
  std::cout << "Write loop ending" << std::endl;
}

void TGRUTLoop::ProcessFromQueue(TRawEvent& event){
  if(event.GetFileType()==kFileType::NSCL_EVT) {
    TNSCLEvent nscl_event(event);
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
  }else if(event.GetFileType()==kFileType::GRETINA_MODE2 || event.GetFileType()==kFileType::GRETINA_MODE3)  {
    TGEBEvent geb_event(event);
    HandleGEBData(geb_event);
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
  if(event.FillCondition()){
    outfile->FillTree("EventTree");
    outfile->Clear();
  }

  TNSCLBuiltRingItem built(event);
  for(unsigned int i=0; i<built.NumFragments(); i++){
    TNSCLFragment& fragment = built.GetFragment(i);
    kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(fragment.GetFragmentSourceID());
    outfile->AddRawData(fragment.GetNSCLEvent(), detector);
  }
}

void TGRUTLoop::HandleUnbuiltNSCLData(TNSCLEvent& event){
  if(event.FillCondition()){
    outfile->FillTree("EventTree");
    outfile->Clear();
  }

  kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(event);
  outfile->AddRawData(event, detector);
}

void TGRUTLoop::HandleGEBData(TGEBEvent& event){
  int type = event.GetEventType();
  TRootOutfileGEB *gebout = (TRootOutfileGEB*)outfile;
  if(type==1 || type ==5 || type==17 ) {
    if(event.FillCondition()){
      gebout->FillTree("EventTree");
      gebout->Clear();
    }
  }
  switch(event.GetEventType()) {
    case 1: // Gretina Mode2 data.
      gebout->AddRawData(event, kDetectorSystems::GRETINA);
      break;
    case 2: // Gretina Mode3 data.
      if(!TGRUTOptions::Get()->IgnoreMode3()) {
        TGEBMode3Event m3event(event);
        TMode3 temp;
        //printf(DBLUE "I AM HERE!!!!" RESET_COLOR "\n"); fflush(stdout);
        while(m3event.GetNextItem(temp)) {
          //printf(DRED "I AM THERE!!!!" RESET_COLOR  "\n"); fflush(stdout);
          gebout->HandleMode3(temp);
        }
      }
      break;
    case 5: // S800 Mode2 equvilant.
      break;
    case 8: // Gretina diag. data.
      break;
    case 17: //PWall Mode2 equivlant.
      break;
    case 29: // Something.
      break;
    default:
      //dance party.
      break;
  };

}

//bool TGRUTLoop::FillCondition(TRawEvent& event){
//  return true;
//}

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
