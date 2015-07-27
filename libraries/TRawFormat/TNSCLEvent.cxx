#include "TNSCLEvent.h"

#include <cassert>

Int_t TNSCLEvent::GetBodyHeaderSize() const {
  Int_t output = *((Int_t*)(GetBody()+0));
  if(output == 0) {
    return 4;  // If only the body header size is present, it is listed as 0.
  } else {
    return output;
  }
}

Long_t TNSCLEvent::GetTimestamp() const {
  if(GetBodyHeaderSize() > 4) {
    return *((Long_t*)(GetBody()+4));
  } else {
    return -1;
  }
}

Int_t TNSCLEvent::GetSourceID() const {
  if(GetBodyHeaderSize() > 4) {
    return *((Int_t*)(GetBody()+12));
  } else {
    return -1;
  }
}

Int_t TNSCLEvent::GetBarrierType() const {
  if(GetBodyHeaderSize() > 4) {
    return *((Int_t*)(GetBody()+16));
  } else {
    return -1;
  }
}

int TNSCLEvent::IsBuiltData(char* payload) const {
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

const char* TNSCLEvent::GetPayload() const {
  return (GetBody() + GetBodyHeaderSize());
}

TSmartBuffer TNSCLEvent::GetPayloadBuffer() const {
  return fBody.BufferSubset(GetBodyHeaderSize());
}

Int_t TNSCLEvent::GetPayloadSize() const {
  return (GetBodySize()-GetBodyHeaderSize());
}

// Seconds since the previous scaler read
Int_t TNSCLScaler::GetIntervalStartOffset(){
  return *(Int_t*)(GetPayload() + 0);
}

// Seconds since beginning of run
Int_t TNSCLScaler::GetIntervalEndOffset(){
  return *(Int_t*)(GetPayload() + 4);
}

// Time when writing to disk
time_t TNSCLScaler::GetUnixTime(){
  return *(Int_t*)(GetPayload() + 8);
}

// Interval (seconds) between each scaler packet
Int_t TNSCLScaler::GetIntervalDivisor(){
  return *(Int_t*)(GetPayload() + 12);
}

// Number of integers to follow.
Int_t TNSCLScaler::GetScalerCount(){
  return *(Int_t*)(GetPayload() + 16);
}

// Are the scalers reset after each read
Int_t TNSCLScaler::IsIncremental(){
  return *(Int_t*)(GetPayload() + 20);
}

Int_t TNSCLScaler::GetScalerValue(size_t scaler_num){
  assert(scaler_num < GetScalerCount());

  return *(Int_t*)(GetPayload() + 24 + 4*scaler_num);
}


TNSCLFragment::TNSCLFragment(TSmartBuffer buf)
  : fBuf(buf) { }

Long_t TNSCLFragment::GetFragmentTimestamp() const {
  return *(Long_t*)(fBuf.GetData()+0);
}

Int_t TNSCLFragment::GetFragmentSourceID() const {
  return *(Int_t*)(fBuf.GetData()+8);
}

Int_t TNSCLFragment::GetTotalFragmentSize() const {
  return (20 + //Fragment header
          8 +
          GetFragBodyHeaderSize() +
          GetFragmentPayloadSize());
}

Int_t TNSCLFragment::GetFragmentPayloadSize() const {
  return *(Int_t*)(fBuf.GetData()+12);
}

Int_t TNSCLFragment::GetFragmentBarrier() const {
  return *(Int_t*)(fBuf.GetData()+16);
}

Int_t TNSCLFragment::GetRingItemSize() const {
  return *(Int_t*)(fBuf.GetData()+20);
}

Int_t TNSCLFragment::GetRingItemType() const {
  return *(Int_t*)(fBuf.GetData()+24);
}

Int_t TNSCLFragment::GetFragBodyHeaderSize() const {
  Int_t output = *((Int_t*)(fBuf.GetData()+28));
  if(output == 0) {
    return 4;  // If only the body header size is present, it is listed as 0.
  } else {
    return output;
  }
}

Long_t TNSCLFragment::GetFragBodyTimestamp() const {
  if(GetFragBodyHeaderSize() > 4) {
    return *((Long_t*)(fBuf.GetData()+32));
  } else {
    return -1;
  }
}

Int_t TNSCLFragment::GetFragBodySourceID() const {
  if(GetFragBodyHeaderSize() > 4) {
    return *((Int_t*)(fBuf.GetData()+40));
  } else {
    return -1;
  }
}

Int_t TNSCLFragment::GetFragBodyBarrierType() const {
  if(GetFragBodyHeaderSize() > 4) {
    return *((Int_t*)(fBuf.GetData()+44));
  } else {
    return -1;
  }
}

const char* TNSCLFragment::GetFragmentPayload() const {
  return fBuf.GetData() + 28 + GetFragBodyHeaderSize();
}

TSmartBuffer TNSCLFragment::GetFragmentPayloadBuffer() const {
  return fBuf.BufferSubset(28 + GetFragBodyHeaderSize());
}


TNSCLBuiltRingItem::TNSCLBuiltRingItem(TSmartBuffer buf)
  : fBuf(buf) { }

TNSCLBuiltRingItem::TNSCLBuiltRingItem(TNSCLEvent& event)
  : fBuf(event.GetPayloadBuffer()) {
  assert(kNSCLEventType(event.GetEventType()) == kNSCLEventType::PHYSICS_EVENT);
  assert(fBuf.GetSize() == GetBuiltRingItemSize());
}

TNSCLFragment TNSCLBuiltRingItem::GetFragment(size_t fragnum) const {
  BuildFragments();
  return fragments.at(fragnum);
}

Int_t TNSCLBuiltRingItem::Size() const {
  BuildFragments();
  return fragments.size();
}

size_t TNSCLBuiltRingItem::NumFragments() const {
  BuildFragments();
  return fragments.size();
}

Int_t TNSCLBuiltRingItem::GetBuiltRingItemSize() const {
  return *(Int_t*)(fBuf.GetData() + 0);
}

void TNSCLBuiltRingItem::BuildFragments() const {
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
