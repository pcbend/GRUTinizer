#ifndef _TJANUSDATAFORMAT_H_
#define _TJANUSDATAFORMAT_H_

struct VMUSB_Header {
  uint16_t data;

  // Size in 16-bit words, not self-inclusive.
  int size()  const { return (data & 0x0fff)>>0; }
  int stack() const { return (data & 0xf000)>>12; }
} __attribute__((packed));

struct CAEN_ADC{
  uint16_t tail, head;

  enum EntryType{ Begin=0x2, Event=0x0, End=0x4, Invalid=0x6 };

  int card_num()         const  { return           (head & 0xf800)>>11; }
  EntryType entry_type() const  { return EntryType((head & 0x0700)>>8); }
  int crate_num()        const  { return           (head & 0x00ff)>>0;  }
  int header_zeros()     const  { return           (tail & 0xc000)>>13; }
  int num_entries()      const  { return           (tail & 0x3f00)>>8;  }
  int trailing_stuff()   const  { return           (tail & 0x00ff)>>0;  }
  int channel_num()      const  { return           (head & 0x003f)>>0;  }
  int adcvalue()         const  { return           (tail & 0x0fff)>>0;  }
  bool overflow()        const  { return            tail & (1<<12);     }
  bool underflow()       const  { return            tail & (1<<13);     }

  bool IsValid() const { return entry_type() == Event; }
} __attribute__((packed));

struct VME_Timestamp{
  uint16_t buffer[6];

  long ts1() const { return buffer[0] + (long(buffer[1])<<16) + (long(buffer[4])<<32); }
  long ts2() const { return buffer[2] + (long(buffer[3])<<16) + (long(buffer[5])<<32); }
} __attribute__((packed));

#endif /* _TJANUSDATAFORMAT_H_ */
