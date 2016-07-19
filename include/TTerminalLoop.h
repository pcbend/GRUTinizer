#ifndef _TTERMINALLOOP_H_
#define _TTERMINALLOOP_H_

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"

class TUnpackedEvent;

class TTerminalLoop : public StoppableThread {
public:
  static TTerminalLoop* Get(std::string name="");

  ~TTerminalLoop() { }

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> >& InputQueue() { return input_queue; }
#endif

  virtual void ClearQueue();

  virtual size_t GetItemsPopped() { return 0; }
  virtual size_t GetItemsPushed() { return 0; }
  virtual size_t GetItemsCurrent() { return 0; }
  virtual size_t GetRate() { return 0; }

protected:
  bool Iteration();

private:
  TTerminalLoop(std::string name);

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> > input_queue;
#endif

  ClassDef(TTerminalLoop,0);
};

#endif /* _TTERMINALLOOP_H_ */
