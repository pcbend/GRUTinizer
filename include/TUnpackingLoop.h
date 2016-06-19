#ifndef _TUNPACKLOOP_H_
#define _TUNPACKLOOP_H_

#ifndef __CINT__
#include <condition_variable>
#include <mutex>
#include <queue>
#endif

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TRawEvent.h"

class TNSCLEvent;
class TGEBEvent;
class TUnpackedEvent;

class TDetector;

class TUnpackingLoop : public StoppableThread {
public:
  static TUnpackingLoop *Get(std::string name="");
  virtual ~TUnpackingLoop();

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<std::vector<TRawEvent> > >& InputQueue() { return input_queue; }
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> >& OutputQueue() { return output_queue; }
#endif

  bool Iteration();
  virtual void ClearQueue();

  size_t GetItemsPushed()  { return output_queue->ItemsPushed(); }
  size_t GetItemsPopped()  { return output_queue->ItemsPopped(); }
  size_t GetItemsCurrent() { return output_queue->Size();        }
  size_t GetRate()         { return 0; }

private:
  TUnpackingLoop(std::string name);
  TUnpackingLoop(const TUnpackingLoop& other);
  TUnpackingLoop& operator=(const TUnpackingLoop& other);

  void HandleNSCLData(TNSCLEvent& event);
  void HandleBuiltNSCLData(TNSCLEvent& event);
  void HandleUnbuiltNSCLData(TNSCLEvent& event);
  void HandleNSCLPeriodicScalers(TNSCLEvent& event);

  void HandleGEBData(TGEBEvent& event);
  void HandleGEBMode3(TGEBEvent& event, kDetectorSystems system);
  void HandleS800Scaler(TGEBEvent& event);

  TUnpackedEvent* fOutputEvent;

  unsigned int fRunStart;

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<std::vector<TRawEvent> > > input_queue;
  std::shared_ptr<ThreadsafeQueue<TUnpackedEvent*> > output_queue;
#endif

  ClassDef(TUnpackingLoop, 0);
};

#endif /* _TUNPACKLOOP_H_ */
