#include "TTerminalLoop.h"

#include "TUnpackedEvent.h"

TTerminalLoop* TTerminalLoop::Get(std::string name){
  if(name.length()==0){
    name = "terminal_loop";
  }

  StoppableThread* thread = StoppableThread::Get(name);
  if(!thread){
    thread = new TTerminalLoop(name);
  }

  return dynamic_cast<TTerminalLoop*>(thread);
}

TTerminalLoop::TTerminalLoop(std::string name)
  : StoppableThread(name),
    input_queue(std::make_shared<ThreadsafeQueue<TUnpackedEvent*> >()) { }



bool TTerminalLoop::Iteration() {
  TUnpackedEvent* event = NULL;
  input_queue->Pop(event);

  if(event) {
    delete event;
    return true;
  } else if(input_queue->IsFinished()) {
    return false;
  } else {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return true;
  }
}

void TTerminalLoop::ClearQueue() {
  while(input_queue->Size()){
    TUnpackedEvent* event = NULL;
    input_queue->Pop(event);
    if(event){
      delete event;
    }
  }
}
