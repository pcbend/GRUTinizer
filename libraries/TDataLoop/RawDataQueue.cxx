#include "RawDataQueue.h"

#include <iostream>

#include "Globals.h"

int RawDataQueue::num_opened = 0;
int RawDataQueue::num_closed = 0;

RawDataQueue::RawDataQueue()
  : queue_number(num_opened), max_queue_size(10000),
    items_pushed(0), items_popped(0) {
  num_opened++;
}

RawDataQueue::~RawDataQueue(){
  num_closed++;
}

void RawDataQueue::Push(TRawEvent* obj){
  std::unique_lock<std::mutex> lock(mutex);
  while(queue.size() > max_queue_size){
    can_push.wait(lock);
  }
  items_pushed++;
  bytes_pushed += obj->GetTotalSize();
  bytes_in_queue += obj->GetTotalSize();

  queue.push(obj);
  can_pop.notify_one();
}

TRawEvent* RawDataQueue::Pop(){
  std::unique_lock<std::mutex> lock(mutex);
  while(!queue.size()){
    can_pop.wait(lock);
  }
  TRawEvent* output = queue.front();
  queue.pop();

  items_popped++;
  bytes_popped += output->GetTotalSize();
  bytes_in_queue -= output->GetTotalSize();

  can_push.notify_one();
  return output;
}

void RawDataQueue::Print(){
  std::cout << "I am queue " << queue_number << " out of " << num_opened << ".\n"
            << "\t" << "Items: " << DCYAN << items_popped << RESET_COLOR << "/" << DMAGENTA << items_pushed << RESET_COLOR << "\n"
            << "\t" << "Bytes: " << DYELLOW << bytes_popped << RESET_COLOR << "/" << BLUE << bytes_pushed << RESET_COLOR << "\n"
            << std::flush;
}
