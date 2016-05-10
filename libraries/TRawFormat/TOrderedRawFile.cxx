#include "TOrderedRawFile.h"

#include <cassert>
#include <iostream>
#include <utility>

ClassImp(TOrderedRawFile)

TOrderedRawFile::TOrderedRawFile(TRawEventSource* unordered)
  : unordered(unordered),
    depth(100000), oldest_timestamp(-1), newest_timestamp(-1),
    finished(false) { }

TOrderedRawFile::~TOrderedRawFile() {
  delete unordered;
}

int TOrderedRawFile::GetEvent(TRawEvent& event) {
  if(fillqueue()==0) {
    return -1;
  }

  auto curr_timestamp = event_queue.begin()->first;
  event = event_queue.begin()->second;
  event_queue.erase(event_queue.begin());

  // If timestamp == +-1 (usually scalers), pass it through automatically.
  if(curr_timestamp == -1 || curr_timestamp == 1){
    return event.GetTotalSize();
  }

  if(oldest_timestamp != -1 && newest_timestamp != -1 &&
     curr_timestamp < newest_timestamp){
    std::cerr << "Sorting failed, insufficient depth "
              << "(" << curr_timestamp << " < " << newest_timestamp << std::endl;
  }

  if(oldest_timestamp == -1 || curr_timestamp < oldest_timestamp){
    oldest_timestamp = curr_timestamp;
  }
  if(newest_timestamp == -1 || curr_timestamp > newest_timestamp){
    newest_timestamp = curr_timestamp;
  }
  return event.GetTotalSize();
}

int TOrderedRawFile::fillqueue() {
  while(event_queue.size()<depth && !unordered->IsFinished()) {
    TRawEvent new_event;
    int bytes = unordered->Read(new_event);
    if(bytes<1)
      break;
    auto timestamp = new_event.GetTimestamp();
    event_queue.insert( std::make_pair(timestamp, new_event) );
  }

  if(!event_queue.size()){
    finished = true;
  }
  return event_queue.size();
}
