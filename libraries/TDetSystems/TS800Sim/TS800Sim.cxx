#include <fstream>
#include <string>
#include <sstream>

#include "TS800Sim.h"

#include "TGEBEvent.h"

TS800Sim::TS800Sim(){
  Clear();
}

TS800Sim::~TS800Sim() {}

void TS800Sim::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TS800Sim& s800sim = (TS800Sim&)obj;
  s800sim.s800sim_hits = s800sim_hits; 
}

void TS800Sim::InsertHit(const TDetectorHit& hit){
  s800sim_hits.emplace_back((TS800SimHit&)hit);
}

int TS800Sim::BuildHits(std::vector<TRawEvent>& raw_data){
  for(auto& event : raw_data){
    TGEBEvent& geb = (TGEBEvent&)event;
    BuildFrom(geb);
  }
  return 0;
}

void TS800Sim::Print(Option_t *opt) const { }

void TS800Sim::Clear(Option_t *opt) {
  TDetector::Clear(opt);
  s800sim_hits.clear(); 
}

TVector3 TS800Sim::Track(double sata,double sbta) const {
  //Dividing by 1000 because GRUTinizer uses radians for ATA/BTA
  double ata = TMath::Sin(GetS800SimHit(0).GetATA()/1000.+sata);
  double bta = TMath::Sin(GetS800SimHit(0).GetBTA()/1000.+sbta);
  TVector3 track(ata,-bta,sqrt(1-ata*ata-bta*bta));
  return track;//.Unit();
}


float TS800Sim::AdjustedBeta(float beta) const {
  const TS800SimHit &s800simhit = GetS800SimHit(0);
  double gamma = 1.0/(sqrt(1.-beta*beta));
  double dp_p = gamma/(1.+gamma) * s800simhit.GetDTA();
  beta *=(1.+dp_p/(gamma*gamma));
  return beta;
}

void TS800Sim::BuildFrom(TGEBEvent &event){
  const char* data = event.GetPayload();
  //  std::cout << " IN HERE !!!! " << std::endl;
  TRawEvent::G4S800* s800pack = (TRawEvent::G4S800*)data;
  //  event.Print("all");
  //  std::cout << *s800pack;

  SetTimestamp(event.GetTimestamp());
  
  //  std::cout << "TS800Sim::BuildFrom: DTA =" << s800pack->GetDTA() << std::endl;

  TS800SimHit hit;
  hit.fATA = s800pack->GetATA(); 
  hit.fBTA = s800pack->GetBTA(); 
  hit.fDTA = s800pack->GetDTA(); 
  hit.fYTA = s800pack->GetYTA(); 
  s800sim_hits.push_back(hit);
  //exit(1);


}
