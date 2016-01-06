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

GH1D* GH2I::ProjectionX(const char* name,
                       int firstybin,
                       int lastybin,
                       Option_t* option) {
  std::string actual_title;
  if(firstybin==0 && lastybin==-1){
    actual_title = Form("%s_totalx",GetName());
  } else {
    actual_title = Form("%s_projx_%d_%d",GetName(),firstybin,lastybin);
  }

  std::string actual_name = name;
  if(actual_name == "_px"){
    actual_name = actual_title;
  }

  TH1D* proj = TH2I::ProjectionX("temp", firstybin, lastybin, option);
  GH1D* output = new GH1D(*proj);
  proj->Delete();
  output->SetName(actual_name.c_str());
  output->SetTitle(actual_title.c_str());
  output->SetParent(this);
  output->SetProjectionAxis(0);
  fProjections->Add(output);
  return output;
}

GH1D* GH2I::ProjectionY(const char* name,
                       int firstxbin,
                       int lastxbin,
                       Option_t* option) {
  std::string actual_title;
  if(firstxbin==0 && lastxbin==-1){
    actual_title = Form("%s_totaly",GetName());
  } else {
    actual_title = Form("%s_projy_%d_%d",GetName(),firstxbin,lastxbin);
  }

  std::string actual_name = name;
  if(actual_name == "_py"){
    actual_name = actual_title;
  }

  TH1D* proj = TH2I::ProjectionY("temp", firstxbin, lastxbin, option);
  GH1D* output = new GH1D(*proj);
  proj->Delete();
  output->SetName(actual_name.c_str());
  output->SetTitle(actual_title.c_str());
  output->SetParent(this);
  output->SetProjectionAxis(1);
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
