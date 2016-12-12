
#include <TLenda.h>

#include <TRawEvent.h>
#include <DDASDataFormat.h>
#include <DDASBanks.h>

ClassImp(TLenda)


int TLenda::BuildHits(std::vector<TRawEvent>& raw_data) {
  for(auto& event : raw_data){
    SetTimestamp(event.GetTimestamp());

    TSmartBuffer buf = event.GetPayloadBuffer();

    int total_size = *(int*)buf.GetData();
    const char* buffer_end = buf.GetData() + total_size;
    buf.Advance(sizeof(int));
    //int ptr = sizeof(int);

    while(buf.GetData() < buffer_end) {
      TLendaHit hit;

      // Constructor advances the buffer to end of each channel
      TDDASEvent<DDASGEBHeader> ddas(buf);

      unsigned int address = 0x12341234;

      hit.SetAddress(address);
      hit.SetCharge(ddas.GetEnergy());
      hit.SetTime(ddas.GetCFDTime());
      hit.SetTimestamp(ddas.GetTimestamp());

      //InsertHit(hit);
      lenda_hits.push_back(hit);
    }


  }
  fSize = lenda_hits.size();

  //printf("lenda build hits called\t%u\t%u\n",lenda_hits.size(),Size());
  //fflush(stdout);
  return fSize;
}
