
#include <TFSU.h>
#include <TNSCLEvent.h>


TFSU::TFSU() { }

TFSU::TFSU(const TFSUHit& other) {
  other.Copy(*this);
}

TFSU::~TFSU() { }

void TFSU::Clear(Option_t *opt) { 
  TDetector::Clear();  

  fEnergy.Clear();
  fDeltaE.Clear();

  fFSUHits.clear();
  fAddbackHits.clear();
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
  if(sopt.Contains("addback")) { 
    for(unsigned int i=0;i<fAddbackHits.size();i++) 
      fAddbackHits.at(i).Print("all");
  }
  printf("+++++++++++++\n");
  fflush(stdout);
}


void TFSU::Copy(TObject& obj) const {
  TDetector::Copy(obj);
  
  TNamed::Copy(obj);

  TFSU& fsu = (TFSU&)obj;
  
  fsu.fDeltaE.fCharge    = GetDeltaE().Charge();
  fsu.fDeltaE.fAddress   = GetDeltaE().Address();
  fsu.fDeltaE.fTimestamp = GetDeltaE().Timestamp();

  fsu.fEnergy.fCharge    = GetE().Charge();
  fsu.fEnergy.fAddress   = GetE().Address();
  fsu.fEnergy.fTimestamp = GetE().Timestamp();

  //fsu.fDeltaE.Copy(fDeltaE);
  //fsu.fEnergy.Copy(fEnergy);


  //for(size_t x=0;x<Size();x++) {
  //  fsu.fFSUHits.push_back(TFSUHit(fFSUHits.at(x)));
  //}
  fsu.fFSUHits = fFSUHits;
  fsu.fAddbackHits = fAddbackHits;

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
    hit.SetTime(nscl.GetTimestamp());
    hit.SetAddress((*((Int_t*)(nscl.GetBody()+4)))&0x00000fff); 
    //TChannel *channel = TChannel::GetChannel(hit.Address());
    //if(!channel) continue;
    int charge = *((Int_t*)(nscl.GetBody()+16));  
    hit.SetCharge(charge&0x0000ffff);
    //hit.Print();
    //InsertHit(hit);

    
    
   // if(channel && !strncmp(channel->GetName(),"E",1)) {
   //   hit.Copy(fEnergy);
   // } else if(channel && !strncmp(channel->GetName(),"dE",2)) {
   //   hit.Copy(fDeltaE);
    if(hit.Address()==0x0000002f) {
      hit.Copy(fEnergy);
    } else if(hit.Address()==0x0000002e) {
      hit.Copy(fDeltaE);
    } else {
     fFSUHits.push_back(hit);
     //fFSUHits.back().Copy(hit);
     //fFSUHits.back().Print();
    }
  }
  //printf(RED "I built %i hits!!" RESET_COLOR "\n", fFSUHits.size());
  SetTimestamp(smallesttime);
  //:w
  //Print("all"); 
 
  return fFSUHits.size();
}

//int TFSU::CleanHits(TCutG *timing,TCutG *pp_timing) {
int TFSU::CleanHits(double low,double high,double timediff) {

  std::vector<TFSUHit>::iterator it;
  for(it=fFSUHits.begin();it!=fFSUHits.end(); ) {
    bool deleted = false;
    if(it->GetEnergy()<low||it->GetEnergy()>high) {
      it = fFSUHits.erase(it);
      deleted=true;
    }
    if(!deleted && fabs(GetDeltaE().Timestamp()-it->Timestamp())>timediff)   {
      it = fFSUHits.erase(it);
      deleted = true;
    } 
    if(!deleted) {
      it++;
    } 
  }
  return fFSUHits.size();
}


int TFSU::WriteToEv2(const char *filename) const {

  if(!fFSUHits.size()) return 0;

  int output[255];
  int adc = fFSUHits.size()+2; // number of hpge that fired + dE and E
  int index=0;
  //printf("((((GetDeltaE().Id())&0xf0)>>4)-3) = 0%08x\n",((((GetDeltaE().Id())))));

  output[index++] = adc;
  if(GetDeltaE().Id()==-1) {
    output[index++] = 15;
    output[index++] = 0;
  } else {
    output[index++] = 15;
    if(GetDeltaE().Charge()<0) 
      output[index++] = 0;
    else 
      output[index++] = GetDeltaE().Charge();
  }

  if(GetE().Id()==-1) {
    output[index++] = 16;
    output[index++] = 0;
  } else {
    output[index++] = 16;
    if(GetDeltaE().Charge()<0) 
      output[index++] = 0;
    else 
      output[index++] = GetE().Charge();
  }

 for(unsigned int i=0;i<fFSUHits.size();i++) {
    TFSUHit hit     = fFSUHits.at(i); 
    if(!TChannel::GetChannel(hit.Address())) continue;
    output[index++] = ((((hit.Id()&0xf0)>>4)-2))*16 + ((hit.Id())&0xf) +1;

    //if(output[index]<0) hit.Print();

    output[index++] = hit.Charge(); 
  }
  output[index++] = adc;

  for(int j=0;j<index;j++) {
    printf("%i ",output[j]);
  }
  printf("\n\n");
  return index-1;
}



int TFSU::MakeAddbackHits() {
  if(!fFSUHits.size()) return 0;
  //Print("all");
  //std::sort(fFSUHits.begin(),fFSUHits.end());
  OrderHits();

  TFSUHit cl1;  cl1.SetCharge(0.0);
  TFSUHit cl2;  cl2.SetCharge(0.0);
  TFSUHit cl3;  cl3.SetCharge(0.0);

  TFSUHit F6;   F6.SetCharge(0.0);
  TFSUHit K11;  K11.SetCharge(0.0);
  TFSUHit J10;  J10.SetCharge(0.0);

  for(int i=0;i<fFSUHits.size();i++) {
    TFSUHit hit = fFSUHits.at(i); 
    TFSUHit *clover = 0;
    if(hit.GetNumber()<=4) {
      clover = &cl1; 
    } else if(hit.GetNumber()>=9 && hit.GetNumber()<=12) { 
      clover = &F6; 
    } else if(hit.GetNumber()>=17 && hit.GetNumber()<=20) { 
      clover = &cl2; 
    } else if(hit.GetNumber()>=25 && hit.GetNumber()<=28) { 
      clover = &K11; 
    } else if(hit.GetNumber()>=33 && hit.GetNumber()<=36) { 
      clover = &cl3; 
    } else if(hit.GetNumber()>=41 && hit.GetNumber()<=44) { 
      clover = &J10; 
    } else {
      fAddbackHits.push_back(hit);
    }
    if(clover) {
      clover->SetAddress(hit.Address());
      clover->SetTimestamp(hit.Timestamp());
      clover->AddEnergy(hit.GetEnergy());
      //clover->Print();
    }

  }

  if(cl1.GetEnergy()>5.0) { fAddbackHits.push_back(cl1); }
  if(cl2.GetEnergy()>5.0) { fAddbackHits.push_back(cl2); }
  if(cl3.GetEnergy()>5.0) { fAddbackHits.push_back(cl3); }
  if(F6.GetEnergy()>5.0)  { fAddbackHits.push_back(F6); }
  if(K11.GetEnergy()>5.0) { fAddbackHits.push_back(K11); }
  if(J10.GetEnergy()>5.0) { fAddbackHits.push_back(J10); }

  //std::cout << "fFSUHits.size()     = " << fFSUHits.size() << std::endl;
  //std::cout << "fAddbackHits.size() = " << fAddbackHits.size() << std::endl;
  //Print("addback");
  //std::cout<< std::endl<<std::endl << std::endl;

  return fAddbackHits.size();
}

