#include "JanusDataFormat.h"

#include <iomanip>

#include "Globals.h"

std::ostream& operator<<(std::ostream& os, const VMUSB_Header& data) {
  os << "VMUSB Header: size = " << data.size() << "\tstack = " << data.stack();
  return os;
}

std::ostream& operator<<(std::ostream& os, const CAEN_DataPacket& data) {
  if(data.entry_type() == CAEN_DataPacket::Event) {
    os << "EntryType: " << data.entry_type() << "\t"
       << "Card: " << std::setfill(' ') << std::setw(2) << data.card_num() << "\t"
       << "Channel: " << data.channel_num() << "\t"
       << "Value: " << data.adcvalue() << "\t"
       << "Underflow: " << data.underflow() << "\t"
       << "Overflow: " << data.overflow();
  } else if (data.entry_type() == CAEN_DataPacket::Begin){
    os << DYELLOW
       << "EntryType: " << data.entry_type() << "\t"
       << "Card: " << std::setfill(' ') << std::setw(2) << data.card_num() << "\t"
       << "Crate: " << data.crate_num()
       << RESET_COLOR;
  } else if (data.entry_type() == CAEN_DataPacket::End) {
    os << MAGENTA
       << "EntryType: " << data.entry_type() << "\t"
       << "Card: " << std::setfill(' ') << std::setw(2) << data.card_num() << "\t"
       << "Evt Counter: " << data.event_counter()
       << RESET_COLOR;
  } else {
    os << RED
       << "EntryType: " << data.entry_type() << " (Invalid)"
       << RESET_COLOR;
  }
  return os;
}
