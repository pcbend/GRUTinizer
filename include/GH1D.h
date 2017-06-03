#ifndef _GH1D_H
#define _GH1D_H

#include <GH1.h>
#include <TVectorD.h>

class GH1D : public GH1, public TArrayD {
  public:
    GH1D();
    GH1D(const char *name,const char *title,int nbinsx,double xlow,double xhigh);
    GH1D(const char *name,const char *title,int nbinsx,const double *xbins);
    GH1D(const char *name,const char *title,int nbinsx,const float  *xbins);
    explicit GH1D(const TVectorD &v);
    GH1D(const GH1D &h1d);
    GH1D(const TH1  &h1d);
    GH1D& operator=(const GH1D &h1);
    virtual ~GH1D() { }

    virtual void AddBinContent(int bin) { ++fArray[bin]; }
    virtual void AddBinContent(int bin,double w) { fArray[bin]+=(double)w; }
     
    virtual void Copy(TObject &hnew) const;
    virtual void Reset(Option_t *opt="");
    virtual void SetBinsLength(int n=-1);

    //Int_t Write(const char *name="",Int_t option=0,Int_t bufsize=0) const;  


  protected:
    virtual double RetrieveBinContent(int bin) const { return fArray[bin]; }
    virtual void   UpdateBinContent(int bin,double content) { fArray[bin]=content; }

  
  ClassDef(GH1D,4)

  public:
    friend GH1D  operator*(double c1, const GH1D &h1);
    friend GH1D  operator*(const GH1D &h1,double c1);
    friend GH1D  operator*(const GH1D &h1,const GH1D &h2);
    friend GH1D  operator/(const GH1D &h1,const GH1D &h2);
    friend GH1D  operator+(const GH1D &h1,const GH1D &h2);
    friend GH1D  operator-(const GH1D &h1,const GH1D &h2);
};

GH1D operator*(double c1,const GH1D &h1);
inline GH1D operator*(const GH1D &h1,double c1) { return operator*(c1,h1); }
GH1D operator*(const GH1D &h1,const GH1D &h2);
GH1D operator/(const GH1D &h1,const GH1D &h2);
GH1D operator+(const GH1D &h1,const GH1D &h2);
GH1D operator-(const GH1D &h1,const GH1D &h2);



#endif /* GH1D_H */
