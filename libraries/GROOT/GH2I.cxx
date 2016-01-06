#include "GH2I.h"

#include <iostream>

#include "GH1D.h"

ClassImp(GH2I)

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

GH2I::GH2I(const TObject &obj) {
  if(obj.InheritsFrom(TH2::Class())){
    obj.Copy(*this);
  } else {
    Init();
  }
}

GH2I::~GH2I() {
  fProjections->Delete();
}


void GH2I::Init() {
  fProjections = new TList();
}


void GH2I::Clear(Option_t *opt) {
  fProjections->Clear();
}

void GH2I::Print(Option_t *opt) const { }

void GH2I::Draw(Option_t *opt) {
  TH2I::Draw(opt);
}

TH1D* GH2I::ProjectionX(const char* name,
                       int firstybin,
                       int lastybin,
                       Option_t* option) {
  TH1D* proj = TH2I::ProjectionX(name, firstybin, lastybin, option);
  GH1D* output = new GH1D(*proj);
  proj->Delete();
  output->SetParent(this);
  fProjections->Add(output);
  return output;
}

TH1D* GH2I::ProjectionY(const char* name,
                       int firstxbin,
                       int lastxbin,
                       Option_t* option) {
  TH1D* proj = TH2I::ProjectionY(name, firstxbin, lastxbin, option);
  GH1D* output = new GH1D(*proj);
  proj->Delete();
  output->SetParent(this);
  fProjections->Add(output);
  return output;
}

GH1D* GH2I::GetPrevious(const GH1D* curr) const {
  TObjLink* link = fProjections->FirstLink();
  while(link){
    if(link->GetObject() == curr){
      break;
    }
    link = link->Next();
  }
  if(!link){
    return 0;
  }

  if(link->Prev()){
    return (GH1D*)link->Prev()->GetObject();
  } else {
    return (GH1D*)fProjections->Last();
  }
}

GH1D* GH2I::GetNext(const GH1D* curr) const {
  TObjLink* link = fProjections->FirstLink();
  while(link){
    if(link->GetObject() == curr){
      break;
    }
    link = link->Next();
  }
  if(!link){
    return 0;
  }

  if(link->Next()){
    return (GH1D*)link->Next()->GetObject();
  } else {
    return (GH1D*)fProjections->First();
  }
}
