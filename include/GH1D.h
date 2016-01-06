#ifndef GH1D_H
#define GH1D_H

#include "TH1.h"
#include "TRef.h"

#include "GH2I.h"

class GH1D : public TH1D {
public:
  GH1D() : TH1D(), parent(NULL), projection_axis(-1) { }
  GH1D(const TVectorD& v)
    : TH1D(v), parent(NULL), projection_axis(-1) { }
  GH1D(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins)
    : TH1D(name, title, nbinsx, xbins), parent(NULL), projection_axis(-1) { }
  GH1D(const char* name, const char* title, Int_t nbinsx, const Double_t* xbins)
    : TH1D(name, title, nbinsx, xbins), parent(NULL), projection_axis(-1) { }
  GH1D(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup)
    : TH1D(name, title, nbinsx, xlow, xup), parent(NULL), projection_axis(-1) { }

  GH1D(const TH1D& source);

  TObject* GetParent() const { return parent.GetObject(); }
  void SetParent(TObject* obj) { parent = obj; }

  int GetProjectionAxis() const { return projection_axis; }
  void SetProjectionAxis(int axis) { projection_axis = axis; }

  void Clear(Option_t* opt="");
  void Print(Option_t* opt="") const;
  void Copy(TObject& obj) const;

  GH1D* GetPrevious() const;
  GH1D* GetNext() const;

  GH1D* Project(int bin_low, int bin_high) const;
  GH1D* Project_Background(int bin_low, int bin_high,
                           int bg_bin_low, int bg_bin_high,
                           kBackgroundSubtraction mode = kRegionBackground) const;

private:
  TRef parent;
  int projection_axis;

  ClassDef(GH1D,1)
};

#endif /* GH1D_H */
