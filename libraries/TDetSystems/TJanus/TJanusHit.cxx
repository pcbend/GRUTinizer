#include "TJanusHit.h"

#include "GValue.h"
#include "TJanus.h"

void TJanusHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TJanusHit& hit = (TJanusHit&)obj;

  hit.fEnergyOverflowBit  = fEnergyOverflowBit;
  hit.fEnergyUnderflowBit = fEnergyUnderflowBit;

  hit.fTimeOverflowBit  = fTimeOverflowBit;
  hit.fTimeUnderflowBit = fTimeUnderflowBit;
}

void TJanusHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);

  fEnergyOverflowBit  = true;
  fEnergyUnderflowBit = true;

  fTimeOverflowBit  = true;
  fTimeUnderflowBit = true;
}

int TJanusHit::GetFrontChannel() const {
  int slotnum = (Address() & 0x0000ff00)>>8;
  int channum = Address() & 0x000000ff;
  return (slotnum-5)*32 + channum;
}

int TJanusHit::GetBackChannel() const {
  int slotnum = (back_hit.Address() & 0x0000ff00)>>8;
  int channum = back_hit.Address() & 0x000000ff;
  return (slotnum-5)*32 + channum;
}

int TJanusHit::GetDetnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetArrayPosition();
  } else {
    return -1;
  }
}

int TJanusHit::GetRing() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetSegment();
  } else {
    return 0;
  }
}

int TJanusHit::GetSector() const {
  TChannel* chan = TChannel::GetChannel(back_hit.Address());
  if(chan){
    return chan->GetSegment();
  } else {
    return 0;
  }
}

void TJanusHit::Print(Option_t *opt) const {
  printf("JanusHit[%03i,%03i]  Ring: %02i  Sector: %02i  Charge: %i   Energy: %.02f\n",
          GetFrontChannel(),GetBackChannel(),GetRing(),GetSector(),Charge(),GetEnergy());
 
}



TVector3 TJanusHit::GetPosition(bool apply_array_offset) const {
  TVector3 output = TJanus::GetPosition(GetDetnum(), GetRing(), GetSector());
  if(apply_array_offset) {
    output += TVector3(GValue::Value("Janus_X_offset"),
                       GValue::Value("Janus_Y_offset"),
                       GValue::Value("Janus_Z_offset"));
  }
  return output;
}
