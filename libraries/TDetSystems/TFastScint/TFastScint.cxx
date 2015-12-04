#include "TFastScint.h"

#include <cassert>
#include <iostream>

#include "TNSCLEvent.h"
#include "TH2.h"

int TFastScint::errors;

TFastScint::TFastScint() {
  fs_hits = new TClonesArray("TFastScintHit");
  Clear();
}

TFastScint::~TFastScint(){
  delete fs_hits;
}

void TFastScint::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TFastScint& fs = (TFastScint&)obj;
  fs_hits->Copy(*fs.fs_hits);
  fs.raw_data.clear();
}

void TFastScint::Clear(Option_t* opt){
  TDetector::Clear(opt);

  tdc_TS = -1;
  qdc_TS = -1;
  fs_hits->Clear(opt);
}

int TFastScint::BuildHits(){
  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    errors+=Build_From(nscl,true);
  }
  return Size();
}

void TFastScint::InsertHit(const TDetectorHit& hit){
  TFastScintHit* new_hit = (TFastScintHit*)fs_hits->ConstructedAt(Size());
  hit.Copy(*new_hit);
}

int TFastScint::Size(){
  return fs_hits->GetEntries();
}

TFastScintHit* TFastScint::GetLaBrHit(int i){
  return (TFastScintHit*)fs_hits->At(i);
}

int TFastScint::GetDetNumberIn_fs_hits(Int_t det){
  int detNumber = -1;
  for(int i_hit = 0; i_hit < Size(); i_hit++){
    TFastScintHit* temp_hit = GetLaBrHit(i_hit);
    if(temp_hit->GetChannel()==det)
      detNumber=i_hit;
  }
  return detNumber;
}

TDetectorHit& TFastScint::GetHit(int i){
  return *(TFastScintHit*)fs_hits->At(i);
}

// This is where the unpacking happens:
int TFastScint::Build_From(TNSCLEvent &event,bool Zero_Suppress){

  bool DEBUG = false;

  bool isQ = false;  
  bool isT = false;

  Int_t words_processed = 0;
  UShort_t buffer_size = 0;
  Int_t detNumber = -1;

  // Payload size in 32 bit words.
  Int_t PayloadSize = event.GetPayloadSize()/4.0;
  
  if(PayloadSize == 1)
    return 1;
  
  const char* data = event.GetPayload();
  
  // NOTE -> buffer_size is 2*PayloadSize.
  buffer_size = *((UShort_t*)(data));
  data+=2;

  if(DEBUG){
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
    }
    else if(Mword->isData()){ // Data
      if(isQ){ // QDC:
	// NOTE -> we do it this way bc QDC should always come first!!!!
	TFastScintHit hit;
	const TRawEvent::M_QDC_Data* Mq = (TRawEvent::M_QDC_Data*)Mword;
	hit.SetChannel(Mq->Chan());
	hit.SetTime(-1);
	if(Mq->isOOR()) hit.SetCharge(5000);
	else            hit.SetCharge(Mq->Charge());	
	
	if(Zero_Suppress){
	  if(Mq->Charge()>0){
	    InsertHit(hit);
	  }
	}
	else
	  InsertHit(hit);
	 
	if(DEBUG){
	  std::cout << " QDC Q : " << std::hex << Mq->Charge() << std::endl;
	  std::cout << " QDC ch: " << std::dec << Mq->Chan() << std::endl;
	  std::cout << " QDC Ov: " << Mq->isOOR() << std::endl;
	}
      }
      else if(isT){ // TDC
	const TRawEvent::M_TDC_Data* Mt = (TRawEvent::M_TDC_Data*)Mword;
	
	detNumber = -1;
	detNumber = GetDetNumberIn_fs_hits(Int_t(Mt->Chan()));
	
	if(detNumber!=-1){
	  TFastScintHit *qdc_hit = GetLaBrHit(detNumber);
	  if(qdc_hit->GetChannel()==Mt->Chan()){
	    qdc_hit->SetTime(Mt->Time());
	    
	    if(DEBUG){
	      std::cout << " TDC ch         : " << Mt->Chan() << std::endl;
	      std::cout << " TDC for QDC ch : " << qdc_hit->GetChannel() << std::endl;
	      std::cout << " TDC time       : " << Mt->Time() << std::endl;
	    }
	  }
	  else{ // should never be here.
	    std::cout << " *** Found a Channel but it didnt match ?? *** " << std::endl;
	  }
	}
	else{ // No Matching QDC 
	  TFastScintHit tdc_hit;
	  tdc_hit.SetTime(Mt->Time());
	  tdc_hit.SetCharge(-1);
	  
	  if(Mt->isTrig())  tdc_hit.SetChannel(-10);
	  else   	    tdc_hit.SetChannel(Mt->Chan());
	    
	  if(Zero_Suppress){
	    if(Mt->Time()>0){
	      InsertHit(tdc_hit);
	    }
	  }
	  else
	    InsertHit(tdc_hit);
	  
	  if(DEBUG){
	    std::cout << " TDC t : " << std::hex << Mt->Time() << std::endl;
	    std::cout << " TDC ch: " << std::dec << Mt->Chan() << std::endl;
	    std::cout << " TDC Tr: " << Mt->isTrig() << std::endl;
	  }
	}
      }	
      else{ // If not QDC or TDC 
	std::cout << " *** Not TDC or QDC **** " << std::endl;
      }
    }
    else if(Mword->isETS()){
      if(DEBUG)
	std::cout << " ETS " << std::endl;
    }
    else if(Mword->isFILL()){
      if(DEBUG)
	std::cout << " Filler " << std::endl;
    }
    else if(Mword->isEOE()){
      // Here we want to set the timestamp.
      const TRawEvent::Mesy_EOE* Meoe = (TRawEvent::Mesy_EOE*)Mword;

      if(isQ){
	SetQDC_TimeStamp(Meoe->TS());
	
	if(DEBUG){
	  std::cout << " QDC TS : " << std::hex << Meoe->TS() << std::endl;
	  std::dec;
	}
      }
      else if (isT){
	SetTDC_TimeStamp(Meoe->TS());
	
	if(DEBUG){
	  std::cout << " TDC TS : " << std::hex << Meoe->TS() << std::endl;
	  std::dec;
	}
      }
      else
	std::cout << " *** Error -- Timestamp not for TDC or QDC *** " << std::endl;
      
      if(DEBUG) std::cout << " EOE " << std::endl;
    }
    else if(Mword->isALLF()){
      if(DEBUG) std::cout << " All F " << std::endl;
    }
  }

  // Sort array:
  //TFastScintHit *hit_sort = 
  
  return 0;
}
