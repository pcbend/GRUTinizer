#ifndef _DDASBANKS_H_
#define _DDASBANKS_H_
using namespace std;
struct DDASHeader{
  unsigned int size; // Size of DDAS packet, inclusive, number of 16-bit words
  unsigned short frequency; // Frequency, in MHz
  unsigned char adc_bits; // Number of bits in ADC
  unsigned char revision; // Firmware revision number

  unsigned int status; // Lots of packed values, unpacked in TDDASEvent
  unsigned int time_low;
  unsigned int time_high_cfd;
  unsigned int energy_tracelength;
} __attribute__((packed));

struct DDAS_Energy_Sum {
  unsigned int energy_sum[4];
} __attribute__((packed));

struct DDAS_QDC_Sum {
  unsigned int qdc_sum[8];
} __attribute__((packed));

struct DDAS_Ext_Clock {
  unsigned int clock_low;
  unsigned int clock_high;
//  unsigned int clock_higher;
} __attribute__((packed));

struct DDASGEBHeader{  //only at the start of the event...
  long         time1; 		     //time ??
  unsigned int source_id1; 	     //source id
  unsigned int paysize;    	     //Actually size1, to satisfy TDDASEvent's requirements
  unsigned int barrier;    	     //Actually barrier1, to satisfy TDDASEvent's requirements
  unsigned int size2;      	     //Actually size2, to satisfy TDDASEvent's requirements
  unsigned int type;
  unsigned int size3;                //Actually size3, to satisfy TDDASEvent's requirements
  long         time2;                //duplicate of time1?
  unsigned int source_id2; 	     //duplicate of source id1?
  unsigned int barrier2;   	     //duplicate of barrier1?

  unsigned int size; 		     //Size of DDAS packet, inclusive, number of 16-bit words
  unsigned short frequency; 	     //Frequency, in MHz
  unsigned char adc_bits; 	     //Number of bits in ADC
  unsigned char revision; 	     //Firmware revision number

  unsigned int status;               //Contains Channel ID and event length words
  unsigned int time_low;	     //Lower 32 nots of timestamp
  unsigned int time_high_cfd;	     //Upper 16 bits of timestamp and CFD information
  unsigned int energy_tracelength;   //Length of energy trace

} __attribute__((__packed__));




#endif /* _DDASBANKS_H_ */
