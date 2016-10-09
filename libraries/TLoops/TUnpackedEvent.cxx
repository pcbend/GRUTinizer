#include "TUnpackedEvent.h"

#include "TClass.h"

#include "TBank29.h"
#include "TCaesar.h"
#include "TGretina.h"
#include "TGretSim.h"
#include "TJanus.h"
#include "TMode3.h"
#include "TNSCLScalers.h"
#include "TPhosWall.h"
#include "TS800.h"
#include "TS800Sim.h"
#include "TS800Scaler.h"
#include "TSega.h"
#include "TFastScint.h"
#include "TCagra.h"
#include "TGrandRaiden.h"

TUnpackedEvent::TUnpackedEvent() { }

TUnpackedEvent::~TUnpackedEvent() {
  for(auto det : detectors) {
    delete det;
  }
}

void TUnpackedEvent::Build() {
  for(auto& item : raw_data_map) {
    kDetectorSystems detector = item.first;
    std::vector<TRawEvent>& raw_data = item.second;

    switch(detector){
    case kDetectorSystems::GRETINA:
      GetDetector<TGretina>(true)->Build(raw_data);
      break;

    case kDetectorSystems::GRETINA_SIM:
      GetDetector<TGretSim>(true)->Build(raw_data);
      break;

    case kDetectorSystems::MODE3:
      GetDetector<TMode3>(true)->Build(raw_data);
      break;

    case kDetectorSystems::S800:
      GetDetector<TS800>(true)->Build(raw_data);
      break;

    case kDetectorSystems::S800_SIM:
      GetDetector<TS800Sim>(true)->Build(raw_data);
      break;

    case kDetectorSystems::S800SCALER:
      GetDetector<TS800Scaler>(true)->Build(raw_data);
      break;

    case kDetectorSystems::BANK29:
      GetDetector<TBank29>(true)->Build(raw_data);
      break;

    case kDetectorSystems::SEGA:
      GetDetector<TSega>(true)->Build(raw_data);
      break;

    case kDetectorSystems::JANUS:
      GetDetector<TJanus>(true)->Build(raw_data);
      break;

    case kDetectorSystems::FASTSCINT:
      GetDetector<TFastScint>(true)->Build(raw_data);
      break;

    case kDetectorSystems::CAESAR:
      GetDetector<TCaesar>(true)->Build(raw_data);
      break;

    case kDetectorSystems::PHOSWALL:
      GetDetector<TPhosWall>(true)->Build(raw_data);
      break;

    case kDetectorSystems::NSCLSCALERS:
      GetDetector<TNSCLScalers>(true)->Build(raw_data);
      break;

    case kDetectorSystems::ANL:
      GetDetector<TCagra>(true)->Build(raw_data);
      break;

    case kDetectorSystems::GRAND_RAIDEN:
      GetDetector<TGrandRaiden>(true)->Build(raw_data);
      break;

    default:
      break;
    }
  }
}

void TUnpackedEvent::AddRawData(const TRawEvent& event, kDetectorSystems detector) {
  raw_data_map[detector].push_back(event);
}

void TUnpackedEvent::ClearRawData() {
  raw_data_map.clear();
}

void TUnpackedEvent::SetRunStart(unsigned int unix_time){
  for(auto det : detectors){
    det->SetRunStart(unix_time);
  }
}
