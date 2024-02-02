#include "TBank88.h"
#include "TGEBEvent.h"
#include "TGRUTOptions.h"

ClassImp(TBank88)

/*******************************************************************************/
/* TBank88 *********************************************************************/
/* Contains external trigger information for FRIB this is the S800 trigger *****/
/*******************************************************************************/
TBank88::TBank88(){
}

TBank88::~TBank88() {
}

/*******************************************************************************/
/* Copies hit ******************************************************************/
/*******************************************************************************/
void TBank88::Copy(TObject& obj) const {
  TDetector::Copy(obj);

  TBank88& bank = (TBank88&)obj;
  bank.channels = channels;
}

/*******************************************************************************/
/* Inserts hit to TBank88 hit vector *******************************************/
/*******************************************************************************/
void TBank88::InsertHit(const TDetectorHit& hit){
  channels.emplace_back((TMode3Hit&)hit);
  fSize++;
}

/*******************************************************************************/
/* Unpacks GEB data and builds TBank88 hit *************************************/
/*******************************************************************************/
int TBank88::BuildHits(std::vector<TRawEvent>& raw_data){
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
void TBank88::Print(Option_t *opt) const { }

/*******************************************************************************/
/* Clear hit *******************************************************************/
/*******************************************************************************/
void TBank88::Clear(Option_t *opt) {
  TDetector::Clear(opt);
  channels.clear();
}
