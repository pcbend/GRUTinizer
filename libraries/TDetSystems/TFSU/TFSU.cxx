
#include <TFSU.h>
#include <TNSCLEvent.h>

TFSU::TFSU() { }

TFSU::~TFSU() { }

void TFSU::Clear(Option_t *opt) { }

void TFSU::Print(Option_t *opt) const { }


void TFSU::Copy(TObject& obj) const {
  TDetector::Copy(obj);
  
  TNamed::Copy(obj);

  TFSU& fsu = (TFSU&)obj;
  fsu.fFSUHits = fFSUHits;

}


int TFSU::BuildHits(std::vector<TRawEvent>& raw_data) { 
  Long_t smallesttime = 0x0ffffffffffff;
  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    if(nscl.GetTimestamp()<smallesttime)
      smallesttime = nscl.GetTimestamp(); 
    //nscl.Print("all");
    TFSUHit hit;
    hit.SetTimestamp(nscl.GetTimestamp());
    hit.SetAddress(*((Int_t*)(nscl.GetBody()+4))); 
    int charge = *((Int_t*)(nscl.GetBody()+16));  
    hit.SetCharge(charge&0x0000ffff);
    //hit.Print();
    //InsertHit(hit);
    fFSUHits.push_back(hit);
  }
  //printf(RED "I built %i hits!!" RESET_COLOR "\n", fFSUHits.size());
  SetTimestamp(smallesttime);

  return fFSUHits.size();
}



