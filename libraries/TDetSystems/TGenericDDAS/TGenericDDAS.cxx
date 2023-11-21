#include "TGenericDDAS.h"

#include <algorithm>
#include <iostream>
#include <fstream>

#include "DDASDataFormat.h"
#include "DDASBanks.h"
#include "TNSCLEvent.h"
#include "TRawEvent.h"
#include "TChannel.h"
#include "TGRUTOptions.h"


bool TGenericDDAS::fFileDetermined = false;
bool TGenericDDAS::fGEB = false;

/*******************************************************************************/
/* TGenericDDAS ****************************************************************/
/* Used to store data from DDAS (Pixie-16s *************************************/
/* Generic Class intented for small setups (single cards) **********************/
/*******************************************************************************/
TGenericDDAS::TGenericDDAS(){ }

TGenericDDAS::~TGenericDDAS(){ }

/*******************************************************************************/
/* Copies hit ******************************************************************/
/*******************************************************************************/
void TGenericDDAS::Copy(TObject& obj) const {
  TDetector::Copy(obj);
  TGenericDDAS& ddas= (TGenericDDAS&)obj;
  ddas.ddas_hits = ddas_hits;
}

/*******************************************************************************/
/* Clear hit *******************************************************************/
/*******************************************************************************/
void TGenericDDAS::Clear(Option_t* opt){
  TDetector::Clear(opt);
  ddas_hits.clear();
}

/*******************************************************************************/
/* Functions to call DDAS hits *************************************************/
/*******************************************************************************/
TGenericDDASHit& TGenericDDAS::GetDDASHit(int i){
  return ddas_hits.at(i);
}

TDetectorHit& TGenericDDAS::GetHit(int i){
  return ddas_hits.at(i);
}

/*******************************************************************************/
/* Unpacks DDAS data and builds TGenericDDAS events ****************************/
/* Detects whether file type is .evt or mode3 due to different header sizes ****/
/*******************************************************************************/
int TGenericDDAS::BuildHits(std::vector<TRawEvent>& raw_data) {
  //Check if sorting data from NSCL .evt file or GRETINA mode2 data
  if(!fFileDetermined) {
    kFileType fFileType = raw_data.at(0).GetFileType();
    switch(fFileType) {
      case GRETINA_MODE2:
	fGEB = true;
        break;
      case NSCL_EVT:
	fGEB = false;
        break;
      default:
        break;
    }
  }

  //Loop over raw data
  for(auto& event : raw_data){
    TSmartBuffer buf = event.GetPayloadBuffer();
    //For .evt files the buffer is advanced in TNSCLEvent so for mode2 data
    //We skip the NSCL headers
    if(fGEB) buf.Advance(52);
    TDDASEvent<DDASHeader> ddasevt(buf);
    unsigned int address = ( (ddasevt.GetCrateID()<<16) + (ddasevt.GetSlotID()<<8) + ddasevt.GetChannelID() );

    //Check for channel address from .cal file otherwise skip this event
    TChannel* chan = TChannel::GetChannel(address);
    static int lines_displayed = 0;
    if(!chan){
      if(lines_displayed < 10) {
        std::cout << "Unknown DDAS (crate, slot, channel): (" << ddasevt.GetCrateID() << ", " << ddasevt.GetSlotID() << ", " << ddasevt.GetChannelID() << ")" << std::endl;
      }
      lines_displayed++;
      continue;
    }

    //Create a TGenericDDASHit
    TGenericDDASHit hit;
    hit.SetAddress(address);
    hit.SetDetectorNumber(chan->GetArrayPosition());
    hit.SetCharge(ddasevt.GetEnergy());
    hit.SetTimestamp(ddasevt.GetTimestamp()); // this is now in ns pcb!!
    hit.SetExtTime(ddasevt.GetExtTimestamp());
    hit.SetTimeFull(ddasevt.GetTime()); // Timestamp + CFD
    hit.SetCFDTime(ddasevt.GetCFDTime()); // CFD ONLY
    hit.SetTrace(ddasevt.GetTraceLength(), ddasevt.trace);
    ddas_hits.push_back(hit);
  }
  return Size();
}

/*******************************************************************************/
/* Required by TDetectorFactory ************************************************/
/*******************************************************************************/
void TGenericDDAS::InsertHit(const TDetectorHit& hit) {
  ddas_hits.emplace_back((TGenericDDASHit&)hit);
  fSize++;
}

