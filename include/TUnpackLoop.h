#ifndef _TUNPACKLOOP_H_
#define _TUNPACKLOOP_H_

#include "TNamed.h"

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TRawEvent.h"
#include "TUnpackedEvent.h"

class TNSCLEvent;
class TGEBEvent;

class TUnpackLoop : public StoppableThread {
public:
  TUnpackLoop(ThreadsafeQueue<TRawEvent>& input_queue,
              ThreadsafeQueue<TUnpackedEvent*>& output_queue);
  virtual ~TUnpackLoop();

  ThreadsafeQueue<TUnpackedEvent*>& GetOutputQueue() { return output_queue; }

protected:
  bool Iteration();

private:
  TUnpackLoop(const TUnpackLoop& other);
  TUnpackLoop& operator=(const TUnpackLoop& other);

  void CheckBuildWindow(long timestamp);

  void HandleNSCLData(TNSCLEvent& event);
  void HandleBuiltNSCLData(TNSCLEvent& event);
  void HandleUnbuiltNSCLData(TNSCLEvent& event);

  void HandleGEBData(TGEBEvent& event);
  void HandleGEBMode3(TGEBEvent& event, kDetectorSystems system);
  void HandleS800Scaler(TGEBEvent& event);

  ThreadsafeQueue<TRawEvent>& input_queue;
  ThreadsafeQueue<TUnpackedEvent*>& output_queue;

  TUnpackedEvent* next_event;
  long event_start;
  long build_window;
};

#endif /* _TUNPACKLOOP_H_ */
