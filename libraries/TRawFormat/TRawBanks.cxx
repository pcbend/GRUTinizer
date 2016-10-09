#include "TRawEvent.h"

#include <ctime>

std::ostream& operator<<(std::ostream& os, const TRawEvent::GEBInteractionPoint &fSeg) {
   std::streamsize ss = std::cout.precision();
   return os << "HPGeSegment[" << std::setw(3) << fSeg.seg << "]\t("
             << std::setprecision(2)  << fSeg.x << ", " << fSeg.y << ", " << fSeg.z << ")\t"
             << fSeg.seg_ener << " / " << fSeg.e << std::setprecision(ss) << std::endl;
}



std::ostream& operator<<(std::ostream& os, const TRawEvent::GEBBankType1 &bank) {
   std::streamsize ss = std::cout.precision();
   //os << "************************" << std::endl;
   os << "type        = "<< std::setw(8) << std::hex << bank.type << std::dec << std::endl;
   os << "crystal_id  = "<< std::setw(8) << bank.crystal_id << std::endl;
   os << "num of pts  = "<< std::setw(8) << bank.num << std::endl;
   os << "CC energy   = "<< std::setw(8) << bank.tot_e << std::endl;
   os << "raw CC charge x4: " << std::endl;
   os << "\t[0]: " << std::setw(10) << bank.core_e[0] << std::endl;
   os << "\t[1]: " << std::setw(10) << bank.core_e[1] << std::endl;
   os << "\t[2]: " << std::setw(10) << bank.core_e[2] << std::endl;
   os << "\t[3]: " << std::setw(10) << bank.core_e[3] << std::endl;
   os << "timestamp   = " << bank.timestamp << std::endl;
   os << std::setprecision(2);
   os << "trig_time   = "<< std::setw(8) << bank.trig_time << std::endl;
   os << "t0          = "<< std::setw(8) << bank.t0 << std::endl;
   os << "cfd         = "<< std::setw(8) << bank.cfd << std::endl;
   os << "chisq       = "<< std::setw(8) << bank.chisq << std::endl;
   os << "norm_chisq  = "<< std::setw(8) << bank.norm_chisq << std::endl;
   os << "baseline    = "<< std::setw(8) << bank.baseline << std::endl;
   os << "bl prestep  = "<< std::setw(8) << bank.prestep  << std::endl;
   os << "bl poststep = "<< std::setw(8) << bank.poststep << std::endl;
   os << std::setprecision(ss);
   os << "pad (error) = "<< std::setw(8) << bank.pad << std::endl;
   for(int x=0;x<bank.num;x++)
      os << "\t" << bank.intpts[x];
   os << "************************" << std::endl;
   return os;
}

std::ostream& operator<<(std::ostream& os, const TRawEvent::G4S800 &s800pack){
  os << " ATA : " << s800pack.GetATA() << std::endl;
  os << " BTA : " << s800pack.GetBTA() << std::endl;
  os << " DTA : " << s800pack.GetDTA() << std::endl;
  os << " YTA : " << s800pack.GetYTA() << std::endl;

  return os;
}

std::ostream& operator<<(std::ostream& os, const TRawEvent::G4SimPacket &packet){
  os << " Type : " << std::hex << packet.head.GetType() << std::endl;
  os << " Num  : " << packet.head.GetNum() << std::endl;
  os << " Full : " << packet.head.GetFull() << std::endl;

  for(int i = 0; i< packet.head.GetNum(); i++){
      os << " --- Gamma Summary ---" << std::endl;
      os << " > Gamma En   : " << packet.data[i].GetEn() << std::endl;
      os << " > Gamma X    : " << packet.data[i].GetX() << std::endl;
      os << " > Gamma Y    : " << packet.data[i].GetY() << std::endl;
      os << " > Gamma Z    : " << packet.data[i].GetZ() << std::endl;
      os << " > Gamma Beta : " << packet.data[i].GetBeta() << std::endl;
      os << " > Gamma Phi  : " << packet.data[i].GetPhi() << std::endl;
      os << " > Gamma Theta: " << packet.data[i].GetTheta() << std::endl;
  }
  return os;
}


UShort_t TRawEvent:: SwapShort(UShort_t datum) {
        UShort_t temp = 0;
        temp = (datum&0x00ff);
        return (temp<<8) + (datum>>8);
}
UInt_t TRawEvent::SwapInt(UInt_t datum) {
        UInt_t t1 = 0, t2 = 0, t3 = 0;
        t1 = (datum&0x000000ff);
        t2 = (datum&0x0000ff00);
        t3 = (datum&0x00ff0000);
        return (t1<<24) + (t2<<8) + (t3>>8) + (datum>>24);
}

void TRawEvent::SwapMode3Head(TRawEvent::GEBMode3Head &head) {
  head.lengthGA = SwapShort(head.lengthGA);
  head.board_id = SwapShort(head.board_id);
}

std::ostream& operator<<(std::ostream& os, const TRawEvent::GEBMode3Head &head) {
  os << "a1       = " << std::hex << head.a1 << std::endl;
  os << "a2       = " << std::hex << head.a2 << std::endl;
  os << "board_id = " << std::hex << head.board_id << std::endl;
  os << "lengthGA = " << std::hex << head.lengthGA << std::endl;
  return os;
}

Int_t TRawEvent::GEBMode3Head::GetLength()     const { return  lengthGA&0x07ff; }
Int_t TRawEvent::GEBMode3Head::GetChannel()    const { return  board_id&0x000f; }
Int_t TRawEvent::GEBMode3Head::GetVME()        const { return (board_id&0x0030)>>4; }
Int_t TRawEvent::GEBMode3Head::GetCrystal()    const { return (board_id&0x00c0)>>6; }
Int_t TRawEvent::GEBMode3Head::GetHole()       const { return (board_id&0x1f00)>>8 ; }
Int_t TRawEvent::GEBMode3Head::GetSegmentId()  const { return GetVME()*10+GetChannel(); }
Int_t TRawEvent::GEBMode3Head::GetCrystalId()  const { return GetHole()*4+GetCrystal(); }

void TRawEvent::SwapMode3Data(TRawEvent::GEBMode3Data &data) {
  data.led_middle   = SwapShort(data.led_middle)   ;
  data.led_low      = SwapShort(data.led_low)      ;
  data.energy_low   = SwapShort(data.energy_low)   ;
  data.led_high     = SwapShort(data.led_high)     ;
  data.cfd_low      = SwapShort(data.cfd_low)      ;
  data.energy_high  = SwapShort(data.energy_high)  ;
  data.cfd_high     = SwapShort(data.cfd_high)     ;
  data.cfd_middle   = SwapShort(data.cfd_middle)   ;
  data.cfd_pt1_high = SwapShort(data.cfd_pt1_high) ;
  data.cfd_pt1_low  = SwapShort(data.cfd_pt1_low)  ;
  data.cfd_pt2_high = SwapShort(data.cfd_pt2_high) ;
  data.cfd_pt2_low  = SwapShort(data.cfd_pt2_low ) ;
}

std::ostream& operator<<(std ::ostream& os, const TRawEvent::GEBMode3Data &data) {
  os << "Led                 : " << data.GetLed() << "\n"
     << "Cfd                 : " << data.GetCfd() << "\n"
     //<< "Energy              : " << std::hex << "0x" << data.GetEnergy() << std::dec << "\n"
     << "Energy Low          : " << data.energy_low << "\n"
     << "Energy High         : " << data.energy_high << "\n"
     << "Energy Low          : " << std::hex << "0x" << data.energy_low << std::dec << "\n"
     << "Energy High         : " << std::hex << "0x" << data.energy_high << std::dec << "\n";
  return os;
}

UShort_t TRawEvent::GEBArgonneHead::GetGA() const { return  ((GA_packetlength & 0xf800) >> 11); }
UShort_t TRawEvent::GEBArgonneHead::GetLength() const { return (GA_packetlength & 0x7ff); }
UShort_t TRawEvent::GEBArgonneHead::GetBoardID() const { return ((ud_channel & 0xfff0) >> 4); }
UShort_t TRawEvent::GEBArgonneHead::GetChannel() const { return (ud_channel & 0xf); }
UInt_t   TRawEvent::GEBArgonneHead::GetHeaderType() const { return (hdrlength_evttype_hdrtype & 0xf); }
UShort_t TRawEvent::GEBArgonneHead::GetEventType() const { return ((hdrlength_evttype_hdrtype & 0x380) >> 7); }
UShort_t TRawEvent::GEBArgonneHead::GetHeaderLength() const { return ((hdrlength_evttype_hdrtype & 0xfc00) >> 10); }
ULong_t  TRawEvent::GEBArgonneHead::GetDisc() const { return (((ULong_t)disc_high) << 32) + ((ULong_t)disc_low); }

ULong_t  TRawEvent::GEBArgonneLEDv11::GetPreviousLED() const { return (((ULong_t)led_high_prev) << 16) + ((ULong_t)led_low_prev); }
UInt_t   TRawEvent::GEBArgonneLEDv11::GetBaseline() const { return ((sampled_baseline & 0x00FFFFFF) >> 0); }
UInt_t   TRawEvent::GEBArgonneLEDv11::GetPreRiseE() const { return (postrise_sum_low_prerise_sum & 0xffffff); }
UInt_t   TRawEvent::GEBArgonneLEDv11::GetPostRiseE() const { return ((postrise_sum_low_prerise_sum & 0xff000000)>>24) + (((UInt_t)postrise_sum_high) << 8); }
ULong_t  TRawEvent::GEBArgonneLEDv11::GetPeakTimestamp() const { return ((ULong_t)timestamp_peak_low) + (((ULong_t)timestamp_peak_high)<<16); }
UShort_t TRawEvent::GEBArgonneLEDv11::GetPostRiseSampleBegin() const { return (postrise_begin_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv11::GetPostRiseSampleEnd() const { return (postrise_end_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv11::GetPreRiseSampleBegin() const { return (prerise_begin_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv11::GetPreRiseSampleEnd() const { return (prerise_end_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv11::GetBaseSample() const { return (base_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv11::GetPeakSample() const { return (peak_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv11::ExternalDiscFlag() const { return ((flags & 0x100)>>8); }
UShort_t TRawEvent::GEBArgonneLEDv11::PeakValidFlag() const { return ((flags & 0x200)>>9); }
UShort_t TRawEvent::GEBArgonneLEDv11::OffsetFlag() const { return ((flags & 0x400)>>10); }
UShort_t TRawEvent::GEBArgonneLEDv11::SyncErrorFlag() const { return ((flags & 0x1000)>>12); }
UShort_t TRawEvent::GEBArgonneLEDv11::GeneralErrorFlag() const { return ((flags & 0x2000)>>13); }
UShort_t TRawEvent::GEBArgonneLEDv11::PileUpOnlyFlag() const { return ((flags & 0x4000)>>14); }
UShort_t TRawEvent::GEBArgonneLEDv11::PileUpFlag() const { return ((flags & 0x8000)>>15); }

ULong_t  TRawEvent::GEBArgonneLEDv18::GetPreviousLED() const { return (((ULong_t)led_high_prev) << 16) + ((ULong_t)led_low_prev); }
UInt_t   TRawEvent::GEBArgonneLEDv18::GetBaseline() const { return ((sampled_baseline & 0x00FFFFFF) >> 0); }
UInt_t   TRawEvent::GEBArgonneLEDv18::GetPreRiseE() const { return (postrise_sum_low_prerise_sum & 0xffffff); }
UInt_t   TRawEvent::GEBArgonneLEDv18::GetPostRiseE() const { return ((postrise_sum_low_prerise_sum & 0xff000000)>>24) + (((UInt_t)postrise_sum_high) << 8); }
ULong_t  TRawEvent::GEBArgonneLEDv18::GetTrigTimestamp() const { return ((ULong_t)timestamp_trigger_low) /*+ (((ULong_t)timestamp_trigger_high)<<16)*/; } // not fully implemented
UShort_t TRawEvent::GEBArgonneLEDv18::GetLastPostRiseEnterSample() const { return (last_postrise_enter_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv18::GetLastPostRiseLeaveSample() const { return (last_postrise_leave_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv18::GetPostRiseLeaveSample() const { return (postrise_leave_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv18::GetPreRiseEnterSample() const { return (prerise_enter_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv18::GetPreRiseLeaveSample() const { return (prerise_leave_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv18::GetBaseSample() const { return (base_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv18::GetPeakSample() const { return (peak_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneLEDv18::WriteFlag() const { return ((flags & 0x20)>>5); }
UShort_t TRawEvent::GEBArgonneLEDv18::VetoFlag() const { return ((flags & 0x40)>>6); }
UShort_t TRawEvent::GEBArgonneLEDv18::ExternalDiscFlag() const { return ((flags & 0x100)>>8); }
UShort_t TRawEvent::GEBArgonneLEDv18::PeakValidFlag() const { return ((flags & 0x200)>>9); }
UShort_t TRawEvent::GEBArgonneLEDv18::OffsetFlag() const { return ((flags & 0x400)>>10); }
UShort_t TRawEvent::GEBArgonneLEDv18::SyncErrorFlag() const { return ((flags & 0x1000)>>12); }
UShort_t TRawEvent::GEBArgonneLEDv18::GeneralErrorFlag() const { return ((flags & 0x2000)>>13); }
UShort_t TRawEvent::GEBArgonneLEDv18::PileUpOnlyFlag() const { return ((flags & 0x4000)>>14); }
UShort_t TRawEvent::GEBArgonneLEDv18::PileUpFlag() const { return ((flags & 0x8000)>>15); }


//ULong_t  TRawEvent::GEBArgonneCFDv18::GetPreviousCFD() const { return (((ULong_t)cfd_high_prev) << 16) + ((ULong_t)cfd_low_prev); }
ULong_t  TRawEvent::GEBArgonneCFDv18::GetPrevCFD(const GEBArgonneHead* header) const {
  if (TSMatchFlag() == 1) {
    ULong_t current_cfd= header->GetDisc();
    return ((current_cfd & 0x00ffffc0000000) + (((ULong_t)(cfd_mid_prev & 0x3fff)) << 16) + (ULong_t)cfd_low_prev);
  } else {
    return 0xffffffffffffffff;
  }
}
#define STR(x) #x << ": " << x
Double_t TRawEvent::GEBArgonneCFDv18::GetCFD()  const {
  // std::cout << STR(CFDValidFlag()) << std::endl;
  // std::cout << STR(cfd_sample0) << std::endl;
  // std::cout << STR(cfd_sample1) << std::endl;
  // std::cout << STR(cfd_sample2) << std::endl;
  // std::cout << STR(GetCFD0()) << std::endl;
  // std::cout << STR(GetCFD1()) << std::endl;
  // std::cout << STR(GetCFD2()) << std::endl;
  // std::cout << std::endl;
  // std::cin.get();
  return 0. - GetCFD2()*2./(GetCFD2()-GetCFD0());
}
#undef STR

Short_t  TRawEvent::GetSigned14BitFromUShort(UShort_t ushort) {
  return ((Short_t)((ushort & 0x3fff) << 2))/4;
}

Short_t  TRawEvent::GEBArgonneCFDv18::GetCFD0() const { return GetSigned14BitFromUShort(cfd_sample0); }
Short_t  TRawEvent::GEBArgonneCFDv18::GetCFD1() const { return GetSigned14BitFromUShort(cfd_sample1); }
Short_t  TRawEvent::GEBArgonneCFDv18::GetCFD2() const { return GetSigned14BitFromUShort(cfd_sample2); }

UInt_t   TRawEvent::GEBArgonneCFDv18::GetBaseline() const { return ((sampled_baseline & 0x00FFFFFF) >> 0); }
UInt_t   TRawEvent::GEBArgonneCFDv18::GetPreRiseE() const { return (postrise_sum_low_prerise_sum & 0xffffff); }
UInt_t   TRawEvent::GEBArgonneCFDv18::GetPostRiseE() const { return ((postrise_sum_low_prerise_sum & 0xff000000)>>24) + (((UInt_t)postrise_sum_high) << 8); }
ULong_t  TRawEvent::GEBArgonneCFDv18::GetTrigTimestamp() const { return ((ULong_t)timestamp_trigger_low) /*+ (((ULong_t)timestamp_trigger_high)<<16)*/; } // not fully implemented
UShort_t TRawEvent::GEBArgonneCFDv18::GetLastPostRiseEnterSample() const { return (last_postrise_enter_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneCFDv18::GetPostRiseSampleBegin() const { return (postrise_begin_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneCFDv18::GetPostRiseSampleEnd() const { return (postrise_end_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneCFDv18::GetPreRiseSampleBegin() const { return (prerise_begin_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneCFDv18::GetPreRiseSampleEnd() const { return (prerise_end_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneCFDv18::GetBaseSample() const { return (base_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneCFDv18::GetPeakSample() const { return (peak_sample & 0x3fff); }
UShort_t TRawEvent::GEBArgonneCFDv18::WriteFlag() const { return ((flags & 0x20)>>5); }
UShort_t TRawEvent::GEBArgonneCFDv18::VetoFlag() const { return ((flags & 0x40)>>6); }
UShort_t TRawEvent::GEBArgonneCFDv18::TSMatchFlag() const { return ((flags & 0x80)>>7); }
UShort_t TRawEvent::GEBArgonneCFDv18::ExternalDiscFlag() const { return ((flags & 0x100)>>8); }
UShort_t TRawEvent::GEBArgonneCFDv18::PeakValidFlag() const { return ((flags & 0x200)>>9); }
UShort_t TRawEvent::GEBArgonneCFDv18::OffsetFlag() const { return ((flags & 0x400)>>10); }
UShort_t TRawEvent::GEBArgonneCFDv18::CFDValidFlag() const { return ((flags & 0x800)>>11); }
UShort_t TRawEvent::GEBArgonneCFDv18::SyncErrorFlag() const { return ((flags & 0x1000)>>12); }
UShort_t TRawEvent::GEBArgonneCFDv18::GeneralErrorFlag() const { return ((flags & 0x2000)>>13); }
UShort_t TRawEvent::GEBArgonneCFDv18::PileUpOnlyFlag() const { return ((flags & 0x4000)>>14); }
UShort_t TRawEvent::GEBArgonneCFDv18::PileUpFlag() const { return ((flags & 0x8000)>>15); }

void TRawEvent::SwapArgonneHead(TRawEvent::GEBArgonneHead& header) {
        header.GA_packetlength = SwapShort(header.GA_packetlength);
        header.ud_channel = SwapShort(header.ud_channel);
        header.disc_low = SwapInt(header.disc_low);
        header.hdrlength_evttype_hdrtype = SwapShort(header.hdrlength_evttype_hdrtype);
        header.disc_high = SwapShort(header.disc_high);
}
void TRawEvent::SwapArgonneLEDv11(TRawEvent::GEBArgonneLEDv11& data) {
        data.led_low_prev = SwapShort(data.led_low_prev);
        data.flags = SwapShort(data.flags);
        data.led_high_prev = SwapInt(data.led_high_prev);
        data.sampled_baseline = SwapInt(data.sampled_baseline);
        data.postrise_sum_low_prerise_sum = SwapInt(data.postrise_sum_low_prerise_sum);
        data.timestamp_peak_low = SwapShort(data.timestamp_peak_low);
        data.postrise_sum_high = SwapShort(data.postrise_sum_high);
        data.timestamp_peak_high = SwapInt(data.timestamp_peak_high);
        data.postrise_end_sample = SwapShort(data.postrise_end_sample);
        data.postrise_begin_sample = SwapShort(data.postrise_begin_sample);
        data.prerise_end_sample = SwapShort(data.prerise_end_sample);
        data.prerise_begin_sample = SwapShort(data.prerise_begin_sample);
        data.base_sample = SwapShort(data.base_sample);
        data.peak_sample = SwapShort(data.peak_sample);
}
void TRawEvent::SwapArgonneLEDv18(TRawEvent::GEBArgonneLEDv18& data) {
        data.led_low_prev = SwapShort(data.led_low_prev);
        data.flags = SwapShort(data.flags);
        data.led_high_prev = SwapInt(data.led_high_prev);
        data.sampled_baseline = SwapInt(data.sampled_baseline);
        data.postrise_sum_low_prerise_sum = SwapInt(data.postrise_sum_low_prerise_sum);
        data.timestamp_peak_low = SwapShort(data.timestamp_peak_low);
        data.postrise_sum_high = SwapShort(data.postrise_sum_high);
        //data.timestamp_peak_high = SwapInt(data.timestamp_peak_high);
	data.timestamp_trigger_low = SwapShort(data.timestamp_trigger_low); // not fully implemented
        data.last_postrise_enter_sample = SwapShort(data.last_postrise_enter_sample);
	data.last_postrise_leave_sample = SwapShort(data.last_postrise_leave_sample);
        data.postrise_leave_sample = SwapShort(data.postrise_leave_sample);
        data.prerise_enter_sample = SwapShort(data.prerise_enter_sample);
        data.prerise_leave_sample = SwapShort(data.prerise_leave_sample);
        data.base_sample = SwapShort(data.base_sample);
        data.peak_sample = SwapShort(data.peak_sample);
}

void TRawEvent::SwapArgonneCFDv18(TRawEvent::GEBArgonneCFDv18& data) {
	data.cfd_low_prev = SwapShort(data.cfd_low_prev);
	data.flags = SwapShort(data.flags);
	data.cfd_sample0 = SwapShort(data.cfd_sample0);
	data.cfd_mid_prev = SwapShort(data.cfd_mid_prev);
	data.sampled_baseline = SwapInt(data.sampled_baseline);
	data.cfd_sample2 = SwapShort(data.cfd_sample2);
	data.cfd_sample1 = SwapShort(data.cfd_sample1);
	data.postrise_sum_low_prerise_sum = SwapInt(data.postrise_sum_low_prerise_sum);
	data.timestamp_peak_low = SwapShort(data.timestamp_peak_low);
	data.postrise_sum_high = SwapShort(data.postrise_sum_high);
	data.timestamp_trigger_low = SwapShort(data.timestamp_trigger_low);
	data.last_postrise_enter_sample = SwapShort(data.last_postrise_enter_sample);
	data.postrise_end_sample = SwapShort(data.postrise_end_sample);
	data.postrise_begin_sample = SwapShort(data.postrise_begin_sample);
	data.prerise_end_sample = SwapShort(data.prerise_end_sample);
	data.prerise_begin_sample = SwapShort(data.prerise_begin_sample);
	data.base_sample = SwapShort(data.base_sample);
	data.peak_sample = SwapShort(data.peak_sample);
}

#define STR(x) "\t GEBArgonne "<< #x <<": " << x
std::ostream& operator<<(std::ostream& os, const TRawEvent::GEBArgonneHead& header) {
        return os << "-- Argonne header packet -- \n"
                << STR(header.GA_packetlength) << "\n"
                << STR(header.ud_channel) << "\n"
                << STR(header.disc_low) << "\n"
                << STR(header.hdrlength_evttype_hdrtype) << "\n"
                << STR(header.disc_high) << std::endl;
}
std::ostream& operator<<(std::ostream& os, const TRawEvent::GEBArgonneLEDv11& data) {
        return os << "-- Argonne LEDv11 data packet --"
		  << STR(data.led_low_prev) << "\n"
		  << STR(data.flags) << "\n"
		  << STR(data.led_high_prev) << "\n"
		  << STR(data.sampled_baseline) << "\n"
		  << STR(data._blank_) << "\n"
		  << STR(data.postrise_sum_low_prerise_sum) << "\n"
		  << STR(data.timestamp_peak_low) << "\n"
		  << STR(data.postrise_sum_high) << "\n"
		  << STR(data.timestamp_peak_high) << "\n"
		  << STR(data.postrise_end_sample) << "\n"
		  << STR(data.postrise_begin_sample) << "\n"
		  << STR(data.prerise_end_sample) << "\n"
		  << STR(data.prerise_begin_sample) << "\n"
		  << STR(data.base_sample) << "\n"
		  << STR(data.peak_sample) << std::endl;
}
std::ostream& operator<<(std::ostream& os, const TRawEvent::GEBArgonneLEDv18& data) {
        return os << "-- Argonne LEDv18 data packet --"
		  << STR(data.led_low_prev) << "\n"
		  << STR(data.flags) << "\n"
		  << STR(data.led_high_prev) << "\n"
		  << STR(data.sampled_baseline) << "\n"
		  << STR(data._blank_) << "\n"
		  << STR(data.postrise_sum_low_prerise_sum) << "\n"
		  << STR(data.timestamp_peak_low) << "\n"
		  << STR(data.postrise_sum_high) << "\n"
		  << STR(data.timestamp_trigger_low) << "\n"
		  << STR(data.last_postrise_enter_sample) << "\n"
		  << STR(data.last_postrise_leave_sample) << "\n"
		  << STR(data.postrise_leave_sample) << "\n"
		  << STR(data.prerise_enter_sample) << "\n"
		  << STR(data.prerise_leave_sample) << "\n"
		  << STR(data.base_sample) << "\n"
		  << STR(data.peak_sample) << std::endl;
}
std::ostream& operator<<(std::ostream& os, const TRawEvent::GEBArgonneCFDv18& data) {
        return os << "-- Argonne CFDv18 data packet --"

		  << STR(data.cfd_low_prev) << "\n"
		  << STR(data.flags) << "\n"
		  << STR(data.cfd_sample0) << "\n"
		  << STR(data.cfd_mid_prev) << "\n"
		  << STR(data.sampled_baseline) << "\n"
		  << STR(data.cfd_sample2) << "\n"
		  << STR(data.cfd_sample1) << "\n"
		  << STR(data.postrise_sum_low_prerise_sum) << "\n"
		  << STR(data.timestamp_peak_low) << "\n"
		  << STR(data.postrise_sum_high) << "\n"
		  << STR(data.timestamp_trigger_low) << "\n"
		  << STR(data.last_postrise_enter_sample) << "\n"
		  << STR(data.postrise_end_sample) << "\n"
		  << STR(data.postrise_begin_sample) << "\n"
		  << STR(data.prerise_end_sample) << "\n"
		  << STR(data.prerise_begin_sample) << "\n"
		  << STR(data.base_sample) << "\n"
		  << STR(data.peak_sample) << "\n"
		  << STR(data.WriteFlag()) << "\n"
		  << STR(data.VetoFlag()) << "\n"
		  << STR(data.TSMatchFlag()) << "\n"
		  << STR(data.ExternalDiscFlag()) << "\n"
		  << STR(data.PeakValidFlag()) << "\n"
		  << STR(data.OffsetFlag()) << "\n"
		  << STR(data.CFDValidFlag()) << "\n"
		  << STR(data.SyncErrorFlag()) << "\n"
		  << STR(data.GeneralErrorFlag()) << "\n"
		  << STR(data.PileUpOnlyFlag()) << "\n"
		  << STR(data.PileUpFlag()) << std::endl;
}
#undef STR

std::ostream& operator<<(std::ostream& os,const TRawEvent::GEBS800Header &head) {
  return os << "-- S800 Header \"packet\" -- \n"
            << "\t S800 timestamp:    " << head.S800_timestamp    << "\n"
            << "\t S800 EC low:       " << std::hex << head.S800_eventnumber_low    << std::dec << "\n"
            << "\t S800 EC middle:    " << std::hex << head.S800_eventnumber_middle << std::dec << "\n"
            << "\t S800 EC high:      " << std::hex << head.S800_eventnumber_high   << std::dec << "\n"
            << "\t S800 event number: " << head.GetEventNumber() << std::endl;
}

std::ostream& operator<<(std::ostream& os,const TRawEvent::S800TriggerPacket &pack) {
  os << "-- S800 Trigger Packet -- \n";
  for(int x=0;x<pack.channel_time_number;x++) {
    os << "\t" << x
       << "Tigger[" << TRawEvent::GetS800Channel(pack.channel_time[x]) << "]   "
       << TRawEvent::GetS800Value(pack.channel_time[x]) << "\n";
  }
  return os;
}

std::ostream& operator<<(std::ostream& os,const TRawEvent::S800TOFPacket &tof) {
  os << "-- S800 TOF Packet -- \n";
  for(int x=0;x<tof.number;x++) {
    switch(TRawEvent::GetS800Channel(tof.value[x])) {
      case 12:
        os << "\t RF:  0x" << std::hex << TRawEvent::GetS800Value(tof.value[x]) << std::dec << "\n";
        break;
      case 13:
        os << "\t Object Scintillator:  0x" << std::hex << TRawEvent::GetS800Value(tof.value[x]) << std::dec << "\n";
        break;
      case 14:
        os << "\t XFP Scintillator:  0x" << std::hex << TRawEvent::GetS800Value(tof.value[x]) << std::dec << "\n";
        break;
      case 15:
        os << "\t Si:  0x" << std::hex << TRawEvent::GetS800Value(tof.value[x]) << std::dec << "\n";
        break;
      default:
        os << "\t " << TRawEvent::GetS800Channel(tof.value[x]) << ":  0x" << std::hex << TRawEvent::GetS800Value(tof.value[x]) << std::dec << "\n";
        break;

    };
  }
  return os;
}


Long_t TRawEvent::GEBMode3Data::GetLed() const { return (((long)led_high)<<32) + (((long)led_middle)<<16) + (((long)led_low)<<0); }
Long_t TRawEvent::GEBMode3Data::GetCfd() const { return (((long)cfd_high)<<32) + (((long)cfd_middle)<<16) + (((long)cfd_low)<<0); }
UShort_t TRawEvent::GEBMode3Data::GetDeltaT1() const { return cfd_low; }
UShort_t TRawEvent::GEBMode3Data::GetDeltaT2() const { return cfd_middle; }


Int_t  TRawEvent::GEBMode3Data::GetEnergy(GEBMode3Head &head) const  {
  int channel = head.GetChannel();
  //if ((channel==1) &&( head.GetHole()==9) && (head.GetCrystal()==3) && (head.GetVME()==3))
    //channel = 9; //  Q5 e5 has an inverted radial box, treat it as a core.  pcb.
  int  temp = (((int)energy_high)<<16) + (((int)energy_low)<<0);
  bool sign = temp&0x01000000;
       temp = temp&0x00ffffff;
  if(sign)
    temp = temp - (int)0x01000000;
  if(channel!=9)  // do not remove.  this is 100% needed. pcb.
    temp = -temp;
  return temp;
}

Int_t  TRawEvent::GEBMode3Data::GetEnergy0(GEBMode3Head &head) const  {
  int channel = head.GetChannel();
  //if ((channel==1) &&( head.GetHole()==9) && (head.GetCrystal()==3) && (head.GetVME()==3))
    //channel = 9; //  Q5 e5 has an inverted radial box, treat it as a core.  pcb.
  int  eng_high_bit = (cfd_pt1_low & 0x01ff); 
  int  temp = (((int) eng_high_bit )<<16) + (((int)cfd_high)<<0);
   bool sign = temp&0x01000000;
       temp = temp&0x00ffffff;
  if(sign)
    temp = temp - (int)0x01000000;
  if(channel!=9)  // do not remove.  this is 100% needed. pcb.
    temp = -temp;
  return temp;
}

Int_t  TRawEvent::GEBMode3Data::GetEnergy1(GEBMode3Head &head) const  {
  int channel = head.GetChannel();
  //if ((channel==1) &&( head.GetHole()==9) && (head.GetCrystal()==3) && (head.GetVME()==3))
    //channel = 9; //  Q5 e5 has an inverted radial box, treat it as a core.  pcb.
 
  int  eng_low_bit  = (cfd_pt1_low & 0xfe00) >>9; 
  int  eng_mid_bit  = (cfd_pt1_high);
  int  eng_high_bit = (cfd_pt2_low&0x0003);
  
  int  temp = (((int) eng_high_bit )<<23) + (((int)eng_mid_bit)<<7) + (((int)eng_low_bit)<<0) ;
   
   bool sign = temp&0x01000000;
       temp = temp&0x00ffffff;
  if(sign)
    temp = temp - (int)0x01000000;
  if(channel!=9)  // do not remove.  this is 100% needed. pcb.
    temp = -temp;
  return temp;
}

Int_t  TRawEvent::GEBMode3Data::GetEnergy2(GEBMode3Head &head) const  {
  int channel = head.GetChannel();
  //if ((channel==1) &&( head.GetHole()==9) && (head.GetCrystal()==3) && (head.GetVME()==3))
    //channel = 9; //  Q5 e5 has an inverted radial box, treat it as a core.  pcb.
 
  int  eng_low_bit  = (cfd_pt2_low &0xfffc)>>2; 
  int  eng_high_bit = (cfd_pt2_high & 0x07ff); 
 
  int  temp = (((int) eng_high_bit )<<14) + (((int)eng_low_bit)<<0);
   
   bool sign = temp&0x01000000;
       temp = temp&0x00ffffff;
  if(sign)
    temp = temp - (int)0x01000000;
  if(channel!=9)  // do not remove.  this is 100% needed. pcb.
    temp = -temp;
  return temp;
}












std::ostream& operator<<(std::ostream& os,const TRawEvent::PWHit &hit) {
   return os << "PWHit[" << std::setw(3) << hit.pix_id <<"]\t"   //  PW Bank @ " << std::hex << &head << std::dec      << std::endl
	     << std::setw(8) << hit.data_a
             << std::setw(8) << hit.data_b
             << std::setw(8) << hit.data_c
             << std::setw(8) << hit.time   << std::endl;
}


std::ostream& operator<<(std::ostream& os,const TRawEvent::PWBank &bank) {
   os << std::setw(24) << "data_a" << std::setw(8) << "data_b" << std::setw(8) << "data_c" << std::setw(8) << "time" << std::endl;
   //os << " size in bytes = " << sizeof(this) << endl;
   for(int x=0;x<bank.nhits;x++)
      os << bank.hit[x];
   os << "*****************************" << std::endl;
   return os;
}


std::ostream& operator<<(std::ostream& os,const TRawEvent::LaBrSeg &hit) {
   return os << "LaBrHit[" << std::setw(2)  << hit.chan_id <<"]\t"   //  PW Bank @ " << std::hex << &head << std::dec      << std::endl
             << std::setw(8) << hit.value << std::endl;
}


std::ostream& operator<<(std::ostream& os,const TRawEvent::LaBrBank &bank) {
   //os << std::setw(24) << "data_a" << std::setw(8) << "data_b" << std::setw(8) << "data_c" << std::setw(8) << "time" << std::endl;
   //os << " size in bytes = " << sizeof(this) << endl;
   os << "LaBr Bank: \n";
   os << "Energy Hits: " << bank.nenghits << std::endl;
   for(int x=0;x<bank.nenghits;x++)
      os << "\t" << bank.energy_hit[x];
   os << "Time Hits: " << bank.ntimhits << std::endl;
   for(int x=0;x<bank.ntimhits;x++)
      os << "\t" << bank.energy_hit[x];
   os << "*****************************" << std::endl;
   return os;
}

std::ostream& operator<<(std::ostream& os, const TRawEvent::RawHeader &head) {
  return os << "\tdatum1      = " << std::setw(8) << head.datum1 << std::endl
            << "\tdatum2      = " << std::setw(8) << head.datum2 << std::endl;
}

std::ostream& operator<<(std::ostream& os, const TRawEvent::TNSCLFragmentHeader& head){
  return os << "Fragment Header: " << "\n"
            << "\tTimestamp: " << head.timestamp << "\n"
            << "\tSource ID: " << head.sourceid << "\n"
            << "\tPayload Size: " << head.payload_size << "\n"
            << "\tBarrier: " << head.barrier << "\n"
            << std::flush;
}

std::ostream& operator<<(std::ostream& os, const TRawEvent::TNSCLBeginRun& begin) {
  time_t time = begin.unix_time;
  return os << "Begin of run " << begin.run_number << "\n"
            << "\tTime Offset: " << begin.time_offset << "\n"
            << "\tStart time: " << begin.unix_time << "\n"
            << "\tStart time: " << std::hex << "0x" << begin.unix_time << std::dec << "\n"
            << "\tStart time: " << std::ctime(&time) << "\n"
            << "\tTitle: " << begin.title << "\n"
            << std::flush;
}
