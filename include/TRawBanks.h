#ifndef TRAWBANKS__H
#define TRAWBANKS__H

//#include <cstdio>
//#include <iostream>
//#include <iomanip>

//#include <stdint.h>
#include <Rtypes.h>

//  https:://gswg.lbl.gov/tiki-index.php?page=GEB+Headers

#define MAX_INTPTS 16
#define MAX_PWID   256
#define MAX_LABRID 16


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
  Int_t  GetEnergy(const int channel) const;
}__attribute__((__packed__));

friend std::ostream& operator<<(std::ostream& os, const GEBMode3Data &data);
static void SwapMode3Data(GEBMode3Data &data);


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

typedef struct TNSCLFragmentHeader {
  long timestamp;
  int sourceid;
  int payload_size;
  int barrier;
} __attribute__((__packed__)) TNSCLFragmentHeader;

friend std::ostream& operator<<(std::ostream& os, const TNSCLFragmentHeader &head);

#endif
