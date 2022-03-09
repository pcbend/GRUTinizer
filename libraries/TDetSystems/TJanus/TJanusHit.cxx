#include "TJanusHit.h"

#include <mutex>

#include "GValue.h"
#include "TJanus.h"
#include "TReaction.h"

TJanusHit::TJanusHit(const TJanusHit& hit) {
  hit.Copy(*this);
}

TJanusHit& TJanusHit::operator=(const TJanusHit& hit) {
  hit.Copy(*this);
  return *this;
}

void TJanusHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TJanusHit& hit = (TJanusHit&)obj;

  hit.fEnergyOverflowBit  = fEnergyOverflowBit;
  hit.fEnergyUnderflowBit = fEnergyUnderflowBit;

  hit.fTimeOverflowBit  = fTimeOverflowBit;
  hit.fTimeUnderflowBit = fTimeUnderflowBit;

  back_hit.Copy(hit.back_hit);
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

TVector3 TJanusHit::GetConjugateDirection() const {
  static std::mutex mutex;
  std::lock_guard<std::mutex> lock(mutex);

  static TReaction reac("78Kr","208Pb","78Kr","208Pb",3.9*78);

  TVector3 pos = GetPosition();

  // Convert from 208Pb angle to 78Kr angle
  double theta_78kr = reac.ConvertThetaLab(pos.Theta(), 3, 2);

  pos.SetTheta(theta_78kr);
  pos.SetPhi(pos.Phi() + 3.1415926535);
  return pos;
}
