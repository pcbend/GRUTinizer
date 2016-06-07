#ifndef _TRCNPEVENT_H_
#define _TRCNPEVENT_H_

#include "TRawEvent.h"
//#include "RCNPEvent.h"

class RCNPEvent;

class TRCNPEvent : public TRawEvent {
public:
  TRCNPEvent();
  TRCNPEvent(const TRawEvent&);
  virtual ~TRCNPEvent();

  long GetTimestamp() const;

  const char* GetPayload() const;
  TSmartBuffer GetPayloadBuffer() const;

  virtual void Clear(Option_t *opt ="");
  virtual void Print(Option_t *opt ="") const;

  RCNPEvent* event;

  ClassDef(TRCNPEvent, 0);
};

#endif /* _TRCNPEVENT_H_ */
