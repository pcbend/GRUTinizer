#ifndef GCUTG_H_
#define GCUTG_H_

#include <TCutG.h>
#include <TClass.h>
#include <TString.h>

//class TMethodCall;


class GCutG : public TCutG {
  public:
    GCutG() : TCutG() { }
    GCutG(const TCutG &cutg) : TCutG(cutg) { }
    GCutG(const char *name,Int_t n=0) : TCutG(name,n) { }
    GCutG(const char *name,Int_t n,const Float_t *x,const Float_t *y) : TCutG(name,n,x,y) { }
    GCutG(const char *name,Int_t n,const Double_t *x,const Double_t *y) : TCutG(name,n,x,y) { } 
    ~GCutG() { }
    
    virtual void Copy(const TObject&);
    virtual void Print(Option_t *opt="") const;
    virtual void Clear(Option_t *opt="");

    int SaveTo(const char *cutname="",const char* filename="",Option_t* option="update"); // *MENU* 

    void SetGateMethod(const char* xclass,const char* xmethod,
                       const char* yclass,const char* ymethod);
    
    Int_t IsInside(Double_t x,Double_t y) const; //  { return TCutG::IsInside(x,y); }
    Int_t Test(Double_t x,Double_t y); 

    //Int_t IsInside(TObject *objx,TObject *objy=0);

    enum EGCutBits { 
      kCutChecked = BIT(14),   // check whether the IsInside method has been called.
      kCutPassed  = BIT(15)    // 0:IsInside returned zero, 1:IsInside return non-zero
    };
    void Reset() { SetBit(kCutChecked,0); SetBit(kCutPassed,0); }

    bool Checked() const { return TestBit(kCutChecked); }
    bool Passed() const { return TestBit(kCutChecked) && TestBit(kCutPassed); }

  
  private:

    TString fXGateClass;  
    TString fYGateClass;  
    TString fXGateMethod; 
    TString fYGateMethod; 

    TMethodCall *fXMethod; //!
    TMethodCall *fYMethod; //!



  ClassDef(GCutG,1) 
};


#endif
