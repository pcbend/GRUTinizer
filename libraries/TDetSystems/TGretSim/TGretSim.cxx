#include <fstream>
#include <string>
#include <sstream>

#include "TGretSim.h"

#include "TGEBEvent.h"

TGretSim::TGretSim(){
  Clear();
}

TGretSim::~TGretSim() {}

void TGretSim::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TGretSim& gretsim = (TGretSim&)obj;
  gretsim.gretsim_hits = gretsim_hits; 
}

void TGretSim::InsertHit(const TDetectorHit& hit){
  gretsim_hits.emplace_back((TGretSimHit&)hit);
  //  fSize++;
}

int TGretSim::BuildHits(std::vector<TRawEvent>& raw_data){
  /*  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    errors+=Build_From(nscl,true);
  }
  return Size();
  */
  for(auto& event : raw_data){
    TGEBEvent& geb = (TGEBEvent&)event;
    BuildFrom(geb);
  }
  return 0;
}

void TGretSim::Print(Option_t *opt) const { }

void TGretSim::Clear(Option_t *opt) {
  TDetector::Clear(opt);
  gretsim_hits.clear(); 
}

void TGretSim::BuildFrom(TGEBEvent &event){
  //std::cout << "In build from for gret sim ----- " << std::endl;
  //event.Print("all");
  const char* data = event.GetPayload();

  TRawEvent::G4SimPacket* packet = (TRawEvent::G4SimPacket*)data;
  //  std::cout << *packet;
 
 
  for(int i =0;i<packet->head.GetNum();i++){
    TGretSimHit hit;
    hit.fEnergy = packet->data[i].GetEn(); 
    hit.fBeta = packet->data[i].GetBeta(); 
    hit.fInteraction.SetXYZ(packet->data[i].GetX(),packet->data[i].GetY(),packet->data[i].GetZ()); 
    hit.fPosit.SetMagThetaPhi(1.0,packet->data[i].GetTheta(),packet->data[i].GetPhi()); 
    hit.fIsFull = packet->head.GetFull();
    hit.fTotalHits = packet->head.GetNum();
    hit.fHitNum = i;

    gretsim_hits.push_back(hit);
  }


}
