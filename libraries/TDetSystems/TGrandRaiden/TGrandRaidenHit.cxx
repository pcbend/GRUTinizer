#ifdef RCNP

#include "TGrandRaidenHit.h"
#include "TGRUTOptions.h"
#include "RCNPEvent.h"
#include "TSmartBuffer.h"

ClassImp(TGrandRaidenHit)

TGrandRaidenHit::TGrandRaidenHit() {
  memset(&ADC[0],0,sizeof(Double_t));
}

TGrandRaidenHit::~TGrandRaidenHit() {

}


void TGrandRaidenHit::BuildFrom(TSmartBuffer& buf){
  Clear();


  // auto event = *reinterpret_cast<RCNPEvent**>(const_cast<char*>(buf.GetData()));
  // auto adc = event->GR_ADC();
  // if (adc != nullptr) {
  //   std::copy(adc->begin(),adc->end(),&ADC[0]);
  // }
  // Timestamp = event->GetTimestamp();
  // RF = event->GR_RF(0);

  // buf.Advance(sizeof(event));
  // if (event) delete event;


  auto event = *reinterpret_cast<RCNPEvent**>(const_cast<char*>(buf.GetData()));
  auto adc = event->GR_ADC();
  if (adc != nullptr) {
    std::copy(adc->begin(),adc->end(),&ADC[0]);
  }
  Timestamp = event->GetTimestamp();
  RF = event->GR_RF(0);

  buf.Advance(sizeof(event));
  if (event) delete event;

}





void TGrandRaidenHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);



}


void TGrandRaidenHit::Print(Option_t *opt) const { }

void TGrandRaidenHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
}















#else


#include "TGrandRaidenHit.h"

ClassImp(TGrandRaidenHit)

TGrandRaidenHit::TGrandRaidenHit() {
  memset(&ADC[0],0,sizeof(Double_t));
}

TGrandRaidenHit::~TGrandRaidenHit() { ; }

void TGrandRaidenHit::BuildFrom(TSmartBuffer& buf){
  Clear();
}

void TGrandRaidenHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);
}

void TGrandRaidenHit::Print(Option_t *opt) const { ; }

void TGrandRaidenHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
}

#endif
