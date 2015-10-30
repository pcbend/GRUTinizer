#ifndef _TTERMINALLOOP_H_
#define _TTERMINALLOOP_H_

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TUnpackedEvent.h"

class TTerminalLoop : public StoppableThread {
public:
  TTerminalLoop(ThreadsafeQueue<TUnpackedEvent*>& input_queue);
  virtual ~TTerminalLoop();

protected:
  bool Iteration();

private:
  ThreadsafeQueue<TUnpackedEvent*>& input_queue;
};

#endif /* _TTERMINALLOOP_H_ */
