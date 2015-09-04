#ifndef _TGEBEVENT_H_
#define _TGEBEVENT_H_

#include "TRawEvent.h"

class TMode3;

class TGEBEvent : public TRawEvent {
  public:
    TGEBEvent();
    TGEBEvent(const TRawEvent&);
    virtual ~TGEBEvent();

    virtual long GetTimestamp() const;

    const char* GetPayload() const;
    TSmartBuffer GetPayloadBuffer() const;

    virtual void Clear(Option_t *opt ="");
    virtual void Print(Option_t *opt ="") const;

//  private:
    bool FillCondition();

  ClassDef(TGEBEvent, 0);
};

class TGEBMode3Event : public TGEBEvent {
  public:
    TGEBMode3Event(const TGEBEvent&);
    ~TGEBMode3Event(); 

    bool GetNextItem(TMode3& output, bool read_waveform = false);  // only used in mode3 data.


  private:
    TSmartBuffer buf;

  ClassDef(TGEBMode3Event,0);
};


#endif /* _TGEBEVENT_H_ */
