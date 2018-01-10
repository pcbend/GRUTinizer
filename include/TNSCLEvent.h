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
    TNSCLEvent();
    TNSCLEvent(const TRawEvent&);
    virtual ~TNSCLEvent();

    Int_t        GetBodyHeaderSize() const;
    long         GetTimestamp()      const;
    Int_t        GetSourceID()       const;
    Int_t        GetBarrierType()    const;
    int          IsBuiltData()       const;
    const char*  GetPayload()        const;
    TSmartBuffer GetPayloadBuffer()  const;
    Int_t        GetPayloadSize()    const;
  ClassDef(TNSCLEvent,0);
};

class TRawNSCLScalers : public TNSCLEvent {
public:
  // Seconds since the previous scaler read
  Int_t GetIntervalStartOffset();

  // Seconds since beginning of run
  Int_t GetIntervalEndOffset();

  // Time when writing to disk
  time_t GetUnixTime();

  // Interval (seconds) between each scaler packet
  Int_t GetIntervalDivisor();

// Number of integers to follow.
  Int_t GetScalerCount();

  // Are the scalers reset after each read
  Int_t IsIncremental();

  Int_t GetScalerValue(size_t scaler_num);

  ClassDef(TRawNSCLScalers,0);
};

class TNSCLFragment : public TObject {

public:
  TNSCLFragment(TSmartBuffer& buf);

  Long_t       GetFragmentTimestamp()     const;
  Int_t        GetFragmentSourceID()      const;
  Int_t        GetFragmentPayloadSize()   const;
  Int_t        GetFragmentBarrier()       const;

  TNSCLEvent&  GetNSCLEvent() { return fNSCLEvent; }

private:
  TRawEvent::TNSCLFragmentHeader fNSCLFragmentHeader;
  TNSCLEvent fNSCLEvent;

  ClassDef(TNSCLFragment,0);
};

class TNSCLBuiltRingItem : public TObject {
public:
  TNSCLBuiltRingItem(TNSCLEvent& event);

  TNSCLFragment& GetFragment(size_t fragnum);
  Int_t          Size()                      const;
  size_t         NumFragments()              const;
  Int_t          GetBuiltRingItemSize()      const;

private:
  void BuildFragments();


  TNSCLEvent fEvent;
  std::vector<TNSCLFragment> fragments;

  ClassDef(TNSCLBuiltRingItem,0);
};


#endif /* _TNSCLEVENT_H_ */
