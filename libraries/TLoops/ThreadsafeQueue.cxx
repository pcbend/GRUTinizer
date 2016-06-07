#include "ThreadsafeQueue.h"

#include <memory>

#include "TUnpackedEvent.h"

#include "RCNPEvent.h"
template<>
int ThreadsafeQueue<TRawEvent>::ObjectSize(TRawEvent& event){
  return event.GetTotalSize();
}

template<>
int ThreadsafeQueue<std::vector<TRawEvent> >::ObjectSize(std::vector<TRawEvent>& event) {
  return event.size();
}

template<>
int ThreadsafeQueue<std::vector<TDetector*> >::ObjectSize(std::vector<TDetector*>& det) {
  return det.size();
}

template<>
int ThreadsafeQueue<TUnpackedEvent*>::ObjectSize(TUnpackedEvent*& event) {
  return event->Size();
}

template<>
int ThreadsafeQueue<RCNPEvent>::ObjectSize(RCNPEvent& event) {
  return event.data.size();
}
