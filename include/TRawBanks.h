#ifndef TRAWBANKS__H
#define TRAWBANKS__H

//#include <cstdio>
//#include <iostream>
//#include <iomanip>

//#include <stdint.h>
#include <Rtypes.h>

//  https:://gswg.lbl.gov/tiki-index.php?page=GEB+Headers

#define MAXSIMSIZE 40
#define MAX_INTPTS 16
#define MAX_PWID   256
#define MAX_LABRID 16
#define MQDC_ID 161
#define MTDC_ID 177

// Simulation
#define MAX_SIM_GAMMAS 10

struct g4sim_ghead{
  int type;          /* defined as abcd1234 */
  int num;           /* # of emitted gammas */
  int full;          /* is full energy */

  int GetType() const { return type; }
  int GetNum()  const { return num; }
  int GetFull() const { return full; }
}__attribute__((__packed__));

struct g4sim_eg{
  float e;
  float x, y, z;
  float phi, theta; //THIS IS REDUNDANT
  float beta;

  float GetEn()       const { return e; }
  float GetX()        const { return x; }
  float GetY()        const { return y; }
  float GetZ()        const { return z; }
  float GetPhi()      const { return phi; }
  float GetTheta()    const { return theta; }
  float GetBeta()     const { return beta; }

}__attribute__((__packed__));


struct G4SimPacket {
  g4sim_ghead head;
  g4sim_eg    data[MAXSIMSIZE];
}__attribute__((__packed__));

friend std::ostream& operator<<(std::ostream& os, const G4SimPacket &packet);

// -- End of Geant4 Gamma Sim stuff
// -- Beginning of Geant4 S800 Sim stuff

struct G4S800 {


  Int_t type;    /* defined abcd1234 for indicating this version */
  // All of this vvv is zero in the sim. ***************************
  float crdc1_x;   /* Crdc x/y positions in mm */
  float crdc1_y;
  float crdc2_x;
  float crdc2_y;
  float ic_sum;    /* ion chamber energy loss         */
  float tof_xfp;   /* TOF scintillator after A1900    */
  float tof_obj;   /* TOF scintillator in object box  */
  float rf;        /* Cyclotron RF for TOF            */
  Int_t trigger; /* Trigger register bit pattern    */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* from here corrected values extracted from data above */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  float ic_de;
  /* TOF values with TOF correction applied (from afp/crdc x) */
  float tof_xfpe1;
  float tof_obje1;
  float tof_rfe1;
  // All of this ^^^ is zero in the sim. ***************************


  /* Trajectory information at target position calculated from
     a map and afp/bfp/xfp/yfp. New map and you need to re-calc */
  float ata; /* dispersive angle        */
  float bta; /* non-dispersive angle    */
  float dta; /* dT/T T:kinetic energy   */
  float yta; /* non-dispersive position */

  Int_t GetType() const { return type; }
  float GetATA()  const { return ata; }
  float GetBTA()  const { return bta; }
  float GetDTA()  const { return dta; }
  float GetYTA()  const { return yta; }
}__attribute__((__packed__));

friend std::ostream& operator<<(std::ostream& os, const G4S800 &s800pack);


// General Mesytec Stuff:
struct Mesy_Word{
  // This is 2x16 bits = 32 bits.  The MQDC talks in 32 bit words.
  UShort_t tail;
  UShort_t nose;

  bool isHeader() const { return((nose&0xff00)==0x4000); }
  bool isData()   const { return((nose&0xffc0)==0x0400); } // for QDC 0x0020 should also be 0.  Left out for easy use.
  bool isETS()    const { return((nose&0xffff)==0x0480); }
  bool isFILL()   const { return(((nose&0xffff)==0) && ((tail&0xffff)==0)); }
  bool isALLF()   const { return(((nose&0xffff)==0xffff) && ((tail&0xffff)==0xffff)); }
  bool isEOE()    const { return(((nose&0xc000)==0xc000) && !(isALLF())); }
}__attribute__((__packed__));

struct Mesy_Header{
  UShort_t tail;
  UShort_t nose;

  UShort_t format()   const { return ((tail&0x8000)>>15); }
  UShort_t size()     const { return tail&0x0fff; }
  UShort_t id()       const { return nose&0x00ff; }
  UShort_t res()      const { return (tail&0x7000)>>12; }
  bool isQDC()        const { return(id()==MQDC_ID); }
  bool isTDC()        const { return(id()==MTDC_ID); }
}__attribute__((__packed__));

struct Mesy_ETS{
  UShort_t tail;
  UShort_t nose;

  UShort_t ETS()    const { return tail&0xffff; }
}__attribute__((__packed__));

struct Mesy_FILL{
  UShort_t tail;
  UShort_t nose;
}__attribute__((__packed__));

struct Mesy_EOE{
  UInt_t data;

  Int_t TS()     const { return data&0x3fffffff; }
}__attribute__((__packed__));

// Mesytec QDC:
struct M_QDC_Data{
  UShort_t tail;
  UShort_t nose;

  UShort_t Chan()   const { return nose&0x001f; }
  UShort_t Charge() const { return tail&0x0fff; }
  UShort_t isOOR()  const { return (tail&0x4000)>>14; }
}__attribute__((__packed__));

// Mesytec TDC:
struct M_TDC_Data{
  UShort_t tail;
  UShort_t nose;

  bool isTrig() const { return((nose&0x0020)>>5); }
  UShort_t Chan()   const { return nose&0x001f; }
  UShort_t Time()   const { return tail&0xffff; }
}__attribute__((__packed__));




typedef struct {
  Int_t   totalsize;
  Short_t moresize;
  Short_t tag;
  Short_t version;
  Short_t ulm;
}__attribute__((__packed__)) CAESARHeader;

typedef struct {
  Short_t size; // Inclusive number of 16-bit values
  Short_t tag;
}__attribute__((__packed__)) CAESARFeraHeader;

typedef struct {
  Short_t header;
  Short_t number_chans() { return  ((header & 0x7800)>>11); }
  Short_t vsn()          { return   (header & 0x00ff)-1; }
//  CAESARFeraItem items[16];

}__attribute__((__packed__)) CAESARFera;


typedef struct {
  Short_t data;
  Short_t channel()  { return  ((data & 0x7800)>>11); }
  Short_t value()    { return   (data & 0x07ff); }
}__attribute__((__packed__)) CAESARFeraItem;









typedef struct { // HPGe Segment Hit Type 1;
   Float_t     x;
   Float_t     y;
   Float_t     z;
   Float_t     e;        //fraction of the energy
   Int_t       seg;
   Float_t     seg_ener; //energy (cahrge) collected in the segment
}__attribute__((__packed__)) GEBInteractionPoint;

friend std::ostream& operator<<(std::ostream& os, const GEBInteractionPoint &fSeg);

typedef struct { // Decomposed GRETINA Data
   Int_t     type;
   Int_t     crystal_id;
   Int_t     num;
   Float_t   tot_e;
   Int_t     core_e[4];
   Long_t    timestamp;
   Long_t    trig_time;
   Float_t   t0;
   Float_t   cfd;
   Float_t   chisq;
   Float_t   norm_chisq;
   Float_t   baseline;
   Float_t   prestep;
   Float_t   poststep;
   Int_t     pad;
   GEBInteractionPoint intpts[MAX_INTPTS];
}__attribute__((__packed__)) GEBBankType1;

friend std::ostream& operator<<(std::ostream& os, const GEBBankType1 &bank);

static UShort_t SwapShort(UShort_t datum);
static UInt_t SwapInt(UInt_t datum);

struct GEBMode3Head {
  UShort_t a2;
  UShort_t a1;
  UShort_t lengthGA;
  UShort_t board_id;
  Int_t  GetLength() const;
  Int_t  GetChannel() const;
  Int_t  GetVME() const;
  Int_t  GetCrystal() const;
  Int_t  GetHole() const;
  Int_t  GetSegmentId() const;
  Int_t  GetCrystalId() const;
}__attribute__((__packed__));

friend std::ostream& operator<<(std::ostream& os, const GEBMode3Head &head);
static void SwapMode3Head(GEBMode3Head &head);

//typedef struct {
struct GEBMode3Data {
  UShort_t led_middle;
  UShort_t led_low;
  UShort_t energy_low;
  UShort_t led_high;
  UShort_t cfd_low;
  UShort_t energy_high;
  UShort_t cfd_high;
  UShort_t cfd_middle;
  UShort_t cfd_pt1_high;
  UShort_t cfd_pt1_low;
  UShort_t cfd_pt2_high;
  UShort_t cfd_pt2_low;
  Long_t GetLed() const;
  Long_t GetCfd() const;
  UShort_t GetDeltaT1() const;
  UShort_t GetDeltaT2() const;

  Int_t  GetEnergy(GEBMode3Head&) const;
  Int_t  GetEnergy0(GEBMode3Head&) const;
  Int_t  GetEnergy1(GEBMode3Head&) const;
  Int_t  GetEnergy2(GEBMode3Head&) const;
}__attribute__((__packed__));

friend std::ostream& operator<<(std::ostream& os, const GEBMode3Data &data);
static void SwapMode3Data(GEBMode3Data &data);

enum ArgonneType { LEDv10, LEDv11, CFDv11, LEDv18, CFDv18 };

struct GEBArgonneHead {
  UShort_t GA_packetlength;
  UShort_t ud_channel;
  UInt_t   disc_low;
  UShort_t hdrlength_evttype_hdrtype;
  UShort_t disc_high;
  UShort_t GetGA() const;
  UShort_t GetLength() const;
  UShort_t GetBoardID() const;
  UShort_t GetChannel() const;
  UInt_t   GetHeaderType() const;
  UShort_t GetEventType() const;
  UShort_t GetHeaderLength() const;
  ULong_t  GetDisc() const;
}__attribute__((__packed__));

friend std::ostream& operator<<(std::ostream& os, const GEBArgonneHead &header);
static void SwapArgonneHead(TRawEvent::GEBArgonneHead& header);

struct GEBArgonneLEDv11 {
  UShort_t led_low_prev;
  UShort_t flags;
  UInt_t   led_high_prev;
  UInt_t   sampled_baseline;
  UInt_t   _blank_;
  UInt_t   postrise_sum_low_prerise_sum;
  UShort_t timestamp_peak_low;
  UShort_t postrise_sum_high;
  UInt_t   timestamp_peak_high;
  UShort_t postrise_end_sample;
  UShort_t postrise_begin_sample;
  UShort_t prerise_end_sample;
  UShort_t prerise_begin_sample;
  UShort_t base_sample;
  UShort_t peak_sample;
  ULong_t  GetPreviousLED() const;
  UInt_t   GetBaseline() const;
  UInt_t   GetPreRiseE() const;
  UInt_t   GetPostRiseE() const;
  ULong_t  GetPeakTimestamp() const;
  UShort_t GetPostRiseSampleBegin() const;
  UShort_t GetPostRiseSampleEnd() const;
  UShort_t GetPreRiseSampleBegin() const;
  UShort_t GetPreRiseSampleEnd() const;
  UShort_t GetBaseSample() const;
  UShort_t GetPeakSample() const;
  UShort_t ExternalDiscFlag() const;
  UShort_t PeakValidFlag() const;
  UShort_t OffsetFlag() const;
  UShort_t SyncErrorFlag() const;
  UShort_t GeneralErrorFlag() const;
  UShort_t PileUpOnlyFlag() const;
  UShort_t PileUpFlag() const;
}__attribute__((__packed__));

struct GEBArgonneLEDv18 {
  UShort_t led_low_prev;
  UShort_t flags;
  UInt_t   led_high_prev;
  UInt_t   sampled_baseline;
  UInt_t   _blank_;
  UInt_t   postrise_sum_low_prerise_sum;
  UShort_t timestamp_peak_low;
  UShort_t postrise_sum_high;
  // begin differences from v11 //
  UShort_t timestamp_trigger_low;
  UShort_t last_postrise_enter_sample;
  // old: UInt_t timestamp_peak_high;
  UShort_t last_postrise_leave_sample;
  UShort_t postrise_leave_sample;
  // old: UShort_t postrise_end_sample;
  // old: UShort_t postrise_begin_sample;
  UShort_t prerise_enter_sample;
  UShort_t prerise_leave_sample;
  // old: UShort_t prerise_end_sample;
  // old: UShort_t prerise_begin_sample;
  UShort_t base_sample;
  UShort_t peak_sample;
  ULong_t  GetPreviousLED() const;
  UInt_t   GetBaseline() const;
  UInt_t   GetPreRiseE() const;
  UInt_t   GetPostRiseE() const;
  // need changes
  ULong_t  GetTrigTimestamp() const;
  UShort_t GetLastPostRiseEnterSample() const;
  UShort_t GetLastPostRiseLeaveSample() const;
  UShort_t GetPostRiseLeaveSample() const;
  UShort_t GetPreRiseEnterSample() const;
  UShort_t GetPreRiseLeaveSample() const;
  // no change needed
  UShort_t GetBaseSample() const;
  UShort_t GetPeakSample() const;

  UShort_t WriteFlag() const;
  UShort_t VetoFlag() const;
  UShort_t ExternalDiscFlag() const;
  UShort_t PeakValidFlag() const;
  UShort_t OffsetFlag() const;
  UShort_t SyncErrorFlag() const;
  UShort_t GeneralErrorFlag() const;
  UShort_t PileUpOnlyFlag() const;
  UShort_t PileUpFlag() const;
}__attribute__((__packed__));

struct GEBArgonneCFDv18 {
  UShort_t cfd_low_prev;
  UShort_t flags;
  UShort_t cfd_sample0; // signed
  UShort_t cfd_mid_prev; // bits 16:29
  UInt_t   sampled_baseline;
  UShort_t cfd_sample2;
  UShort_t cfd_sample1;
  UInt_t   postrise_sum_low_prerise_sum;
  UShort_t timestamp_peak_low;
  UShort_t postrise_sum_high;
  UShort_t timestamp_trigger_low;
  UShort_t last_postrise_enter_sample;
  UShort_t postrise_end_sample;
  UShort_t postrise_begin_sample;
  UShort_t prerise_end_sample;
  UShort_t prerise_begin_sample;
  UShort_t base_sample;
  UShort_t peak_sample;
  Double_t GetCFD() const;
  Short_t  GetCFD0() const;
  Short_t  GetCFD1() const;
  Short_t  GetCFD2() const;
  ULong_t  GetPrevCFD(const GEBArgonneHead*) const;
  UInt_t   GetBaseline() const;
  UInt_t   GetPreRiseE() const;
  UInt_t   GetPostRiseE() const;
  ULong_t  GetTrigTimestamp() const;
  UShort_t GetLastPostRiseEnterSample() const;
  UShort_t GetPostRiseSampleBegin() const;
  UShort_t GetPostRiseSampleEnd() const;
  UShort_t GetPreRiseSampleBegin() const;
  UShort_t GetPreRiseSampleEnd() const;
  UShort_t GetBaseSample() const;
  UShort_t GetPeakSample() const;
  UShort_t WriteFlag() const;
  UShort_t VetoFlag() const;
  UShort_t TSMatchFlag() const;
  UShort_t CFDValidFlag() const;
  UShort_t ExternalDiscFlag() const;
  UShort_t PeakValidFlag() const;
  UShort_t OffsetFlag() const;
  UShort_t SyncErrorFlag() const;
  UShort_t GeneralErrorFlag() const;
  UShort_t PileUpOnlyFlag() const;
  UShort_t PileUpFlag() const;

}__attribute__((__packed__));

static Short_t GetSigned14BitFromUShort(UShort_t ushort);

friend std::ostream& operator<<(std::ostream& os, const GEBArgonneLEDv11& data);
static void SwapArgonneLEDv11(TRawEvent::GEBArgonneLEDv11& data);

friend std::ostream& operator<<(std::ostream& os, const GEBArgonneLEDv18& data);
static void SwapArgonneLEDv18(TRawEvent::GEBArgonneLEDv18& data);

friend std::ostream& operator<<(std::ostream& os, const GEBArgonneCFDv18& data);
static void SwapArgonneCFDv18(TRawEvent::GEBArgonneCFDv18& data);

struct GEBS800Header {
  Int_t    total_size;
  UShort_t total_size2;
  UShort_t S800_packet;
  UShort_t S800_packet_size;
  UShort_t S800_version;
  UShort_t S800_timestamp_packet;
  ULong_t  S800_timestamp;
  UShort_t S800_eventnumber_packet_size;
  UShort_t S800_eventnumber_packet;
  UShort_t S800_eventnumber_low;
  UShort_t S800_eventnumber_middle;
  UShort_t S800_eventnumber_high;
  Long_t GetEventNumber() const {
     long temp = (long)S800_eventnumber_low;
     temp     += ((long)S800_eventnumber_middle) << 16;
     temp     += ((long)S800_eventnumber_high)   << 32;
     return temp;
  }

} __attribute__((__packed__));

friend std::ostream& operator<<(std::ostream& os, const GEBS800Header &head);

static Int_t GetS800Channel(UShort_t input) { return (input>>12);    }
static Int_t GetS800Value(UShort_t input)   { return (input&0x0fff); }


struct S800TriggerPacket {
  UShort_t trgger_pattern;
  UShort_t channel_time[4];
  UShort_t channel_time_number;
} __attribute__((__packed__));

friend std::ostream& operator<<(std::ostream& os, const S800TriggerPacket &pack);

struct S800TOFPacket {
  UShort_t value[4];
  UShort_t number;
} __attribute__((__packed__));

friend std::ostream& operator<<(std::ostream& os, const S800TOFPacket &tof);

struct S800SCINTPacket {
  UShort_t value[2][4];
  UShort_t number;
} __attribute__((__packed__));

// https://wikihost.nscl.msu.edu/S800Doc/doku.php?id=s800_version_0x0006

struct S800FPICPacket {
  UShort_t number;
  UShort_t subid;
  UShort_t value[16];
} __attribute__((__packed__));

struct S800FPCRDCPacket {
  UShort_t id;
  UShort_t number;
  UShort_t subid;
  /// not finished.
} __attribute__((__packed__));

typedef struct {
   Short_t pix_id;  //int16_t
   Short_t data_a;  //int16_t
   Short_t data_b;  //int16_t
   Short_t data_c;  //int16_t
   Short_t time;    //int16_t
}__attribute__((__packed__)) PWHit;

friend std::ostream& operator<<(std::ostream& os,const PWHit &hit);

typedef struct{
   PWHit hit[MAX_PWID];
   Short_t nhits;       //int16_t   -> not actually part of the bank recorded to file.
}__attribute__((__packed__)) PWBank;

friend std::ostream& operator<<(std::ostream& os,const PWBank &bank);

typedef struct {
  Short_t chan_id;   //int16_t
  Short_t value;     //int16_t
}__attribute__((__packed__)) LaBrSeg;

friend std::ostream& operator<<(std::ostream& os,const LaBrSeg &hit);

typedef struct {
  Short_t nenghits;
  Short_t ntimhits;
  LaBrSeg energy_hit[MAX_LABRID];
  LaBrSeg time_hit[MAX_LABRID];
}__attribute__((__packed__)) LaBrBank;

friend std::ostream& operator<<(std::ostream& os,const LaBrBank &bank);

typedef struct RawHeader {
  Int_t datum1;
  Int_t datum2;
} __attribute__((__packed__)) RawHeader;

typedef struct GEBHeader : public RawHeader {
  Int_t type()      { return datum1; } //Int_t  type; //int32_t
  Int_t size()      { return datum2; } //int32_t
  ClassDef(GEBHeader, 0);
} __attribute__((__packed__)) GEBHeader;


typedef struct EVTHeader : public RawHeader {
  Int_t size()      { return datum1; } //Int_t  size;       //int32_t
  Int_t type()      { return datum2; } //Int_t  type;       //int32_t
  ClassDef(EVTHeader, 0);
} __attribute__((__packed__)) EVTHeader;

friend std::ostream& operator<<(std::ostream& os, const RawHeader &head);

typedef struct NSCLBodyHeader {
  int body_header_size;
  long timestamp;
  int sourceid;
  int barrier;
} __attribute__((__packed__)) NSCLBodyHeader;

typedef struct TNSCLFragmentHeader {
  long timestamp;
  int sourceid;
  int payload_size;
  int barrier;
} __attribute__((__packed__)) TNSCLFragmentHeader;

friend std::ostream& operator<<(std::ostream& os, const TNSCLFragmentHeader &head);

typedef struct TNSCLBeginRun {
  unsigned int run_number;
  unsigned int time_offset;
  unsigned int unix_time;
  unsigned int offset_divisor;
  char title[81];
} __attribute__((__packed__)) TNSCLBeginRun;

friend std::ostream& operator<<(std::ostream& os, const TNSCLBeginRun& begin);

#endif
