#include "TNSCLScalers.h"

#include <ctime>
#include <iostream>

#include "TNSCLEvent.h"

TNSCLScalers::TNSCLScalers() {
  Clear();
}

TNSCLScalers::TNSCLScalers(const TNSCLScalers& scale )
  : TDetector(scale) {
  scale.Copy(*this);
}

TNSCLScalers::~TNSCLScalers() { }

void TNSCLScalers::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TNSCLScalers& other = (TNSCLScalers&)obj;
  other.scalers = scalers;
  other.interval_start = interval_start;
  other.interval_end = interval_end;
  other.interval_div = interval_div;
  other.unix_time = unix_time;
  other.num_scalers = num_scalers;
  other.source_id = source_id;
}

void TNSCLScalers::Print(Option_t *opt) const {
  time_t time = unix_time;

  std::cout << "Scalers from " << interval_start << " to " << interval_end << std::endl;
  std::cout << "Scalers period: " << interval_div << std::endl;
  std::cout << "Time: " << std::ctime(&time) << std::endl;
  std::cout << "Num scalers: " << num_scalers << std::endl;
  for(unsigned int i=0; i<scalers.size(); i++) {
    std::cout << "\t" << i << ": " << scalers[i] << std::endl;
  }
}

void TNSCLScalers::Clear(Option_t *opt) {
  TDetector::Clear(opt);

  interval_start = -1;
  interval_end   = -1;
  interval_div   = -1;
  unix_time      = -1;
  num_scalers    = -1;
  source_id = -1;

  scalers.clear();
}

int TNSCLScalers::BuildHits(std::vector<TRawEvent>& raw_data) {
  for(auto& event : raw_data){
    SetTimestamp(event.GetTimestamp());
    source_id = ((TNSCLEvent&)event).GetSourceID();

    TRawNSCLScalers& raw = (TRawNSCLScalers&)event;

    interval_start = raw.GetIntervalStartOffset();
    interval_end = raw.GetIntervalEndOffset();
    unix_time = raw.GetUnixTime();
    interval_div = raw.GetIntervalDivisor();

    num_scalers = raw.GetScalerCount();
    scalers.reserve(num_scalers);
    for(int i=0; i<num_scalers; i++) {
      scalers.push_back(raw.GetScalerValue(i));
    }
  }
  return Size();
}

void TNSCLScalers::InsertHit(const TDetectorHit&) {
  std::cout << __PRETTY_FUNCTION__ << " should never be called" << std::endl;
}

TDetectorHit& TNSCLScalers::GetHit(int i) {
  std::cout << __PRETTY_FUNCTION__ << " should never be called" << std::endl;
  return *(TDetectorHit*)0;
}
