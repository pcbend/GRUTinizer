#include "TRootOutfileGEB.h"

#include "Globals.h"

#include "TGEBEvent.h"
#include "TDetectorEnv.h"
#include "TGRUTOptions.h"

#include "TGretina.h"
#include "TBank29.h"
#include "TMode3.h"
#include "TS800.h"

ClassImp(TRootOutfileGEB)

TRootOutfileGEB::TRootOutfileGEB() {
  fMode3Init = false;
  // gretina = NULL;
  // bank29  = NULL;
  // mode3   = NULL;
  // s800    = NULL;
  gretina = new TGretina;
  bank29  = new TBank29;
  mode3   = new TMode3;
  s800    = new TS800;
  // // phoswall = NULL;
}

TRootOutfileGEB::~TRootOutfileGEB() { }

void TRootOutfileGEB::Init(const char* output_filename){
  bool is_online = TGRUTOptions::Get()->IsOnline();

  if(is_online){
    SetOutfile(NULL);
  } else {
    if(output_filename==NULL){
      output_filename = "my_output.root";
    }
    SetOutfile(output_filename);
  }

  //       Name          Title       Build?  Window  obvious
  AddTree("EventTree", "Mode 2 data",true,    1000,  is_online);
  AddTree("ScalerTree","ScalerTree", false,   -1,    is_online);

  if(TDetectorEnv::Gretina()){
    AddBranch("EventTree", "TGretina", "TGretina",
              (TDetector**)&gretina, kDetectorSystems::GRETINA);
  }

  if(TDetectorEnv::Bank29()) {
    AddBranch("EventTree","TBank29","TBank29",
              (TDetector**)&bank29,kDetectorSystems::BANK29);
  }

  if(TDetectorEnv::S800()){
    AddBranch("EventTree","TS800","TS800",
              (TDetector**)&s800, kDetectorSystems::S800);
  }

  if(TDetectorEnv::Mode3()){
    AddBranch("EventTree","TMode3","TMode3",
              (TDetector**)&mode3, kDetectorSystems::MODE3);
  }


  // if(TDetectorEnv::Caesar()){
  //   event_tree->Branch("TCaesar","TCaesar",&caesar);
  //   det_list["TSega"] = sega;
  // }

  // if(TDetectorEnv::Phoswall()){
  //   event_tree->Branch("TPhoswall","TPhoswall",&phoswall);
  //   det_list["TPhoswall"] = phoswall;
  // }
  InitHists();

}

void TRootOutfileGEB::InitHists() {
  return;
}

void TRootOutfileGEB::FillHists() {
  return;
}

void TRootOutfileGEB::Clear(Option_t *opt) {
  TRootOutfile::Clear(opt);
  if(gretina) gretina->Clear();
  if(mode3)   mode3->Clear();
  if(bank29)  bank29->Clear();
  if(s800)    s800->Clear();
}


void TRootOutfileGEB::HandleMode3(const TMode3 &rhs) {
  //printf("mode3 = 0x%08x\n",mode3);  fflush(stdout);
  if(!fMode3Init) {
    AddTree("Data","Mode 3 data");
    TTree *mode3_tree = FindTree("Data");
    mode3_tree->Branch("TMode3","TMode3",&mode3);
    fMode3Init=true;
  }
  //printf("mode3 = 0x%08x\n",mode3);  fflush(stdout);
  rhs.Copy(*mode3);
  FindTree("Data")->Fill();
}




void TRootOutfileGEB::Print(Option_t* opt) const {
  std::cout << "Comming Soon!\n"
            << std::flush;
}
