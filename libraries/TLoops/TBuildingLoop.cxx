#include "TBuildingLoop.h"

#include <chrono>
#include <thread>

ClassImp(TBuildingLoop)

TBuildingLoop *TBuildingLoop::Get(std::string name) {
  if(name.length()==0) {
    name = "build_loop";
  }

  TBuildingLoop *loop = (TBuildingLoop*)StoppableThread::Get(name);
  if(!loop) {
    loop = new TBuildingLoop(name);
  }
  return loop;
}

TBuildingLoop::TBuildingLoop(std::string name)
  : StoppableThread(name),
    input_queue(std::make_shared<ThreadsafeQueue<TRawEvent> >()),
    output_queue(std::make_shared<ThreadsafeQueue<std::vector<TRawEvent> > >()) {

  SetBuildWindow(1000);
  event_start = 0;
}

TBuildingLoop::~TBuildingLoop() { }

void TBuildingLoop::ClearQueue() {
  TRawEvent single_event;
  while(input_queue->Size()) {
    input_queue->Pop(single_event);
  }

  std::vector<TRawEvent> event;
  while(output_queue->Size()){
    output_queue->Pop(event);
  }
}

bool TBuildingLoop::Iteration(){
  TRawEvent event;

  int error = input_queue->Pop(event);
  if(error<0) {
    if(input_queue->IsFinished()){
      // Source is dead, push the last event and stop.
      output_queue->Push(next_event);
      output_queue->SetFinished();
      return false;
    } else {
      // Wait for the source to give more data.
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      return true;
    }
  }


  CheckBuildWindow(&event);
  next_event.push_back(event);

  return true;
}

bool TBuildingLoop::CheckBuildWindow(TRawEvent *event) {
  long timestamp = event->GetTimestamp();

  if(timestamp == -1){
    return false;
  }

  if(timestamp > event_start + build_window ||
     timestamp < event_start - build_window) {
    output_queue->Push(next_event);
    next_event.clear();
    event_start = timestamp;
    return true;
  }
  return false;
}
