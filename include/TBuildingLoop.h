#ifndef _TBUILDINGLOOP_H_
#define _TBUILDINGLOOP_H_

#ifndef __CINT__
#include <memory>
#endif

#include "StoppableThread.h"
#include "ThreadsafeQueue.h"
#include "TRawEvent.h"

class TBuildingLoop : public StoppableThread {
public:
  static TBuildingLoop *Get(std::string name="");
  virtual ~TBuildingLoop();

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TRawEvent> >& InputQueue() { return input_queue; }
  std::shared_ptr<ThreadsafeQueue<std::vector<TRawEvent> > >& OutputQueue() { return output_queue; }
#endif

  bool Iteration();

  virtual void ClearQueue();

  size_t GetItemsPushed()  { return output_queue->ItemsPushed(); }
  size_t GetItemsPopped()  { return output_queue->ItemsPopped(); }
  size_t GetItemsCurrent() { return output_queue->Size();        }
  size_t GetRate()         { return 0; }

  void SetBuildWindow(long clock_ticks) { build_window = clock_ticks; }

private:
  TBuildingLoop(std::string name);
  TBuildingLoop(const TBuildingLoop& other);
  TBuildingLoop& operator=(const TBuildingLoop& other);

  bool CheckBuildWindow(TRawEvent*);

#ifndef __CINT__
  std::shared_ptr<ThreadsafeQueue<TRawEvent> > input_queue;
  std::shared_ptr<ThreadsafeQueue<std::vector<TRawEvent> > > output_queue;
#endif


  std::vector<TRawEvent> next_event;
  long event_start;
  long build_window;

  ClassDef(TBuildingLoop, 0);
};

#endif /* _TBUILDINGLOOP_H_ */
