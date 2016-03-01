#include "TDataLoop.h"

#include <chrono>
#include <thread>
#include <utility>

#include "TGRUTOptions.h"
#include "TRawSource.h"
#include "TString.h"


TDataLoop::TDataLoop(std::string name,TRawEventSource* source)
  : StoppableThread(name), source(source), fSelfStopping(true) {
}

TDataLoop::~TDataLoop(){
  delete source; // do we really want the loop to take ownership?
}

TDataLoop *TDataLoop::Get(std::string name,TRawEventSource* source) {
  if(name.length()==0)
    name = "input_loop";
  //Stoppable::StoppableThread(name);
  TDataLoop *loop = dynamic_cast<TDataLoop*>(StoppableThread::Get(name));
  if(!loop && source)
    loop = new TDataLoop(name,source);
  return loop;
}

void TDataLoop::ClearQueue() {
  TRawEvent event;
  while(output_queue.Size()){
    output_queue.Pop(event);
  }
}

void TDataLoop::ReplaceSource(TRawEventSource* new_source) {
  std::lock_guard<std::mutex> lock(source_mutex);
  delete source;
  source = new_source;
}

void TDataLoop::ResetSource() {
  std::lock_guard<std::mutex> lock(source_mutex);
  source->Reset();
}

bool TDataLoop::Iteration() {
  TRawEvent evt;
  int bytes_read;
  {
    std::lock_guard<std::mutex> lock(source_mutex);
    bytes_read = source->Read(evt);
  }

  if(bytes_read < 0 && fSelfStopping){
    // Error, and no point in trying again.
    printf("finished sorting all input.\n");
    return false;
  } else if(bytes_read > 0){
    // A good event was returned
    output_queue.Push(evt);
    return true;
  } else {
    static TRawEventSource* source_ptr = NULL;
    if(source_ptr != source){
      std::cout << "Finished reading source" << std::endl;
      source_ptr = source;
    }
    // Nothing returned this time, but I might get something next time.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return true;
  }
  return true;
}

int TDataLoop::Pop(TRawEvent &event) {
  return output_queue.Pop(event);
}


std::string TDataLoop::Status() {
  return source->Status();
}
