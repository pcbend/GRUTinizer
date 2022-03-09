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
#include "TLenda.h"

TUnpackedEvent::TUnpackedEvent() { }

TUnpackedEvent::~TUnpackedEvent() {
  for(auto det : detectors) {
    delete det;
  }
}

void TUnpackedEvent::Build() {
  //printf("i am called\n"); fflush(stdout);
  int counter =0;
  for(auto& item : raw_data_map) {
    //printf("counter = %i\n",counter++);
    kDetectorSystems detector = item.first;
    std::vector<TRawEvent>& raw_data = item.second;
 
    //printf("det %s\n",GetDetector(detector, true)->Class()->GetName());
    GetDetector(detector, true)->Build(raw_data);

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


TDetector* TUnpackedEvent::GetDetector(std::string detname) const {
  for(auto det : detectors) {
    if(detname.compare(det->IsA()->GetName())==0) {
      TDetector* output = det;
      if(output){
        return output;
      }
    }
  }
  return NULL;
}


TDetector* TUnpackedEvent::GetDetector(kDetectorSystems detector, bool make_if_not_found) {
  TDetectorFactoryBase* factory = detector_factory_map[detector];
  if(!factory){
    std::cout << "No factory to construct type " << detector << "\n"
              << "Please add it in libraries/TGRUTint/TGRUTTypes.cxx"
              << std::endl;
    return NULL;
  }

  TDetector* current_det = NULL;
  for(auto det : detectors) {
    if(factory->is_instance(det)) {
      current_det = det;
      break;
    }
  }

  if(make_if_not_found && !current_det){
    current_det = factory->construct();
    detectors.push_back(current_det);
  }

  return current_det;
}
