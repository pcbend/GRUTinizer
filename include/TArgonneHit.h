#ifndef TARGONNEHIT_H
#define TARGONNEHIT_H

#include "TDetector.h"
#include "TDetectorHit.h"



#define MAXTRACE 1024

class TArgonneHit : public TDetectorHit {
  public:
    TArgonneHit();
    ~TArgonneHit();

    virtual void Copy(TObject& obj) const;
    //virtual void Compare(TObject &obj) const;
    virtual void Print(Option_t *opt = "") const;
    virtual void Clear(Option_t *opt = "");

  void     BuildFrom(TSmartBuffer buf);

    Int_t    GetChannel()   const { return (board_id & 0x000f);     }
    Int_t    GetVME()       const { return (board_id & 0x0030)>>4;    }
    Int_t    GetCrystal()   const { return (board_id & 0x00c0)>>6;    }
    Int_t    GetHole()      const { return (board_id & 0x1f00)>>8;        }
    Int_t    GetSegmentId() const { return GetVME()*10 + GetChannel(); }
    Int_t    GetCrystalId() const { return GetHole()*4 + GetCrystal(); }

    Long_t   GetLed()       const { return led; }
    Long_t   GetCfd()       const { return cfd; }




  private:

    UShort_t global_addr;
    UShort_t board_id;
    UShort_t channel;
    ULong_t led;
    ULong_t cfd;


    ULong_t led_prev;
    // store flags as one uint
    UInt_t flags;
    //UShort_t external_disc;
    //UShort_t peak_valid;
    //UShort_t offset;
    //UShort_t sync_error;
    //UShort_t general_error;
    //UShort_t pile_up_only;
    //UShort_t pile_up;

    //UInt_t sampled_baseline;
    UInt_t prerise_energy;
    UInt_t postrise_energy;
    //ULong_t peak_timestamp;
    UShort_t postrise_end_sample;
    UShort_t postrise_begin_sample;
    UShort_t prerise_end_sample;
    UShort_t prerise_begin_sample;
    //UShort_t base_sample;
    //UShort_t peak_sample;

  ClassDef(TArgonneHit,1);
};


#endif
