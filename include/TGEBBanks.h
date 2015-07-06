#ifndef TGEBBANKS__H
#define TGEBBANKS__H

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
}__attribute__((__packed__)) TGEBGeSegType1;


friend std::ostream& operator<<(std::ostream& os, const TGEBEvent::TGEBGeSegType1 &fSeg);


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
   TGEBGeSegType1 intpts[MAX_INTPTS];
}__attribute__((__packed__)) TGEBBankType1;

friend std::ostream& operator<<(std::ostream& os, const TGEBEvent::TGEBBankType1 &bank);


typedef struct {
   Short_t pix_id;  //int16_t
   Short_t data_a;  //int16_t
   Short_t data_b;  //int16_t
   Short_t data_c;  //int16_t
   Short_t time;    //int16_t
}__attribute__((__packed__)) TPWHit;


friend std::ostream& operator<<(std::ostream& os,const TGEBEvent::TPWHit &hit);

typedef struct{
   TPWHit hit[MAX_PWID];
   Short_t nhits;       //int16_t   -> not actually part of the bank recorded to file.
}__attribute__((__packed__)) TPWBank;

friend std::ostream& operator<<(std::ostream& os,const TGEBEvent::TPWBank &bank);


typedef struct {
  Short_t chan_id;   //int16_t
  Short_t value;     //int16_t
}__attribute__((__packed__)) TLaBrSeg;

friend std::ostream& operator<<(std::ostream& os,const TGEBEvent::TLaBrSeg &hit);

typedef struct {
  Short_t nenghits;
  Short_t ntimhits;
  TLaBrSeg energy_hit[MAX_LABRID];
  TLaBrSeg time_hit[MAX_LABRID];
}__attribute__((__packed__)) TLaBrBank;

friend std::ostream& operator<<(std::ostream& os,const TGEBEvent::TLaBrBank &bank);


#endif


