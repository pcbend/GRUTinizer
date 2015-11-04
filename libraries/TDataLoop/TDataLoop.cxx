#include "TDataLoop.h"

#include <chrono>
#include <thread>

#include "TRawEventSource.h"

TDataLoop::TDataLoop(TRawEventSource* source,
                     ThreadsafeQueue<TRawEvent>& output_queue)
  : output_queue(output_queue), source(source) { }

TDataLoop::~TDataLoop(){
  delete source;
}

bool TDataLoop::Iteration() {
  TRawEvent evt;
  int bytes_read = source->Read(evt);

  if(bytes_read < 0){
    // Error, and no point in trying again.
    return false;
  } else if(bytes_read > 0){
    // A good event was returned
    output_queue.Push(evt);
    return true;
  } else {
    // Nothing returned this time, but I might get something next time.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return true;
  }
}

std::string TDataLoop::Status() {
  return source->Status();
}
