#ifndef _TDATAQUEUE_H_
#define _TDATAQUEUE_H_

#include <condition_variable>
#include <mutex>
#include <queue>

#include "TRawEvent.h"

class RawDataQueue {
public:
  RawDataQueue();
  virtual ~RawDataQueue();

  TRawEvent* Pop();
  void Push(TRawEvent* obj);

  void Print();

private:
  std::mutex mutex;
  std::queue<TRawEvent*> queue;
  std::condition_variable can_push;
  std::condition_variable can_pop;

  int queue_number;
  size_t max_queue_size;
  int items_pushed;
  int items_popped;

  size_t bytes_in_queue;
  size_t bytes_pushed;
  size_t bytes_popped;

  static int num_opened;
  static int num_closed;
};

#endif /* _TDATAQUEUE_H_ */
