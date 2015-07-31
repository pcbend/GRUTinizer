#ifndef _DDASBANKS_H_
#define _DDASBANKS_H_

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

struct DDAS_QDC_Sum {
  unsigned int qdc_sum[4];
} __attribute__((packed));

struct DDAS_Energy_Sum {
  unsigned int energy_sum[8];
} __attribute__((packed));

#endif /* _DDASBANKS_H_ */
