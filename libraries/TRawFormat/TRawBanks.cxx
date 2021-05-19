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
   //for(int x=0;x<16;x++)
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
  //std::cout << __PRETTY_FUNCTION__ << std::endl;
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
