#ifndef GH2I__H
#define GH2I__H

#include <cstdio>
#include <map>

#include <TNamed.h>
#include <TH2.h>
#include <TList.h>

class GH1D;

class GH2I : public TH2I {

public:
  GH2I(const TObject&);
  GH2I(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,Int_t nbinsy, const Double_t *ybins);
  GH2I(const char *name,const char *title,Int_t nbinsx,const Float_t *xbins,Int_t nbinsy, const Float_t *ybins);
  GH2I(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,
                                          Int_t nbinsy, Double_t ylow, Double_t yup);
  GH2I(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                          Int_t nbinsy, Double_t *ybins);
  GH2I(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                          Int_t nbinsy, Double_t ylow, Double_t yup);
  ~GH2I();

  // Int_t Fill(Double_t x, Double_t y)                           { return TH2::Fill(x,y); }
  // Int_t Fill(Double_t x, Double_t y, Double_t w)               { return TH2::Fill(x,y,w); }
  // Int_t Fill(Double_t x, const char *namey, Double_t w)        { return TH2::Fill(x,namey,w); }
  // Int_t Fill(const char* namex, Double_t y, Double_t w)        { return TH2::Fill(namex,y,w); }
  // Int_t Fill(const char* namex, const char *namey, Double_t w) { return TH2::Fill(namex,namey,w); }

  virtual void Draw(Option_t *opt="");

  virtual void Clear(Option_t *opt="");
  virtual void Print(Option_t *opt="") const;

  GH1D* ProjectionX(const char* name="_px",
                    int firstybin = 0,
                    int lastybin = -1,
                    Option_t* option=""); // *MENU*

  GH1D* ProjectionY(const char* name="_py",
                    int firstxbin = 0,
                    int lastxbin = -1,
                    Option_t* option=""); // *MENU*

  GH1D* GetPrevious(const GH1D* curr) const;
  GH1D* GetNext(const GH1D* curr) const;

private:
  void Init();
  TList *fProjections;

  ClassDef(GH2I,1)
};

#endif
