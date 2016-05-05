#ifndef TINVERSEMAP_H_
#define TINVERSEMAP_H_

#include<map>

#include <TNamed.h>

class TS800;

class TInverseMap : public TNamed {

  public:
    static TInverseMap *Get(const char *filename="");
    virtual ~TInverseMap();

    virtual void Print(Option_t *opt="") const; //{ ; }
    virtual void Clear(Option_t *opt="")       { ; }

    float Ata(int degree, double xfp, double afp, double yfp, double bfp) const;
    float Ata(int,const TS800*);
    float Bta(int degree, double xfp, double afp, double yfp, double bfp) const;
    float Bta(int,const TS800*);
    float Yta(int degree, double xfp, double afp, double yfp, double bfp) const;
    float Yta(int,const TS800*);
    float Dta(int degree, double xfp, double afp, double yfp, double bfp) const;
    float Dta(int,const TS800*);

    float MapCalc(int,int,float*) const;

    int Size() { return fMap.size(); }

    

  private:
    TInverseMap(const char* filename);
    static TInverseMap *fInverseMap;

    bool ReadMapFile(const char *filename);  

    struct InvMapRow{
      double coefficient;
      int    order;
      int    exp[6];
    };
    //data cleared on reset; i.e. Read new inverse map.
    std::map<int,std::vector<InvMapRow> > fMap;
    float  fBrho;
    int    fMass;
    int    fCharge;
    std::string info;

  ClassDef(TInverseMap,0)
};
#endif
