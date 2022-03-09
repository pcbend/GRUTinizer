#include "GH2I.h"



ClassImp(GH2I)

GH2I::GH2I(): GH2(),TArrayI() {
  SetBinsLength(9);
  if(GH1::fgDefaultSumw2) Sumw2();
}

GH2I::GH2I(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,
                                              Int_t nbinsy, const Double_t *ybins) :
  GH2(name,title,nbinsx,xbins,nbinsy,ybins) { 
  TArrayI::Set(fNcells);
  if(GH1::fgDefaultSumw2) Sumw2();
}


GH2I::GH2I(const char *name,const char *title,Int_t nbinsx,const Float_t *xbins,
                                              Int_t nbinsy, const Float_t *ybins) :
  GH2(name,title,nbinsx,xbins,nbinsy,ybins) { 
  TArrayI::Set(fNcells);
  if(GH1::fgDefaultSumw2) Sumw2();
}


GH2I::GH2I(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,
                                              Int_t nbinsy, Double_t ylow, Double_t yup) :
  GH2(name,title,nbinsx,xbins,nbinsy,ylow,yup) {
  TArrayI::Set(fNcells);
  if(GH1::fgDefaultSumw2) Sumw2();
}


GH2I::GH2I(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                              Int_t nbinsy, Double_t *ybins) :
  GH2(name,title,nbinsx,xlow,xup,nbinsy,ybins) {
  TArrayI::Set(fNcells);
  if(GH1::fgDefaultSumw2) Sumw2();
}


GH2I::GH2I(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                              Int_t nbinsy, Double_t ylow, Double_t yup) :
  GH2(name,title,nbinsx,xlow,xup,nbinsy,ylow,yup) {
  TArrayI::Set(fNcells);
  if(GH1::fgDefaultSumw2) Sumw2();
  if(xlow>=xup||ylow>=yup) SetBuffer(fgBufferSize);
}

GH2I::GH2I(const GH2I &obj) : GH2(), TArrayI() {
    ((GH2I&)obj).Copy(*this);
}

GH2I::GH2I(const TH1 &h2d) : GH2(), TArrayI() {
    ((TH1&)h2d).Copy(*this);
}


GH2I::~GH2I() {  }

void GH2I::AddBinContent(int bin) {
  if(fArray[bin] < 2147483647) fArray[bin]++;
}

void GH2I::AddBinContent(int bin,double w) {
  long newvalue = fArray[bin] + int(w);
  if(newvalue > -2147483647 && newvalue < 2147483647) {
    fArray[bin] = int(newvalue);
    return;
  }
  if(newvalue<-2147483647) fArray[bin] = -2147483647;
  if(newvalue>2147483647)  fArray[bin] =  2147483647;
}

void GH2I::Copy(TObject &obj) const {
  GH2::Copy((GH2I&)obj);
}

void GH2I::Reset(Option_t *opt) {
  GH2::Reset(opt);
  TArrayI::Reset();
}

void GH2I::SetBinsLength(int n) {
  if(n<0) n = (fXaxis.GetNbins()+2)*(fYaxis.GetNbins()+2);
  fNcells = n;
  TArrayI::Set(n);
}

GH2I& GH2I::operator=(const GH2I &h1) {
  if(this!=&h1) ((GH2I&)h1).Copy(*this);
  return *this;
}

GH2I operator*(float c1,GH2I &h1) {
  GH2I hnew = h1;
  hnew.Scale(c1);
  hnew.SetDirectory(0);
  return hnew;
}

GH2I operator*(GH2I &h1,GH2I &h2) {
  GH2I hnew = h1;
  hnew.Multiply(&h2);
  hnew.SetDirectory(0);
  return hnew;
}

GH2I operator/(GH2I &h1,GH2I &h2) {
  GH2I hnew = h1;
  hnew.Divide(&h2);
  hnew.SetDirectory(0);
  return hnew;
}

GH2I operator+(GH2I &h1,GH2I &h2) {
  GH2I hnew = h1;
  hnew.Add(&h2);
  hnew.SetDirectory(0);
  return hnew;
}

GH2I operator-(GH2I &h1,GH2I &h2) {
  GH2I hnew = h1;
  hnew.Add(&h2,-1);
  hnew.SetDirectory(0);
  return hnew;
}

















/*



void GH2I::Copy(TObject &obj) const {
  TH2::Copy(obj);
  //fProjections->Copy(*(((GH2I&)obj).fProjections));
  //fSummaryProjections->Copy(*(((GH2I&)obj).fSummaryProjections));
}

TObject *GH2I::Clone(const char *newname) const {
  std::string name = newname;
  if(!name.length())
    name = Form("%s_clone",GetName());
  return TH2::Clone(name.c_str());
}

void GH2I::Clear(Option_t *opt) {
  TString sopt(opt);
  if(!sopt.Contains("projonly")){
    TH2I::Clear(opt);
  }
  GH2Clear();
}

void GH2I::Print(Option_t *opt) const { }

void GH2I::Draw(Option_t *opt) {
  std::string option = opt;
  if(option == ""){
    option = "colz";
  }
  TH2I::Draw(option.c_str());
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
}

TH1 *GH2I::DrawCopy(Option_t *opt) const {
  TH1 *h = TH2I::DrawCopy(opt);
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
  return h;
}

TH1 *GH2I::DrawNormalized(Option_t *opt,Double_t norm) const {
  TH1 *h = TH2I::DrawNormalized(opt,norm);
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
  return h;
}










GH1D* GH2I::ProjectionX(const char* name,
                       int firstbin,
                       int lastbin,
                       Option_t* option) {
  return GH2ProjectionX(name,firstbin,lastbin,option);
}



GH1D* GH2I::ProjectionY(const char* name,
                       int firstbin,
                       int lastbin,
                       Option_t* option) {
  return GH2ProjectionY(name,firstbin,lastbin,option);
}


void GH2I::Streamer(TBuffer &b) {
  if(b.IsReading()) {
    Version_t v = b.ReadVersion(); 
    TH2I::Streamer(b);
    TDirectory *current = gDirectory;
    if(TDirectory::Cd(Form("%s_projections",this->GetName()))) { 
      TList *list = gDirectory->GetList();
      TIter iter(list);
      while(TObject *obj = iter.Next()) {
        if(obj->InheritsFrom(TH1::Class())) {
          GH1D *h = new GH1D(*obj);
          h->SetParent(this);
          fProjections.Add(h);
        }  
      }
    }
    current->cd();
  } else {
    b.WriteVersion(GH2I::IsA());
    TH2I::Streamer(b);
    if(fProjections.GetEntries()) {
      TDirectory *current = gDirectory;
      TDirectory *newdir  =  current->mkdir(Form("%s_projections",this->GetName());
      newdir->cd();
      fProjections->Write();
      current->cd();
    }

  }
}
*/







