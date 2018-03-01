#include "TJanusDDASHit.h"

#include "GValue.h"
#include "TJanusDDAS.h"

TJanusDDASHit::TJanusDDASHit(const TJanusDDASHit& hit) {
  hit.Copy(*this);
}

TJanusDDASHit& TJanusDDASHit::operator=(const TJanusDDASHit& hit) {
  hit.Copy(*this);
  return *this;
}

void TJanusDDASHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TJanusDDASHit& hit = (TJanusDDASHit&)obj;
  back_hit.Copy(hit.back_hit);
}

void TJanusDDASHit::Clear(Option_t* opt) {
  TDetectorHit::Clear(opt);
}

float TJanusDDASHit::RawCharge() const {
  if(fCharge > 30000) {
    return fCharge - 32768;
  } else {
    return fCharge;
  }
}

int TJanusDDASHit::GetFrontChannel() const {
  int address = Address();

  int crate_id = (address & 0x00ff0000) >> 16;
  int slot_id  = (address & 0x0000ff00) >> 8;
  int chan_id  = (address & 0x000000ff) >> 0;

  return (slot_id-2)*16 + chan_id;
}

int TJanusDDASHit::GetBackChannel() const {
  int address = back_hit.Address();

  int crate_id = (address & 0x00ff0000) >> 16;
  int slot_id  = (address & 0x0000ff00) >> 8;
  int chan_id  = (address & 0x000000ff) >> 0;

  return (slot_id-2)*16 + chan_id;
}

int TJanusDDASHit::GetDetnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetArrayPosition();
  } else {
    return -1;
  }
}

int TJanusDDASHit::GetRing() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetSegment();
  } else {
    return 0;
  }
}

int TJanusDDASHit::GetSector() const {
  TChannel* chan = TChannel::GetChannel(back_hit.Address());
  if(chan){
    return chan->GetSegment();
  } else {
    return 0;
  }
}

void TJanusDDASHit::Print(Option_t *opt) const {
  printf("Det %i JanusDDASHit[%03i,%03i]  Ring: %02i  Sector: %02i  Charge: %i   Energy: %.02f\n",
          GetDetnum(),GetFrontChannel(),GetBackChannel(),GetRing(),GetSector(),Charge(),GetEnergy());
}



TVector3 TJanusDDASHit::GetPosition(bool apply_array_offset) const {
  TVector3 output = TJanusDDAS::GetPosition(GetDetnum(), GetRing(), GetSector());
  if(apply_array_offset) {
    output += TVector3(GValue::Value("Janus_X_offset"),
                       GValue::Value("Janus_Y_offset"),
                       GValue::Value("Janus_Z_offset"));
  }
  return output;
}

bool TJanusDDASHit::IsRing() const {
  TChannel* chan = TChannel::GetChannel(Address());
  if(chan){
    return (*chan->GetArraySubposition() == 'F');
  } else {
    return false;
  }
}

bool TJanusDDASHit::IsSector() const {
  TChannel* chan = TChannel::GetChannel(Address());
  if(chan){
    return (*chan->GetArraySubposition() != 'F');
  } else {
    return false;
  }
}
