
#include <TOrderedRawFile.h>

#include <cassert>
#include <iostream>
#include <utility>

ClassImp(TOrderedRawFile);

TOrderedRawFile::TOrderedRawFile(const char* filename, kFileType file_type)
  : TRawFileIn(filename, file_type),
    depth(5000), oldest_timestamp(-1), newest_timestamp(-1),
    finished(false) {

  // Currently, only supports GEB file types, not NSCL file types.
  assert(file_type == GRETINA_MODE2 ||
	 file_type == GRETINA_MODE3);
}

TOrderedRawFile::~TOrderedRawFile() {  }

int TOrderedRawFile::Read(TRawEvent *event) {
  if(!event)
    return -1;

  if(IsFinished()){
    return 0;
  }

  if(fillqueue()==0) 
   return -1;

  auto curr_timestamp = event_queue.begin()->first;
  *event = event_queue.begin()->second;
  event_queue.erase(event_queue.begin());
 
  if(oldest_timestamp != -1 && newest_timestamp != -1 &&
     curr_timestamp < newest_timestamp){
    std::cerr << "Sorting failed, insufficient depth" << std::endl;
  }
  
  if(oldest_timestamp == -1 || curr_timestamp < oldest_timestamp){
    oldest_timestamp = curr_timestamp;
  }
  if(newest_timestamp == -1 || curr_timestamp > newest_timestamp){
    newest_timestamp = curr_timestamp;
  }
 return event->GetTotalSize();
  
}

bool TOrderedRawFile::IsFinished() const {
  return finished;
}


int TOrderedRawFile::fillqueue() {
  while(event_queue.size()<depth && !TRawFileIn::IsFinished()) {
    TRawEvent new_event;
    TRawFileIn::Read(&new_event);
    auto timestamp = TGEBEvent(new_event).GetTimestamp();
    event_queue.insert( std::make_pair(timestamp, new_event) );
  }
  
  if(!event_queue.size()){
    finished = true;
  }
  return event_queue.size();
}

