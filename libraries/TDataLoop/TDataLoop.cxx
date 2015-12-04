#include "TDataLoop.h"

#include <chrono>
#include <thread>
#include <utility>

#include "TRawSource.h"
#include "TString.h"


TDataLoop::TDataLoop(std::string name,TRawEventSource* source)
  : StoppableThread(name), source(source) {
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


bool TDataLoop::Iteration() {
  TRawEvent evt;
  int bytes_read = source->Read(evt);

  if(bytes_read < 0){
    // Error, and no point in trying again.
    printf("file finished??\n");
    return false;
  } else if(bytes_read > 0){
    // A good event was returned
    output_queue.Push(evt);
    return true;
  } else {
    printf("file finished??\n");
    // Nothing returned this time, but I might get something next time.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return true;
  }
}

int TDataLoop::Pop(TRawEvent &event) {
  return output_queue.Pop(event);
}


std::string TDataLoop::Status() {
  return source->Status();
}
