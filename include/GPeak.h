#ifndef GPEAK_H
#define GPEAK_h

#include <TF1.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>

#include <string>
#include <algorithm>

class GPeak : public TF1 {
  public:
    TPeak(Double_t cent,Double_t xlow,Double_t xhigh,Option_t *opt="gsc");
    TPeak(const TPeak&);
    TPeak();
    virtual ~TPeak();

  private:
    //double fArea;
    //double fDArea;
    //double fChi2;
    //double fNdf;

    TF1 *background;

  ClassDef(GPeak,1)
}


#endif

