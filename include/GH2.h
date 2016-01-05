#ifndef GH2__H
#define GH2__H

#include <cstdio>
#include <map>

#include <TNamed.h>
#include <TH2.h>
#include <TList.h>

class GH2 : public TNamed {

  public:
    GH2(TH2*);
    GH2(const TObject&);
    GH2(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,Int_t nbinsy, const Double_t *ybins);
    GH2(const char *name,const char *title,Int_t nbinsx,const Float_t *xbins,Int_t nbinsy, const Float_t *ybins);
    GH2(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,
                                           Int_t nbinsy, Double_t ylow, Double_t yup);
    GH2(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                           Int_t nbinsy, Double_t *ybins);
    GH2(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                           Int_t nbinsy, Double_t ylow, Double_t yup);
    ~GH2();

    Int_t Fill(Double_t x, Double_t y)                           { return fHist->Fill(x,y); }
    Int_t Fill(Double_t x, Double_t y, Double_t w)               { return fHist->Fill(x,y,w); }
    Int_t Fill(Double_t x, const char *namey, Double_t w)        { return fHist->Fill(x,namey,w); }
    Int_t Fill(const char* namex, Double_t y, Double_t w)        { return fHist->Fill(namex,y,w); }
    Int_t Fill(const char* namex, const char *namey, Double_t w) { return fHist->Fill(namex,namey,w); }

    virtual void Draw(Option_t *opt="");

    virtual void Clear(Option_t *opt="");
    virtual void Print(Option_t *opt="") const;
    virtual void Copy(TObject&) const;

  private:
    void Init();
    mutable TH2   *fHist;
    TList *fProjections;

    int    fKey;
    static std::map<int,GH2*> fGh2Map;

  ClassDef(GH2,1)
};

#endif

