#ifndef TANLEVENT_H
#define TANLEVENT_H

#include "TObject.h"

#include "TSmartBuffer.h"

#include <vector>

#include "GValue.h"



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
    UShort_t GetFlags() const { return flags; }
    UShort_t GetBaseSample() const { return base_sample; }
    static Float_t& GetShapingTime() {
        if (std::isnan(shaping_time)) { shaping_time = GValue::Value("ShapingTime"); }
        return shaping_time;
    }


    // Parse flags
    UShort_t ExternalDiscFlag() const { return ((flags & 0x100)>>8); }
    UShort_t PeakValidFlag() const { return ((flags & 0x200)>>9); }
    UShort_t OffsetFlag() const { return ((flags & 0x400)>>10); }
    UShort_t SyncErrorFlag() const { return ((flags & 0x1000)>>12); }
    UShort_t GeneralErrorFlag() const { return ((flags & 0x2000)>>13); }
    UShort_t PileUpOnlyFlag() const { return ((flags & 0x4000)>>14); }
    UShort_t PileUpFlag() const { return ((flags & 0x8000)>>15); }
    static UShort_t WriteFlag(const UShort_t& flags) { return ((flags & 0x20)>>5); }
    static UShort_t VetoFlag(const UShort_t& flags) { return ((flags & 0x40)>>6); }
    static UShort_t TSMatchFlag(const UShort_t& flags) { return ((flags & 0x80)>>7); }
    static UShort_t ExternalDiscFlag(const UShort_t& flags) { return ((flags & 0x100)>>8); }
    static UShort_t PeakValidFlag(const UShort_t& flags) { return ((flags & 0x200)>>9); }
    static UShort_t OffsetFlag(const UShort_t& flags) { return ((flags & 0x400)>>10); }
    static UShort_t CFDValidFlag(const UShort_t& flags) { return ((flags & 0x800)>>11); }
    static UShort_t SyncErrorFlag(const UShort_t& flags) { return ((flags & 0x1000)>>12); }
    static UShort_t GeneralErrorFlag(const UShort_t& flags) { return ((flags & 0x2000)>>13); }
    static UShort_t PileUpOnlyFlag(const UShort_t& flags) { return ((flags & 0x4000)>>14); }
    static UShort_t PileUpFlag(const UShort_t& flags) { return ((flags & 0x8000)>>15); }


    double   GetEnergy() const { return ((GetPostE() - GetPreE())/GetShapingTime()); }
    Double_t GetCFD() const { return (Double_t)discriminator + d_cfd; }
    std::vector<Short_t>& GetTrace() { return wave_data; }

  private:
    static Float_t shaping_time;

    UShort_t global_addr;
    UShort_t board_id;
    UShort_t channel;
    ULong_t discriminator;

    Double_t d_cfd;

    ULong_t disc_prev;
    UShort_t flags;
    //UInt_t sampled_baseline;
    UInt_t prerise_energy;
    UInt_t postrise_energy;
    //ULong_t peak_timestamp;
    UShort_t postrise_end_sample;
    UShort_t postrise_begin_sample;
    UShort_t prerise_end_sample;
    UShort_t prerise_begin_sample;
    UShort_t base_sample;
    //UShort_t peak_sample;
    std::vector<Short_t> wave_data;

  ClassDef(TANLEvent,0);
};


#endif
