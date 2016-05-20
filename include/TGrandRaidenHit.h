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


    Long_t test;
    // Long_t   GetLED()       const { return led; }
    // Long_t   GetCFD()       const { return cfd; }
    // Int_t    GetPreE()      const { return prerise_energy; }
    // Int_t    GetPostE()     const { return postrise_energy; }
    // UShort_t GetBoardID()   const { return board_id; }
    // UShort_t GetChannel()   const { return channel; }
    // Long_t   GetPrevLED()   const { return led_prev; }
    // UShort_t GetPostBegin() const { return postrise_begin_sample; }
    // UShort_t GetPostEnd()   const { return postrise_end_sample; }
    // UShort_t GetPreBegin()  const { return prerise_begin_sample; }
    // UShort_t GetPreEnd()    const { return prerise_end_sample; }





  private:


  ClassDef(TGrandRaidenHit,1);
};


#endif
