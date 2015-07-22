#ifndef _TNSCLEVENT_H_
#define _TNSCLEVENT_H_

#include "TRawEvent.h"

enum kNSCLEventType {
  BEGIN_RUN            = 1,    // 0x0001
  END_RUN              = 2,    // 0x0002
  PAUSE_RUN            = 3,    // 0x0003
  RESUME_RUN           = 4,    // 0x0004
  // ABNORMAL_ENDRUN      = 4,    // 0x0004
  PACKET_TYPES         = 10,   // 0x000A
  MONITORED_VARIABLES  = 11,   // 0x000B
  RING_FORMAT          = 12,   // 0x000C
  PERIODIC_SCALERS     = 20,   // 0x0014
  PHYSICS_EVENT        = 30,   // 0x001E
  PHYSICS_EVENT_COUNT  = 31,   // 0x001F
  EVB_FRAGMENT         = 40,   // 0x0028
  EVB_UNKNOWN_PAYLOAD  = 41,   // 0x0029
  EVB_GLOM_INFO        = 42,   // 0x002A
  FIRST_USER_ITEM_CODE = 32768 // 0x8000
};

class TNSCLEvent : public TRawEvent {
public:
  Int_t GetBodyHeaderSize() {
    Int_t output = *((Int_t*)(GetBody()+0));
    if(output == 0) {
      return 4;  // If only the body header size is present, it is listed as 0.
    } else {
      return output;
    }
  }

  Long_t GetTimestamp() {
    if(GetBodyHeaderSize() > 4) {
      return *((Long_t*)(GetBody()+4));
    } else {
      return -1;
    }
  }

  Int_t GetSourceID() {
    if(GetBodyHeaderSize() > 4) {
      return *((Int_t*)(GetBody()+12));
    } else {
      return -1;
    }
  }

  Int_t GetBarrierType() {
    if(GetBodyHeaderSize() > 4) {
      return *((Int_t*)(GetBody()+16));
    } else {
      return -1;
    }
  }

  int IsBuiltData(char* payload) {
    static int is_built_data = -1;
    if(is_built_data != -1) {
      return is_built_data;
    }

    Int_t sourceid = *((Int_t*)(GetPayload() + sizeof(Long_t)));
    Int_t barrier  = *((Int_t*)(GetPayload() + sizeof(Long_t) + 2*sizeof(Int_t)));

    is_built_data = ((sourceid < 10) &&
                     (barrier == 0));

    return is_built_data;
  }

  char* GetPayload() {
    return (GetBody() + GetBodyHeaderSize());
  }

  Int_t GetPayloadSize() {
    return (GetBodySize()-GetBodyHeaderSize());
  }

  ClassDef(TNSCLEvent,0);
};

class TNSCLScaler : public TNSCLEvent {
public:
  // Seconds since the previous scaler read
  Int_t GetIntervalStartOffset(){
    return *(Int_t*)(GetPayload() + 0);
  }

  // Seconds since beginning of run
  Int_t GetIntervalEndOffset(){
    return *(Int_t*)(GetPayload() + 4);
  }

  // Time when writing to disk
  time_t GetUnixTime(){
    return *(Int_t*)(GetPayload() + 8);
  }

  // Interval (seconds) between each scaler packet
  Int_t GetIntervalDivisor(){
    return *(Int_t*)(GetPayload() + 12);
  }

  // Number of integers to follow.
  Int_t GetScalerCount(){
    return *(Int_t*)(GetPayload() + 16);
  }

  // Are the scalers reset after each read
  Int_t IsIncremental(){
    return *(Int_t*)(GetPayload() + 20);
  }

  Int_t GetScalerValue(size_t scaler_num){
    assert(scaler_num < GetScalerCount());

    return *(Int_t*)(GetPayload() + 24 + 4*scaler_num);
  }

  ClassDef(TNSCLScaler,0);
};


class TNSCLFragment : public TObject {

public:
  TNSCLFragment(char *data)
    : fData(data) { }

  Long_t GetFragmentTimestamp(){
    return *(Long_t*)(fData+0);
  }

  Int_t GetFragmentSourceID(){
    return *(Int_t*)(fData+8);
  }

  Int_t GetTotalFragmentSize(){
    return (20 + //Fragment header
            8 + //Ring item header
            GetFragBodyHeaderSize() +
            GetFragmentPayloadSize());
  }

  Int_t GetFragmentPayloadSize(){
    return *(Int_t*)(fData+12);
  }

  Int_t GetFragmentBarrier(){
    return *(Int_t*)(fData+16);
  }

  Int_t GetRingItemSize(){
    return *(Int_t*)(fData+20);
  }

  Int_t GetRingItemType(){
    return *(Int_t*)(fData+24);
  }

  Int_t GetFragBodyHeaderSize() {
    Int_t output = *((Int_t*)(fData+28));
    if(output == 0) {
      return 4;  // If only the body header size is present, it is listed as 0.
    } else {
      return output;
    }
  }

  Long_t GetFragBodyTimestamp() {
    if(GetFragBodyHeaderSize() > 4) {
      return *((Long_t*)(fData+32));
    } else {
      return -1;
    }
  }

  Int_t GetFragBodySourceID() {
    if(GetFragBodyHeaderSize() > 4) {
      return *((Int_t*)(fData+40));
    } else {
      return -1;
    }
  }

  Int_t GetFragBodyBarrierType() {
    if(GetFragBodyHeaderSize() > 4) {
      return *((Int_t*)(fData+44));
    } else {
      return -1;
    }
  }

  char* GetFragmentPayload(){
    return fData + 28 + GetFragBodyHeaderSize();
  }

private:
  char* fData;

  ClassDef(TNSCLFragment,0);
};

class TNSCLBuiltRingItem : public TObject {
public:
  TNSCLBuiltRingItem(char* data)
    : fData(data) { }

  TNSCLBuiltRingItem(TNSCLEvent& event)
    : fData(event.GetPayload()) {
    assert(kNSCLEventType(event.GetEventType()) == kNSCLEventType::PHYSICS_EVENT);
  }

  TNSCLFragment GetFragment(size_t fragnum){
    BuildFragments();
    return fragments.at(fragnum);
  }

  Int_t Size(){
    BuildFragments();
    return fragments.size();
  }

  size_t NumFragments(){
    BuildFragments();
    return fragments.size();
  }

  Int_t GetBuiltRingItemSize(){
    return *(Int_t*)(fData + 0);
  }

private:
  void BuildFragments(){
    if(fragments.size()){
      return;
    }

    char* curr = fData + sizeof(Int_t);
    char* end  = fData + GetBuiltRingItemSize();

    while(curr < end){
      fragments.emplace_back(curr);
      curr += fragments.back().GetTotalFragmentSize();
    }
  }

  char* fData;
  std::vector<TNSCLFragment> fragments;

  ClassDef(TNSCLBuiltRingItem,0);
};


#endif /* _TNSCLEVENT_H_ */
