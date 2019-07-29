#include "TBank88.h"

#include "TGEBEvent.h"
#include "TGRUTOptions.h"

ClassImp(TBank88)

TBank88::TBank88(){
}

TBank88::~TBank88() {
}

void TBank88::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TBank88& bank = (TBank88&)obj;
  bank.channels = channels;
}

void TBank88::InsertHit(const TDetectorHit& hit){
  channels.emplace_back((TMode3Hit&)hit);
  fSize++;
}

int TBank88::BuildHits(std::vector<TRawEvent>& raw_data){
  for(auto& event : raw_data){
    //event.Print("all");
    TGEBEvent* geb = (TGEBEvent*)&event;
    SetTimestamp(event.GetTimestamp()); //  geb->GetTimestamp());
    TMode3Hit hit;
    TSmartBuffer buf = geb->GetPayloadBuffer();
    hit.BuildFrom(buf);
    //hit.SetTimestamp(event.GetTimestamp());  //the ts is now set from the hit led in the build
    InsertHit(hit);
    //std::cout << "----------------------\n";
    //std::cout <<"event  " << event.GetTimestamp() <<"\t"<< hit.Timestamp()<<"\tdt: "<< event.GetTimestamp() -hit.Timestamp() << std::endl;
    //std::cout <<"gevent  " << geb->GetTimestamp() <<"\t"<< hit.Timestamp()<<"\tdt: "<< geb->GetTimestamp() -hit.Timestamp() << std::endl;
    //std::cout <<"bank   " <<Timestamp() <<"\t"<< hit.Timestamp()<<"\tdt: "<< Timestamp() -hit.Timestamp() << std::endl;
    //std::cout << "----------------------\n";
  }
  //if(Size()) {
  //  SetTimestamp(channels.at(0).GetLed());
  //}
  
  
  return Size();
}

void TBank88::Print(Option_t *opt) const { 
  //printf("----- Bank 88 @ %lu -------\n",Timestamp());
  std::cout << "----- Bank 88 @ " << Timestamp() << "-------\n"; //,Timestamp());
  printf("--------- size %lu --------------\n",Size());
  for(size_t i=0;i<Size();i++) {
    //ichannels.at(i).Print();
    printf("\t%lu:  0x%08x  %i  %lu  ",i,
             channels.at(i).Address(),
             channels.at(i).Charge(),
             channels.at(i).Timestamp());
    
  }    
  printf("-------------------------------\n");

}

void TBank88::Clear(Option_t *opt) {
  TDetector::Clear(opt);
  channels.clear();
}
