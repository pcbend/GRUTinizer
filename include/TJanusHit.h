#ifndef _TJANUSDETECTORHIT_H_
#define _TJANUSDETECTORHIT_H_

#include "TDetectorHit.h"

class TJanusHit : public TDetectorHit {
public:
  TJanusHit() { }

  void Clear(Option_t* opt = "");
  void Copy(TObject& obj) const;

  void SetAnalogChannel(int chan) { fChannel = chan; }
  void SetADCOverflowBit(bool bit)   { fEnergyOverflowBit = bit; }
  void SetADCUnderflowBit(bool bit)  { fEnergyUnderflowBit = bit; }
  void SetADCEntryType(char type)    { fEnergyEntryType = type; }
  void SetTDCOverflowBit(bool bit)   { fTimeOverflowBit = bit; }
  void SetTDCUnderflowBit(bool bit)  { fTimeUnderflowBit = bit; }
  void SetTDCEntryType(char type)    { fTimeEntryType = type; }
  void SetTimestamp(long ts)      { fTimestamp = ts; }
  void SetCharge(int chg)         { fCharge = chg; }
  void SetTDC(int tdc) {fTDC = tdc;}

  virtual Int_t Charge() const { return fCharge;  }
  int GetAnalogChannel() const { return fChannel; }
  bool GetADCOverflowBit()  const { return fEnergyOverflowBit; }
  bool GetADCUnderflowBit() const { return fEnergyUnderflowBit;}
  char GetADCEntryType() const  { return fEnergyEntryType; }
  long GetTimestamp() const { return fTimestamp; }

  int GetAnalogTDC() const { return fTDC; }
  char GetTDCEntryType() const  { return fTimeEntryType; }
  bool GetTDCOverflowBit()  const { return fTimeOverflowBit; }
  bool GetTDCUnderflowBit() const { return fTimeUnderflowBit;}

  bool IsADCValid() const { return fEnergyEntryType == 0; } //CAEN_ADC::EntryType::Event
  bool IsTDCValid() const { return fTimeEntryType == 0; } //CAEN_ADC::EntryType::Event

private:
  long  fTimestamp;
  int   fChannel;
	 // Energy
  int   fCharge;
  char  fEnergyEntryType;
  bool  fEnergyOverflowBit;
  bool  fEnergyUnderflowBit;
	 // Time
  int   fTDC;
  char  fTimeEntryType;
  bool  fTimeOverflowBit;
  bool  fTimeUnderflowBit;

  ClassDef(TJanusHit,3);
};

#endif /* _TJANUSDETECTORHIT_H_ */
