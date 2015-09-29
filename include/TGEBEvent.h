#ifndef _TGEBEVENT_H_
#define _TGEBEVENT_H_

#include "TRawEvent.h"

class TMode3;

class TGEBEvent : public TRawEvent {
public:
  TGEBEvent();
  TGEBEvent(const TRawEvent&);
  virtual ~TGEBEvent();

  long GetTimestamp() const;

  const char* GetPayload() const;
  TSmartBuffer GetPayloadBuffer() const;

  virtual void Clear(Option_t *opt ="");
  virtual void Print(Option_t *opt ="") const;

  ClassDef(TGEBEvent, 0);
};

class TGEBMode3Event : public TObject {
public:
  TGEBMode3Event(TGEBEvent& event)
    : fEvent(event) {
    BuildFragments();
  }

  TGEBEvent& GetFragment(size_t fragnum) { return fragments[fragnum]; }
  size_t     NumFragments() const { return fragments.size(); }

private:
  void BuildFragments();

  TGEBEvent fEvent;
  std::vector<TGEBEvent> fragments;

  ClassDef(TGEBMode3Event,0);
};

// TODO remove commented code
// class TGEBMode3Event : public TGEBEvent {
// public:
//   TGEBMode3Event(const TGEBEvent&);
//   ~TGEBMode3Event();

//   bool GetNextItem(TMode3& output, bool read_waveform = false);  // only used in mode3 data.
//   //TSmartBuffer GetNextItem(bool read_waveform = false);  // only used in mode3 data.


// private:
//   TSmartBuffer buf;

//   ClassDef(TGEBMode3Event,0);
// };


#endif /* _TGEBEVENT_H_ */
