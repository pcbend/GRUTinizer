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

template<typename T>
class ThreadsafeQueue {
public:
  ThreadsafeQueue();
  ~ThreadsafeQueue();
  void Push(T obj);
  int Pop(T& output, int millisecond_wait = 1000);
  size_t Size();

  void Close();
  bool IsClosed();

private:
#ifndef __CINT__
  std::atomic_bool is_finished;
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
  : is_finished(false), max_queue_size(10000),
    items_in_queue(0), items_pushed(0), items_popped(0) { }

template<typename T>
ThreadsafeQueue<T>::~ThreadsafeQueue() {
  is_finished = true;
}

template<typename T>
void ThreadsafeQueue<T>::Push(T obj) {
  std::unique_lock<std::mutex> lock(mutex);
  while(queue.size() > max_queue_size && !is_finished){
    can_push.wait(lock);
  }

  if(is_finished){
    std::cerr << "Attempted to push data to a closed queue" << std::endl;
    assert(false);
  }

  items_pushed++;

  queue.push(obj);
  can_pop.notify_one();
}

template<typename T>
int ThreadsafeQueue<T>::Pop(T& output, int millisecond_wait) {
  std::unique_lock<std::mutex> lock(mutex);
  while(!queue.size() && !is_finished){
    can_pop.wait_for(lock, std::chrono::milliseconds(millisecond_wait));
  }

  if(!queue.size()){
    return 0;
  }

  output = queue.front();
  queue.pop();

  items_popped++;

  can_push.notify_one();
  return 1;
}

template<typename T>
size_t ThreadsafeQueue<T>::Size() {
  std::unique_lock<std::mutex> lock(mutex);
  return items_in_queue;
}

template<typename T>
void ThreadsafeQueue<T>::Close() {
  is_finished = true;
}

template<typename T>
bool ThreadsafeQueue<T>::IsClosed() {
  return is_finished;
}
#endif /* __CINT__ */

#endif /* _THREADSAFEQUEUE_H_ */
