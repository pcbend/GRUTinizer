#include "TGRUTLoop.h"

#include <chrono>
#include <thread>

#include "TRawEventSource.h"

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
  tree = NULL;
}

TGRUTLoop::~TGRUTLoop(){
  delete queue;
  if(outfile){
    delete outfile;
  }
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

int TGRUTLoop::ProcessEvent(TRawEvent& event){
  queue->Push(event);
}

void TGRUTLoop::ProcessFile(const char* input, const char* output){
  TDataLoop::ProcessFile(input, TGRUTOptions::Get()->IsOnline());
  if(!outfile){
    InitOutfile(TGRUTOptions::Get()->DetermineFileType(input),
                output);
  }
}

void TGRUTLoop::ProcessFile(const std::vector<std::string>& input, const char* output){
  TDataLoop::ProcessFile(input);
  if(!outfile){
    InitOutfile(TGRUTOptions::Get()->DetermineFileType(input.at(0)),
                output);
  }
}

void TGRUTLoop::ProcessRing(const char* input, const char* output) {
  TDataLoop::ProcessRing(input);
  if(!outfile){
    InitOutfile(TGRUTOptions::Get()->DefaultFileType(),
                output);
  }
}

void TGRUTLoop::ProcessTree(TTree* input, const char* output){
  tree = input;
  TDataLoop::ProcessFile("/dev/null");
  InitOutfile(kFileType::GRETINA_MODE2, NULL);
  outfile->InitTree(tree);
}

void TGRUTLoop::InitOutfile(kFileType file_type, const char* output) {
  switch(file_type) {
    case kFileType::NSCL_EVT:
      outfile = new TRootOutfileNSCL();
      break;
    case kFileType::GRETINA_MODE2:
    case kFileType::GRETINA_MODE3:
      outfile = new TRootOutfileGEB();
      break;
    default:
      fprintf(stderr,"%s: trying to sort unknown filetype:%s\n",__PRETTY_FUNCTION__,file_type);
      exit(1);
  }
  // Part of a dirty hack to loop over trees
  // This way, I can initialize it myself, elsewhere
  if(output){
    outfile->InitFile(output);
  }
  if(TGRUTOptions::Get()->CompiledHistogramFile().length()){
    outfile->LoadCompiledHistogramFile(TGRUTOptions::Get()->CompiledHistogramFile());
  }
}

bool TGRUTLoop::Initialize(){
  if(!outfile) { // bad things are about to happen.
    std::cerr << "Cannot start data loop without output file defined" << std::endl;
    return false;
  }

  write_thread = std::thread(&TGRUTLoop::WriteLoop, this);
  return true;
}

void TGRUTLoop::WriteLoop(){
  // Ugly hack to loop over a tree instead if
  // Probably should have a different class entirely that holds the TRootOutfile.
  // Will need some other way of letting the python GUI grab compiled histograms.
  if(tree){
    TreeLoop();
    return;
  }

  std::cout << "Write loop starting" << std::endl;
  while(running || queue->Size()){
    if(queue->Size()){
      TRawEvent event = queue->Pop();


      if(running || !TGRUTOptions::Get()->IsOnline()){
        ProcessFromQueue(event);
      }


      if(!running && queue->Size() % 100 == 0){
	std::cout << "Queue size: " << queue->Size() << "     \r" << std::flush;
      }
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  std::cout << "\nFlushing last event" << std::endl;
  outfile->FillAllTrees();
  std::cout << "Write loop ending" << std::endl;
}

void TGRUTLoop::TreeLoop(){
  long nentries = tree->GetEntries();
  for(long ientry = 0; ientry<nentries; ientry++){
    if(ientry % 10000 == 0){
      std::cout << "Tree loop: " << ientry << "     \r" << std::flush;
    }

    tree->GetEntry(ientry);
    outfile->FillHistograms();
  }
  std::cout << std::endl;
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
  TNSCLBuiltRingItem built(event);
  for(unsigned int i=0; i<built.NumFragments(); i++){
    TNSCLFragment& fragment = built.GetFragment(i);
    kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(fragment.GetFragmentSourceID());
    outfile->FillTree("EventTree",fragment.GetFragmentTimestamp());
    outfile->AddRawData(fragment.GetNSCLEvent(), detector);
  }
}

void TGRUTLoop::HandleUnbuiltNSCLData(TNSCLEvent& event){
  kDetectorSystems detector = TDetectorEnv::Get().DetermineSystem(event);
  outfile->FillTree("EventTree", event.GetTimestamp());
  outfile->AddRawData(event, detector);
}

void TGRUTLoop::HandleGEBMode3(TGEBEvent& event, kDetectorSystems system){
  TGEBMode3Event built(event);
  for(unsigned int i=0; i<built.NumFragments(); i++){
    TGEBEvent& fragment = built.GetFragment(i);
    outfile->AddRawData(fragment, system);
  }
}

void TGRUTLoop::HandleGEBData(TGEBEvent& event){
  int type = event.GetEventType();
  TRootOutfileGEB *gebout = (TRootOutfileGEB*)outfile;

  switch(type) {
    case 1: // Gretina decomp data.
      gebout->FillTree("EventTree",event.GetTimestamp());
      gebout->AddRawData(event, kDetectorSystems::GRETINA);
      break;
    case 2: // Gretina Mode3 data.
      if(!TGRUTOptions::Get()->IgnoreMode3()) {
        gebout->FillTree("EventTree",event.GetTimestamp());
        HandleGEBMode3(event, kDetectorSystems::MODE3);
      }
      break;
    case 5: // S800 Mode2 equvilant.
      gebout->FillTree("EventTree",event.GetTimestamp());
      gebout->AddRawData(event, kDetectorSystems::S800);
      break;
    case 8: // Gretina diag. data.
      gebout->FillTree("EventTree",event.GetTimestamp());
      HandleGEBMode3(event, kDetectorSystems::BANK29);
      break;
    case 10:
      gebout->AddRawData(event,kDetectorSystems::S800SCALER);
      gebout->FillTree("ScalerTree");
      // S800 scaler data....
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

void TGRUTLoop::Status() {
  if(!GetInfile())  {
    std::cout << "Status: Not running" << std::endl;
  } else {
    std::cout << "Status: " << std::endl;
    while(GetInfile() && !GetInfile()->IsFinished())  {
      std::cout << "\r" << GetInfile()->Status() << "             " << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }
}
