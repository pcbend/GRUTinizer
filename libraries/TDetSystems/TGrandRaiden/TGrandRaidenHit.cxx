#include "TGrandRaidenHit.h"

#include "TGRUTOptions.h"

ClassImp(TGrandRaidenHit)

TGrandRaidenHit::TGrandRaidenHit(){

}

TGrandRaidenHit::~TGrandRaidenHit() {

}




void TGrandRaidenHit::BuildFrom(TSmartBuffer& buf){
  Clear();

  //auto header = (TRawEvent::GEBGrandRaidenHead*)buf.GetData();
  //buf.Advance(sizeof(TRawEvent::GEBGrandRaidenHead));

}





void TGrandRaidenHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);



}


void TGrandRaidenHit::Print(Option_t *opt) const { }

void TGrandRaidenHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
}

