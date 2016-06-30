#ifndef TANLEVENT_H
#define TANLEVENT_H

#include "TObject.h"

#include "TSmartBuffer.h"

#include <vector>


class TANLEvent : public TObject {
  public:
    TANLEvent(TSmartBuffer& buf);
    ~TANLEvent();

    Long_t   GetDiscriminator()       const { return discriminator; }
    Int_t    GetPreE()      const { return prerise_energy; }
    Int_t    GetPostE()     const { return postrise_energy; }
    UShort_t GetBoardID()   const { return board_id; }
    UShort_t GetChannel()   const { return channel; }
    Long_t   GetPrevDisc()   const { return disc_prev; }
    UShort_t GetPostBegin() const { return postrise_begin_sample; }
    UShort_t GetPostEnd() const { return postrise_end_sample; }
    UShort_t GetPreBegin() const { return prerise_begin_sample; }
    UShort_t GetPreEnd() const { return prerise_end_sample; }

    // Parse flags
    UShort_t ExternalDiscFlag() const { return ((flags & 0x100)>>8); }
    UShort_t PeakValidFlag() const { return ((flags & 0x200)>>9); }
    UShort_t OffsetFlag() const { return ((flags & 0x400)>>10); }
    UShort_t SyncErrorFlag() const { return ((flags & 0x1000)>>12); }
    UShort_t GeneralErrorFlag() const { return ((flags & 0x2000)>>13); }
    UShort_t PileUpOnlyFlag() const { return ((flags & 0x4000)>>14); }
    UShort_t PileUpFlag() const { return ((flags & 0x8000)>>15); }

    // TODO: add to input calibrations file for second order corrections
    //       e.g. pole zero etc.
    double   GetEnergy() const { return ((GetPostE() - GetPreE())/350.0); }
    Double_t GetCFD() const { return (Double_t)discriminator + d_cfd; }
    std::vector<Short_t>& GetTrace() { return wave_data; }

  private:

    UShort_t global_addr;
    UShort_t board_id;
    UShort_t channel;
    ULong_t discriminator;

    Double_t d_cfd;

    ULong_t disc_prev;
    UInt_t flags;
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
    std::vector<Short_t> wave_data;

  ClassDef(TANLEvent,0);
};


#endif
