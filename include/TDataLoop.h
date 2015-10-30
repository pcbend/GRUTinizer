#ifndef _TDATALOOP_H_
#define _TDATALOOP_H_

#ifndef __CINT__
#include <atomic>
#include <condition_variable>
#include <thread>
#endif

#include <cstdlib>
#include <iostream>

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TRawEvent.h"

class TRawEventSource;

class TDataLoop : public StoppableThread {
public:
  TDataLoop(TRawEventSource* source,
            ThreadsafeQueue<TRawEvent>& output_queue);
  virtual ~TDataLoop();

  const TRawEventSource& GetSource() const { return *source; }

protected:
  bool Iteration();

private:
  TDataLoop();
  TDataLoop(const TDataLoop& other);
  TDataLoop& operator=(const TDataLoop& other);

  ThreadsafeQueue<TRawEvent>& output_queue;
  TRawEventSource* source;

  ClassDef(TDataLoop, 0);
};

#endif /* _TDATALOOP_H_ */
