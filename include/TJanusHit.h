#ifndef _TJANUSDETECTORHIT_H_
#define _TJANUSDETECTORHIT_H_

#include "TDetectorHit.h"

class TJanusHit : public TDetectorHit {
public:
  TJanusHit() { }

  int GetDetnum() const;

  void Clear(Option_t* opt = "");
  void Print(Option_t* opt = "") const;
  void Copy(TObject& obj) const;

  void SetADCOverflowBit(bool bit)   { fEnergyOverflowBit = bit; }
  void SetADCUnderflowBit(bool bit)  { fEnergyUnderflowBit = bit; }
  bool GetADCOverflowBit()  const { return fEnergyOverflowBit; }
  bool GetADCUnderflowBit() const { return fEnergyUnderflowBit;}

  void SetTDCOverflowBit(bool bit)   { fTimeOverflowBit = bit; }
  void SetTDCUnderflowBit(bool bit)  { fTimeUnderflowBit = bit; }
  bool GetTDCOverflowBit()  const { return fTimeOverflowBit; }
  bool GetTDCUnderflowBit() const { return fTimeUnderflowBit;}

  TDetectorHit& GetBackHit() { return back_hit; }

  int GetFrontChannel() const;
  int GetBackChannel() const;

  int GetRing() const;
  int GetSector() const;
  TVector3 GetPosition(bool apply_array_offset = true) const;

private:
	 // Energy
  bool  fEnergyOverflowBit;
  bool  fEnergyUnderflowBit;
	 // Time
  bool  fTimeOverflowBit;
  bool  fTimeUnderflowBit;

  TDetectorHit back_hit;

  ClassDef(TJanusHit,4);
};

#endif /* _TJANUSDETECTORHIT_H_ */
