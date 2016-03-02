#include "TNSCLEvent.h"

#include <cassert>

TNSCLEvent::TNSCLEvent() { }

TNSCLEvent::TNSCLEvent(const TRawEvent &raw) {
  raw.Copy(*this);
}


TNSCLEvent::~TNSCLEvent() { }


Int_t TNSCLEvent::GetBodyHeaderSize() const {
  Int_t output = *((Int_t*)(GetBody()+0));
  if(output == 0) {
    return 4;  // If only the body header size is present, it is listed as 0.
  } else {
    return output;
  }
}

long TNSCLEvent::GetTimestamp() const {
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

int TNSCLEvent::IsBuiltData() const {
  static int is_built_data = -1;
  if(is_built_data != -1) {
    return is_built_data;
  }

  // If the event has only 4 bytes, then it might
  // The e13701 filter wasn't smart enough to remove these entirely,
  //   so they exist in the filtered data.
  // They shouldn't exist in normal data, though.
  // For now, if the only thing in the payload is the 0x00000004,
  //   call it a built ring item, but wait until the next event to decide for sure.
  if(GetPayloadSize() == 4){
    int inclusive_size = *(int*)GetPayload();
    if(inclusive_size == 4){
      return true;
    }
  }

  // If this is built data, there will be a 4-byte size, followed by a fragment header
  TRawEvent::TNSCLFragmentHeader* header = (TRawEvent::TNSCLFragmentHeader*)(GetPayload() + sizeof(Int_t));
  unsigned int sourceid = header->sourceid;
  unsigned int barrier = header->barrier;

  // Best guess as to whether this is a built event or not.
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
  assert(scaler_num < size_t(GetScalerCount()));

  return *(Int_t*)(GetPayload() + 24 + 4*scaler_num);
}

TNSCLFragment::TNSCLFragment(TSmartBuffer& buf){
  memcpy((char*)&fNSCLFragmentHeader, buf.GetData(), sizeof(fNSCLFragmentHeader));
  buf = buf.BufferSubset(sizeof(fNSCLFragmentHeader));

  memcpy((char*)fNSCLEvent.GetRawHeader(), buf.GetData(), sizeof(TRawEvent::RawHeader));
  buf = buf.BufferSubset(sizeof(TRawEvent::RawHeader));

  fNSCLEvent.SetFileType(kFileType::NSCL_EVT);

  fNSCLEvent.SetData(buf.BufferSubset(0,fNSCLEvent.GetBodySize()));
  buf = buf.BufferSubset(fNSCLEvent.GetBodySize());
}


Long_t       TNSCLFragment::GetFragmentTimestamp()     const{
  return fNSCLFragmentHeader.timestamp;
}

Int_t        TNSCLFragment::GetFragmentSourceID()      const{
  return fNSCLFragmentHeader.sourceid;
}

Int_t        TNSCLFragment::GetFragmentPayloadSize()   const{
  return fNSCLFragmentHeader.payload_size;
}

Int_t        TNSCLFragment::GetFragmentBarrier()       const{
  return fNSCLFragmentHeader.barrier;
}

TNSCLBuiltRingItem::TNSCLBuiltRingItem(TNSCLEvent& event)
  : fEvent(event) {
  assert(kNSCLEventType(event.GetEventType()) == kNSCLEventType::PHYSICS_EVENT);
  BuildFragments();
}

TNSCLFragment& TNSCLBuiltRingItem::GetFragment(size_t fragnum) {
  return fragments.at(fragnum);
}

Int_t TNSCLBuiltRingItem::Size() const {
  return fragments.size();
}

size_t TNSCLBuiltRingItem::NumFragments() const {
  return fragments.size();
}

Int_t TNSCLBuiltRingItem::GetBuiltRingItemSize() const {
  return *(Int_t*)fEvent.GetPayloadBuffer().GetData();
}

void TNSCLBuiltRingItem::BuildFragments() {
  // Skip past the size of the fragments
  TSmartBuffer buf = fEvent.GetPayloadBuffer().BufferSubset(sizeof(Int_t));

  // Loop through, extracting each fragment
  while(buf.GetSize()){
    // TNSCLFragment constructor advances the buffer
    fragments.emplace_back(buf);
  }
}
