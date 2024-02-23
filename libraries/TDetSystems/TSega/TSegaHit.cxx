#include "TSegaHit.h"

#include <algorithm>
#include <iostream>

#include "TString.h"

#include "GCanvas.h"
#include "GValue.h"
#include "TSega.h"


TSegaHit::TSegaHit() {
  Clear();
}

/*******************************************************************************/
/* Copies hit ******************************************************************/
/*******************************************************************************/
void TSegaHit::Copy(TObject& obj) const{
  TDetectorHit::Copy(obj);
  TSegaHit& sega = (TSegaHit&)obj;
  sega.fTrace = fTrace;
}

/*******************************************************************************/
/* Clear hit *******************************************************************/
/*******************************************************************************/
void TSegaHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fTrace.clear();
}

/*******************************************************************************/
/* Basic Print function ********************************************************/
/*******************************************************************************/
void TSegaHit::Print(Option_t *opt) const {
  std::cout << "TSegaHit:\n" << "\tChannel: " << GetChannel() << "\n" << "\tCharge: " << Charge() << "\n" << std::flush;
}

/*******************************************************************************/
/* Sets trace information if present in data ***********************************/
/*******************************************************************************/
void TSegaHit::SetTrace(unsigned int trace_length, const unsigned short* trace) {
  if(!trace){
    fTrace.clear();
    return;
  }

  fTrace.clear();
  return;
  fTrace.reserve(trace_length);
  for(unsigned int i=0; i<trace_length; i++){
    fTrace.push_back(trace[i]);
  }
}

/*******************************************************************************/
/* Check if core energy present, used when building hits ***********************/
/*******************************************************************************/
bool TSegaHit::HasCore() const {
  return fCharge != -1;
}

/*******************************************************************************/
/* Returns detector number based on channels.cal file definition ***************/
/*******************************************************************************/
int TSegaHit::GetDetnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  int output = -1;
  if(chan && fAddress!=-1){
    output = chan->GetArrayPosition();
  } else if(fSegments.size()) {
    output = fSegments[0].GetDetnum();
  } else {
    // std::cout << "Unknown address: " << std::hex << fAddress << std::dec << std::endl;
    output = -1;
  }
  return output;
}

/*******************************************************************************/
/* Returns DDAS crate/slot channel number **************************************/
/*******************************************************************************/
int TSegaHit::GetCrate() const {
  return (fAddress&0x00ff0000)>>16;
}

int TSegaHit::GetSlot() const {
  return (fAddress&0x0000ff00)>>8;
}

int TSegaHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}

/*******************************************************************************/
/* Adds segments to SeGA event *************************************************/
/*******************************************************************************/
TSegaSegmentHit& TSegaHit::MakeSegmentByAddress(unsigned int address){
  fSegments.emplace_back();
  TSegaSegmentHit& output = fSegments.back();
  output.SetAddress(address);
  return output;
}


/*******************************************************************************/
/* Gets SeGA core energy trace *************************************************/
/*******************************************************************************/
std::vector<unsigned short>* TSegaHit::GetTrace(int segnum) {
  if(segnum == 0){
    return &fTrace;
  }
  for(auto& seg : fSegments) {
    if(seg.GetSegnum() == segnum) {
      return &seg.GetTrace();
    }
  }
  return NULL;
}

/*******************************************************************************/
/* Returns the highest energy segment number used for doppler correction *******/
/*******************************************************************************/
int TSegaHit::GetMainSegnum() const {
  int output = 0;
  double max_energy = -9e99;
  for(auto& segment : fSegments){
    if((segment.GetEnergy() > max_energy) && segment.GetEnergy() < 30000) {
      output = segment.GetSegnum();
      max_energy = segment.GetEnergy();
    }
  }
  return output;
}

/*******************************************************************************/
/* Returns position of interaction based on highest segment energy *************/
/*******************************************************************************/
TVector3 TSegaHit::GetPosition(bool apply_array_offset, TVector3 array_offset) const {
  TVector3 array_pos = TSega::GetSegmentPosition(GetDetnum(), GetMainSegnum());
  if(apply_array_offset){
    if(std::isnan(array_offset.X()) && std::isnan(array_offset.Y()) && std::isnan(array_offset.Z())) {
      array_offset = TVector3(GValue::Value("Sega_X_offset"), GValue::Value("Sega_Y_offset"), GValue::Value("Sega_Z_offset"));
    }
    array_pos += array_offset;
  }
  return array_pos;
}

/*******************************************************************************/
/* Uncalibrated SeGA energies **************************************************/
/*******************************************************************************/
Int_t TSegaHit::Charge() const {
  if(fCharge > 30000) {
    return fCharge - 32768;
  } else {
    return fCharge;
  }
}

/*******************************************************************************/
/* Returns doppler corrected energies based on highest segment energy **********/
/* SeGA positions, particle vectors and beam beta ******************************/
/*******************************************************************************/
double TSegaHit::GetDoppler(double beta,const TVector3& particle_vec, const TVector3& sega_offset) const {
  if(GetNumSegments()<1) {
    return std::sqrt(-1);
  }
  double gamma = 1/(sqrt(1-pow(beta,2)));
  TVector3 pos = GetPosition(true, sega_offset);
  double cos_angle = TMath::Cos(pos.Angle(particle_vec));
  double dc_en = GetEnergy()*gamma *(1 - beta*cos_angle);
  return dc_en;
}

/*******************************************************************************/
/* Simple energy evaluation from traces ****************************************/
/*******************************************************************************/
double TSegaHit::GetTraceHeight() const {
  if(fTrace.size() < 20){
    return std::sqrt(-1);
  }
  double low = 0;
  double high = 0;
  for(unsigned int i=0; i<10; i++){
    low += fTrace[i];
    high += fTrace[fTrace.size()-i-1];
  }
  return (high-low)/10;
}

/*******************************************************************************/
/* Simple doppler correction with energy evaluated from trace ******************/
/*******************************************************************************/
double TSegaHit::GetTraceHeightDoppler(double beta,const TVector3& vec) const {
  if(GetNumSegments()<1) {
    return std::sqrt(-1);
  }
  double gamma = 1/(sqrt(1-pow(beta,2)));
  TVector3 pos = GetPosition();
  double cos_angle = TMath::Cos(pos.Angle(vec));
  double dc_en = GetTraceHeight()*gamma *(1 - beta*cos_angle);
  return dc_en;
}
