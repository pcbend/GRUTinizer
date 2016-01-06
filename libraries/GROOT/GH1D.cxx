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
