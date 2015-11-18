#ifndef _TDATALOOP_H_
#define _TDATALOOP_H_

#ifndef __CINT__
#include <atomic>
#include <condition_variable>
#include <thread>
#endif

#include <cstdlib>
#include <iostream>
#include <map>

#include <TNamed.h>

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TRawEvent.h"

class TRawEventSource;

class TDataLoop : public StoppableThread  {
public:
  static TDataLoop *Get(std::string name="", TRawEventSource* source=0); 
  virtual ~TDataLoop();

  const TRawEventSource& GetSource() const { return *source; }

  std::string Status();

//protected:
  bool Iteration();
  int Pop(TRawEvent &event) { return output_queue.Pop(event); }


private:
  TDataLoop(std::string name,TRawEventSource* source);
            //ThreadsafeQueue<TRawEvent>& output_queue,std::string name="");
  TDataLoop();
  TDataLoop(const TDataLoop& other);
  TDataLoop& operator=(const TDataLoop& other);

  ThreadsafeQueue<TRawEvent> output_queue;
  TRawEventSource* source;

  ClassDef(TDataLoop, 0);
};

#endif /* _TDATALOOP_H_ */
