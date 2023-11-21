#include "TBank29.h"

#include "TGEBEvent.h"
#include "TGRUTOptions.h"

ClassImp(TBank29)


/*******************************************************************************/
/* TBank29 *********************************************************************/
/* Actually Bank88 which contains external trigger information *****************/
/* For FRIB this is the S800 trigger *******************************************/
/*******************************************************************************/
TBank29::TBank29(){
}

TBank29::~TBank29() {
}


/*******************************************************************************/
/* Copies hit ******************************************************************/
/*******************************************************************************/
void TBank29::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TBank29& bank = (TBank29&)obj;
  bank.channels = channels;
}

/*******************************************************************************/
/* Inserts hit to TBank29 hit vector *******************************************/
/*******************************************************************************/
void TBank29::InsertHit(const TDetectorHit& hit){
  channels.emplace_back((TMode3Hit&)hit);
  fSize++;
}

/*******************************************************************************/
/* Unpacks GEB data and builds TBank29 hit *************************************/
/*******************************************************************************/
int TBank29::BuildHits(std::vector<TRawEvent>& raw_data){
  for(auto& event : raw_data){
    TGEBEvent* geb = (TGEBEvent*)&event;
    SetTimestamp(geb->GetTimestamp());
    TMode3Hit hit;
    TSmartBuffer buf = geb->GetPayloadBuffer();
    hit.BuildFrom(buf);
    InsertHit(hit);
  }
  if(Size()) {
    SetTimestamp(channels.at(0).GetLed());
  }
  return Size();
}

/*******************************************************************************/
/* Blank Print Function ********************************************************/
/*******************************************************************************/
void TBank29::Print(Option_t *opt) const { }

/*******************************************************************************/
/* Clear hit *******************************************************************/
/*******************************************************************************/
void TBank29::Clear(Option_t *opt) {
  TDetector::Clear(opt);
  channels.clear();
}
