#ifndef GCUTG_H_
#define GCUTG_H_

#include <TCutG.h>
#include <TClass.h>


class GCutG : public TCutG {
  public:
    GCutG() : TCutG() {  }
    GCutG(const TCutG &cutg) : TCutG(cutg) {  }
    GCutG(const char *name,Int_t n=0) : TCutG(name,n) {  }
    GCutG(const char *name,Int_t n,const Float_t *x,const Float_t *y) : TCutG(name,n,x,y) {  }
    GCutG(const char *name,Int_t n,const Double_t *x,const Double_t *y) : TCutG(name,n,x,y) {  }

    ~GCutG() { }
    
    
    virtual void Print(Option_t *opt="") const;

    int SaveTo(const char* filename="",Option_t* option="update"); // *MENU* 
    
  private:

  ClassDef(GCutG,1) 
};


#endif
