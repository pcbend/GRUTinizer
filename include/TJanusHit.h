#ifndef _TJANUSDETECTORHIT_H_
#define _TJANUSDETECTORHIT_H_

#include "TDetectorHit.h"

class TJanusHit : public TDetectorHit {
public:
  TJanusHit() { }

  void SetAnalogChannel(int chan) { channel = chan; }
  bool SetOverflowBit(bool bit)   { bit = overflow_bit; }
  bool SetUnderflowBit(bool bit)  { bit = underflow_bit; }

  int GetAnalogChannel() { return channel; }
  bool GetOverflowBit()  { return overflow_bit; }
  bool GetUnderflowBit() { return underflow_bit;}

private:
  int channel;
  bool overflow_bit;
  bool underflow_bit;

  ClassDef(TJanusHit,1);
};

#endif /* _TJANUSDETECTORHIT_H_ */
