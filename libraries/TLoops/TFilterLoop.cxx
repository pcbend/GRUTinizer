#include "TFilterLoop.h"

#include "MakeUnique.h"
#include "TGRUTOptions.h"
#include "TRawFileOut.h"

TFilterLoop* TFilterLoop::Get(std::string name) {
  if(name.length()==0)
    name = "filter_loop";
  TFilterLoop *loop = dynamic_cast<TFilterLoop*>(StoppableThread::Get(name));
  if(!loop)
    loop = new TFilterLoop(name);
  return loop;
}

TFilterLoop::TFilterLoop(std::string name)
  : StoppableThread(name),
    input_queue(std::make_shared<ThreadsafeQueue<TUnpackedEvent*> >()),
    output_queue(std::make_shared<ThreadsafeQueue<TUnpackedEvent*> >()),
    filtered_output(nullptr) {

  LoadLib(TGRUTOptions::Get()->CompiledFilterFile());
}

TFilterLoop::~TFilterLoop() { }

void TFilterLoop::ClearQueue() {
  while(input_queue->Size()){
    TUnpackedEvent* event = NULL;
    input_queue->Pop(event);
    if(event){
      delete event;
    }
  }

  while(output_queue->Size()){
    TUnpackedEvent* event = NULL;
    output_queue->Pop(event);
    if(event){
      delete event;
    }
  }
}

bool TFilterLoop::Iteration() {
  TUnpackedEvent* event = NULL;
  input_queue->Pop(event);

  if(event) {
    HandleEvent(event);
    return true;
  } else if(input_queue->IsFinished()) {
    output_queue->SetFinished();
    return false;
  } else {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return true;
  }
}

void TFilterLoop::HandleEvent(TUnpackedEvent* event) {
  if(compiled_filter.MatchesCondition(*event)) {
    if(filtered_output) {
      filtered_output->Write(*event);
    }
    // No need to keep the raw data around after writing it to disk,
    //   can save on RAM at this point.
    event->ClearRawData();
    output_queue->Push(event);
  } else {
    delete event;
  }
}

void TFilterLoop::OpenRawOutputFile(const std::string& output_filename) {
  filtered_output = make_unique<TRawFileOut>(output_filename);
}

void TFilterLoop::AddCutFile(TFile* cut_file) {
  compiled_filter.AddCutFile(cut_file);
}

void TFilterLoop::LoadLibrary(std::string library) {
  compiled_filter.Load(library);
}

std::string TFilterLoop::GetLibraryName() const {
  return compiled_filter.GetLibraryName();
}
