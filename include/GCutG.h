#ifndef GCUTG_H_
#define GCUTG_H_

#include <TCutG.h>
#include <TClass.h>
#include <TString.h>

class TRuntimeObjects;

class GCutG : public TCutG {
  public:
    GCutG() : TCutG() { Init(); }
    GCutG(const TCutG &cutg) : TCutG(cutg) { Init(); }
    GCutG(const char *name,Int_t n=0) : TCutG(name,n) { Init(); }
    GCutG(const char *name,double low,double high) { this->SetName(name); Init(low,high);  }
    GCutG(const char *name,Int_t n,const Float_t *x,const Float_t *y) : TCutG(name,n,x,y) { Init(); }
    GCutG(const char *name,Int_t n,const Double_t *x,const Double_t *y) : TCutG(name,n,x,y) { Init(); } 
    ~GCutG() { }
   

    virtual void Print(Option_t *opt="") const;
    int SaveTo(const char *cutname="",const char* filename="",Option_t* option="update"); // *MENU* 

    void SetGateMethod(const char* xclass,const char* xmethod,
                       const char* yclass="",const char* ymethod="",
                       const char* xparam="",const char* yparam="");
    Int_t IsInside(TObject *objx,TObject *objy=0) const;
    Int_t IsInside(Double_t x,Double_t y=0) const; // { return TCutG::IsInside(x,y); }
    Int_t IsInside(const TRuntimeObjects*) const;   


    void Set1D(double low,double high) { RemoveAllPoints(); fXlow=low;fXhigh=high; } 
    double GetLow()    const { return fXlow;  }
    double GetHigh()   const { return fXhigh; }
    Int_t  Dimension() const { if(GetN()) return 2; 
                               else if(fXlow==fXlow&&fXhigh==fXhigh) return 1; 
                               else return 0; } 

  private:
    void Init(double low=sqrt(-1.),double high=sqrt(-1.));
    void RemoveAllPoints();

    TString fXGateClass;  
    TString fYGateClass;  
    TString fXGateMethod; 
    TString fYGateMethod; 
    TString fXGateParam; 
    TString fYGateParam; 
 
    double  fXlow;
    double  fXhigh;

  ClassDef(GCutG,2) 
};


#endif
