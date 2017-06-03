#ifndef GH2D__H
#define GH2D__H

//#include <cstdio>
//#include <map>

//#include <TNamed.h>
//#include <TH2.h>
//#include <TList.h>
//#include <TVirtualPad.h>
//#include <TFrame.h>

//#include <GH2Base.h>
//class GH1D;

#include <GH2.h>

class GH2D : public GH2, public TArrayI {

public:
  GH2D(); 
  GH2D(const GH2D&);
  GH2D(const TH1  &h1d);
  GH2D(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,Int_t nbinsy, const Double_t *ybins);
  GH2D(const char *name,const char *title,Int_t nbinsx,const Float_t *xbins,Int_t nbinsy, const Float_t *ybins);
  GH2D(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins,
                                          Int_t nbinsy, Double_t ylow, Double_t yup);
  GH2D(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                          Int_t nbinsy, Double_t *ybins);
  GH2D(const char *name,const char *title,Int_t nbinsx, Double_t xlow, Double_t xup,
                                          Int_t nbinsy, Double_t ylow, Double_t yup);
  ~GH2D();

  virtual void AddBinContent(int bin);
  virtual void AddBinContent(int bin,double w);

  virtual void Copy(TObject &obj) const;
  
  GH2D &operator=(const GH2D &h1);
  
  virtual void Reset(Option_t *opt="");
  virtual void SetBinsLength(int n=-1);


  //Int_t Write(const char *name="",Int_t option=0,Int_t bufsize=0) const;  

protected:
  virtual double RetrieveBinContent(int bin) const { return double (fArray[bin]); }
  virtual void   UpdateBinContent(int bin,double content) { fArray[bin] = float(content); }

  ClassDef(GH2D,3)
};

GH2D operator*(float c1,const GH2D &h1);
inline GH2D operator*(const GH2D &h1,float c1) { return operator*(c1,h1); }
GH2D operator*(const GH2D &h1,const GH2D &h2);
GH2D operator/(const GH2D &h1,const GH2D &h2);
GH2D operator+(const GH2D &h1,const GH2D &h2);
GH2D operator-(const GH2D &h1,const GH2D &h2);


#endif
