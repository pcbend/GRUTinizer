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

  auto event = reinterpret_cast<const RCNPEvent*>(buf.GetData());
  buf.Advance(sizeof(event));

  cout << "TGrandRaidenHit::BuildFrom" << endl;
  test = 1;

}





void TGrandRaidenHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);



}


void TGrandRaidenHit::Print(Option_t *opt) const { }

void TGrandRaidenHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
}

