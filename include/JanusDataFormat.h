#ifndef _TJANUSDATAFORMAT_H_
#define _TJANUSDATAFORMAT_H_

#include <cstdint>
#include <iostream>

struct VMUSB_Header {
  uint16_t data;

  // Size in 16-bit words, not self-inclusive.
  int size()  const { return (data & 0x0fff)>>0; }
  int stack() const { return (data & 0xf000)>>12; }
} __attribute__((packed));

std::ostream& operator<<(std::ostream& os, const VMUSB_Header& data);

struct CAEN_DataPacket{
  uint16_t tail, head;

  enum EntryType{ Begin=0x2, Event=0x0, End=0x4, Invalid=0x6 };

  int card_num()         const  { return           (head & 0xf800)>>11; }
  EntryType entry_type() const  { return EntryType((head & 0x0700)>>8); }
  int crate_num()        const  { return           (head & 0x00ff)>>0;  }
  int header_zeros()     const  { return           (tail & 0xc000)>>13; }
  int num_entries()      const  { return           (tail & 0x3f00)>>8;  }
  int trailing_stuff()   const  { return           (tail & 0x00ff)>>0;  }
//  int channel_num()      const  { return           (head & 0x003f)>>0;  }
  int channel_num()      const  { return           (head & 0x001f)>>0;  }
  int adcvalue()         const  { return           (tail & 0x0fff)>>0;  }
  bool overflow()        const  { return            tail & (1<<12);     }
  bool underflow()       const  { return            tail & (1<<13);     }

  int event_counter() const {
    uint32_t low_bits = tail;
    uint32_t high_bits = head & 0x00ff;
    return low_bits + (high_bits << 16);
  }

  bool IsValid() const { return entry_type() == Event; }
} __attribute__((packed));

std::ostream& operator<<(std::ostream& os, const CAEN_DataPacket& data);

struct VME_Timestamp{
  uint16_t buffer[6];

  long ts1() const { return buffer[0] + (long(buffer[1])<<16) + (long(buffer[4])<<32); }
  long ts2() const { return buffer[2] + (long(buffer[3])<<16) + (long(buffer[5])<<32); }
} __attribute__((packed));

#endif /* _TJANUSDATAFORMAT_H_ */


// 0x2a00 = card 5, begin
// 0x2c00 = card 5, end
// 0x3200 = card 6, begin
// 0x3400 = card 6, end
// 0x3a00 = card 7, begin
// 0x3c00 = card 7, end
// 0x4200 = card 8, begin
// 0x4400 = card 8, begin
// 0x4a00 = card 9, begin
// 0x4c00 = card 9, end
// 0x5200 = card 10, begin
// 0x5400 = card 10, end
// 0x5a00 = card 11, begin
// 0x5c00 = card 11, end
// 0x6200 = card 12, begin
// 0x6400 = card 12, end
