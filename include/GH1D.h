#ifndef GH1D_H
#define GH1D_H

#include "TH1.h"
#include "TRef.h"

#include "GH2I.h"

class TF1;
class TClass;
class TMethodCall;

class GPeak;

class GH1D : public TH1D {
public:
  GH1D() : TH1D(), parent(NULL), projection_axis(-1),fFillClass(0),fFillMethod(0) { }
  GH1D(const TVectorD& v)
    : TH1D(v), parent(NULL), projection_axis(-1),fFillClass(0),fFillMethod(0) { }
  GH1D(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins)
    : TH1D(name, title, nbinsx, xbins), parent(NULL), projection_axis(-1),fFillClass(0),fFillMethod(0) { }
  GH1D(const char* name, const char* title, Int_t nbinsx, const Double_t* xbins)
    : TH1D(name, title, nbinsx, xbins), parent(NULL), projection_axis(-1),fFillClass(0),fFillMethod(0) { }
  GH1D(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup)
    : TH1D(name, title, nbinsx, xlow, xup), parent(NULL), projection_axis(-1),fFillClass(0),fFillMethod(0) { }

  GH1D(const TF1& function,Int_t nbinsx,Double_t xlow,Double_t xup);

  GH1D(const TH1& source);
  //GH1D(const TH1 *source);
  //virtual void SetOption(Option_t* option=" ");

  TObject* GetParent() const { return parent.GetObject(); }
  void SetParent(TObject* obj) { parent = obj; }

  int GetProjectionAxis() const { return projection_axis; }
  void SetProjectionAxis(int axis) { projection_axis = axis; }

  virtual Int_t Fill(const TObject* obj);
  virtual Int_t Fill(double x) { return TH1D::Fill(x); }
  virtual Int_t Fill(double x,double w) { return TH1D::Fill(x,w); }
  virtual Int_t Fill(const char *name,double w) { return TH1D::Fill(name,w); }

  void Clear(Option_t* opt="");
  void Print(Option_t* opt="") const;
  void Copy(TObject& obj) const;
  void Draw(Option_t* opt="");
  TH1 *DrawCopy(Option_t *opt="") const;
  TH1 *DrawNormalized(Option_t *opt="",Double_t norm=1) const;

  bool WriteDatFile(const char *outFile);
  
  GH1D* Project(int bins=-1);

  GH1D* GetPrevious(bool DrawEmpty=false) const;
  GH1D* GetNext(bool DrawEmpty=false) const;

  GH1D* Project(double bin_low, double bin_high) const;
  GH1D* Project_Background(double bin_low, double bin_high,
                           double bg_bin_low, double bg_bin_high,
                           kBackgroundSubtraction mode = kRegionBackground) const;

  void SetFillMethod(const char *classname,const char *methodname,const char* param="");
  //void SetGateMethod(const char *classname,const char *methodnamex,const char* paramx="",
  //                                         const char *methodnamey="",const char* paramy="");
  //void SetGateMethod(const char *classname,const char *methodnamex,
  //                   const char *classname,const char *methodnamey,
  //                   const char* paramx="",const char* paramy="");
  

  GPeak* DoPhotoPeakFit(double xlow,double xhigh,Option_t *opt=""); // *MENU* 
  GPeak* DoPhotoPeakFitNormBG(double xlow,double xhigh,Option_t *opt=""); // *MENU* 

  double GetLastXlow()  const { return xl_last;}
  double GetLastXhigh() const { return xh_last;}

private:
  TRef parent;
  int projection_axis;

  TClass      *fFillClass;  //!
  TMethodCall *fFillMethod; //!

  double xl_last; //!
  double xh_last; //!

  ClassDef(GH1D,1)
};

#endif /* GH1D_H */
