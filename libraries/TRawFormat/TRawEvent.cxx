#include "TRawEvent.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <assert.h>

#include "TString.h"

#include "TGEBEvent.h"
#include "TNSCLEvent.h"

ClassImp(TRawEvent)

TRawEvent::TRawEvent() {
  fEventHeader.datum1      = -1;
  fEventHeader.datum2      =  0;
  fFileType = kFileType::UNKNOWN_FILETYPE;
  fTimestamp = -1;
  fDataPtr = nullptr;
}

void TRawEvent::Copy(TObject &rhs) const {
  rhs.Clear();
  ((TRawEvent&)rhs).fEventHeader = fEventHeader;
  ((TRawEvent&)rhs).fBody        = fBody;
  ((TRawEvent&)rhs).fFileType    = fFileType;
  ((TRawEvent&)rhs).fTimestamp   = fTimestamp;
  ((TRawEvent&)rhs).fDataPtr     = fDataPtr;
}

TRawEvent::TRawEvent(const TRawEvent &rhs)
  : TObject(rhs) {
  rhs.Copy(*this);
}

TRawEvent::~TRawEvent() {
  Clear();
}

Int_t TRawEvent::Compare(const TObject &rhs) const { return 0; }

TRawEvent &TRawEvent::operator=(const TRawEvent &rhs) {
  if(&rhs!=this)
    Clear();
  fEventHeader = rhs.fEventHeader;
  fBody        = rhs.fBody;
  fFileType    = rhs.fFileType;
  fTimestamp   = rhs.fTimestamp;
  fDataPtr   = rhs.fDataPtr;
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

  case ANL_RAW:
  case GRETINA_MODE2:
  case GRETINA_MODE3:
    return ((GEBHeader*)(&fEventHeader))->type();

  default:
    ;
  }

  return 0;
}

Int_t TRawEvent::GetBodySize() const {
  switch(fFileType){
  case NSCL_EVT:
    return ((EVTHeader*)(&fEventHeader))->size() - sizeof(RawHeader); //Size in nscldaq is inclusive

  case ANL_RAW:
  case GRETINA_MODE2:
  case GRETINA_MODE3:
    return ((GEBHeader*)(&fEventHeader))->size() + sizeof(Long_t);  //Size in gretinadaq is exclusive, plus timestamp
  case RCNP_BLD:
    return sizeof(void*);

  default:
    return 0;
  }

  return 0;
}

Long_t TRawEvent::GetTimestamp() const {
  if(fTimestamp != -1){
    return fTimestamp;
  }

  if( fFileType == kFileType::UNKNOWN_FILETYPE) {
    printf("Unknown filetype: Size = %i\n",GetTotalSize());
    fflush(stdout);
    Print("all");
    //return 0;
  }
  assert(fFileType != kFileType::UNKNOWN_FILETYPE);

  switch(fFileType){
   case NSCL_EVT:
     return ((TNSCLEvent*)this)->GetTimestamp();

   case ANL_RAW:
   case GRETINA_MODE2:
   case GRETINA_MODE3:
     return ((TGEBEvent*)this)->GetTimestamp();

   default:
     ;
  }

  return 0;
}

const char* TRawEvent::GetPayload() const {
  if( fFileType == kFileType::UNKNOWN_FILETYPE) {
    printf("Unknown filetype: Size = %i\n",GetTotalSize());
    fflush(stdout);
    Print("all");
    //return 0;
  }
  assert(fFileType != kFileType::UNKNOWN_FILETYPE);

  switch(fFileType){
   case NSCL_EVT:
     return ((TNSCLEvent*)this)->GetPayload();

   case ANL_RAW:
   case GRETINA_MODE2:
   case GRETINA_MODE3:
     return ((TGEBEvent*)this)->GetPayload();

   default:
     ;
  }

  return 0;
}

TSmartBuffer TRawEvent::GetPayloadBuffer() const {
  if( fFileType == kFileType::UNKNOWN_FILETYPE) {
    printf("Unknown filetype: Size = %i\n",GetTotalSize());
    fflush(stdout);
    Print("all");
    //return 0;
  }
  assert(fFileType != kFileType::UNKNOWN_FILETYPE);

  switch(fFileType){
   case NSCL_EVT:
     return ((TNSCLEvent*)this)->GetPayloadBuffer();

   case ANL_RAW:
   case GRETINA_MODE2:
   case GRETINA_MODE3:
     return ((TGEBEvent*)this)->GetPayloadBuffer();

   default:
     ;
  }

  return TSmartBuffer();
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
  std::cout << "Timestamp: " << GetTimestamp() << std::endl;
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
