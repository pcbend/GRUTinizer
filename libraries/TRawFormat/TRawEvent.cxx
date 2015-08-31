

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <assert.h>

#include "TRawEvent.h"
#include "TString.h"

ClassImp(TRawEvent)

TRawEvent::TRawEvent() {
  fEventHeader.datum1      = -1;
  fEventHeader.datum2      =  0;
  fFileType = kFileType::UNKNOWN_FILETYPE;
}

void TRawEvent::Copy(const TRawEvent &rhs) {
  Clear();
  fEventHeader = rhs.fEventHeader;
  fBody = rhs.fBody;
  fFileType = rhs.fFileType;
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
  fEventHeader.datum1      =  0;
  fEventHeader.datum2      =  0;
  fBody.Clear();
  fFileType = kFileType::UNKNOWN_FILETYPE;
}

void TRawEvent::SetData(TSmartBuffer body) {
  fBody = body;
}

Int_t TRawEvent::GetEventType() const {
  assert(fFileType != kFileType::UNKNOWN_FILETYPE);

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

Int_t TRawEvent::GetTotalSize() const {
  return GetBodySize() + sizeof(RawHeader);
}

const char *TRawEvent::GetBody() const {
  return fBody.GetData();
}

TRawEvent::RawHeader* TRawEvent::GetRawHeader() {
  return &fEventHeader;
}

bool TRawEvent::IsGoodSize() const {
  return (GetBodySize()>0)&&(GetBodySize()<=500*1024*1024);
}


void TRawEvent::Print(Option_t *opt) const {
  TString options(opt);
  if(!options.Contains("bodyonly"))
    std::cout << fEventHeader;
  fBody.Print(options.Data());
  // printf("\t");
  // for(int x=0; x<GetBodySize(); x+=2) {
  //   if((x%16 == 0) &&
  //      (x!=GetBodySize())){
  //     printf("\n\t");
  //   }
  //   printf("0x%04x  ",*(unsigned short*)(GetBody()+x));
  // }
  // printf("\n--------------------------\n");

}
