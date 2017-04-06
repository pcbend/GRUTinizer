#ifndef __GGRAPH2D_H__
#define __GGRAPH2D_H__

#include <TGraph2DErrors.h>

class GGraph;

class GGraph2D : public TGraph2DErrors {
  public:
    GGraph2D() : TGraph2DErrors() { }
    GGraph2D(int n,double *x,double *y,double *z,double *dz=0,double *dy=0,double *dx=0)
      : TGraph2DErrors(n,x,y,z,dx,dy,dx) { }
    GGraph2D(const char *name,const char *title,
             int n,double *x,double *y,double *z,double *dz=0,double *dy=0,double *dx=0)
      : TGraph2DErrors(n,x,y,z,dx,dy,dx) { this->SetNameTitle(name,title); }

    GGraph2D(const char *filename,Option_t *opt="");


    GGraph *ProjectionX(double low, double high,Option_t *opt="draw") const;
    GGraph *ProjectionY(double low, double high,Option_t *opt="draw") const;

  private: 


  ClassDef(GGraph2D,1)
};

#endif

