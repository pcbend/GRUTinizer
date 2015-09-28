#ifndef GPEAK_H
#define GPEAK_h

#include <TF1.h>

#include <string>
#include <algorithm>

class GPeak : public TF1 {
  public:
    GPeak(Double_t cent,Double_t xlow,Double_t xhigh,Option_t *opt="gsc");
    GPeak(const GPeak&);
    GPeak();
    virtual ~GPeak();

    void Copy(TObject&) const;
    void Print(Option_t *opt="") const;
    void Clear(Option_t *opt="");

    void InitNames();
    bool InitParams(TH1*);
    bool Fit(TH1*,Option_t *opt="");
    void DrawResiduals(TH1*) const;

    TF1 *Background() { return &background; }

  private:
    double farea;
    double fd_area;
    double fchi2;
    double fNdf;

    Bool_t IsInitialized() const { return init_flag; }
    void SetInitialized(Bool_t flag = true) {init_flag = flag;}
    bool init_flag;

    TF1 background;

  ClassDef(GPeak,1)
};


#endif

