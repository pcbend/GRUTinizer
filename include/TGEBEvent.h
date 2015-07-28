#ifndef _TGEBEVENT_H_
#define _TGEBEVENT_H_

#include "TRawEvent.h"

class TGEBEvent : public TRawEvent {
public:
  TGEBEvent();
  virtual ~TGEBEvent();
private:
  ClassDef(TGEBEvent, 0);
};

#endif /* _TGEBEVENT_H_ */
