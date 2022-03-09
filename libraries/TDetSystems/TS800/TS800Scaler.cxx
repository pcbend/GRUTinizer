#include <TS800Scaler.h>

#include <ctime>
#include <iostream>

#include <TGEBEvent.h>

ClassImp(TS800Scaler)

TS800Scaler::TS800Scaler() { }

TS800Scaler::TS800Scaler(const TS800Scaler& scale ) : TDetector(scale) { scale.Copy(*this); }

//TS800Scaler::TS800Scaler(const TGEBEvent&) { }

TS800Scaler::~TS800Scaler() { }

void TS800Scaler::Copy(TObject& obj) const {
  TDetector::Copy(obj);
  ((TS800Scaler&)obj).scalers = scalers;
}

void TS800Scaler::Print(Option_t *opt) const {
  time_t time = unix_time;

  std::cout << "Scalers from " << interval_start << " to " << interval_end << std::endl;
  std::cout << "Scaler period: " << interval_div << std::endl;
  std::cout << "Time: " << std::ctime(&time) << std::endl;
  std::cout << "Num scalers: " << num_scalers << std::endl;
  for(unsigned int i=0; i<scalers.size(); i++) {
    std::cout << "\t" << i << ": " << scalers[i] << std::endl;
  }
}

void TS800Scaler::Clear(Option_t *opt) {
  TDetector::Clear(opt);

  interval_start = -1;
  interval_end   = -1;
  interval_div   = -1;
  unix_time      = -1;
  num_scalers    = -1;

  scalers.clear();
}

int TS800Scaler::BuildHits(std::vector<TRawEvent>& raw_data) {
  for(auto& event : raw_data){
    SetTimestamp(event.GetTimestamp());
    int *data = (int*)event.GetPayload();
    int ptr=0;
    /* int  nscl_size = *(data+ptr); */ ptr++;
    /* int  nscl_type = *(data+ptr); */ ptr++;
    /* long nscl_time = *((long*)(data+ptr)); */ ptr+=2;
    interval_start = *(data+ptr); ptr++;
    interval_end   = *(data+ptr); ptr++;
    interval_div   = *(data+ptr); ptr++;
    unix_time      = *(data+ptr); ptr++;
    num_scalers    = *(data+ptr); ptr++;
    for(int x=0;x<num_scalers;x++)
      scalers.push_back(*(data+ptr+x));
  }
  return Size();
}

void TS800Scaler::InsertHit(const TDetectorHit&) { }

TDetectorHit& TS800Scaler::GetHit(int i) {
  std::cout << __PRETTY_FUNCTION__ << " should never be called" << std::endl;
  return *(TDetectorHit*)0;
}
