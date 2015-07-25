#ifndef _TNSCLEVENT_H_
#define _TNSCLEVENT_H_

#include "TRawEvent.h"
#include "TSmartBuffer.h"

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
  Int_t GetBodyHeaderSize() const {
    Int_t output = *((Int_t*)(GetBody()+0));
    if(output == 0) {
      return 4;  // If only the body header size is present, it is listed as 0.
    } else {
      return output;
    }
  }

  Long_t GetTimestamp() const {
    if(GetBodyHeaderSize() > 4) {
      return *((Long_t*)(GetBody()+4));
    } else {
      return -1;
    }
  }

  Int_t GetSourceID() const {
    if(GetBodyHeaderSize() > 4) {
      return *((Int_t*)(GetBody()+12));
    } else {
      return -1;
    }
  }

  Int_t GetBarrierType() const {
    if(GetBodyHeaderSize() > 4) {
      return *((Int_t*)(GetBody()+16));
    } else {
      return -1;
    }
  }

  int IsBuiltData(char* payload) const {
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

  const char* GetPayload() const {
    return (GetBody() + GetBodyHeaderSize());
  }

  TSmartBuffer GetPayloadBuffer() const {
    return fBody.BufferSubset(GetBodyHeaderSize());
  }

  Int_t GetPayloadSize() const {
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
  TNSCLFragment(TSmartBuffer buf)
    : fBuf(buf) { }

  Long_t GetFragmentTimestamp() const {
    return *(Long_t*)(fBuf.GetData()+0);
  }

  Int_t GetFragmentSourceID() const {
    return *(Int_t*)(fBuf.GetData()+8);
  }

  Int_t GetTotalFragmentSize() const {
    return (20 + //Fragment header
            8 +
            GetFragBodyHeaderSize() +
            GetFragmentPayloadSize());
  }

  Int_t GetFragmentPayloadSize() const {
    return *(Int_t*)(fBuf.GetData()+12);
  }

  Int_t GetFragmentBarrier() const {
    return *(Int_t*)(fBuf.GetData()+16);
  }

  Int_t GetRingItemSize() const {
    return *(Int_t*)(fBuf.GetData()+20);
  }

  Int_t GetRingItemType() const {
    return *(Int_t*)(fBuf.GetData()+24);
  }

  Int_t GetFragBodyHeaderSize() const {
    Int_t output = *((Int_t*)(fBuf.GetData()+28));
    if(output == 0) {
      return 4;  // If only the body header size is present, it is listed as 0.
    } else {
      return output;
    }
  }

  Long_t GetFragBodyTimestamp() const {
    if(GetFragBodyHeaderSize() > 4) {
      return *((Long_t*)(fBuf.GetData()+32));
    } else {
      return -1;
    }
  }

  Int_t GetFragBodySourceID() const {
    if(GetFragBodyHeaderSize() > 4) {
      return *((Int_t*)(fBuf.GetData()+40));
    } else {
      return -1;
    }
  }

  Int_t GetFragBodyBarrierType() const {
    if(GetFragBodyHeaderSize() > 4) {
      return *((Int_t*)(fBuf.GetData()+44));
    } else {
      return -1;
    }
  }

  const char* GetFragmentPayload() const {
    return fBuf.GetData() + 28 + GetFragBodyHeaderSize();
  }

  TSmartBuffer GetFragmentPayloadBuffer() const {
    return fBuf.BufferSubset(28 + GetFragBodyHeaderSize());
  }

private:
  TSmartBuffer fBuf;

  ClassDef(TNSCLFragment,0);
};

class TNSCLBuiltRingItem : public TObject {
public:
  TNSCLBuiltRingItem(TSmartBuffer buf)
    : fBuf(buf) { }

  TNSCLBuiltRingItem(TNSCLEvent& event)
    : fBuf(event.GetPayloadBuffer()) {
    assert(kNSCLEventType(event.GetEventType()) == kNSCLEventType::PHYSICS_EVENT);
    assert(fBuf.GetSize() == GetBuiltRingItemSize());
  }

  TNSCLFragment GetFragment(size_t fragnum) const {
    BuildFragments();
    return fragments.at(fragnum);
  }

  Int_t Size() const {
    BuildFragments();
    return fragments.size();
  }

  size_t NumFragments() const {
    BuildFragments();
    return fragments.size();
  }

  Int_t GetBuiltRingItemSize() const {
    return *(Int_t*)(fBuf.GetData() + 0);
  }

private:
  void BuildFragments() const {
    if(fragments.size()){
      return;
    }

    TSmartBuffer buf = fBuf.BufferSubset(sizeof(Int_t));

    while(buf.GetSize()){
      TNSCLFragment temp(buf);
      std::cout << "Buffer size: " << buf.GetSize()
                << "\tFragment size: " << temp.GetTotalFragmentSize()
                << std::endl;
      Int_t fragment_size = temp.GetTotalFragmentSize();
      fragments.emplace_back(buf.BufferSubset(0,fragment_size));
      buf = buf.BufferSubset(fragment_size);
      break;
    }
  }

  TSmartBuffer fBuf;
  mutable std::vector<TNSCLFragment> fragments;

  ClassDef(TNSCLBuiltRingItem,0);
};


#endif /* _TNSCLEVENT_H_ */
