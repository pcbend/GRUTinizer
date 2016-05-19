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


    // Long_t   GetLED()       const { return led; }
    // Long_t   GetCFD()       const { return cfd; }
    // Int_t    GetPreE()      const { return prerise_energy; }
    // Int_t    GetPostE()     const { return postrise_energy; }
    // UShort_t GetBoardID()   const { return board_id; }
    // UShort_t GetChannel()   const { return channel; }
    // Long_t   GetPrevLED()   const { return led_prev; }
    // UShort_t GetPostBegin() const { return postrise_begin_sample; }
    // UShort_t GetPostEnd() const { return postrise_end_sample; }
    // UShort_t GetPreBegin() const { return prerise_begin_sample; }
    // UShort_t GetPreEnd() const { return prerise_end_sample; }

    // // Parse flags
    // UShort_t ExternalDiscFlag() const { return ((flags & 0x100)>>8); }
    // UShort_t PeakValidFlag() const { return ((flags & 0x200)>>9); }
    // UShort_t OffsetFlag() const { return ((flags & 0x400)>>10); }
    // UShort_t SyncErrorFlag() const { return ((flags & 0x1000)>>12); }
    // UShort_t GeneralErrorFlag() const { return ((flags & 0x2000)>>13); }
    // UShort_t PileUpOnlyFlag() const { return ((flags & 0x4000)>>14); }
    // UShort_t PileUpFlag() const { return ((flags & 0x8000)>>15); }


  private:


  ClassDef(TGrandRaidenHit,1);
};


#endif
