#include "TFastScint.h"

#include <cassert>
#include <iostream>

#include "TNSCLEvent.h"
#include "TH2.h"

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
}

void TFastScint::Clear(Option_t* opt){
  TDetector::Clear(opt);

  //tdc_trigger = -1;
  //qdc_channels = -1;
  //tdc_channels = -1;
  //fs_hits->Clear(opt);
  fs_hits.clear();
}

int TFastScint::BuildHits(std::vector<TRawEvent>& raw_data){
  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    errors+=Build_From(nscl,true);
  }
  return Size();
}

void TFastScint::InsertHit(const TDetectorHit& hit){
  //TFastScintHit* new_hit = (TFastScintHit*)fs_hits->ConstructedAt(Size());
  //hit.Copy(*new_hit);
  fs_hits.push_back((TFastScintHit)hit);
}

int TFastScint::Size(){
  return fs_hits.size(); //->GetEntries();
}

TFastScintHit* TFastScint::GetLaBrHit(int i){
  return &fs_hits.at(i);
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
  return fs_hits.at(i);
}

// This is where the unpacking happens:
int TFastScint::Build_From(TNSCLEvent &event,bool Zero_Suppress){
  bool DEBUG = false;


  bool isQ = false;
  bool isT = false;

  Int_t words_processed = 0;
  Int_t detNumber = -1;

  // Payload size in 32 bit words.
  Int_t PayloadSize = event.GetPayloadSize()/4.0;

  if(PayloadSize == 1)
    return 1;

  const char* data = event.GetPayload();

  // NOTE -> buffer_size is 2*PayloadSize.
  // UShort_t buffer_size = *((UShort_t*)(data));
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
    }// end if is header
    else if(Mword->isData()){ // Data
      if(isQ){ // QDC:
	// NOTE -> we do it this way bc QDC should always come first!!!!
	TFastScintHit hit;
	const TRawEvent::M_QDC_Data* Mq = (TRawEvent::M_QDC_Data*)Mword;
	hit.SetChannel(Mq->Chan());
	hit.SetTime(-1);
	if(Mq->isOOR()) hit.SetCharge(5000);
	else            hit.SetCharge(Mq->Charge());

	hit.SetEnergy();

	if(Zero_Suppress){
	  if(Mq->Charge()>0){
	    InsertHit(hit);
	  }
	}
	else{
	  InsertHit(hit);
	}

	if(DEBUG){
	  std::cout << " QDC Q : " << std::hex << Mq->Charge() << std::endl;
	  std::cout << " QDC ch: " << std::dec << Mq->Chan() << std::endl;
	  std::cout << " QDC Ov: " << Mq->isOOR() << std::endl;
	}
      } // end if isQ
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
          tdc_hit.SetEnergy();

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
    }// end elif isfill
    else if(Mword->isEOE()){
      if(DEBUG) std::cout << " EOE " << std::endl;
    }// end elif eoe
  }// end for
  return 0;
}
