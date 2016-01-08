#include "TCaesar.h"

#include "TNSCLEvent.h"

#define FERA_TIME_ID        0x2301
#define FERA_ENERGY_ID      0x2302
#define FERA_TIMESTAMP_ID   0x2303
#define FERA_ERROR_ID       0x23ff

#define DEBUG_BRANDON 1

TCaesar::TCaesar() {
  Clear();
}

TCaesar::~TCaesar(){ }

void TCaesar::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TCaesar& caesar = (TCaesar&)obj;
  caesar.caesar_hits = caesar_hits;
  caesar.raw_data.clear();
}

void TCaesar::Clear(Option_t* opt){
  TDetector::Clear(opt);

  fUlm = -1;
  caesar_hits.clear();
}

int TCaesar::BuildHits(){
  //if(raw_data.size()!=1) 
    //printf("\nCaesar event size = %i\n",raw_data.size());

  for(auto& event : raw_data){
    TNSCLEvent& nscl = (TNSCLEvent&)event;
    SetTimestamp(nscl.GetTimestamp());
    Build_Single_Read(nscl.GetPayloadBuffer());
//  Print();
//  event.Print("all");
//  printf("********************************\n");
//  printf("********************************\n");
//  printf("********************************\n");
//  printf("********************************\n");
//  printf("********************************\n");
  }
  //if(caesar_hits.size()<1) {
  //  raw_data.at(0).Print("all"); 
  //  printf("\nCaesar hits size = %i\n",caesar_hits.size());
  //  Print();
  //}
  return caesar_hits.size();
}

TCaesarHit& TCaesar::GetCaesarHit(int i){
  return caesar_hits.at(i);
}

const TCaesarHit& TCaesar::GetCaesarHit(int i) const{
  return caesar_hits.at(i);
}

TDetectorHit& TCaesar::GetHit(int i){
  return caesar_hits.at(i);
}

void TCaesar::Print(Option_t *opt) const {
  printf("Casear event @ %lu\n",Timestamp());
  printf("Number of Hits: %i\n",Size());
  for(int i=0;i<Size();i++) {
    printf("\t"); 
    GetCaesarHit(i).Print();
  }
  printf("---------------------------------------\n");
}

void TCaesar::Build_Single_Read(TSmartBuffer buf){
  const char* data = buf.GetData();
  const char* data_end = data + buf.GetSize();

  if (DEBUG_BRANDON){
    std::cout << "--------------------------------------------------\n\n" << std::endl;
    buf.Print("all");
  }
  

  TRawEvent::CAESARHeader* header = (TRawEvent::CAESARHeader*)data;
  data += sizeof(TRawEvent::CAESARHeader);

  // FERA_VERSION_00 does not have a ULM register
  if(header->version == 0x0001){
    data -= sizeof(Short_t);
    SetULM(-1);
  } else {
    SetULM(header->ulm);
  }

  while(data < data_end){
    //fera_header contains the size & tag of the current packet 
    TRawEvent::CAESARFeraHeader* fera_header = (TRawEvent::CAESARFeraHeader*)data;
    
    
    if((fera_header->tag != FERA_ENERGY_ID) && (fera_header->tag != FERA_TIME_ID)){
      data += fera_header->size * 2; // Size is inclusive number of 16-bit values.
      if(fera_header->tag == FERA_TIMESTAMP_ID){
        continue;
      }
      else if (fera_header->tag == FERA_ERROR_ID){
        break; // what we actually need to do is check if the fera is bad and take some action.
      }
      else {
        std::cout << "Unknown fera pkt tag" << (std::hex) << fera_header->tag << std::endl;
        break; 
      }
    }
    const char* fera_end = data + fera_header->size*2;
    data += sizeof(TRawEvent::CAESARFeraHeader);

    if (DEBUG_BRANDON){
      std::cout << "fera_header->size = 0x" << (std::hex) << fera_header->size << std::endl;
      std::cout << "fera_header->tag = 0x" << (std::hex) << fera_header->tag << std::endl;
    }


    //printf("fera end:  0x%04x\n",*((unsigned short*)fera_end));
    //buf.Print("all");


    while(data < fera_end){
      //This should contain all the data until the end of the fera
      TRawEvent::CAESARFera* fera = (TRawEvent::CAESARFera*)data;
      data += sizeof(TRawEvent::CAESARFera);
      int nchan = fera->number_chans();
      if(nchan==0){
	nchan = 16;
      }

      //Now we have the first header and know the VSN and number of channels
      //We need to start grabbing CAESARFeraItems now
      if (DEBUG_BRANDON){
        std::cout << "fera->header = 0x" << (std::hex) << fera->header << std::endl;
        std::cout << "fera->number_chans() = " << (std::dec) << fera->number_chans() << std::endl;
        std::cout << "fera->vsn() = " <<  (std::dec) << fera->vsn() << std::endl;
      }
      for(int i=0; i<nchan; i++){
        TRawEvent::CAESARFeraItem *item = (TRawEvent::CAESARFeraItem*)data;
        if (DEBUG_BRANDON){
          std::cout << "item->data = 0x"    << (std::hex)     << item->data    << std::endl;
          std::cout << "item->channel() = " << (std::dec) << item->channel() << std::endl;
          std::cout << "item->value() = "   << (std::dec) << item->value()   << std::endl;
        }

	if(fera_header->tag == FERA_ENERGY_ID){
	  SetCharge(fera->vsn(), item->channel(), item->value());
	} else { //FERA_TIME_ID
	  SetTime(fera->vsn(), item->channel(), item->value());
	}
        data += sizeof(TRawEvent::CAESARFeraItem);//just read in a single CAESARFeraItem
      }
      //data += 2*fera->number_chans() + 2;
    }
    if (DEBUG_BRANDON){
      std::cout << "--------------------------------------------------\n\n" << std::endl;
    }
  }
}

TCaesarHit& TCaesar::GetHit_VSNChannel(int vsn, int channel){
  for(auto& hit : caesar_hits){
    if(hit.GetVSN() == vsn &&
       hit.GetChannel() == channel){
      return hit;
    }
  }

  // No such thing existing right now, make one
  caesar_hits.emplace_back();
  TCaesarHit& output = caesar_hits.back();
  output.SetVSN(vsn);
  output.SetChannel(channel);
  output.SetAddress( (37<<24) +
		     (vsn<<16) +
		     (channel) );
  return output;
}


void TCaesar::SetCharge(int vsn, int channel, int data) {
  TCaesarHit& hit = GetHit_VSNChannel(vsn, channel);
  hit.SetCharge(data);
}

void TCaesar::SetTime(int vsn, int channel, int data) {
  TCaesarHit& hit = GetHit_VSNChannel(vsn, channel);
  hit.SetTime(data);
}

void TCaesar::InsertHit(const TDetectorHit& hit) {
  caesar_hits.emplace_back((TCaesarHit&)hit);
  fSize++;
}
