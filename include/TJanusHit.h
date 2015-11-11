#ifndef _TJANUSDETECTORHIT_H_
#define _TJANUSDETECTORHIT_H_

#include "TDetectorHit.h"

class TJanusHit : public TDetectorHit {
public:
  TJanusHit() { }

  void Clear(Option_t* opt = "");
  void Copy(TObject& obj) const;

  void SetAnalogChannel(int chan) { fChannel = chan; }
  void SetOverflowBit(bool bit)   { fOverflowBit = bit; }
  void SetUnderflowBit(bool bit)  { fUnderflowBit = bit; }
  void SetEntryType(char type)    { fEntryType = type; }
//  void SetTimestamp(long ts)      { fTimestamp = ts; }
  void SetCharge(int chg)         { fCharge = chg; }

  virtual Int_t Charge() const { return fCharge;  }
  int GetAnalogChannel() const { return fChannel; }
  bool GetOverflowBit()  const { return fOverflowBit; }
  bool GetUnderflowBit() const { return fUnderflowBit;}
  char GetEntryType() const  { return fEntryType; }
//  long GetTimestamp() const { return fTimestamp; }

  bool IsValid() const { return fEntryType == 0; } //CAEN_ADC::EntryType::Event

private:
//  long  fTimestamp;
  short fEntryType;
  int   fChannel;
  int   fCharge;
  bool  fOverflowBit;
  bool  fUnderflowBit;

  ClassDef(TJanusHit,2);
};

#endif /* _TJANUSDETECTORHIT_H_ */
