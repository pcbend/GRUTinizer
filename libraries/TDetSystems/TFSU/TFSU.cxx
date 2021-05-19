
#include <TFSU.h>
#include <TNSCLEvent.h>

TFSU::TFSU() { }

TFSU::~TFSU() { }

void TFSU::Clear(Option_t *opt) { 
  TDetector::Clear();  

  fEnergy.Clear();
  fDeltaE.Clear();

  fFSUHits.clear();
}

void TFSU::Print(Option_t *opt) const { 
  TString sopt(opt);
  sopt.ToLower();
  printf("+++++++++++++\n");
  printf("TFSU @ %lu \n",Timestamp());
  printf("dE: %i\t%ld\n",fDeltaE.Charge(),fDeltaE.Timestamp());
  printf("E:  %i\t%ld\n",fEnergy.Charge(),fEnergy.Timestamp());
  printf("contains %lu hits\n",fFSUHits.size());
  if(sopt.Contains("all")) { 
    for(unsigned int i=0;i<fFSUHits.size();i++) 
      fFSUHits.at(i).Print("all");
  }
  printf("+++++++++++++\n");
  fflush(stdout);
}


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
    hit.SetAddress((*((Int_t*)(nscl.GetBody()+4)))&0x00000fff); 
    TChannel *channel = TChannel::GetChannel(hit.Address());
    if(!channel) continue;
    int charge = *((Int_t*)(nscl.GetBody()+16));  
    hit.SetCharge(charge&0x0000ffff);
    //hit.Print();
    //InsertHit(hit);
    if(!strcmp(channel->GetName(),"E")) {
      hit.Copy(fEnergy);
    } else if(!strcmp(channel->GetName(),"dE")) {
      hit.Copy(fDeltaE);
    } else {
     fFSUHits.push_back(hit);
    }
  }
  //printf(RED "I built %i hits!!" RESET_COLOR "\n", fFSUHits.size());
  SetTimestamp(smallesttime);
  //:w
  //Print("all"); 
 
  return fFSUHits.size();
}



