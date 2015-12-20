#ifndef TTRANSITION_H
#define TTRANSITION_H

#include <cstdio>
#include "TObject.h"
#include "TString.h"

class TTransition : public TObject {
  friend class TNucleus;
 public:
  TTransition();
  virtual ~TTransition();

  bool isSortable() const { return true; }
  Int_t CompareIntensity(const TObject *obj) const;

  void Set(Double_t En,Double_t Intens=-1, Double_t EnUnc = -1, Double_t IntensUnc=-1) { fEnergy = En; fIntensity = Intens; fEnUncertainty = EnUnc; fIntUncertainty = IntensUnc;}
  void SetEnergy(Double_t En)           { fEnergy = En; }
  void SetIntensity(Double_t Intensity) { fIntensity = Intensity; }
  void SetEnUncertainty(Double_t Unc)   { fEnUncertainty = Unc; }
  void SetIntUncertainty(Double_t Unc)  { fIntUncertainty = Unc; }

  Double_t GetEnergy()         const { return fEnergy; }
  Double_t GetIntensity()      const { return fIntensity; }
  Double_t GetEnUncertainty()  const { return fEnUncertainty; }
  Double_t GetIntUncertainty() const { return fIntUncertainty; }

  void Clear(Option_t *opt="");
  void Print(Option_t *opt="") const;

  std::string PrintToString();
				       
 protected:
  Double_t fEnergy;
  Double_t fIntensity;
  Double_t fEnUncertainty;
  Double_t fIntUncertainty;

  ClassDef(TTransition,1);
};

#endif
