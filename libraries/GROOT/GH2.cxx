
#include "GH2.h"

ClassImp(GH2)

std::map<int,GH2*> GH2::fGh2Map;

GH2::GH2(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,Int_t nbinsy, const Double_t *ybins) {
  fHist = new TH2I(name,title,nbinsx,xbins,nbinsy,ybins);
  Init();
  //this->Copy(hist);
  //hist.Copy(*this);
}


GH2::GH2(const char *name,const char *title,Int_t nbinsx,const Float_t *xbins,Int_t nbinsy, const Float_t *ybins) {
  fHist = new TH2I(name,title,nbinsx,xbins,nbinsy,ybins);
  Init();
  //this->Copy(hist);
  //hist.Copy(*this);
}


GH2::GH2(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,
                                            Int_t nbinsy, Double_t ylow, Double_t yup) {
  fHist = new TH2I(name,title,nbinsx,xbins,nbinsy,ylow,yup);
  Init();
  //this->Copy(hist);
  //hist.Copy(*this);
}


GH2::GH2(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                            Int_t nbinsy, Double_t *ybins) {
  fHist = new TH2I(name,title,nbinsx,xlow,xup,nbinsy,ybins);
  Init();
  //this->Copy(hist);
  //hist.Copy(*this);
}


GH2::GH2(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                            Int_t nbinsy, Double_t ylow, Double_t yup) {
  fHist = new TH2I(name,title,nbinsx,xlow,xup,nbinsy,ylow,yup);
  Init();
  //hist.Copy(*this);
  //this->Copy(hist);
}

GH2::GH2(TH2 *hist) {
  Init();
  //TH2::Copy(*hist);
  fHist = (TH2*)hist->Clone();
}

GH2::GH2(const TObject &obj) {
  Init();
  GH2 &gh2 = (GH2&)obj;
  fHist = (TH2*)gh2.fHist->Clone();
  //gh2.Copy(*this);
}

GH2::~GH2() {
  if(fProjections)
    fProjections->Delete();
}


void GH2::Init() { 
  int i=0;
  while(fGh2Map.count(i)!=0)
    i++;
  fKey = i;
  fProjections = new TList();
}


void GH2::Clear(Option_t *opt) {
  fHist->Clear();
}

void GH2::Print(Option_t *opt) const {
  fHist->Print();
}

void GH2::Copy(TObject &obj) const {
  //TH2 &th2 = (TH2&)obj;
  //th2.Copy((TH2&)(*this));
  GH2 &gh2 = (GH2&)obj;
  gh2.fProjections->Copy(*(this->fProjections));
  fHist = (TH2*)gh2.fHist->Clone();
}

void GH2::Draw(Option_t *opt) {
  TString option(opt);
  if(option.Length()==0) {
    fHist->Draw("colz"); 
    return;
  }
  fHist->Draw(opt);
}





