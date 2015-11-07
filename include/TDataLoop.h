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

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TRawEvent.h"

class TRawEventSource;

class TDataLoop : public StoppableThread {
public:
  static TDataLoop *Get(std::string name, TRawEventSource* source=0,ThreadsafeQueue<TRawEvent>* output_queue=0);
  static int GetNDataLoops();
  virtual ~TDataLoop();

  const TRawEventSource& GetSource() const { return *source; }

  std::string Status();

protected:
  bool Iteration();
  

private:
  TDataLoop(TRawEventSource* source,
            ThreadsafeQueue<TRawEvent>& output_queue,std::string name="");
  TDataLoop();
  TDataLoop(const TDataLoop& other);
  TDataLoop& operator=(const TDataLoop& other);
  static std::map<std::string,TDataLoop*> fdataloopmap;

  ThreadsafeQueue<TRawEvent>& output_queue;
  TRawEventSource* source;
  std::string fname;


  ClassDef(TDataLoop, 0);
};

#endif /* _TDATALOOP_H_ */
