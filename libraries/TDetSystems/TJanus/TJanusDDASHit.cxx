#include "TJanusDDASHit.h"

#include "GValue.h"
#include "TJanusDDAS.h"
#include "TReaction.h"
#include "TSRIM.h"

#include "TRandom.h"

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


TVector3 TJanusDDASHit::GetPosition(bool apply_array_offset) const {
  TVector3 output = TJanusDDAS::GetPosition(GetDetnum(), GetRing(), GetSector());
  if(apply_array_offset) {
    output += TVector3(GValue::Value("Janus_X_offset"),
                       GValue::Value("Janus_Y_offset"),
                       GValue::Value("Janus_Z_offset"));
  }
  return output;
}


TVector3 TJanusDDASHit::GetReconPosition(const char *beamname,const char *targetname,bool apply_array_offset) const {
  double theta = Reconstruct(beamname,targetname);
  double phi   = GetPosition(apply_array_offset).Phi()-3.141596;

  TVector3 v;
  v.SetMagThetaPhi(1,theta,phi);
  return v;
}



double TJanusDDASHit::Reconstruct(const char *beamname,const char *targetname,const char *srimfile) const {
  static auto beam = std::make_shared<TNucleus>(beamname);   // "78Kr");
  static auto targ = std::make_shared<TNucleus>(targetname); // "208Pb");
  static TSRIM srim(srimfile); //"se72_in_pb208.txt");
  double thickness = ( GValue::Value("targetthick") / 11342.0) * 1e4; // (0.75 mg/cm^2) / (11342 mg/cm^3) * (10^4 um/cm)

  //double collision_pos = gRandom->Uniform();

  //double collision_energy = srim.GetAdjustedEnergy(GValue::Value("beamenergy")*1e3, thickness*collision_pos)/1e3;
  double energy_mid = srim.GetAdjustedEnergy(GValue::Value("beamenergy")*1e3, thickness*0.5)/1e3;

  //TReaction reac(beam, targ, beam, targ, collision_energy);
  TReaction reac_mid(beam, targ, beam, targ, energy_mid);

  //double targ_angle_rad = reac.ConvertThetaLab(GetPosition().Theta(),3,2);//90 * (3.1415926/180), 2, 3);
  //double beam_angle_rad_recon = reac_mid.ConvertThetaLab(targ_angle_rad, 3, 2);

  //printf("input theta: %.02f\n",GetPosition().Theta()*TMath::RadToDeg());
  //printf("targ_angle_rad: %.02f\n",targ_angle_rad*TMath::RadToDeg());
  //printf("beam_angle_rad_recon: %.02f\n",beam_angle_rad_recon*TMath::RadToDeg());


  return reac_mid.ConvertThetaLab(GetPosition().Theta(),3,2);  //return beam based on target (give 3, calculate 2).
}







