#ifndef _TUNPACKLOOP_H_
#define _TUNPACKLOOP_H_

#include "TNamed.h"

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TRawEvent.h"
//#include "TBuildingEvent.h"

#ifndef __CINT__
#include <condition_variable>
#include <mutex>
#include <queue>
#endif

#include "TRawEvent.h"
#include "TStopwatch.h"


class TBuildingLoop;
class TNSCLEvent;
class TGEBEvent;
class TUnpackedEvent;

class TDetector;

class TUnpackingLoop : public StoppableThread {
public:
  static TUnpackingLoop *Get(std::string name="",TBuildingLoop *input=0);
  virtual ~TUnpackingLoop();

  //ThreadsafeQueue<TBuiltEvent*>& GetOutputQueue() { return output_queue; }

  //protected:
  bool Iteration();
  virtual void ClearQueue();

  size_t GetItemsPushed()  { return output_queue.ItemsPushed(); }
  size_t GetItemsPopped()  { return output_queue.ItemsPopped(); }
  size_t GetItemsCurrent() { return output_queue.Size();        }
  size_t GetRate()         { return 0; }

  int Pop(TUnpackedEvent*& event, int millisecond_wait=1000) {
    return output_queue.Pop(event, millisecond_wait);
  }

private:
  TUnpackingLoop(std::string name, TBuildingLoop*);
  TUnpackingLoop(const TUnpackingLoop& other);
  TUnpackingLoop& operator=(const TUnpackingLoop& other);

  void HandleNSCLData(TNSCLEvent& event);
  void HandleBuiltNSCLData(TNSCLEvent& event);
  void HandleUnbuiltNSCLData(TNSCLEvent& event);
  void HandleNSCLPeriodicScalers(TNSCLEvent& event);

  void HandleGEBData(TGEBEvent& event);
  void HandleGEBMode3(TGEBEvent& event, kDetectorSystems system);
  void HandleS800Scaler(TGEBEvent& event);

  TBuildingLoop *input_source;

  //ThreadsafeQueue<TRawEvent>               & input_queue;
  //ThreadsafeQueue<std::vector<TDetector*> > output_queue;
  TUnpackedEvent* fOutputEvent;
  ThreadsafeQueue<TUnpackedEvent*> output_queue;

  unsigned int fRunStart;
};

#endif /* _TUNPACKLOOP_H_ */
