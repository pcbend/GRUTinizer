

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <assert.h>

#include "TRawEvent.h"

ClassImp(TRawEvent)

TRawEvent::TRawEvent() {
  fData = NULL;
  fAllocatedByUs = false;

  //fEventHeader.type      = -1;
  //fEventHeader.size      =  0;
  fEventHeader.datum1      = -1;
  fEventHeader.datum2      =  0;
  //fEventHeader.timestamp = -1;
}

void TRawEvent::Copy(const TRawEvent &rhs) {
  Clear();
  fEventHeader = rhs.fEventHeader;

  fData = (char*)malloc(GetBodySize());
  if(!fData) {
    printf("\n\tmalloc failed?\n");
    printf("\n\n\n\n\n");
    printf("\tfEventHeader.type  = %i\n",GetEventType());
    printf("\tfEventHeader.size  = %i\n",GetBodySize());
    printf("\tfTimeStamp         = %lu\n",fTimeStamp);
  }
  assert(fData);
  memcpy(fData,rhs.fData,GetBodySize());
  fAllocatedByUs = true;

}

TRawEvent::TRawEvent(const TRawEvent &rhs) {
  Copy(rhs);
}

TRawEvent::~TRawEvent() {
  Clear();
}

Int_t TRawEvent::Compare(const TRawEvent &rhs) const { }

TRawEvent &TRawEvent::operator=(const TRawEvent &rhs) {
  if(&rhs!=this)
    Clear();
  this->Copy(rhs);
  return *this;
}

void TRawEvent::Clear(Option_t *opt) {
  printf("RawEvent Clear.\n");
  printf("\tfData  = 0x%08x\n",fData);
  printf("\tBodySize  = 0x%08x\n",GetBodySize());
  printf("\tfAllocatedByUs  = 0x%08x\n",fAllocatedByUs);
//  if(fData && fAllocatedByUs)
//    free(fData);
  printf("I am Here.\n");
  fData=NULL;
  fAllocatedByUs = false;
  fFileType = kFileType::UNKNOWN;
  fEventHeader.datum1      =  0;
  fEventHeader.datum2      =  0;
  fTimeStamp = -1;

}

void TRawEvent::SetData(Int_t size, char *data) {
  
  //fEventHeader.size = size;
  assert(!fAllocatedByUs);
  assert(IsGoodSize());
  fData = data;
  fAllocatedByUs = false;
  
}



Int_t TRawEvent::GetEventType() const {
  assert(fFileType != kFileType::UNKNOWN);

  switch(fFileType){
  case NSCL_EVT:
    return ((EVTHeader*)(&fEventHeader))->type();

  case GRETINA_MODE2:
  case GRETINA_MODE3:
    return ((GEBHeader*)(&fEventHeader))->type();

  }

  return 0;
}

Int_t TRawEvent::GetBodySize() const {
  assert(fFileType != kFileType::UNKNOWN);

  switch(fFileType){
  case NSCL_EVT:
    return ((EVTHeader*)(&fEventHeader))->size() - sizeof(RawHeader); //Size in nscldaq is inclusive

  case GRETINA_MODE2:
  case GRETINA_MODE3:
    return ((GEBHeader*)(&fEventHeader))->size();  //Size in gretinadaq is exclusive

  }

  return 0;
}

Long_t TRawEvent::GetTimeStamp() const {
  return fTimeStamp;
}


char *TRawEvent::GetData() const {
  if(!fData)
    AllocateData();
  return fData;
}

TRawEvent::RawHeader* TRawEvent::GetRawHeader() {
  return &fEventHeader;
}

bool TRawEvent::IsGoodSize() const {
  return (GetBodySize()>0)&&(GetBodySize()<=500*1024*1024);
}


void TRawEvent::Print(Option_t *opt) const {
  
  //std::cout << fEventHeader;
  printf("\t");
  int counter=0;
  for(int x=0;x<GetBodySize();x+=4) {
    printf("0x%08x  ",*(((int*)GetData())+x));
    if((((++counter)%8)==0) && ((counter)!=GetBodySize()))
      printf("\n\t");

  }
  printf("\n--------------------------\n");
  
}


void TRawEvent::AllocateData()  const {
  assert(!fAllocatedByUs);
  assert(IsGoodSize());
  fData = (char*)malloc(GetBodySize());
  assert(fData);
  fAllocatedByUs = true;
  
}


//std::ostream& operator<<(std::ostream& os, const TRawEventHeader& head){
//  return os;
//}

//std::ostream& operator<<(std::ostream& os, const TGEBEventHeader &head) {
   //time_t t = (time_t)head.timestamp;
//   return os << "Bank @ " << std::hex << &head << std::dec << std::endl
//             << "\ttype      = " << std::setw(8) << head.type << std::endl
//             << "\tsize      = " << std::setw(8) << head.size << std::endl
//             << "\ttimestamp = " << head.timestamp << std::endl;
//};

