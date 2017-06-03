#ifndef __GH1_H__
#define __GH1_H__

#include <TH1.h>
#include <TRef.h>

class GH1D;



class GH1 : public TH1 {
  protected:  
    GH1() : TH1(), fProjectionAxis(kNoAxis) { }
    GH1(const char *name,const char *title,int nbinsx,double xlow,double xhigh) :
       TH1(name,title,nbinsx,xlow,xhigh),fProjectionAxis(kNoAxis) { }
    GH1(const char *name,const char *title,int nbinsx,const double *xbins) :
       TH1(name,title,nbinsx,xbins),fProjectionAxis(kNoAxis) { }
    GH1(const char *name,const char *title,int nbinsx,const float *xbins) :
       TH1(name,title,nbinsx,xbins),fProjectionAxis(kNoAxis) { }

  public:
    virtual ~GH1() { } 

    enum { kIsSummary = BIT(20) };

    TH1 *GetNext()     { return 0; }
    TH1 *GetPrevious() { return 0; }
    
    //GH1D *Project(double low=sqrt(-1),double high=(sqrt(-1));    
    //GH1D *Project(double low,double high,double bg_low,double bg_high,double scale=-1)    { return 0; }
    GH1D *Project(int low=0,int high=-1);    
    GH1D *Project(int low,int high,int bg_low,int bg_high,double scale=-1);  
    
    TObject *GetHistParent()    const          { return fParent.GetObject(); } 
    void     SetParent(TObject *obj) { fParent = obj; }
    
    void SetProjectionAxis(int axis) { fProjectionAxis = axis; }
    int  GetProjectionAxis() const { return fProjectionAxis; }

    virtual bool IsSummary() const          { return TestBit(kIsSummary); }

    double FitEval(double *,double*);
    TF1  *ConstructTF1() const; 
    bool WriteDatFile(const char *outFile);

    void Draw(Option_t *opt="");

    void Calibrate(double offset,double gain,int axis=1); // *MENU*



  private:
    TRef fParent;
    int  fProjectionAxis; //!

  protected:  
    virtual void SetSummary(bool flag=true) { SetBit(kIsSummary,flag);    } 

    friend class GH2;
    
  ClassDef(GH1,1)
};

#endif
