

#include <GH1D.h>

#include <TClass.h>
#include <TBuffer.h>

ClassImp(GH1D) 

GH1D::GH1D(): GH1(),TArrayD() { 
  fDimension=1;
  SetBinsLength(3);
  if(GH1::fgDefaultSumw2) 
    Sumw2();
}

GH1D::GH1D(const char *name,const char *title,int nbins,double xlow,double xhigh) :
      GH1(name,title,nbins,xlow,xhigh) {
  fDimension=1;
  TArrayD::Set(fNcells);
  if(GH1::fgDefaultSumw2) 
    Sumw2();
} 

GH1D::GH1D(const char *name,const char *title,int nbins,const double *bins) :
      GH1(name,title,nbins,bins) {
  fDimension=1;
  TArrayD::Set(fNcells);
  if(GH1::fgDefaultSumw2) 
    Sumw2();
} 

GH1D::GH1D(const char *name,const char *title,int nbins,const float *bins) :
      GH1(name,title,nbins,bins) {
  fDimension=1;
  TArrayD::Set(fNcells);
  if(GH1::fgDefaultSumw2) 
    Sumw2();
} 

GH1D::GH1D(const TVectorD &v): GH1("TVectorD","",v.GetNrows(),0,v.GetNrows()) { 
  TArrayD::Set(fNcells);
  fDimension=1;
  int ivlow=v.GetLwb();
  for(int i=0;i<fNcells-2;i++) {
    SetBinContent(i+1,v(i+ivlow));
  }
  TArrayD::Set(fNcells);
  if(GH1::fgDefaultSumw2) 
    Sumw2();
}

GH1D::GH1D(const GH1D &h1d) : GH1(),TArrayD() {
  ((GH1D&)h1d).Copy(*this);
}

GH1D::GH1D(const TH1 &h1d) : GH1(),TArrayD() {
  //((GH1D&)h1d).Copy(*this);
  ((TH1&)h1d).Copy(*this);
}

void GH1D::Copy(TObject &obj) const {
  GH1::Copy(obj);
}

void GH1D::Reset(Option_t *opt) {
  GH1::Reset(opt);
  TArrayD::Reset();
}

void GH1D::SetBinsLength(int n) {
  if(n<0) 
    n=fXaxis.GetNbins()+2;
  fNcells = n;
  TArrayD::Set(n);
}



//void GH1D::Streamer(TBuffer &r__b) {
//  unsigned int r__s,r__c;
//  if(r__b.IsReading()) {
//    Version_t r__v = r__b.ReadVersion(&r__s,&r__c);
//    printf("I AM HEER; version = %i\n",r__v);
//    printf("r__b.GetParent()->GetName() = %s\n",r__b.GetParent()->GetName());
//    if(r__v<3) {
//      printf("I AM HEER; version = %i\n",r__v);
//      TH1D h; 
//      r__b.ReadClassBuffer(TH1D::Class(),&h);
//      h.Copy(*this);
//      //printf("h->GetBinContent(80) = %.02f\n",h.GetBinContent(80) );
//      //h.DrawCopy();
//      //h.Streamer(r__b); 
//      //h.Copy(*this);
//    } else {
//      //r__b.ReadClassBuffer(GH1::Class(),this);
//      r__b.ReadClassBuffer(GH1D::Class(),this);
//      //GH1D::Class()->ReadBuffer(r__b, this);
//    }
//  } else {
//    GH1::Class()->WriteBuffer(r__b,this);
//    GH1D::Class()->WriteBuffer(r__b,this);
//  }
//}



GH1D &GH1D::operator=(const GH1D &h1) {
  if(this!=&h1) ((TH1D&)h1).Copy(*this);
  return *this;
}

GH1D operator*(double c1,const GH1D &h1) {
  GH1D hnew = h1;
  hnew.Scale(c1);
  hnew.SetDirectory(0);
  return hnew;
}

GH1D operator*(const GH1D &h1,const GH1D &h2) {
  GH1D hnew = h1;
  hnew.Multiply(&h2);
  hnew.SetDirectory(0);
  return hnew;
}

GH1D operator/(const GH1D &h1,const GH1D &h2) {
  GH1D hnew = h1;
  hnew.Divide(&h2);
  hnew.SetDirectory(0);
  return hnew;
}
  
GH1D operator+(const GH1D &h1,const GH1D &h2) {
  GH1D hnew = h1;
  hnew.Add(&h2);
  hnew.SetDirectory(0);
  return hnew;
}

GH1D operator-(const GH1D &h1,const GH1D &h2) {
  GH1D hnew = h1;
  hnew.Add(&h2,-1);
  hnew.SetDirectory(0);
  return hnew;
}



//Int_t GH1D::Write(const char *name,Int_t option,Int_t bufsize) const {
//  TH1D hist;
  //hist.Copy(*this);
//  this->Copy(hist);
//  hist.SetNameTitle(this->GetName(),this->GetTitle());
//  return hist.Write();
//}




































