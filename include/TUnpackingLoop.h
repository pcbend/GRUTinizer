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

class TUnpackingLoop : public StoppableThread {
  public:
    static TUnpackingLoop *Get(std::string name="",TBuildingLoop *input=0);
    virtual ~TUnpackingLoop();

    //ThreadsafeQueue<TBuiltEvent*>& GetOutputQueue() { return output_queue; }

    //protected:
    bool Iteration();

    size_t GetItemsIn()  { return output_queue.ItemsPushed(); }
    size_t GetItemsOut() { return output_queue.ItemsPopped(); }
    size_t GetRate()     { return 0; }


  private:
    TUnpackingLoop(std::string name, TBuildingLoop*); 
    TUnpackingLoop(const TUnpackingLoop& other);
    TUnpackingLoop& operator=(const TUnpackingLoop& other);

    void HandleNSCLData(TNSCLEvent& event);
    void HandleBuiltNSCLData(TNSCLEvent& event);
    void HandleUnbuiltNSCLData(TNSCLEvent& event);

    void HandleGEBData(TGEBEvent& event);
    void HandleGEBMode3(TGEBEvent& event, kDetectorSystems system);
    void HandleS800Scaler(TGEBEvent& event);

    TBuildingLoop *input_source;

    //ThreadsafeQueue<TRawEvent>               & input_queue;
    ThreadsafeQueue<std::vector<TDetectors*> > output_queue;
};

#endif /* _TUNPACKLOOP_H_ */
