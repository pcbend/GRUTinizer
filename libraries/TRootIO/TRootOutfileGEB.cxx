#include "TRootOutfileGEB.h"

#include "Globals.h"

#include "TDetectorEnv.h"
#include "TGRUTOptions.h"
#include "TGretina.h"
#include "TMode3.h"
#include "TGEBEvent.h"


TRootOutfileGEB::TRootOutfileGEB() {
  gretina = NULL;
  // s800 = NULL;
  // phoswall = NULL;
}

TRootOutfileGEB::~TRootOutfileGEB() {
  if(outfile){
    outfile->Close();
    delete outfile;
  }
}

void TRootOutfileGEB::Init(const char* output_filename){
  if(output_filename==NULL){
    output_filename = "my_output.root";
  }
  outfile = new TFile(output_filename, "RECREATE");

  event_tree = new TTree("EventTree","EventTree");
  scaler_tree = new TTree("ScalerTree","ScalerTree");

  if(TDetectorEnv::Janus()){
    event_tree->Branch("TGretina","TGretina",&gretina);
    det_list[kDetectorSystems::GRETINA] = gretina;
  }

  if(TDetectorEnv::Sega()){
    event_tree->Branch("TSega","TSega",&sega);
    det_list[kDetectorSystems::SEGA] = sega;
  }

  // if(TDetectorEnv::S800()){
  //   event_tree->Branch("TS800","TS800",&s800);
  //   det_list["TS800"] = s800;
  // }

  // if(TDetectorEnv::Gretina()){
  //   event_tree->Branch("TGretina","TGretina",&gretina);
  //   det_list["TGretina"] = gretina;
  // }

  // if(TDetectorEnv::Caesar()){
  //   event_tree->Branch("TCaesar","TCaesar",&caesar);
  //   det_list["TSega"] = sega;
  // }

  // if(TDetectorEnv::Phoswall()){
  //   event_tree->Branch("TPhoswall","TPhoswall",&phoswall);
  //   det_list["TPhoswall"] = phoswall;
  // }
}

void TRootOutfileGEB::AddRawData(const TRawEvent& event, kDetectorSystems det_type){
  try{
    TDetector* det = det_list.at(det_type);
    det->AddRawData(event);
  } catch (std::out_of_range& e) { }
}

void TRootOutfileGEB::FillTree(){
  for(auto& item : det_list){
    item.second->Build();
  }

  event_tree->Fill();
  scaler_tree->Fill();

  for(auto& item : det_list){
    item.second->Clear();
  }
}

void TRootOutfileGEB::FinalizeFile(){
  if(outfile){
    outfile->cd();
  } else {
    return;
  }

  std::cout << "Writing trees" << std::endl;
  event_tree->Write();
  scaler_tree->Write();

  CloseFile();
}

void TRootOutfileGEB::CloseFile(){
  outfile->Close();
}

void TRootOutfileGEB::Print(Option_t* opt){
  std::cout << "Janus: " << janus << "\n"
            << "Sega: " << sega << "\n"
            // << "S800: " << s800 << "\n"
            // << "Gretina: " << gretina << "\n"
            // << "Caesar: " << caesar << "\n"
            // << "Phoswall: " << phoswall << "\n"
            << std::flush;
}


// void TChannel::Streamer(TBuffer &R__b) {
//    this->SetBit(kCanDelete);
//    UInt_t R__s, R__c;
//    if(R__b.IsReading()) { // reading from file
//       Version_t R__v = R__b.ReadVersion(&R__s,&R__c); if (R__v) { }
//       TNamed::Streamer(R__b);
//       { TString R__str; R__str.Streamer(R__b); fFileName.assign(R__str.Data()); }
//       { TString R__str; R__str.Streamer(R__b); fFileData.assign(R__str.Data()); }
//       InitChannelInput();
//       R__b.CheckByteCount(R__s,R__c,TChannel::IsA());
//    } else {               // writing to file
//       R__c = R__b.WriteVersion(TChannel::IsA(),true);
//       TNamed::Streamer(R__b);
//       {TString R__str = fFileName.c_str(); R__str.Streamer(R__b);}
//       {TString R__str = fFileData.c_str(); R__str.Streamer(R__b);}
//       R__b.SetByteCount(R__c,true);
//    }
// }
