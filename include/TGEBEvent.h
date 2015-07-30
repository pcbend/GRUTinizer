#ifndef _TGEBEVENT_H_
#define _TGEBEVENT_H_

#include "TRawEvent.h"

class TGEBEvent : public TRawEvent {
public:
  TGEBEvent();
  virtual ~TGEBEvent();

  Long_t GetTimestamp() const;

  TSmartBuffer GetPayloadBuffer() const;

  virtual void Clear(Option_t *opt ="");
  virtual void Print(Option_t *opt ="") const;


private:
  ClassDef(TGEBEvent, 0);
};

#endif /* _TGEBEVENT_H_ */
