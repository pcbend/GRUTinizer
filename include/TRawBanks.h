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

  /*
  int Set(const char *buffer,kFileType fileType) {
    //Assumes sizeof(buufer) == sizeof(int[2])  !!!!!!!!
    this->file_type = file_type;
    switch(this->type){
      case kFileType::NSCL_EVT: {
        const TEvtEventHeader* header = (const TEvtEventHeader*)buffer;
        size = header->size;
        type = header->type;
        timestamp = -1;
        return 0;   // size in header is inclusive.
      }
      break;
      case kFileType::GRETINA_MODE2:
      case kFileType::GRETINA_MODE3: {
        const TGEBEventHeader* header = (const TGEBEventHeader*)buffer;
        size = header->size;
        type = header->type;
        //timestamp = header->timestamp;
        return sizeof(int[2]); //TGEBEventHeader); //size of header is exclusive;
      }
      break;
      case kFileType::UNKNOWN:
        size      = -1;
        type      = -1;
        timestamp = -1;
        return      -1;   // error state.
    }
    return -1;
  }
  */
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

#endif
