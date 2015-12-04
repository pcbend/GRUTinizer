#ifndef _THREADSAFEQUEUE_H_
#define _THREADSAFEQUEUE_H_

#include <cassert>
#include <iostream>

#ifndef __CINT__
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#endif

#include "TRawEvent.h"

class TDetector;

template<typename T>
class ThreadsafeQueue {
public:
  ThreadsafeQueue();
  ~ThreadsafeQueue();
  void Push(T obj);
  int Pop(T& output, int millisecond_wait = 1000);

  size_t ItemsPushed();
  size_t ItemsPopped();
  size_t Size();

  int ObjectSize(T&);

private:
#ifndef __CINT__
  std::mutex mutex;
  std::queue<T> queue;
  std::condition_variable can_push;
  std::condition_variable can_pop;

  size_t max_queue_size;

  size_t items_in_queue;
  size_t items_pushed;
  size_t items_popped;
#endif
};

#ifndef __CINT__
template<typename T>
ThreadsafeQueue<T>::ThreadsafeQueue()
  : max_queue_size(10000),
    items_in_queue(0), items_pushed(0), items_popped(0) { }

template<typename T>
ThreadsafeQueue<T>::~ThreadsafeQueue() { }

template<typename T>
void ThreadsafeQueue<T>::Push(T obj) {
  std::unique_lock<std::mutex> lock(mutex);
  if(queue.size() > max_queue_size){
    can_push.wait(lock);
  }

  items_pushed++;
  items_in_queue++;

  queue.push(obj);
  can_pop.notify_one();
}

template<typename T>
int ThreadsafeQueue<T>::Pop(T& output, int millisecond_wait) {
  std::unique_lock<std::mutex> lock(mutex);
  if(!queue.size()){
    can_pop.wait_for(lock, std::chrono::milliseconds(millisecond_wait));
  }

  if(!queue.size()){
    return -1;
  }

  output = queue.front();
  queue.pop();

  items_popped++;
  items_in_queue--;

  can_push.notify_one();
  return ObjectSize(output);
}

template<typename T>
size_t ThreadsafeQueue<T>::Size() {
  std::unique_lock<std::mutex> lock(mutex);
  return items_in_queue;
}

template<typename T>
size_t ThreadsafeQueue<T>::ItemsPushed() {
  std::unique_lock<std::mutex> lock(mutex);
  return items_pushed;
}

template<typename T>
size_t ThreadsafeQueue<T>::ItemsPopped() {
  std::unique_lock<std::mutex> lock(mutex);
  return items_popped;
}
#endif /* __CINT__ */

#endif /* _THREADSAFEQUEUE_H_ */
