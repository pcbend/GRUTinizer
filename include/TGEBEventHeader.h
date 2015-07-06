
#ifndef TGEBEVENTHEADER__H
#define TGEBEVENTHEADER__H

#include <iostream>
#include <iomanip>

struct TGEBEventHeader { 
   Int_t  type;       //int32_t
   Int_t  size;       //int32_t
   Long_t  timestamp; //int64_t
}__attribute__((__packed__));





inline std::ostream& operator<<(std::ostream& os, const TGEBEventHeader &head) {
   //time_t t = (time_t)head.timestamp;
   return os << "Bank @ " << std::hex << &head << std::dec << std::endl
             << "\ttype      = " << std::setw(8) << head.type << std::endl 
             << "\tsize      = " << std::setw(8) << head.size << std::endl
             << "\ttimestamp = " << head.timestamp << std::endl;
};



#endif


