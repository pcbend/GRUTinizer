#ifndef _TBUILDINGLOOP_H_
#define _TBUILDINGLOOP_H_

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


class TDataLoop;

class TBuildingLoop : public StoppableThread {
  public:
    static TBuildingLoop *Get(std::string name="",TDataLoop *input=0);
    virtual ~TBuildingLoop();

    //ThreadsafeQueue<TBuiltEvent*>& GetOutputQueue() { return output_queue; }

    //protected:
    bool Iteration();

    virtual void ClearQueue();

    size_t GetItemsPushed()  { return output_queue.ItemsPushed(); }
    size_t GetItemsPopped()  { return output_queue.ItemsPopped(); }
    size_t GetItemsCurrent() { return output_queue.Size();        }
    size_t GetRate()         { return 0; }

    void SetBuildWindow(long clock_ticks) { build_window = clock_ticks; }


    int Pop(std::vector<TRawEvent> &event);


  private:
    TBuildingLoop(std::string name, TDataLoop*); //StoppableThread*);  //ThreadsafeQueue<TRawEvent>& input_queue);
    TBuildingLoop(const TBuildingLoop& other);
    TBuildingLoop& operator=(const TBuildingLoop& other);

    bool CheckBuildWindow(TRawEvent*);

    //void HandleNSCLData(TNSCLEvent& event);
    //void HandleBuiltNSCLData(TNSCLEvent& event);
    //void HandleUnbuiltNSCLData(TNSCLEvent& event);

    //void HandleGEBData(TGEBEvent& event);
    //void HandleGEBMode3(TGEBEvent& event, kDetectorSystems system);
    //void HandleS800Scaler(TGEBEvent& event);

    TDataLoop *input_source;

    //ThreadsafeQueue<TRawEvent>               & input_queue;
    ThreadsafeQueue<std::vector<TRawEvent> > output_queue;


    std::vector<TRawEvent> next_event;
    //TBuiltEvent* next_event;        //std::vector<TRawEvents>
    long event_start;
    long build_window;
};





/*



class RawDataQueue {
public:
  RawDataQueue();
  virtual ~RawDataQueue();

  TRawEvent Pop();
  void Push(TRawEvent obj);
  size_t Size();

  void Print();
  void Status();

private:
  std::mutex mutex;
  std::queue<TRawEvent> queue;
  std::condition_variable can_push;
  std::condition_variable can_pop;

  int queue_number;
  size_t max_queue_size;
  size_t items_pushed;
  size_t items_popped;

  size_t bytes_in_queue;
  size_t bytes_pushed;
  size_t bytes_popped;

  static int num_opened;
  static int num_closed;

  TStopwatch clock;
};

*/












#endif /* _TBUILDINGLOOP_H_ */
