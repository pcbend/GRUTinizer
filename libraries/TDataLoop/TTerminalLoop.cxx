#include "TTerminalLoop.h"

TTerminalLoop::TTerminalLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue)
  : input_queue(input_queue) { }


TTerminalLoop::~TTerminalLoop() { }

bool TTerminalLoop::Iteration() {
  TUnpackedEvent* event = NULL;
  int error = input_queue.Pop(event);

  if(error && input_queue.IsClosed()){
    return false;
  }

  if(!error) {
    delete event;
  }
  return true;
}
