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

bool TJanusDDASHit::operator==(const TJanusDDASHit& rhs) {

  return Address()==rhs.Address() && Charge()==rhs.Charge() && Timestamp()==rhs.Timestamp();
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
  TChannel* chan = TChannel::GetChannel(back_hit.Address());
  if(chan){
    return chan->GetSegment();
  } else {
    return 0;
  }
}

int TJanusDDASHit::GetSector() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
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


TVector3 TJanusDDASHit::GetPosition(bool before, bool apply_array_offset) const {
  TVector3 output = TJanusDDAS::GetPosition(GetDetnum(), GetRing(), GetSector(), before);
  if(apply_array_offset) {
    output += TVector3(GValue::Value("Janus_X_offset"),
                       GValue::Value("Janus_Y_offset"),
                       GValue::Value("Janus_Z_offset"));
  }
  return output;
}

TVector3 TJanusDDASHit::GetReconPosition(TReaction& reac, int d_p, int r_p, bool s2, bool before,
					 bool apply_offset) const {

  TVector3 det_pos = GetPosition(before,apply_offset);
  //TVector3 recon_pos = TJanusDDAS::GetReconPosition(det_pos.Theta(),det_pos.Phi(),reac,d_p,r_p,s2);

  //return recon_pos;

  return TJanusDDAS::GetReconPosition(det_pos.Theta(),det_pos.Phi(),reac,d_p,r_p,s2);
  
  //double theta = TJanusDDAS::GetReconPosition(pos.Theta(),pos.Phi(),beamname,targetname,sfile).Theta();
  //double phi   = GetPosition(before,apply_array_offset).Phi()-TMath::Pi();

  //TVector3 v;
  //v.SetMagThetaPhi(1,theta,phi);
  //v.SetMagThetaPhi(1,recon_pos.Theta(),recon_pos.Phi());
  //return v;

}
/*
TVector3 TJanusDDASHit::GetReconPosition(int d_p, int r_p, bool s2, bool before, bool apply_offset) const {

  TVector3 det_pos = GetPosition(before,apply_offset);
  return TJanusDDAS::GetReconPosition(det_pos.Theta(),det_pos.Phi(),d_p,r_p,s2);
  
}
*/

/*
TVector3 TJanusDDASHit::GetReconPosition(const char *beamname, const char *targetname, const char* sfile, bool before,
					 bool apply_array_offset) const {
  double theta = Reconstruct(beamname,targetname,sfile,before,apply_array_offset);
  double phi   = GetPosition(before,apply_array_offset).Phi()-3.141596;

  TVector3 v;
  v.SetMagThetaPhi(1,theta,phi);
  return v;
}
*/

double TJanusDDASHit::GetLabSolidAngle() const { return TJanusDDAS::LabSolidAngle(GetDetnum(),GetRing()); }

double TJanusDDASHit::GetCmSolidAngle(TReaction& reac, int part, bool before) const
{ return TJanusDDAS::CmSolidAngle(GetDetnum(),GetRing(),reac,part,before); }

/*
double TJanusDDASHit::Reconstruct(const char *beamname, const char *targetname, const char *srimfile, bool before,
				  bool apply_array_offset) const {
  static auto beam = std::make_shared<TNucleus>(beamname);   // "78Kr");
  static auto targ = std::make_shared<TNucleus>(targetname); // "208Pb");
  static TSRIM srim(srimfile); 
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

  //return beam based on target (give 3, calculate 2).
  return reac_mid.ConvertThetaLab(GetPosition(before,apply_array_offset).Theta(),3,2);  
}
*/







