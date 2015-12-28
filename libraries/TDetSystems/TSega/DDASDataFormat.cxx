#include "DDASDataFormat.h"

ClassImp(TDDASEvent)

TDDASEvent::TDDASEvent(const TSmartBuffer& buf)
  : qdc_sum(NULL), energy_sum(NULL), trace(NULL),
    header(NULL), buf(buf) {
  const char* data = buf.GetData();

  header = (DDASHeader*)data;
  data += sizeof(DDASHeader);

  if(HasQDCSum()){
    qdc_sum = (DDAS_QDC_Sum*)data;
    data += sizeof(DDAS_QDC_Sum);
  }

  if(HasEnergySum()){
    energy_sum = (DDAS_Energy_Sum*)data;
    data += sizeof(DDAS_Energy_Sum);
  }

  if(GetTraceLength()){
    trace = (unsigned short*)data;
  }
}


bool TDDASEvent::HasQDCSum() const {
  return (GetChannelHeaderLength() == 8 ||
          GetChannelHeaderLength() == 16);
}

bool TDDASEvent::HasEnergySum() const {
  return (GetChannelHeaderLength() == 12 ||
          GetChannelHeaderLength() == 16);
}

std::ostream& operator<<(std::ostream& out, const TDDASEvent& event){
  out << "DDAS Channel: \n";
  return out;
}
