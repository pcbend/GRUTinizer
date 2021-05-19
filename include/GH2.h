#ifndef __GH2_H_
#define __GH2_H_

#include <map>

#include <GH1.h>
#include <TH2.h>
#include <TRef.h>
#include <TList.h>

class TProfile;

class GH2 : public GH1 { 
  protected:
    Double_t     fScalefactor;     //Scale factor
    Double_t     fTsumwy;          //Total Sum of weight*Y
    Double_t     fTsumwy2;         //Total Sum of weight*Y*Y
    Double_t     fTsumwxy;         //Total Sum of weight*X*Y

    GH2();
    GH2(const char *name,const char *title,Int_t nbinsx,Double_t xlow,Double_t xup
        ,Int_t nbinsy,Double_t ylow,Double_t yup);
    GH2(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins
        ,Int_t nbinsy,Double_t ylow,Double_t yup);
    GH2(const char *name,const char *title,Int_t nbinsx,Double_t xlow,Double_t xup
        ,Int_t nbinsy,const Double_t *ybins);
    GH2(const char *name,const char *title,Int_t nbinsx,const Double_t *xbins
        ,Int_t nbinsy,const Double_t *ybins);
    GH2(const char *name,const char *title,Int_t nbinsx,const Float_t  *xbins
        ,Int_t nbinsy,const Float_t  *ybins);

    virtual Int_t BufferFill(Double_t x, Double_t y, Double_t w);
    virtual GH1D  *DoProjection(bool onX, const char *name, Int_t firstbin, Int_t lastbin, Option_t *option) const;
    virtual TProfile *DoProfile(bool onX, const char *name, Int_t firstbin, Int_t lastbin, Option_t *option) const;
    virtual GH1D  *DoQuantiles(bool onX, const char *name, Double_t prob) const;
    virtual void  DoFitSlices(bool onX, TF1 *f1, Int_t firstbin, Int_t lastbin, 
        Int_t cut, Option_t *option, TObjArray* arr);

    Int_t    BufferFill(Double_t, Double_t) {return -2;} //may not use
    Int_t    Fill(Double_t); //MayNotUse
    Int_t    Fill(const char*, Double_t) { return Fill(0);}  //MayNotUse

  private:

    GH2(const GH2&);
    GH2& operator=(const GH2&); // Not implemented

  public:
    operator TH2*() {return (TH2*)((TH1*)this) ;  }
    
    virtual ~GH2();
    virtual Int_t    BufferEmpty(Int_t action=0);
    virtual void     Copy(TObject &hnew) const;
    virtual Int_t    Fill(Double_t x, Double_t y);
    virtual Int_t    Fill(Double_t x, Double_t y, Double_t w);
    virtual Int_t    Fill(Double_t x, const char *namey, Double_t w);
    virtual Int_t    Fill(const char *namex, Double_t y, Double_t w);
    virtual Int_t    Fill(const char *namex, const char *namey, Double_t w);
    virtual void     FillN(Int_t, const Double_t *, const Double_t *, Int_t) {;} //MayNotUse
    virtual void     FillN(Int_t ntimes, const Double_t *x, const Double_t *y, const Double_t *w, Int_t stride=1);
    virtual void     FillRandom(const char *fname, Int_t ntimes=5000);
    virtual void     FillRandom(GH1 *h, Int_t ntimes=5000);
    virtual Int_t    FindFirstBinAbove(Double_t threshold=0, Int_t axis=1) const;
    virtual Int_t    FindLastBinAbove (Double_t threshold=0, Int_t axis=1) const;
    virtual void     FitSlicesX(TF1 *f1=0,Int_t firstybin=0, Int_t lastybin=-1, 
        Int_t cut=0, Option_t *option="QNR", TObjArray* arr = 0); // *MENU*
    virtual void     FitSlicesY(TF1 *f1=0,Int_t firstxbin=0, Int_t lastxbin=-1, 
        Int_t cut=0, Option_t *option="QNR", TObjArray* arr = 0); // *MENU*
    virtual Int_t    GetBin(Int_t binx, Int_t biny, Int_t binz = 0) const;
    virtual Double_t GetBinWithContent2(Double_t c, Int_t &binx, Int_t &biny, 
        Int_t firstxbin=1, Int_t lastxbin=-1,
        Int_t firstybin=1, Int_t lastybin=-1, Double_t maxdiff=0) const;
    virtual Double_t GetBinContent(Int_t bin) const { return GH1::GetBinContent(bin); }
    virtual Double_t GetBinContent(Int_t binx, Int_t biny) const 
    { return GH1::GetBinContent( GetBin(binx, biny) ); }
    virtual Double_t GetBinContent(Int_t binx, Int_t biny, Int_t) const 
    { return GH1::GetBinContent( GetBin(binx, biny) ); }
    using GH1::GetBinErrorLow;
    using GH1::GetBinErrorUp;
    virtual Double_t GetBinErrorLow(Int_t binx, Int_t biny) { return GH1::GetBinErrorLow( GetBin(binx, biny) ); }
    virtual Double_t GetBinErrorUp(Int_t binx, Int_t biny) { return GH1::GetBinErrorUp( GetBin(binx, biny) ); }
    virtual Double_t GetCorrelationFactor(Int_t axis1=1,Int_t axis2=2) const;
    virtual Double_t GetCovariance(Int_t axis1=1,Int_t axis2=2) const;
    virtual void     GetRandom2(Double_t &x, Double_t &y);
    virtual void     GetStats(Double_t *stats) const;
    virtual Double_t Integral(Option_t *option="") const;
    //virtual Double_t Integral(Int_t, Int_t, Option_t * ="") const {return 0;}
    using GH1::Integral;
    virtual Double_t Integral(Int_t binx1, Int_t binx2, Int_t biny1, Int_t biny2, Option_t *option="") const;
    virtual Double_t Integral(Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Option_t * ="") const {return 0;}
    using GH1::IntegralAndError;
    virtual Double_t IntegralAndError(Int_t binx1, Int_t binx2, 
        Int_t biny1, Int_t biny2, Double_t & err, Option_t *option="") const;
    virtual Double_t Interpolate(Double_t x);
    virtual Double_t Interpolate(Double_t x, Double_t y);
    virtual Double_t Interpolate(Double_t x, Double_t y, Double_t z);
    virtual Double_t KolmogorovTest(const GH1 *h2, Option_t *option="") const;
    virtual Long64_t Merge(TCollection *list);
    virtual GH2     *RebinX(Int_t ngroup=2, const char *newname="");
    virtual GH2     *RebinY(Int_t ngroup=2, const char *newname="");
    virtual GH2     *Rebin2D(Int_t nxgroup=2, Int_t nygroup=2, const char *newname="");
    TProfile        *ProfileX(const char *name="_pfx", Int_t firstybin=1, Int_t lastybin=-1, Option_t *option="") const;   // *MENU*
    TProfile        *ProfileY(const char *name="_pfy", Int_t firstxbin=1, Int_t lastxbin=-1, Option_t *option="") const;   // *MENU*
    GH1D            *ProjectionX(const char *name="_px", Int_t firstybin=0, Int_t lastybin=-1, Option_t *option="+") const; // *MENU*
    GH1D            *ProjectionY(const char *name="_py", Int_t firstxbin=0, Int_t lastxbin=-1, Option_t *option="+") const; // *MENU*
    virtual void     PutStats(Double_t *stats);
    GH1D            *QuantilesX(Double_t prob = 0.5, const char * name = "_qx" ) const;
    GH1D            *QuantilesY(Double_t prob = 0.5, const char * name = "_qy" ) const;
    virtual void     Reset(Option_t *option="");
    virtual void     SetBinContent(Int_t bin, Double_t content);
    virtual void     SetBinContent(Int_t binx, Int_t biny, Double_t content) { SetBinContent(GetBin(binx, biny), content); }
    virtual void     SetBinContent(Int_t binx, Int_t biny, Int_t, Double_t content) { SetBinContent(GetBin(binx, biny), content); }
    virtual void     SetShowProjectionX(Int_t nbins=1);  // *MENU*
    virtual void     SetShowProjectionY(Int_t nbins=1);  // *MENU*
    virtual GH1     *ShowBackground(Int_t niter=20, Option_t *option="same");
    virtual Int_t    ShowPeaks(Double_t sigma=2, Option_t *option="", Double_t threshold=0.05); // *MENU*
    virtual void     Smooth(Int_t ntimes=1, Option_t *option=""); // *MENU*



 ////////////////////////////
 //  added functionality   //
 ////////////////////////////

  public:
    GH1D            *ProjectionX_BG(const char *name="_bg_px", Int_t ylowbin=0,Int_t yhighbin=-1, 
                                                               Int_t ylowbgbin=0,Int_t yhighbgbin=-1,double scale=-1,Option_t *opt="") const; // *MENU*
    GH1D            *ProjectionY_BG(const char *name="_bg_px", Int_t xlowbin=0,Int_t xhighbin=-1, 
                                                               Int_t xlowbgbin=0,Int_t xhighbgbin=-1,double scale=-1,Option_t *opt="") const; // *MENU*
    enum { kForward = 1, kBackward = -1 };
    GH1D            *SummaryProjection(GH1 *hist,int axis,int direction,bool show_empty) const;
 
    GH1  *GetNext(TObject *obj=0,int direction=GH2::kForward) const;

    TList *GetProjections() const { return &fProjections; }

    std::map<int,double> FitSummary(double low,double high,int axis=1,Option_t *opt="PhotoPeak") const;

  private:
    void    AddToProjections(GH1 *hist) const { fProjections.Add(hist); hist->SetDirectory(0); }
    mutable TList fProjections;

    //friend class GH1;

  ClassDef(GH2,1)  //2-Dim histogram base class
};



#endif
