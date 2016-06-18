#ifndef TGRANDRAIDENHIT_H
#define TGRANDRAIDENHIT_H

#include "TDetector.h"
#include "TDetectorHit.h"

class TGrandRaidenHit : public TDetectorHit {
  public:
    TGrandRaidenHit();
    TGrandRaidenHit(void* ptr);
    ~TGrandRaidenHit();

    virtual void Copy(TObject& obj) const;
    //virtual void Compare(TObject &obj) const;
    virtual void Print(Option_t *opt = "") const;
    virtual void Clear(Option_t *opt = "");

    void     BuildFrom(TSmartBuffer& buf);


    Double_t* GetADC() { return &ADC[0]; }
    Double_t GetADC(const Int_t& i) const { return ADC[i]; }
    const Double_t& GetRF() { return RF; }
    //void SetADC(Int_t chan, const Double_t& val) { ADC[chan] = val; }
    Long_t Timestamp;

  private:
    Double_t ADC[4];
    Double_t RF;



    void* fDataPtr; //! do not save
    ClassDef(TGrandRaidenHit,1);
};


#endif
