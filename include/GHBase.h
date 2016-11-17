#ifndef _GHBASE_H_
#define _GHBASE_H_

#include <cstdio>

#include <TRef.h>

class TObject;
class TH1;
class GH1D;
class GPeak;


class GHBase {
   public:
    GHBase(TH1 *h):parent(0),hist((TObject*)h) { } 
    virtual ~GHBase() { }

    TObject* GetParent()     const { return parent.GetObject(); }
    int  GetProjectionAxis() const { return projection_axis;    }
   
    void Clear(Option_t *opt="") { parent=NULL;hist=NULL;projection_axis=-1; }
    void Copy(TObject &obj) const { }

    void SetParent(TObject* obj)     { parent = obj; }
    void SetProjectionAxis(int axis) { projection_axis = axis; }

    bool WriteDatFile(const char *outFile);
    
    GH1D* Flip(int bins=-1);
    GH1D* Project(double bin_low, double bin_high) const;
    GH1D* Project(double bin_low, double bin_high,GH1D *bg) const;
    GH1D* Project(double bin_low, double bin_high,double bg_bin_low,double bg_bin_high,double scale=-1.0) const;
    
    GH1D* GetPrevious(bool DrawEmpty=false) const;
    GH1D* GetNext(bool DrawEmpty=false)     const;

    GPeak* DoPhotoPeakFit(double xlow,double xhigh,Option_t *opt="");       // *MENU*
    GPeak* DoPhotoPeakFitNormBG(double xlow,double xhigh,Option_t *opt=""); // *MENU*

  private:
    TRef parent;
    TRef hist;
    TH1 *GetHist() const { return (TH1*)hist.GetObject(); }
    int  projection_axis;



  ClassDef(GHBase,1)
};


#endif
