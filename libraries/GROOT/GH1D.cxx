#include "GH1D.h"

#include <iostream>

#include "GH2I.h"

GH1D::GH1D(const TH1D& source)
  : parent(NULL), projection_axis(-1) {
  source.Copy(*this);
}

void GH1D::Clear(Option_t* opt) {
  TH1D::Clear(opt);
  parent = NULL;
}

void GH1D::Print(Option_t* opt) const {
  TH1D::Print(opt);
  std::cout << "\tParent: " << parent << std::endl;
}

void GH1D::Copy(TObject& obj) const {
  TH1D::Copy(obj);

  ((GH1D&)obj).parent = parent;
}

GH1D* GH1D::GetPrevious() const {
  if(parent && parent->InheritsFrom(GH2I::Class())) {
    GH2I* gpar = (GH2I*)parent;
    return gpar->GetPrevious(this);
  } else {
    return NULL;
  }
}

GH1D* GH1D::GetNext() const {
  if(parent && parent->InheritsFrom(GH2I::Class())) {
    GH2I* gpar = (GH2I*)parent;
    return gpar->GetNext(this);
  } else {
    return NULL;
  }
}

GH1D* GH1D::Project(int bin_low, int bin_high) const {
  if(bin_low > bin_high){
    std::swap(bin_low, bin_high);
  }

  if(parent && parent->InheritsFrom(GH2I::Class()) &&
     projection_axis!=-1) {
    GH2I* gpar = (GH2I*)parent;
    if(projection_axis == 0){
      return gpar->ProjectionY("_py", bin_low, bin_high);
    } else {
      return gpar->ProjectionX("_px", bin_low, bin_high);
    }
  } else {
    return NULL;
  }
}

GH1D* GH1D::Project_Background(int bin_low, int bin_high,
                               int bg_bin_low, int bg_bin_high,
                               kBackgroundSubtraction mode) const {
  if(bin_low > bin_high){
    std::swap(bin_low, bin_high);
  }
  if(bg_bin_low > bg_bin_high){
    std::swap(bg_bin_low, bg_bin_high);
  }

  if(parent && parent->InheritsFrom(GH2I::Class()) &&
     projection_axis!=-1) {
    GH2I* gpar = (GH2I*)parent;
    if(projection_axis == 0){
      return gpar->ProjectionY_Background(bin_low, bin_high,
                                          bg_bin_low, bg_bin_high,
                                          mode);
    } else {
      return gpar->ProjectionX_Background(bin_low, bin_high,
                                          bg_bin_low, bg_bin_high,
                                          mode);
    }
  } else {
    return NULL;
  }
}
