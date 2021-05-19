#include "TMode3Hit.h"

#include "TRandom.h"

#include "TGEBEvent.h"
#include "TGRUTOptions.h"
#include "TChannel.h"

#include "GH1D.h"
#include "GCanvas.h"

ClassImp(TMode3Hit)

bool TMode3Hit::fExtractWaves = true;

TMode3Hit::TMode3Hit(){ }

TMode3Hit::~TMode3Hit() { }

void TMode3Hit::BuildFrom(TSmartBuffer& buf){
  Clear();

  //buf.Print("all");

  bool read_waveform = TGRUTOptions::Get()->ExtractWaves();

  auto header = (TRawEvent::GEBMode3Head*)buf.GetData();
  buf.Advance(sizeof(TRawEvent::GEBMode3Head));

  board_id = header->board_id;
  SetAddress((1<<24) + board_id);

  auto data   = (TRawEvent::GEBMode3Data*)buf.GetData();
  buf.Advance(sizeof(TRawEvent::GEBMode3Data));

  led = data->GetLed();
  int charge = data->GetEnergy(*header);
  //SetCharge(data->GetEnergy(*header));
  if(charge<0) { charge*=-1; }  // we should prob remove this... pcb 
  SetCharge(charge);


  //std::cout << "Board ID:" << board_id << " and still sorting!" << std::endl ;

  //std::cout << "LED:" << led << std::endl ;

  dt1  = data->GetDeltaT1();
  dt2  = data->GetDeltaT2();

  //std::cout << "DT1:" << dt1 << std::endl ;
  //std::cout << "DT2:" << dt2 << std::endl ;

  charge0  = data->GetEnergy0(*header);
  charge1  = data->GetEnergy1(*header);
  charge2  = data->GetEnergy2(*header);

  cfd = data->GetCfd();

  size_t wave_bytes = header->GetLength()*4 - sizeof(*header) + 4 - sizeof(*data);
  //std::cout << "wave_bytes:  " << wave_bytes << std::endl;
  //std::cout << "timestamp:   " << led << std::endl;  
  //std::cout << "timestamp:   " << std::hex << led << std::dec << std::endl; 
  SetTimestamp(led);

  if(read_waveform){
    size_t wavesize = wave_bytes/sizeof(short);

    //std::cout << "wave size:  " << wavesize << std::endl;
    waveform.resize(wavesize);

    memcpy((char*)&waveform[0], buf.GetData(), wave_bytes);

    for(unsigned int i=0; i<wavesize; i+=2){
      short tmp      = TRawEvent::SwapShort(waveform[i+1]);
      waveform[i+1] = TRawEvent::SwapShort(waveform[i]);
      waveform[i]   = tmp;
    }
  }
  //std::cout << "Charge:  " << Charge() << std::endl;
  //std::cout << "Charge0:  " << GetCharge0() << std::endl;
  //Print("all");
  buf.Advance(wave_bytes);
  //exit(0);
}





void TMode3Hit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);

  TMode3Hit& mode3 = (TMode3Hit&)obj;

  mode3.board_id = board_id;
  mode3.led      = led;
  mode3.cfd      = cfd;
  mode3.waveform = waveform;
}


void TMode3Hit::Print(Option_t *opt) const {
  TString sopt(opt);
  if(sopt.Contains("long")) {
    printf("TMode3Hit @ %lu \n",Timestamp());
    printf("\tHole:     %i\n",GetHole());
    printf("\tXtal:     %i\n",GetCrystal());
    printf("\tSegment:  %i\n",GetSegmentId());
    printf("\tCharge:   %i\n",Charge());
    printf("\tPickoff:  %i\n",GetCharge0());
    printf("\tWaveSize: %i\t%i\n",wavesize,(int)waveform.size());
  } else {
    printf("hole[%03i] xtal[%i] seg[%03i]:   %i\n",GetHole(),GetCrystal(),GetSegmentId(),GetCharge0());
  }
}

void TMode3Hit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  board_id = -1;
  led      = -1;
  cfd      = -1;
  dt1      =  0xffff;
  dt2      =  0xffff;
  charge0  = -1;
  charge1  = -1;
  charge2  = -1;

  waveform.clear();
}

double TMode3Hit::AverageWave(int samples) const {
  if(waveform.size() == 0) {
    return 0.0;
  }
  if(samples < 0 ||
     samples > int(waveform.size())) {
    samples = waveform.size();
  }
  double sum = 0.0;
  for(int i=0;i<samples;i++) {
    //wsum += (double)i*(std::abs((double)waveform[i]));
    sum += waveform[i];
  }
  //return wsum/sum;
  return sum / ((double)samples);
}


void TMode3Hit::Draw(Option_t *opt) const {
  if(!waveform.size())
    return;
  TString option = opt;
  if(!gPad || option.Contains("new",TString::kIgnoreCase)) {
    new GCanvas;
  } else {
    gPad->Clear();
  }
  //double avg = AverageWave(10);
  
  GH1D wave("wave",Form("0x%08x",Address()),(int)waveform.size(),0,(double)waveform.size());
  for(unsigned int x=0;x<waveform.size();x++) 
    wave.Fill(x,waveform.at(x));
  wave.DrawCopy(opt);

}

double TMode3Hit::GetEnergy0() const {
  double energy;
  TChannel* chan = TChannel::GetChannel(Address());
  if(!chan){
    energy = charge0/128 + gRandom->Uniform();
    //return Charge() + gRandom->Uniform();
  } else {
    energy = chan->CalEnergy(charge0/128, fTimestamp);
  }
  return energy;
}

double TMode3Hit::GetEnergy1() const {
  double energy;
  TChannel* chan = TChannel::GetChannel(Address());
  if(!chan){
    energy = charge1/128 + gRandom->Uniform();
    //return Charge() + gRandom->Uniform();
  } else {
    energy = chan->CalEnergy(charge1/128, fTimestamp);
  }
  return energy;
}

double TMode3Hit::GetEnergy2() const {
  double energy;
  TChannel* chan = TChannel::GetChannel(Address());
  if(!chan){
    energy = charge2/128 + gRandom->Uniform();
    //return Charge() + gRandom->Uniform();
  } else {
    energy = chan->CalEnergy(charge2/128, fTimestamp);
  }
  return energy;
}
