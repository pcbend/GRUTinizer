#ifndef _DDASDATAFORMAT_H_
#define _DDASDATAFORMAT_H_

#include <ostream>

#include "TObject.h"

#include "TSmartBuffer.h"

#define CHANNELIDMASK             0xF  // Bits 0-3 inclusive
#define SLOTIDMASK               0xF0  // Bits 4-7 inclusive
#define CRATEIDMASK             0xF00  // Bits 8-11 inclusive
#define HEADERLENGTHMASK      0x1F000  // Bits 12-16 inclusive
#define CHANNELLENGTHMASK  0x3FFE0000  // Bits 17-29 inclusive
#define OVERFLOWMASK       0x40000000  // Bit 30 has overflow information (1 - overflow)
#define FINISHCODEMASK     0x80000000  // Bit 31 has pileup information (1 - pileup)
#define LOWER16BITMASK         0xFFFF  // Lower 16 bits
#define UPPER16BITMASK     0xFFFF0000  // Upper 16 bits
#define LOWER12BITMASK          0xFFF  // Lower 12 bits
#define BIT31MASK          0x80000000  // Bit 31
#define BIT30MASK          0x40000000  // Bit 30
#define BIT30TO29MASK      0x60000000  // Bits 30 through 29
#define BIT31TO29MASK      0xE0000000  // Bits 31 through 29
#define BIT30TO16MASK      0x7FFF0000  // Bits 30 through 16
#define BIT29TO16MASK      0x3FFF0000  // Bits 29 through 16
#define BIT28TO16MASK      0x1FFF0000  // Bits 28 through 16

#include "DDASBanks.h"

template<typename HeaderType>
class TDDASEvent : public TObject {
public:
  TDDASEvent(TSmartBuffer& buf)
    : qdc_sum(NULL), energy_sum(NULL), trace(NULL),
      header(NULL), buf(buf) {
    header = (HeaderType*)buf.GetData();
    buf.Advance(sizeof(HeaderType));

    if(HasQDCSum()){
      qdc_sum = (DDAS_QDC_Sum*)buf.GetData();
      buf.Advance(sizeof(DDAS_QDC_Sum));
    }

    if(HasEnergySum()){
      energy_sum = (DDAS_Energy_Sum*)buf.GetData();
      buf.Advance(sizeof(DDAS_Energy_Sum));
    }

    if(GetTraceLength()){
      trace = (unsigned short*)buf.GetData();
      buf.Advance(GetTraceLength()*sizeof(unsigned short));
    }
  }

  DDAS_QDC_Sum* qdc_sum;
  DDAS_Energy_Sum* energy_sum;
  unsigned short* trace;

  bool HasQDCSum() const {
    return (GetChannelHeaderLength() == 8 ||
            GetChannelHeaderLength() == 16);
  }


  bool HasEnergySum() const {
    return (GetChannelHeaderLength() == 12 ||
            GetChannelHeaderLength() == 16);
  }

  unsigned int GetSize()        const { return header->size; }
  unsigned short GetFrequency() const { return header->frequency; }
  unsigned char GetADCBits()    const { return header->adc_bits; }
  unsigned char GetRevision()   const { return header->revision; }

  int GetChannelID()               const { return (header->status & CHANNELIDMASK)     >> 0;           }
  int GetSlotID()                  const { return (header->status & SLOTIDMASK)        >> 4;           }
  int GetCrateID()                 const { return (header->status & CRATEIDMASK)       >> 8;           }
  int GetChannelHeaderLength()     const { return (header->status & HEADERLENGTHMASK)  >> 12;          }
  int GetChannelLength()           const { return (header->status & CHANNELLENGTHMASK) >> 17;          }
  int GetOverflowCode()            const { return (header->status & OVERFLOWMASK)      >> 30;          }
  int GetFinishCode()              const { return (header->status & FINISHCODEMASK)    >> 31;          }

  unsigned int GetTimeLow()        const { return header->time_low;                                    }
  unsigned int GetTimeHigh()       const { return (header->time_high_cfd & LOWER16BITMASK);            }
  unsigned long GetTimestamp()     const {
    return (((unsigned long)GetTimeHigh())<<32) + GetTimeLow();
  }
  int GetCFDFailBit()              const { return (header->time_high_cfd & BIT31MASK)     >> 31;       }
  int GetCFDTime()                 const { return (header->time_high_cfd & BIT30TO16MASK) >> 16;       }

  int GetEnergy()                  const { return (header->energy_tracelength & LOWER16BITMASK);       }
  int GetTraceLength()             const { return (header->energy_tracelength & UPPER16BITMASK) >> 16; }

  friend std::ostream& operator<<(std::ostream& out, const TDDASEvent& event) {
    out << "DDAS Channel: \n";
    return out;
  }

private:
  HeaderType* header;

  TSmartBuffer buf;

  ClassDef(TDDASEvent, 0);
};





#endif /* _DDASDATAFORMAT_H_ */
