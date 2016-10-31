
#include <TLenda.h>

#include <TGEBEvent.h>
#include <DDASBanks.h>

ClassImp(TLenda)


int TLenda::BuildHits(std::vector<TRawEvent>& raw_data) { 
  for(auto& event : raw_data){
    TGEBEvent &geb = (TGEBEvent&)event;
    SetTimestamp(geb.GetTimestamp());

    int total_size = *((int*)event.GetPayload());
    int ptr = sizeof(int);

    while(ptr<total_size) {
      TLendaHit hit;
      DDASGEBHeader *head = (DDASGEBHeader*)(event.GetPayload()+4);
      //head->print();
      ptr+= sizeof(head);
      hit.SetTrace(head->trace_length()/2,(const unsigned short*)(event.GetPayload()+ptr));
      ptr+= 2*head->trace_length();
      //InsertHit(hit);
      lenda_hits.push_back(hit);
    }
    

  }
  fSize = lenda_hits.size();  
  
  //printf("lenda build hits called\t%u\t%u\n",lenda_hits.size(),Size()); 
  //fflush(stdout);
  return fSize;
}


