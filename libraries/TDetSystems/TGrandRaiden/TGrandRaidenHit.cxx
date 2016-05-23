#include "TGrandRaidenHit.h"
#include "TGRUTOptions.h"
#include "rootalyze.h"
#include "TSmartBuffer.h"

ClassImp(TGrandRaidenHit)

TGrandRaidenHit::TGrandRaidenHit() {
  memset(&ADC[0],0,sizeof(Double_t));
}

TGrandRaidenHit::~TGrandRaidenHit() {

}




void TGrandRaidenHit::BuildFrom(TSmartBuffer& buf){
  Clear();

  // method 1
  //char* ptrbytes = (char*)calloc(1,sizeof(RCNPEvent*));
  //auto buffer = buf.GetData();
  //memcpy(&ptrbytes, &buffer, sizeof(RCNPEvent*));

  // method 2
  //TSmartBuffer temp(std::move(buf));

  // method 3
  auto event = const_cast<RCNPEvent*>((const RCNPEvent*)buf.GetData());
  auto adc = event->GR_ADC();
  if (adc != nullptr) {
    std::copy(adc->begin(),adc->end(),&ADC[0]);
  }

  // for (int i=0; i<4; i++) {
  //   std::cout << ADC[i] << " ";
  // } std::cout << std::endl;

  //std::cout << event->GR_ADC() << std::endl;
  buf.Advance(sizeof(event));
  //buf.Clear();
  //if (event) delete event;
}





void TGrandRaidenHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);



}


void TGrandRaidenHit::Print(Option_t *opt) const { }

void TGrandRaidenHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
}

