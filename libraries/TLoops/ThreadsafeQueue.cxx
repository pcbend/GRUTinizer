#include "ThreadsafeQueue.h"

#include <memory>

#include "TUnpackedEvent.h"

template<>
int ThreadsafeQueue<TRawEvent>::ObjectSize(TRawEvent& event) const {
  return event.GetTotalSize();
}

template<>
int ThreadsafeQueue<std::vector<TRawEvent> >::ObjectSize(std::vector<TRawEvent>& event) const {
  return event.size();
}

template<>
int ThreadsafeQueue<std::vector<TDetector*> >::ObjectSize(std::vector<TDetector*>& det) const {
  return det.size();
}

template<>
int ThreadsafeQueue<TUnpackedEvent*>::ObjectSize(TUnpackedEvent*& event) const {
  return event->Size();
}
