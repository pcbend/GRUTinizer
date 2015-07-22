

#include <cstdio>
#include <cstdlib>
#include <iostream>
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
  fFileType = kFileType::UNKNOWN;
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
  if(fData && fAllocatedByUs) {
    free(fData);
  }
  fData=NULL;
  fAllocatedByUs = false;
  fEventHeader.datum1      =  0;
  fEventHeader.datum2      =  0;
  fFileType = kFileType::UNKNOWN;
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
  switch(fFileType){
  case NSCL_EVT:
    return ((EVTHeader*)(&fEventHeader))->size() - sizeof(RawHeader); //Size in nscldaq is inclusive

  case GRETINA_MODE2:
  case GRETINA_MODE3:
    return ((GEBHeader*)(&fEventHeader))->size() + sizeof(Long_t);  //Size in gretinadaq is exclusive, plus timestamp

  default:
    return 0;
  }

  return 0;
}

char *TRawEvent::GetBody() const {
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

  std::cout << fEventHeader;
  printf("\t");
  int counter=0;
  for(int x=0;x<GetBodySize();x+=2) {
    printf("0x%04x  ",*((unsigned short*)(GetBody()+x)));
    if((((++counter)%8)==0) && ((counter)!=GetBodySize()))
      printf("\n\t");

  }
  printf("\n--------------------------\n");

}


void TRawEvent::AllocateData()  const {
  assert(!fAllocatedByUs);
  assert(IsGoodSize());
  assert(fFileType != kFileType::UNKNOWN);

  size_t bytes = GetBodySize();
  if(fFileType == GRETINA_MODE2 ||
     fFileType == GRETINA_MODE3){
    bytes += sizeof(Long_t);
  }

  fData = (char*)malloc(bytes);
  assert(fData);
  fAllocatedByUs = true;
}
