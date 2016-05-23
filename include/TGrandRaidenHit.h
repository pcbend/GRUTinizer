#ifndef TGRANDRAIDENHIT_H
#define TGRANDRAIDENHIT_H

#include "TDetector.h"
#include "TDetectorHit.h"

class TGrandRaidenHit : public TDetectorHit {
  public:
    TGrandRaidenHit();
    ~TGrandRaidenHit();

    virtual void Copy(TObject& obj) const;
    //virtual void Compare(TObject &obj) const;
    virtual void Print(Option_t *opt = "") const;
    virtual void Clear(Option_t *opt = "");

    void     BuildFrom(TSmartBuffer& buf);


    Double_t* GetADC() { return &ADC[0]; }
    Double_t GetADC(const Int_t& i) const { return ADC[i]; }
    //void SetADC(Int_t chan, const Double_t& val) { ADC[chan] = val; }


  private:
    Double_t ADC[4];


  ClassDef(TGrandRaidenHit,1);
};


#endif
