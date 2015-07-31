#include "TRawEvent.h"

std::ostream& operator<<(std::ostream& os, const TRawEvent::GEBInteractionPoint &fSeg) {
   std::streamsize ss = std::cout.precision();
   return os << "HPGeSegment[" << std::setw(3) << fSeg.seg << "]\t("
             << std::setprecision(2)  << fSeg.x << ", " << fSeg.y << ", " << fSeg.z << ")\t"
             << fSeg.seg_ener << " / " << fSeg.e << std::setprecision(ss) << std::endl;
};



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
};




std::ostream& operator<<(std::ostream& os,const TRawEvent::PWHit &hit) {
   return os << "PWHit[" << std::setw(3) << hit.pix_id <<"]\t"   //  PW Bank @ " << std::hex << &head << std::dec      << std::endl
	     << std::setw(8) << hit.data_a
             << std::setw(8) << hit.data_b
             << std::setw(8) << hit.data_c
             << std::setw(8) << hit.time   << std::endl;
};


std::ostream& operator<<(std::ostream& os,const TRawEvent::PWBank &bank) {
   os << std::setw(24) << "data_a" << std::setw(8) << "data_b" << std::setw(8) << "data_c" << std::setw(8) << "time" << std::endl;
   //os << " size in bytes = " << sizeof(this) << endl;
   for(int x=0;x<bank.nhits;x++)
      os << bank.hit[x];
   os << "*****************************" << std::endl;
   return os;
};


std::ostream& operator<<(std::ostream& os,const TRawEvent::LaBrSeg &hit) {
   return os << "LaBrHit[" << std::setw(2)  << hit.chan_id <<"]\t"   //  PW Bank @ " << std::hex << &head << std::dec      << std::endl
             << std::setw(8) << hit.value << std::endl;
};


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
};

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
