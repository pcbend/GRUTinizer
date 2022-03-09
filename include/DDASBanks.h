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


struct DDASGEBHeader{  //only at the start of the event...
  //unsigned int size; // Size of DDAS packet, inclusive, number of 16-bit words
  long         time1; // time ??
  unsigned int source_id1; // source id
  unsigned int size;      // Actually size1, to satisfy TDDASEvent's requirements
  unsigned int frequency; // Actually barrier1, to satisfy TDDASEvent's requirements
  unsigned int adc_bits;  // Actually size2, to satisfy TDDASEvent's requirements
  unsigned int type;
  unsigned int revision;  // Actually size3, to satisfy TDDASEvent's requirements
  long         time2;      //duplicate of time1?
  unsigned int source_id2; //duplicate of source id1?
  unsigned int barrier2;   //duplicate of barrier1?
  unsigned int body_words;  //inclusize!
  unsigned int status;
  unsigned int time_low;
  unsigned int time_high_cfd;
  unsigned int energy_tracelength;


  // void print() {
  //   printf("--------------------------------\n");
  //   printf("time1:time2  %lu \t %lu  \n",time1,time2);
  //   printf("sid1:sid2     0x%08x \t 0x%08x \n",source_id1,source_id2);
  //   printf("sz1:sz2:sz3   0x%08x \t 0x%08x \t 0x%08x \n",size1,size2,size3);
  //   printf("br1:br2       0x%08x \t 0x%08x \n",barrier1,barrier2);
  //   printf("type:         0x%08x\n",type);
  //   printf("body words:   0x%08x\n",body_words);
  //   printf("status:       0x%08x\n",status);
  //   printf("wd1:wd2       0x%08x \t 0x%08x \n",word1,word2);
  //   printf("wd3:wd4       0x%08x \t 0x%08x \n",word3,word4);
  //   printf("wd5:wd6       0x%08x \t 0x%08x \n",word5,word6);
  //   printf("--------------------------------\n");
  // }

} __attribute__((__packed__));




#endif /* _DDASBANKS_H_ */
