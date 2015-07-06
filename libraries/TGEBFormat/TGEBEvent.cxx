


#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <assert.h>

#include "TGEBEvent.h"
#include "TGEBBanksFunctions.h"

ClassImp(TGEBEvent) 

TGEBEvent::TGEBEvent() {
  fData = NULL;
  fAllocatedByUs = false;

  fEventHeader.type      = -1; 
  fEventHeader.size      =  0;
  fEventHeader.timestamp = -1;
}

void TGEBEvent::Copy(const TGEBEvent &rhs) {
  Clear();
  fEventHeader = rhs.fEventHeader;

  fData = (char*)malloc(fEventHeader.size);
  if(!fData) {
    printf("\n\tmalloc failed?\n");
    printf("\n\n\n\n\n");
    printf("\tfEventHeader.type      = %i\n",fEventHeader.type);
    printf("\tfEventHeader.size      = %i\n",fEventHeader.size);
    printf("\tfEventHeader.timestamp = %lu\n",fEventHeader.timestamp);
  }
  assert(fData);
  memcpy(fData,rhs.fData,fEventHeader.size);
  fAllocatedByUs = true;
}

TGEBEvent::TGEBEvent(const TGEBEvent &rhs) {
  Copy(rhs);
}

TGEBEvent::~TGEBEvent() { 
  Clear();
}

Int_t TGEBEvent::Compare(const TGEBEvent &rhs) const { }

TGEBEvent &TGEBEvent::operator=(const TGEBEvent &rhs) {
  if(&rhs!=this)
    Clear();
  this->Copy(rhs);
  return *this;
}

void TGEBEvent::Clear(Option_t *opt) {
  if(fData && fAllocatedByUs)
    free(fData);
  fData=NULL;
  fAllocatedByUs = false;
  
  fEventHeader.type      = -1; 
  fEventHeader.size      =  0;
  fEventHeader.timestamp = -1;
}

void TGEBEvent::SetData(Int_t size, char *data) {
  fEventHeader.size = size;
  assert(!fAllocatedByUs);
  assert(IsGoodSize());
  fData = data;
  fAllocatedByUs = false;
}



Int_t TGEBEvent::GetEventType() const {
  return fEventHeader.type;
}

Int_t TGEBEvent::GetDataSize() const {
  return fEventHeader.size;
}

Long_t TGEBEvent::GetTimeStamp() const {
  return fEventHeader.timestamp;
}


char *TGEBEvent::GetData() const {
  if(!fData)
    AllocateData();
  return fData;
}

TGEBEventHeader *TGEBEvent::GetEventHeader() {
  return &fEventHeader;
}

bool TGEBEvent::IsGoodSize() const {
  return fEventHeader.size>0&&fEventHeader.size<=500*1024*1024;
}


void TGEBEvent::Print(Option_t *opt) const {
  std::cout << fEventHeader;
  printf("\t");
  int counter=0;
  for(int x=0;x<GetDataSize();x+=4) {
    printf("0x%08x  ",*(((int*)GetData())+x));
    if((((++counter)%8)==0) && ((counter)!=GetDataSize()))
      printf("\n\t");
    
  }
  printf("\n--------------------------\n");
}


void TGEBEvent::AllocateData()  const {
  assert(!fAllocatedByUs);
  assert(IsGoodSize());
  fData = (char*)malloc(fEventHeader.size);
  assert(fData);
  fAllocatedByUs = true;
}










































