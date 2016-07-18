#include "TFastScint.h"

#include <cassert>
#include <iostream>

#include "TNSCLEvent.h"
#include "TH2.h"
#include "TMath.h"

int TFastScint::errors;

TFastScint::TFastScint() {
  //fs_hits = new TClonesArray("TFastScintHit");
  //Clear();
}

TFastScint::~TFastScint(){
  //delete fs_hits;
}

void TFastScint::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TFastScint& fs = (TFastScint&)obj;
  //fs_hits->Copy(*fs.fs_hits);
  fs.fs_hits = fs_hits;
  fs.trig_time = trig_time;
  //fs.raw_data.clear();
}

void TFastScint::Clear(Option_t* opt){
  TDetector::Clear(opt);

  //tdc_trigger = -1;
  //qdc_channels = -1;
  //tdc_channels = -1;
  //fs_hits->Clear(opt);
  fs_hits.clear();
  trig_time = -1;
}

void TFastScint::Print(Option_t *opt) const { 
  printf("TFastScint @  %lu \n",Timestamp());
  for(unsigned int x=0;x<Size();x++) {
    printf("\t");
    GetLaBrHit(static_cast<int>(x)).Print();
  }
}



int TFastScint::BuildHits(std::vector<TRawEvent>& raw_data){
  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    errors+=Build_From(nscl,true);
  }
  return (int)Size();
}

void TFastScint::InsertHit(const TDetectorHit& hit){
  //TFastScintHit* new_hit = (TFastScintHit*)fs_hits->ConstructedAt(Size());
  //hit.Copy(*new_hit);
  fs_hits.push_back((TFastScintHit)hit);
}

unsigned int TFastScint::Size() const {
  return fs_hits.size(); //->GetEntries();
}


int TFastScint::GoodSize() const {
  int size=0;
  for(unsigned int i =0;i<Size();i++) {
    if(fs_hits[i].Charge()>100)
      size++;
  }

  return  size; // fs_hits.size(); //->GetEntries();
}

TFastScintHit TFastScint::GetLaBrHit(int i) const {
  const TFastScintHit hit = fs_hits.at(i);
  return hit;
}

int TFastScint::GetDetNumberIn_fs_hits(Int_t det){
  int detNumber = -1;
  for(unsigned int i_hit = 0; i_hit < Size(); i_hit++){
    TFastScintHit& temp_hit = fs_hits.at(i_hit);
    if(temp_hit.GetChannel()==det)
      detNumber=i_hit;
  }
  return detNumber;
}

TFastScintHit *TFastScint::FindHit(int channelnumber) {
  for(unsigned int i=0;i<Size();i++) {
    if(fs_hits.at(i).GetChannel()==channelnumber)
      return &fs_hits[i];
  }
  return 0;
}


TDetectorHit& TFastScint::GetHit(int i){
  return fs_hits.at(i);
}

// This is where the unpacking happens:
int TFastScint::Build_From(TNSCLEvent &event,bool Zero_Suppress){
  bool DEBUG = false;
  Zero_Suppress = true;
  bool isQ = false;
  bool isT = false;

  Int_t words_processed = 0;
  //Int_t detNumber = -1;

  // Payload size in 32 bit words.
  Int_t PayloadSize = event.GetPayloadSize()/4.0;

  if(PayloadSize == 1)
    return 1;

  const char* data = event.GetPayload();

  // NOTE -> buffer_size is 2*PayloadSize.
  // UShort_t buffer_size = *((UShort_t*)(data));
  data+=2;

  if(DEBUG){
    std::cout << "===========================================================================================>" << std::endl;
    event.Print("all");
    std::cout << " Payload Size : " << PayloadSize << std::endl;
  }


  for(Int_t i = 0; i <PayloadSize; i++ ){ 
    const TRawEvent::Mesy_Word* Mword = (TRawEvent::Mesy_Word*)data;
    data+=sizeof(TRawEvent::Mesy_Word); words_processed++;

    if(Mword->isHeader()){ // Header
      const TRawEvent::Mesy_Header* Mhead = (TRawEvent::Mesy_Header*)Mword;
      if(Mhead->isQDC()){
        isQ = true; isT = false;
      }
      else if(Mhead->isTDC()){
        isQ = false; isT = true;
      }
      else std::cout << " *** Error -- Not QDC or TDC *** " << std::endl; 

    }// end if is header
    else if(Mword->isData()){ // Data
      if(isQ){ // QDC:
        // NOTE -> we do it this way bc QDC should always come first!!!!
        const TRawEvent::M_QDC_Data* Mq = (TRawEvent::M_QDC_Data*)Mword;
        TFastScintHit *hit = FindHit(Mq->Chan());
	if(!hit){
	  TFastScintHit qdc_hit;
	  qdc_hit.SetChannel(Mq->Chan());
	  qdc_hit.SetTime(-1);
	  qdc_hit.SetAddress((12<<28) + qdc_hit.GetChannel());
	  
	  if(Mq->isOOR()) qdc_hit.SetCharge(5000);
	  else            qdc_hit.SetCharge(Mq->Charge());
	  
	  //hit.SetEnergy();
	  
	  if(Zero_Suppress){
	    if(Mq->Charge()>0){
	      InsertHit(qdc_hit);
	    }
	  }
	  else{
	    InsertHit(qdc_hit);
	  }
	} else{
	  if(Mq->isOOR()) hit->SetCharge(5000);
	  else            hit->SetCharge(Mq->Charge());
	}

        // if(DEBUG){
        //   std::cout << " QDC Q : " << std::hex << Mq->Charge() << std::endl;
        //   std::cout << " QDC ch: " << std::dec << Mq->Chan() << std::endl;
        //   std::cout << " QDC Ov: " << Mq->isOOR() << std::endl;
        // }
      } // end if isQ
      else if(isT){ // TDC
        const TRawEvent::M_TDC_Data* Mt = (TRawEvent::M_TDC_Data*)Mword;

        //detNumber = -1;
        //detNumber = GetDetNumberIn_fs_hits(Int_t(Mt->Chan()));

        TFastScintHit *hit = FindHit(Mt->Chan());
        if(!hit) {
          TFastScintHit tdc_hit;
          tdc_hit.SetTime(Mt->Time());
          tdc_hit.SetCharge(-1);
	  
          tdc_hit.SetChannel(Mt->Chan());
          tdc_hit.SetAddress((12<<28) + tdc_hit.GetChannel());
	  
          if(Zero_Suppress){
            if(Mt->Time()>0){
              InsertHit(tdc_hit);
            }
          }
          else {
	    //hit->SetTime(Mt->Time());
	    InsertHit(tdc_hit);
          }
          if(DEBUG){
            std::cout << " TDC t : " << std::hex << Mt->Time() << std::endl;
            std::cout << " TDC ch: " << std::dec << Mt->Chan() << std::endl;
            std::cout << " TDC Tr: " << Mt->isTrig() << std::endl;
          }
        } else {
	  TFastScintHit *qdc_hit = hit;
	  if(qdc_hit->GetChannel()==Mt->Chan()){
	    qdc_hit->SetTime(Mt->Time());
	    
	    
	    if(DEBUG){
	      std::cout << " TDC ch         : " << Mt->Chan() << std::endl;
	      std::cout << " TDC for QDC ch : " << qdc_hit->GetChannel() << std::endl;
	      std::cout << " TDC time       : " << Mt->Time() << std::endl;
	      std::cout << " QDC Charge     : " << qdc_hit->Charge() << std::endl;
	      std::cout << " Time Check     : " << qdc_hit->Time() << std::endl;
	    }
	  }
	  else{ // should never be here.
	    std::cout << " *** Found a Channel but it didnt match ?? *** " << std::endl;
	  }
	}
	if(Mt->isTrig()){  
	  SetTrigTime(Mt->Time());
	  continue;
	}
	
      } // end elif isT	

      else{ // If not QDC or TDC

        std::cout << " *** Not TDC or QDC **** " << std::endl;
      }
    }// end elif is data
    else if(Mword->isETS()){
      if(DEBUG)
        std::cout << " ETS " << std::endl;
    }//end elif ets
    else if(Mword->isFILL()){
      if(DEBUG)
        std::cout << " Filler " << std::endl;
      else if(Mword->isEOE()){
        if(DEBUG) std::cout << " EOE " << std::endl;
      }// end elif eoe    
    }
  }// end for

  if(Zero_Suppress) {
    std::vector<TFastScintHit>::iterator it;
    for(it=fs_hits.begin();it!=fs_hits.end();) {
      if( (it->Charge()<100) || (it->Time()<0) ) {
        it = fs_hits.erase(it);
      } else {
        it++;
      }
    }
  }

  // TFastScintHit *CheckHit = FindHit(3);
  // std::cout << " ------- Double Check Here -----------" << std::endl;
  // std::cout << " Channel = " << CheckHit->GetChannel() << std::endl;
  // std::cout << " Time = " << CheckHit->GetTime() << std::endl;
  // std::cout << " Energy = " << CheckHit->GetEnergy() << std::endl;
  
  return 0;
}

TVector3 &TFastScint::GetPosition(int detector) { 
  static std::map<int,TVector3> fFastScintDetectorMap;
  if(fFastScintDetectorMap.size()==0) {
    fFastScintDetectorMap[16] = TVector3(0,0,1);
  }
  if(detector>15) {
    fprintf(stderr,"%s, detector out of range.\n",__PRETTY_FUNCTION__);
    detector=16;
  }
  if(fFastScintDetectorMap.count(detector)==0) {
    double phi = TMath::Pi()/2;
    double theta = TMath::Pi()/2 - TMath::ATan(24.13/139.7);
    //double z   = 24.13;
    if(detector<8) {
      phi = phi + TMath::Pi()/8. + static_cast<double>(detector)*TMath::Pi()/4.;
    } else {
      theta = -theta;
      //z = -24.13;
      phi = phi + static_cast<double>(detector-8)*TMath::Pi()/4.;
    }
    if(phi>2*TMath::Pi())
      phi -= 2*TMath::Pi();
    double mag = TMath::Sqrt(139.7*139.7 + 24.13*24.13);
   
    printf("\t\tmag   = %f\n",mag);
    printf("\t\ttheta = %f\n",theta*TMath::RadToDeg());
    printf("\t\tphi   = %f\n",phi*TMath::RadToDeg());

    TVector3 v;
    v.SetMagThetaPhi(mag,theta,phi);
    fFastScintDetectorMap[detector] = v;
  }
  return fFastScintDetectorMap.at(detector);
}


