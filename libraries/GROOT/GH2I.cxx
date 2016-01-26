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
  Init();
  if(obj.InheritsFrom(TH2::Class())){
    obj.Copy(*this);
  }
}

GH2I::~GH2I() {
  fProjections->Delete();
  fSummaryProjections->Delete();
}


void GH2I::Copy(TObject &obj) const {
  ((GH2I&)obj).Init();
  TH2::Copy(obj);
  fProjections->Copy(*(((GH2I&)obj).fProjections));
  fSummaryProjections->Copy(*(((GH2I&)obj).fSummaryProjections));
}

TObject *GH2I::Clone(const char *newname) const {
  std::string name = newname;
  if(!name.length())
    name = Form("%s_clone",GetName());
  return TH2::Clone(name.c_str());
}

void GH2I::Init() {
  fProjections = new TList();
  fSummaryProjections = new TList();
  fIsSummary = false;
  fSummaryDirection = kXDirection;
}


void GH2I::Clear(Option_t *opt) {
  TString sopt(opt);
  if(!sopt.Contains("projonly")){
    TH2I::Clear(opt);
  }
  fProjections->Clear();
  fSummaryProjections->Clear();
}

void GH2I::Print(Option_t *opt) const { }

void GH2I::Draw(Option_t *opt) {
  std::string option = opt;
  if(option == ""){
    option = "colz";
  }
  TH2I::Draw(option.c_str());
}

GH1D* GH2I::Projection_Background(int axis,
                                  int firstbin,
                                  int lastbin,
                                  int first_bg_bin,
                                  int last_bg_bin,
                                  kBackgroundSubtraction mode) {
  std::string title;
  TH1D* proj = NULL;
  TH1D* bg_proj = NULL;

  if(axis==0){
    title = Form("%s_projx_%d_%d_bg_%d_%d",
                 GetName(), firstbin, lastbin,
                 first_bg_bin, last_bg_bin);
    proj = TH2I::ProjectionX("temp1", firstbin, lastbin);
    bg_proj = TH2I::ProjectionX("temp2", first_bg_bin, last_bg_bin);
  } else if (axis==1){
    title = Form("%s_projy_%d_%d_bg_%d_%d",
                 GetName(), firstbin, lastbin,
                 first_bg_bin, last_bg_bin);
    proj = TH2I::ProjectionY("temp1", firstbin, lastbin);
    bg_proj = TH2I::ProjectionY("temp2", first_bg_bin, last_bg_bin);
  } else {
    return NULL;
  }

  double bg_scaling = double(lastbin-firstbin)/double(last_bg_bin-first_bg_bin);
  if(mode == kNoBackground){
    bg_scaling = 0;
  }

  proj->Add(bg_proj, -bg_scaling);
  GH1D* output = new GH1D(*proj);
  proj->Delete();
  bg_proj->Delete();

  std::string name = title;
  output->SetName(name.c_str());
  output->SetTitle(title.c_str());
  output->SetParent(this);
  output->SetProjectionAxis(axis);
  output->SetDirectory(0);
  fProjections->Add(output);
  return output;
}

GH1D* GH2I::ProjectionX(const char* name,
                       int firstbin,
                       int lastbin,
                       Option_t* option) {
  std::string title;
  if(firstbin==0 && lastbin==-1){
    title = Form("%s_totalx",GetName());
  } else {
    title = Form("%s_projx_%d_%d",GetName(),firstbin,lastbin);
  }

  std::string actual_name = name;
  if(actual_name == "_px"){
    actual_name = title;
  }

  TH1D* proj = TH2I::ProjectionX("temp", firstbin, lastbin, option);
  GH1D* output = new GH1D(*proj);
  proj->Delete();

  output->SetName(actual_name.c_str());
  output->SetTitle(title.c_str());
  output->SetParent(this);
  output->SetProjectionAxis(0);
  output->SetDirectory(0);

  if(fIsSummary){
    fSummaryProjections->Add(output);
  } else {
    fProjections->Add(output);
  }
  return output;
}

GH1D* GH2I::ProjectionX_Background(int firstbin,
                                   int lastbin,
                                   int first_bg_bin,
                                   int last_bg_bin,
                                   kBackgroundSubtraction mode) {
  return Projection_Background(0, firstbin, lastbin,
                               first_bg_bin, last_bg_bin,
                               mode);
}


GH1D* GH2I::ProjectionY(const char* name,
                       int firstbin,
                       int lastbin,
                       Option_t* option) {
  std::string title;
  if(firstbin==0 && lastbin==-1){
    title = Form("%s_totaly",GetName());
  } else {
    title = Form("%s_projy_%d_%d",GetName(),firstbin,lastbin);
  }

  std::string actual_name = name;
  if(actual_name == "_py"){
    actual_name = title;
  }

  TH1D* proj = TH2I::ProjectionY("temp", firstbin, lastbin, option);
  GH1D* output = new GH1D(*proj);
  proj->Delete();
  output->SetName(actual_name.c_str());
  output->SetTitle(title.c_str());
  output->SetParent(this);
  output->SetProjectionAxis(1);
  output->SetDirectory(0);

  if(fIsSummary){
    fSummaryProjections->Add(output);
  } else {
    fProjections->Add(output);
  }
  return output;
}

GH1D* GH2I::ProjectionY_Background(int firstbin,
                                   int lastbin,
                                   int first_bg_bin,
                                   int last_bg_bin,
                                   kBackgroundSubtraction mode) {
  return Projection_Background(1, firstbin, lastbin,
                               first_bg_bin, last_bg_bin,
                               mode);
}

GH1D* GH2I::GetPrevious(const GH1D* curr) {
  if(fIsSummary){
    return GetPrevSummary(curr);
  }

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

GH1D* GH2I::GetNext(const GH1D* curr) {
  if(fIsSummary){
    return GetNextSummary(curr);
  }

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

GH1D* GH2I::GetNextSummary(const GH1D* curr) {
  std::string name = curr->GetName();
  size_t underscore_pos = name.rfind('_');
  int binnum = std::atoi(name.c_str() + underscore_pos + 1);
  binnum++;

  int max_binnum;
  if(fSummaryDirection == kXDirection){
    max_binnum = GetXaxis()->GetNbins();
  } else {
    max_binnum = GetYaxis()->GetNbins();
  }

  if(binnum > max_binnum){
    binnum = 1;
  }

  return SummaryProject(binnum);
}

GH1D* GH2I::GetPrevSummary(const GH1D* curr) {
  std::string name = curr->GetName();
  size_t underscore_pos = name.rfind('_');
  int binnum = std::atoi(name.c_str() + underscore_pos + 1);
  binnum--;

  int max_binnum;
  if(fSummaryDirection == kXDirection){
    max_binnum = GetXaxis()->GetNbins();
  } else {
    max_binnum = GetYaxis()->GetNbins();
  }

  if(binnum <= 0){
    binnum = max_binnum;
  }

  return SummaryProject(binnum);
}

GH1D* GH2I::SummaryProject(int binnum) {
  std::string hist_name = Form("%s_%d",GetName(),binnum);
  TObject* obj = fSummaryProjections->FindObject(hist_name.c_str());
  if(obj) {
    return (GH1D*)obj;
  }

  switch(fSummaryDirection) {
    case kXDirection:
      return ProjectionY(hist_name.c_str(), binnum, binnum);
    case kYDirection:
      return ProjectionX(hist_name.c_str(), binnum, binnum);
  }

  return NULL;
}
