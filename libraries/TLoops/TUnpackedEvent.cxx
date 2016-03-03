#include "TUnpackedEvent.h"

#include "TClass.h"

#include "TBank29.h"
#include "TCaesar.h"
#include "TGretina.h"
#include "TGretSim.h"
#include "TJanus.h"
#include "TMode3.h"
#include "TPhosWall.h"
#include "TS800.h"
#include "TS800Sim.h"
#include "TS800Scaler.h"
#include "TSega.h"
#include "TFastScint.h"

TUnpackedEvent::TUnpackedEvent() { }

TUnpackedEvent::~TUnpackedEvent() {
  for(auto det : detectors) {
    delete det;
  }
}

void TUnpackedEvent::Build() {
  for(auto det : detectors) {
    det->Build();
  }
}

void TUnpackedEvent::AddRawData(const TRawEvent& event, kDetectorSystems detector) {
  switch(detector){
  case kDetectorSystems::GRETINA:
    GetDetector<TGretina>(true)->AddRawData(event);
    break;

  case kDetectorSystems::GRETINA_SIM:
    GetDetector<TGretSim>(true)->AddRawData(event);
    break;

  case kDetectorSystems::MODE3:
    GetDetector<TMode3>(true)->AddRawData(event);
    break;

  case kDetectorSystems::S800:
    GetDetector<TS800>(true)->AddRawData(event);
    break;

  case kDetectorSystems::S800_SIM:
    GetDetector<TS800Sim>(true)->AddRawData(event);
    break;

  case kDetectorSystems::S800SCALER:
    GetDetector<TS800Scaler>(true)->AddRawData(event);
    break;

  case kDetectorSystems::BANK29:
    GetDetector<TBank29>(true)->AddRawData(event);
    break;

  case kDetectorSystems::SEGA:
    GetDetector<TSega>(true)->AddRawData(event);
    break;

  case kDetectorSystems::JANUS:
    GetDetector<TJanus>(true)->AddRawData(event);
    break;

  case kDetectorSystems::FASTSCINT:
    GetDetector<TFastScint>(true)->AddRawData(event);
    break;

  case kDetectorSystems::CAESAR:
    GetDetector<TCaesar>(true)->AddRawData(event);
    break;

  case kDetectorSystems::PHOSWALL:
    GetDetector<TPhosWall>(true)->AddRawData(event);
    break;

  case kDetectorSystems::ANL:
    GetDetector<TArgonne>(true)->AddRawData(event);
    break;

  default:
    break;
  }
}
