#include "GH1D.h"

#include <iostream>

#include "TVirtualPad.h"
#include "GH2I.h"
#include "GH2D.h"

#include "TFrame.h"
//#include "TROOT.h"
//#include "TSystem.h"

GH1D::GH1D(const TH1& source)
  : parent(NULL), projection_axis(-1) {
  source.Copy(*this);
}

/*
GH1D::GH1D(const TH1 *source)
  : parent(NULL), projection_axis(-1) {
  if(source->GetDiminsion()>1) {
    return;
  }

  // Can copy from any 1-d TH1, not just a TH1D
  source->Copy(*this);

  // Force a refresh of any parameters stored in the option string.
  SetOption(GetOption());
}

void GH1D::SetOption(Option_t* opt) {
  fOption = opt;

  TString sopt = opt;
  if(sopt.Index("axis:")) {
    projection_axis = 0;// TODO
  }
}
*/

void GH1D::Clear(Option_t* opt) {
  TH1D::Clear(opt);
  parent = NULL;
}

void GH1D::Print(Option_t* opt) const {
  TH1D::Print(opt);
  std::cout << "\tParent: " << parent.GetObject() << std::endl;
}

void GH1D::Copy(TObject& obj) const {
  TH1D::Copy(obj);

  ((GH1D&)obj).parent = parent;
}


void GH1D::Draw(Option_t* opt) {
  TH1D::Draw(opt);
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
}

TH1 *GH1D::DrawCopy(Option_t *opt) const {
  TH1 *h = TH1D::DrawCopy(opt);
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
  return h;
}

TH1 *GH1D::DrawNormalized(Option_t *opt,Double_t norm) const {
  TH1 *h = TH1D::DrawNormalized(opt,norm);
  if(gPad) {
    gPad->Update();
    gPad->GetFrame()->SetBit(TBox::kCannotMove);
  }
  return h;
}



GH1D* GH1D::GetPrevious() const {
  if(parent.GetObject() && parent.GetObject()->InheritsFrom(GH2Base::Class())) {
    GH2D* gpar = (GH2D*)parent.GetObject();
    return gpar->GetPrevious(this);
  } else {
    return NULL;
  }
}

GH1D* GH1D::GetNext() const {
  if(parent.GetObject() && parent.GetObject()->InheritsFrom(GH2Base::Class())) {
    GH2D* gpar = (GH2D*)parent.GetObject();
    return gpar->GetNext(this);
  } else {
    return NULL;
  }
}

GH1D* GH1D::Project(double value_low, double value_high) const {

  if(parent.GetObject() && parent.GetObject()->InheritsFrom(GH2Base::Class()) &&
     projection_axis!=-1) {
    if(value_low > value_high){
      std::swap(value_low, value_high);
    }
    GH2D* gpar = (GH2D*)parent.GetObject();
    if(projection_axis == 0){
      int bin_low  = gpar->GetXaxis()->FindBin(value_low);
      int bin_high = gpar->GetXaxis()->FindBin(value_high);
      return gpar->ProjectionY("_py", bin_low, bin_high);
    } else {
      int bin_low  = gpar->GetYaxis()->FindBin(value_low);
      int bin_high = gpar->GetYaxis()->FindBin(value_high);
      return gpar->ProjectionX("_px", bin_low, bin_high);
    }
  } else {
    return NULL;
  }
}

GH1D* GH1D::Project_Background(double value_low, double value_high,
                               double bg_value_low, double bg_value_high,
                               kBackgroundSubtraction mode) const {
  if(parent.GetObject() && parent.GetObject()->InheritsFrom(GH2Base::Class()) &&
     projection_axis!=-1) {
    if(value_low > value_high){
      std::swap(value_low, value_high);
    }
    if(bg_value_low > bg_value_high){
      std::swap(bg_value_low, bg_value_high);
    }

    GH2D* gpar = (GH2D*)parent.GetObject();
    if(projection_axis == 0){
      int bin_low     = gpar->GetXaxis()->FindBin(value_low);
      int bin_high    = gpar->GetXaxis()->FindBin(value_high);
      int bg_bin_low  = gpar->GetXaxis()->FindBin(bg_value_low);
      int bg_bin_high = gpar->GetXaxis()->FindBin(bg_value_high);

      return gpar->ProjectionY_Background(bin_low, bin_high,
                                          bg_bin_low, bg_bin_high,
                                          mode);
    } else {
      int bin_low     = gpar->GetYaxis()->FindBin(value_low);
      int bin_high    = gpar->GetYaxis()->FindBin(value_high);
      int bg_bin_low  = gpar->GetYaxis()->FindBin(bg_value_low);
      int bg_bin_high = gpar->GetYaxis()->FindBin(bg_value_high);

      return gpar->ProjectionX_Background(bin_low, bin_high,
                                          bg_bin_low, bg_bin_high,
                                          mode);
    }
  } else {
    return NULL;
  }
}

GH1D *GH1D::Project(int bins) {
  GH1D *proj = 0;
  double ymax = GetMinimum();
  double ymin = GetMaximum();
  if(bins==-1) {
    bins = abs(ymax-ymin);
    if(bins<1)
      bins=100;
  }
  proj = new GH1D(Form("%s_y_axis_projection",GetName()),
                  Form("%s_y_axis_projection",GetName()),
                  bins,ymin,ymax);
  for(int x=0;x<GetNbinsX();x++) {
    if(GetBinContent(x)!=0)
      proj->Fill(GetBinContent(x));
  }

  return proj;
}

