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
  TDetectorFactoryBase* factory = detector_factory_map[detector];
  if(!factory){
    std::cout << "No factory to construct type " << detector << "\n"
              << "Please add it in libraries/TGRUTint/TGRUTTypes.cxx"
              << std::endl;
    return;
  }

  TDetector* current_det = NULL;
  for(auto det : detectors) {
    if(factory->is_instance(det)) {
      current_det = det;
      break;
    }
  }

  if(!current_det){
    current_det = factory->construct();
    detectors.push_back(current_det);
  }

  current_det->AddRawData(event);
}
