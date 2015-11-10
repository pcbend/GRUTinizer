#ifndef _TUNPACKLOOP_H_
#define _TUNPACKLOOP_H_

#include "TNamed.h"

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TRawEvent.h"
#include "TBuildingedEvent.h"

class TNSCLEvent;
class TGEBEvent;

class TBuildingLoop : public StoppableThread {
public:
  TBuildingLoop(std::string name, ThreadsafeQueue<TRawEvent>& input_queue);
              //ThreadsafeQueue<TBuildingedEvent*>& output_queue);
  virtual ~TBuildingLoop();

  ThreadsafeQueue<TBuiltEvent*>& GetOutputQueue() { return output_queue; }

//protected:
  bool Iteration();

private:
  TBuildingLoop(const TBuildingLoop& other);
  TBuildingLoop& operator=(const TBuildingLoop& other);

  void CheckBuildWindow(long timestamp);

  void HandleNSCLData(TNSCLEvent& event);
  void HandleBuiltNSCLData(TNSCLEvent& event);
  void HandleUnbuiltNSCLData(TNSCLEvent& event);

  void HandleGEBData(TGEBEvent& event);
  void HandleGEBMode3(TGEBEvent& event, kDetectorSystems system);
  void HandleS800Scaler(TGEBEvent& event);

  ThreadsafeQueue<TRawEvent>& input_queue;
  ThreadsafeQueue<TBuiltEvent*>& output_queue;

  TBuiltEvent* next_event;
  long event_start;
  long build_window;
};

#endif /* _TUNPACKLOOP_H_ */
