#include "TRootOutfileNSCL.h"

#include "Globals.h"

#include "TDetectorEnv.h"
#include "TGRUTOptions.h"
#include "TJanus.h"
#include "TSega.h"
#include "TNSCLEvent.h"

ClassImp(TRootOutfileNSCL);

TRootOutfileNSCL::TRootOutfileNSCL() {
  janus = NULL;
  sega  = NULL;
  // s800 = NULL;
  // caesar = NULL;

}

TRootOutfileNSCL::~TRootOutfileNSCL() {
  if(GetOutfile()){
    GetOutfile()->Close();
    GetOutfile()->Delete();
  }
}

void TRootOutfileNSCL::Init(const char* output_filename){
  if(output_filename==NULL){
    output_filename = "myNSCLoutput.root";
  }
  SetOutfile(output_filename);

  TTree *event_tree  = AddTree("EventTree","Events, yo.",true);
  TTree *scaler_tree = AddTree("ScalerTree","I can count.",false);

  if(TDetectorEnv::Janus()){
    event_tree->Branch("TJanus","TJanus",&janus);
    det_list[kDetectorSystems::JANUS] = janus;
  }

  if(TDetectorEnv::Sega()){
    event_tree->Branch("TSega","TSega",&sega);
    det_list[kDetectorSystems::SEGA] = sega;
  }

  InitHists();

  // if(TDetectorEnv::S800()){
  //   event_tree->Branch("TS800","TS800",&s800);
  //   det_list["TS800"] = s800;
  // }

  // if(TDetectorEnv::Caesar()){
  //   event_tree->Branch("TCaesar","TCaesar",&caesar);
  //   det_list["TSega"] = sega;
  // }

}

void TRootOutfileNSCL::InitHists() {

  return;
}

void TRootOutfileNSCL::FillHists() {
  return;
}




void TRootOutfileNSCL::Clear(Option_t *opt) {
  TRootOutfile::Clear(opt);
  if(janus)    janus->Clear(opt);
  if(sega)      sega->Clear(opt);
  //if(s800)      s800->Clear(opt);
  //if(caesar)  caesar->Clear(opt);

}

void TRootOutfileNSCL::Print(Option_t* opt) const {
  TRootOutfile::Print(opt);
  std::cout << "Janus: " << janus << "\n"
            << "Sega: " << sega << "\n"
            << std::flush;
}


