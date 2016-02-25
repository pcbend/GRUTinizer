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
  //GH1D(const TH1 *source);
  //virtual void SetOption(Option_t* option=" ");

  TObject* GetParent() const { return parent.GetObject(); }
  void SetParent(TObject* obj) { parent = obj; }

  int GetProjectionAxis() const { return projection_axis; }
  void SetProjectionAxis(int axis) { projection_axis = axis; }

  void Clear(Option_t* opt="");
  void Print(Option_t* opt="") const;
  void Copy(TObject& obj) const;
  void Draw(Option_t* opt="");
  TH1 *DrawCopy(Option_t *opt="") const;
  TH1 *DrawNormalized(Option_t *opt="",Double_t norm=1) const;



  GH1D* GetPrevious() const;
  GH1D* GetNext() const;

  GH1D* Project(double bin_low, double bin_high) const;
  GH1D* Project_Background(double bin_low, double bin_high,
                           double bg_bin_low, double bg_bin_high,
                           kBackgroundSubtraction mode = kRegionBackground) const;

private:
  TRef parent;
  int projection_axis;

  ClassDef(GH1D,1)
};

#endif /* GH1D_H */
