#ifndef __GGRAPH_H__
#define __GGRAPH_H__

#include <TGraphErrors.h>
#include <TRef.h>

class GGraph : public TGraphErrors {
  public:
    GGraph() : TGraphErrors() { }
    GGraph(int n) : TGraphErrors(n) { }
    GGraph(int n,float *x,float *y,float *dy=0,float *dx=0)
      : TGraphErrors(n,x,y,dx,dy) { }
    GGraph(int n,double *x,double *y,double *dy=0,double *dx=0)
      : TGraphErrors(n,x,y,dx,dy) { }
    GGraph(const GGraph &gr)       : TGraphErrors((TGraphErrors&)gr) { }
    GGraph(const TGraphErrors &gr) : TGraphErrors(gr)                { }
    GGraph(const TH1 *h) : TGraphErrors(h) { }   
    GGraph(const char *filename,const char *format="%lg %lg %lg %lg",Option_t *opt="")
      : TGraphErrors(filename,format,opt) { }

    virtual ~GGraph() { } 

  private:

    TRef parent;

  ClassDef(GGraph,1)
};

#endif

