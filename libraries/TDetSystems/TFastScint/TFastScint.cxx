#include "TFastScint.h"

#include <cassert>
#include <iostream>

//#include "FastScintDataFormat.h"
#include "TNSCLEvent.h"
#include "TH2.h"

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

  tdc_trigger = -1;
  qdc_channels = -1;
  tdc_channels = -1;
  fs_hits->Clear(opt);
}

int TFastScint::BuildHits(){
  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    Build_From(nscl);
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

TDetectorHit& TFastScint::GetHit(int i){
  return *(TFastScintHit*)fs_hits->At(i);
}

// This is where the unpacking happens:
void TFastScint::Build_From(TNSCLEvent &event){
  
  bool DEBUG = true;
  
  int counter = 0;
  int tdc_counter = 0;
  int qdc_counter = 0;

  //---------------------------------------
  // Note : The things we want to fill are
  //        channel, charge, and time
  unsigned int header_qdc,size_qdc,id_qdc;
  unsigned int header_tdc,size_tdc, id_tdc, res_tdc;
  int data;
  int channel_qdc,charge_qdc,ts_qdc;
  int channel_tdc,time_tdc,trig_tdc,ts_tdc;
  int ptr = 0;
  int BufferSize = 0;

  BufferSize = event.GetBodySize();

  if(DEBUG){
    event.Print("all");
    std::cout << " E Type : " << event.GetEventType() << std::endl;
    std::cout << " Size   : " << BufferSize << std::endl;
  }

  
  //=======================
  // Enter this while loop:
  while(true){

    //===================
    // Zero these things:
    data = 0;
    channel_qdc = charge_qdc = header_qdc = size_qdc = ts_qdc = 0;
    channel_tdc = header_tdc = size_tdc = ts_tdc = 0;
    time_tdc = trig_tdc = 0;
    ptr = 0;
    qdc_counter = 0;
    tdc_counter = 0;
    
    //=======================================================
    // Set the initial value for where we are 
    // looking in the buffer.  Words take up units of
    // 2, so ptr+=2 means ptr (which was 0) is 2.  This
    // means look at words 1 and 2 (if we start at word 0).
    ptr+=2;
    
    //====================
    // Get the QDC header:
    header_qdc = *((unsigned int*)(event.GetPayload()+ptr));

    //===================================================
    // The size of the buffer is the number of 32 bit 
    // data words. It includes a 32 bit End of Event (EOE)
    // word that contains a 30 bit timestamp.
    size_qdc = header_qdc&0x00000fff;
    
    //================
    // Get the QDC ID:
    id_qdc = (header_qdc&0x00ff0000)>>16;
    
    if(DEBUG){
      if(size_qdc>3){
	std::cout << " Head   : " << std::hex << header_qdc << std::endl;
	std::cout << " Words  : " << size_qdc << std::endl;
	std::cout << " ID     : " << id_qdc << std::endl;
      }
    }

    /*****************************************************/
    // Want to loop over the number of words in the buffer
    // minus the EOE words.
    std::map<int,int> chanmap;
    for(int i_i = 0; i_i<size_qdc-1;i_i++){

      //===================================================
      // Get payload gets new data words in intervals of 4:
      ptr+=4;
      data =*((unsigned int*)(event.GetPayload()+ptr));

      //===========================================
      // If these words are an "extended timestamp"
      // skip it for now.
      if((data&0xffff0000)==0x04800000)
	continue;
      
      if((data&0xffffffff)==0x00000000)
      	continue;

      //=========================================
      // Now pick off detector number and charge:
      if((data&0xfff00000) != 0x04000000)
        continue;
      channel_qdc = (data&0x001f0000)>>16;
      //hit.SetChannel(channel_qdc);
      charge_qdc = (data&0x00000fff);	     
      //hit.SetCharge(charge_qdc);
      chanmap[channel_qdc]=charge_qdc;
      if(charge_qdc>0)
	qdc_counter++;
      //InsertHit(hit);

      if(DEBUG){
	if(size_qdc>3){
	  std::cout << " Charge : " << charge_qdc << std::endl;
	  std::cout << " Chan   : " << channel_qdc << std::endl;
	}
      }
 
    }
    std::map<int,int>::iterator it;
    for(it=chanmap.begin();it!=chanmap.end();it++){
      TFastScintHit hit;      
      hit.SetChannel(it->first);
      hit.SetCharge(it->second);
      InsertHit(hit);

    }

    /******************************************/
    // Now handle the last words. These are TS
    ptr+=4;
    data =*((unsigned int*)(event.GetPayload()+ptr));
    ts_qdc = (data&0x00ffffff);
    
    if(DEBUG){
      if(size_qdc>3){
	std::cout << " TS     : " << ts_qdc << std::endl;
      }
    }

    //---------------- END QDC --------------------------    
    
    /**************************************************/
    // Here we will handle the TDC part of the buffer.
 /*   ptr+=8;
    header_tdc =*((unsigned int*)(event.GetPayload()+ptr));
    size_tdc = header_tdc&0x00000fff;
    id_tdc  = (header_tdc&0x00ff0000)>>16;
    res_tdc = (header_tdc&0x0000f000)>>12;
  
  
    if(DEBUG){
      if(size_qdc>3){
	std::cout << " Head T : " << header_tdc << std::endl;
	std::cout << " Size T : " << size_tdc << std::endl;
	std::cout << " ID T   : " << id_tdc << std::endl;
	std::cout << " Res T  : " << res_tdc <<std::endl;
      }
    }
    
    for(int i_j = 0; i_j<size_tdc-1;i_j++){
      if((header_tdc&0xff000000)!=0x40000000)
	continue;
      
      //===================================================
      // Get payload gets new data words in intervals of 4:
      ptr+=4;
      data =*((unsigned int*)(event.GetPayload()+ptr));

      //===========================================
      // If these words are an "extended timestamp"
      // skip it for now.
      if((data&0xffff0000)==0x04800000)
	continue;
      
      if((data&0xffffffff)==0x00000000)
	continue;

      //===================================
      // If these words have a trigger flag, 
      // do this:
      if((data&0x00200000)>>21==1){
	trig_tdc = data&0x0000ffff;
        SetTDCTrigger(trig_tdc);	
	continue;
      }
      
      channel_tdc = (data&0x001f0000)>>16;
      time_tdc = data&0x0000ffff;
      bool found = false;
      for(int k=0;k<Size();k++) {
        TFastScintHit *qdchit = GetLaBrHit(k);
        if(qdchit->GetChannel()==channel_tdc) {
          found = true;
          qdchit->SetTime(time_tdc);
	  if(time_tdc>0)
	    tdc_counter++;
	  break;
        }
      }
      if(!found) {
        printf("I AM NEVER HERE!\n");
        TFastScintHit tdchit;
        tdchit.SetChannel(channel_tdc);
        tdchit.SetCharge(0);
        tdchit.SetTime(time_tdc);
	tdc_counter++;
        InsertHit(tdchit);
      }

      if(DEBUG){
	if(size_tdc>3){
	  std::cout << " Chan T : " << channel_tdc << std::endl;
	  std::cout << " Time T : " << time_tdc << std::endl;
	}
      }
 
    }

    SetTDC_Count(tdc_counter);
    SetQDC_Count(qdc_counter);
    
    //===================
    // Get TDC Timestamp:
    ptr+=4;
    data =*((unsigned int*)(event.GetPayload()+ptr));
    ts_tdc = (data&0x00ffffff);
   */ 
    if(DEBUG){
      if(size_tdc>3){
	std::cout << " T  TS  : " << ts_tdc << std::endl;
	std::cout << " Trig T : " << trig_tdc << std::endl;
      }
    }
    break;
  }// end while.
}
