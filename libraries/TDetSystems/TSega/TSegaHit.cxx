#include "TSegaHit.h"

#include <algorithm>
#include <iostream>

#include "TString.h"

#include "GCanvas.h"
#include "TSega.h"


TSegaHit::TSegaHit() {
  Clear();
}

void TSegaHit::Copy(TObject& obj) const{
  TDetectorHit::Copy(obj);

  TSegaHit& sega = (TSegaHit&)obj;
  sega.fTrace = fTrace;
}

void TSegaHit::Draw(Option_t* opt) {
  TString option = opt;
  if(!gPad || option.Contains("new", TString::ECaseCompare::kIgnoreCase)) {
    new GCanvas;
  } else {
    gPad->Clear();
  }

  TVirtualPad* base_pad = gPad;

  bool draw_all = option.Contains("all", TString::ECaseCompare::kIgnoreCase);
  if(draw_all){
    base_pad->Divide(1, 2, 0.005, 0.005);
    base_pad->cd(2)->Divide(4,8,0,0);
    base_pad->cd(1);
  }

  DrawTrace(0);

  if(draw_all){
    for(int i=1; i<33; i++) {
      base_pad->cd(2)->cd(i);
      DrawTrace(i);
    }
  }
  base_pad->cd();
}

void TSegaHit::DrawTrace(int segnum) {
  std::vector<unsigned short>* trace = GetTrace(segnum);
  if(!trace){
    std::cout << "No segment trace found for segment " << segnum << std::endl;
    return;
  }

  TH1I hist("hist", "", trace->size(), 0, 10*trace->size());
  hist.SetStats(false);

  if(segnum==0){
    hist.SetTitle(Form("SeGA Detector %d at %d ns", GetDetnum(), Timestamp()));
    hist.GetXaxis()->SetTitle("Time (ns)");
    hist.GetYaxis()->SetTitle("ADC units");
  }

  for(int i=0; i<trace->size(); i++) {
    hist.SetBinContent(i+1,(*trace)[i]);
  }
  hist.DrawCopy();
}

void TSegaHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fTrace.clear();
}

void TSegaHit::Print(Option_t *opt) const {
  std::cout << "TSegaHit:\n"
            << "\tChannel: " << GetChannel() << "\n"
            << "\tCharge: " << Charge() << "\n"
            << std::flush;
}

void TSegaHit::SetTrace(unsigned int trace_length, const unsigned short* trace) {
  if(!trace){
    fTrace.clear();
    return;
  }

  fTrace.clear();
  fTrace.reserve(trace_length);
  for(unsigned int i=0; i<trace_length; i++){
    fTrace.push_back(trace[i]);
  }
}

int TSegaHit::GetDetnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  if(chan){
    return chan->GetArrayPosition();
  } else if(fSegments.size()) {
    return fSegments[0].GetDetnum();
  } else {
    return -1;
  }
}

int TSegaHit::GetCrate() const {
  return (fAddress&0x00ff0000)>>16;
}

int TSegaHit::GetSlot() const {
  return (fAddress&0x0000ff00)>>8;
}

int TSegaHit::GetChannel() const {
  return (fAddress&0x000000ff)>>0;
}

TSegaSegmentHit& TSegaHit::MakeSegmentByAddress(unsigned int address){
  for(auto& segment : fSegments){
    if(segment.Address() == address){
      return segment;
    }
  }

  fSegments.emplace_back();
  TSegaSegmentHit& output = fSegments.back();
  output.SetAddress(address);
  return output;
}

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

int TSegaHit::GetMainSegnum() const {
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

TVector3 TSegaHit::GetPosition() const {
  return TSega::GetSegmentPosition(GetDetnum(), GetMainSegnum());
}

Int_t TSegaHit::Charge() const {
  if(fCharge > 30000) {
    return fCharge - 32768;
  } else {
    return fCharge;
  }
}

double TSegaHit::GetDCEnergy(double beta, TVector3 particle_dir) const {
  TVector3 pos = GetPosition();
  double dc_angle = pos.Angle(particle_dir);
  double cos_angle = std::cos(dc_angle);
  double gamma = 1/std::sqrt(1.0 - beta*beta);
  double dc_en = gamma * (1 - beta*cos_angle) * GetEnergy();

  return dc_en;
}
