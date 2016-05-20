#include "TGrandRaidenHit.h"
#include "TGRUTOptions.h"
#include "rootalyze.h"
#include "TSmartBuffer.h"

ClassImp(TGrandRaidenHit)

TGrandRaidenHit::TGrandRaidenHit() : test(0) {

}

TGrandRaidenHit::~TGrandRaidenHit() {

}




void TGrandRaidenHit::BuildFrom(TSmartBuffer& buf){
  Clear();

  //char* ptrbytes = (char*)calloc(1,sizeof(RCNPEvent*));
  //auto buffer = buf.GetData();
  //memcpy(&ptrbytes, &buffer, sizeof(RCNPEvent*));
  //TSmartBuffer temp(std::move(buf));

  auto event = const_cast<RCNPEvent*>((const RCNPEvent*)buf.GetData());

  //std::cout << event->GR_ADC() << std::endl;
  test = 1;
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

