
#include "GH2I.h"

ClassImp(GH2I)

std::map<int,GH2I*> GH2I::fGh2Map;

GH2I::GH2I(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,Int_t nbinsy, const Double_t *ybins) :
  TH2I(name,title,nbinsx,xbins,nbinsy,ybins) {
  Init();
}


GH2I::GH2I(const char *name,const char *title,Int_t nbinsx,const Float_t *xbins,Int_t nbinsy, const Float_t *ybins) :
  TH2I(name,title,nbinsx,xbins,nbinsy,ybins) {
  Init();
}


GH2I::GH2I(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,
                                            Int_t nbinsy, Double_t ylow, Double_t yup) : 
  TH2I(name,title,nbinsx,xbins,nbinsy,ylow,yup) {
  Init();
}


GH2I::GH2I(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                            Int_t nbinsy, Double_t *ybins) :
  TH2I(name,title,nbinsx,xlow,xup,nbinsy,ybins) {
  Init();
}


GH2I::GH2I(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                            Int_t nbinsy, Double_t ylow, Double_t yup) :
  TH2I(name,title,nbinsx,xlow,xup,nbinsy,ylow,yup) {
  Init();
}

GH2I::GH2I(TH2 &hist) {
  Init();
  hist.Copy(*this);
}

GH2I::GH2I(const TObject &obj) {
  Init();
  if(obj.InheritsFrom(TH2::Class()) 
    obj.Copy(*this);
}

GH2I::~GH2I() {
  if(fProjections)
    fProjections->Delete();
}


void GH2I::Init() { 
  fProjections = new TList();
}


void GH2I::Clear(Option_t *opt) {
  fHist->Clear();
  fProjections->Clear();
}

void GH2I::Print(Option_t *opt) const {
  fHist->Print();
}

void GH2I::Copy(TObject &obj) const {
  if(obj.InheritsFrom(GH2I::Class())
    ((GH2I&)obj).fProjections->Copy(*(this->fProjections));
  else
    Init();
  ((TH2*)obj).Copy(*this);
}

void GH2I::Draw(Option_t *opt) {
  TString option(opt);
  if(option.Length()==0) {
    fHist->Draw("colz"); 
    return;
  }
  fHist->Draw(opt);
}





