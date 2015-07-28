#ifndef _TJANUSDETECTORHIT_H_
#define _TJANUSDETECTORHIT_H_

#include "TDetectorHit.h"

class TJanusHit : public TDetectorHit {
public:
  TJanusHit() { }

  void SetAnalogChannel(int chan) { fChannel = chan; }
  void SetOverflowBit(bool bit)   { fOverflowBit = bit; }
  void SetUnderflowBit(bool bit)  { fUnderflowBit = bit; }
  void SetEntryType(char type)    { fEntryType = type; }

  int GetAnalogChannel() { return fChannel; }
  bool GetOverflowBit()  { return fOverflowBit; }
  bool GetUnderflowBit() { return fUnderflowBit;}
  char GetEntryType()   { return fEntryType; }

  bool IsValid() const { return fEntryType == 0; } //CAEN_ADC::EntryType::Event

private:
  char fEntryType;
  int  fChannel;
  bool fOverflowBit;
  bool fUnderflowBit;

  ClassDef(TJanusHit,1);
};

#endif /* _TJANUSDETECTORHIT_H_ */
