#include "TCagraHit.h"

#include "TCagra.h"

#include <algorithm>
#include <iostream>

#include "TString.h"

#include "GCanvas.h"
#include "GValue.h"

#include "TGRUTOptions.h"

ClassImp(TCagraHit)

TCagraHit::TCagraHit(){

}

TCagraHit::~TCagraHit() {

}
void TCagraHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);
}
void TCagraHit::Print(Option_t *opt) const {
}
void TCagraHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fTrace.clear();
}
bool TCagraHit::HasCore() const {
  return fCharge != -1;
}

int TCagraHit::GetDetnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  int output = -1;
  if(chan && fAddress!=-1){
    output = chan->GetArrayPosition();
  } else if(fSegments.size()) {
    output = fSegments[0].GetDetnum();
  } else {
    // std::cout << "Unknown address: " << std::hex << fAddress << std::dec
    //           << std::endl;
    output = -1;
  }

  if(output == -1 && chan){
    // std::cout << "Chan with det=-1: " << chan->GetName() << std::endl;
    // std::cout << "address: " << fAddress << std::endl;
  }

  return output;
}
char TCagraHit::GetLeaf() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  char output = (char)-1;
  if(chan && fAddress!=-1){
    output = *chan->GetArraySubposition();
  } else if(fSegments.size()) {
    output = fSegments[0].GetLeaf();
  } else {
    // std::cout << "Unknown address: " << std::hex << fAddress << std::dec
    //           << std::endl;
    output = (char)-1;
  }

  if(output == -1 && chan){
    // std::cout << "Chan with det=-1: " << chan->GetName() << std::endl;
    // std::cout << "address: " << fAddress << std::endl;
  }

  return output;
}
// int TCagraHit::GetCrate() const {
//   return (fAddress&0x00ff0000)>>16;
// }

int TCagraHit::GetBoardID() const {
  return (fAddress&0x0000ff00)>>8;
}

int TCagraHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}

TCagraSegmentHit& TCagraHit::MakeSegmentByAddress(unsigned int address){
  // for(auto& segment : fSegments){
  //   if(segment.Address() == address){
  //     return segment;
  //   }
  // }

  fSegments.emplace_back();
  TCagraSegmentHit& output = fSegments.back();
  output.SetAddress(address);
  return output;
}

int TCagraHit::GetMainSegnum() const {
  int output = 0;
  double max_energy = -9e99;
  for(auto& segment : fSegments){
    if(segment.GetEnergy() > max_energy){
      output = segment.GetSegnum();
      max_energy = segment.GetEnergy();
    }
  }
  return output;
}

TVector3 TCagraHit::GetPosition(bool apply_array_offset) const {
  TVector3 array_pos = TCagra::GetSegmentPosition(GetDetnum(), GetLeaf(), GetMainSegnum());
  if(apply_array_offset){
    array_pos += TVector3(GValue::Value("Cagra_X_offset"),
                          GValue::Value("Cagra_Y_offset"),
                          GValue::Value("Cagra_Z_offset"));
  }
  return array_pos;
}

double TCagraHit::GetDoppler(double beta,const TVector3& particle_vec, const TVector3& offset) const {
  if(GetNumSegments()<1) {
    return std::sqrt(-1);
  }

  double gamma = 1/(sqrt(1-pow(beta,2)));
  TVector3 pos = GetPosition() + offset;
  double cos_angle = TMath::Cos(pos.Angle(particle_vec));
  double dc_en = GetEnergy()*gamma *(1 - beta*cos_angle);
  return dc_en;
}

Int_t TCagraHit::Charge() const {
  if(fCharge > 30000) {
    return fCharge - 32768;
  } else {
    return fCharge;
  }
}

void TCagraHit::DrawTrace(int segnum) {
  std::vector<Short_t>* trace = GetTrace(segnum);
  if(!trace){
    std::cout << "No segment trace found for segment " << segnum << std::endl;
    return;
  }

  TH1I hist("hist", "", trace->size(), 0, 10*trace->size());
  hist.SetStats(false);

  if(segnum==0){
    hist.SetTitle(Form("CAGRA Detector %d at %ld ns", GetDetnum(), Timestamp()));
    hist.GetXaxis()->SetTitle("Time (ns)");
    hist.GetYaxis()->SetTitle("ADC units");
  }

  for(size_t i=0; i<trace->size(); i++) {
    hist.SetBinContent(i+1,(*trace)[i]);
  }
  hist.DrawCopy();
}

void TCagraHit::SetTrace(std::vector<Short_t>& trace) {
  fTrace.clear();
  fTrace.swap(trace);
}

std::vector<Short_t>* TCagraHit::GetTrace(int segnum) {
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

double TCagraHit::GetTraceHeight() const {
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

double TCagraHit::GetTraceHeightDoppler(double beta,const TVector3& vec) const {
  if(GetNumSegments()<1) {
    return std::sqrt(-1);
  }

  double gamma = 1/(sqrt(1-pow(beta,2)));
  TVector3 pos = GetPosition();
  double cos_angle = TMath::Cos(pos.Angle(vec));
  double dc_en = GetTraceHeight()*gamma *(1 - beta*cos_angle);
  return dc_en;
}

Double_t TCagraHit::GetTraceEnergy(const UShort_t& a,const UShort_t& b,const UShort_t& x,const UShort_t& y) const {
  if (!fTrace.size()) { return 0; }

  if (fTrace.size() < y) {
    static int nprint = 0;
    if (nprint < 10) {
      std::cout << "Warning: Trace length less than requested sampling window: " << fTrace.size() <<std::endl;
    } nprint++;
    return 0;
  }

  double baseline = 0;
  for (int i=a; i<b; i++) { baseline+=fTrace[i]; }
  double integral = 0;
  for (int i=x; i<y; i++) { integral+=fTrace[i]; }

  return (integral/(y-x) - baseline/(b-a));
}
