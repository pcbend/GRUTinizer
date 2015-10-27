#include "TRootOutfileNSCL.h"

#include "Globals.h"

#include "TDetectorEnv.h"
#include "TGRUTOptions.h"
#include "TJanus.h"
#include "TSega.h"
#include "TNSCLEvent.h"

ClassImp(TRootOutfileNSCL);

TRootOutfileNSCL::TRootOutfileNSCL() {
  janus = new TJanus;
  sega  = new TSega;
  // s800 = NULL;
  // caesar = NULL;

}

TRootOutfileNSCL::~TRootOutfileNSCL() {
  if(GetOutfile()){
    GetOutfile()->Close();
    GetOutfile()->Delete();
  }
  delete janus;
  delete sega;
}

void TRootOutfileNSCL::Init(){
  AddTree("EventTree", "Events, yo.", true, 1000);
  AddTree("ScalerTree","I can count.",false, -1);

  if(TDetectorEnv::Janus()){
    AddBranch("EventTree","TJanus","TJanus",
              (TDetector**)&janus,kDetectorSystems::JANUS);
  }

  if(TDetectorEnv::Sega()){
    AddBranch("EventTree","TSega","TSega",
              (TDetector**)&sega,kDetectorSystems::SEGA);
  }

  // if(TDetectorEnv::S800()){
  //   event_tree->Branch("TS800","TS800",&s800);
  //   det_list["TS800"] = s800;
  // }

  // if(TDetectorEnv::Caesar()){
  //   event_tree->Branch("TCaesar","TCaesar",&caesar);
  //   det_list["TSega"] = sega;
  // }

}




void TRootOutfileNSCL::Clear(Option_t *opt) {
  TRootOutfile::Clear(opt);
}

void TRootOutfileNSCL::Print(Option_t* opt) const {
  TRootOutfile::Print(opt);
  std::cout << "Janus: " << janus << "\n"
            << "Sega: " << sega << "\n"
            << std::flush;
}
