
#include <GHBase.h>
#include <GH1D.h>
#include <GH2D.h>
#include <GRootCommands.h>

#include <fstream>

bool GHBase::WriteDatFile(const char *outFile) {
  if(strlen(outFile)<1) 
    return 0;
  
  std::ofstream out;
  out.open(outFile);

  if(!(out.is_open())) 
    return 0;

  for(int i=0;i<GetHist()->GetNbinsX();i++){
    out << GetHist()->GetXaxis()->GetBinCenter(i) << "\t" << GetHist()->GetBinContent(i) << std::endl;
  }
  out << std::endl;
  out.close();
  return 1;
}

GH1D *GHBase::Flip(int bins) {
  TH1 *h = GetHist();
  GH1D *proj = 0;
  double ymax = h->GetMinimum();
  double ymin = h->GetMaximum();
  if(bins==-1) {
    bins = abs(ymax-ymin);
    if(bins<1)
      bins=100;
  }

  proj = new GH1D(Form("%s_x_as_y",h->GetName()),
                  Form("%s_x_as_y;%s;%s",h->GetName(),h->GetYaxis()->GetTitle(),h->GetXaxis()->GetTitle()),
                  bins,ymin,ymax);
  for(int x=0;x<GetHist()->GetNbinsX();x++) {
    if(h->GetBinContent(x)!=0)
      proj->Fill(h->GetBinContent(x));
  }
  return proj;
}
 
GH1D* GHBase::Project(double value_low, double value_high) const {
  if(parent.GetObject() && parent.GetObject()->InheritsFrom(GH2Base::Class()) &&
    projection_axis!=-1) {
    if(value_low > value_high) {
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

GH1D* GHBase::Project(double bin_low, double bin_high,GH1D *bg) const {
  if(!GetHist()->InheritsFrom(GHBase::Class()))
    return 0;
  GH1D *proj = ((GHBase*)GetHist())->Project(bin_low,bin_high);
  proj->Add(bg,-1);
  proj->SetTitle(Form("%s - %s",proj->GetTitle(),bg->GetTitle()));
  return proj;
}

GH1D* GHBase::Project(double bin_low, double bin_high,double bg_bin_low,double bg_bin_high,double scale) const {
  if(!GetHist()->InheritsFrom(GHBase::Class()))
    return 0;
  GH1D *project = ((GHBase*)GetHist())->Project(bin_low,bin_high);    
  GH1D *bg      = ((GHBase*)GetHist())->Project(bg_bin_low,bg_bin_high);    

  if(scale>0.000) { scale*=-1.; }

  project->SetTitle(Form("%s - %s",project->GetTitle(),bg->GetTitle()));
  return project;
}

GH1D* GHBase::GetPrevious(bool DrawEmpty) const {
  if(!GetHist()->InheritsFrom(GHBase::Class()))
    return 0;
  if(parent.GetObject() && parent.GetObject()->InheritsFrom(GH2Base::Class())) {
    GH2D* gpar = (GH2D*)parent.GetObject();
    int first  = GetHist()->GetXaxis()->GetFirst();
    int last   = GetHist()->GetXaxis()->GetLast();
    GH1D *prev = gpar->GetPrevious(((GH1D*)GetHist()),DrawEmpty);
    prev->GetXaxis()->SetRange(first,last);
    return prev; //gpar->GetPrevious(this,DrawEmpty);
  } else {
    return NULL;
  }
}

GH1D* GHBase::GetNext(bool DrawEmpty) const {
  if(!GetHist()->InheritsFrom(GHBase::Class()))
    return 0;
  if(parent.GetObject() && parent.GetObject()->InheritsFrom(GH2Base::Class())) {
    GH2D* gpar = (GH2D*)parent.GetObject();
    int first  = GetHist()->GetXaxis()->GetFirst();
    int last   = GetHist()->GetXaxis()->GetLast();
    GH1D *next = gpar->GetNext(((GH1D*)GetHist()),DrawEmpty);
    next->GetXaxis()->SetRange(first,last);
    return next; //gpar->GetPrevious(this,DrawEmpty);
  } else {
    return NULL;
  }
}

GPeak* GHBase::DoPhotoPeakFit(double xlow,double xhigh,Option_t *opt) {
  return PhotoPeakFit(GetHist(),xlow,xhigh,opt);
}


GPeak* GHBase::DoPhotoPeakFitNormBG(double xlow,double xhigh,Option_t *opt) {
  return PhotoPeakFitNormBG(GetHist(),xlow,xhigh,opt);
}



