#ifndef GH1D_H
#define GH1D_H

#include "TH1.h"

class GH1D : public TH1D {
public:
  GH1D() : TH1D() { }
  GH1D(const TVectorD& v)
    : TH1D(v) { }
  GH1D(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins)
    : TH1D(name, title, nbinsx, xbins) { }
  GH1D(const char* name, const char* title, Int_t nbinsx, const Double_t* xbins)
    : TH1D(name, title, nbinsx, xbins) { }
  GH1D(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup)
    : TH1D(name, title, nbinsx, xlow, xup) { }

  GH1D(const TH1D& source);

  TObject* GetParent() { return parent; }
  void SetParent(TObject* obj) { parent = obj; }

  void Clear(Option_t* opt="");
  void Print(Option_t* opt="") const;
  void Copy(TObject& obj) const;

  GH1D* GetPrevious() const;
  GH1D* GetNext() const;

private:
  // TODO: We'll need a custom streamer here to set the parent correctly.
  TObject* parent;

  ClassDef(GH1D,1)
};

#endif /* GH1D_H */
