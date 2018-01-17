#include <iostream>

#include "TTree.h"
#include "TFile.h"

#include "TS800Scaler.h"
#include "TDetector.h"

void getScalerCounts(char *input_root_file_name, int final_entry){
  TFile *input_root_file = new TFile(input_root_file_name, "read");
  if (!input_root_file){
    std::cout << "Failed to open the input root file!"<<std::endl;
    return;
  }
  
  TTree *in_tree = (TTree*)input_root_file->Get("EventTree");
  if (!in_tree){
    std::cout << "Failed to open the input tree file!"<<std::endl;
    return;
  }

  TS800Scaler *scalers= 0; 
  in_tree->SetBranchAddress("TS800Scaler", &scalers);
  std::vector<int> scaler_250(250);//incremental scalers
  std::vector<int> scaler_32(32);//periodic scaler
  std::vector<int> scaler_32_overflows(32);//periodic scaler overflow check. adds 2^24 to result

  int n_entries = in_tree->GetEntries();
  if (final_entry != 0){
    n_entries = final_entry;//forces readout to stop wherever final entry is
  }

  double prev_live_trig = 0;
  double prev_raw_trig  = 0;
  for (int entry = 0; entry < n_entries; entry++){
    scalers->Clear("");
    in_tree->GetEntry(entry);

    if (scalers->Size() == 250){
      //ignoring incremental scalers for now
      continue;
//    for (int i = 0; i < 250;i++){
//      scaler_250.at(i) += scalers->GetScaler(i);
//    }
    }
    if (scalers->Size() == 32){
      for (int i = 31; i > 0;i--){
        if (scalers->GetScaler(12) != scalers->GetScaler(11)){
          if (scalers->GetScaler(9) != prev_raw_trig|| scalers->GetScaler(10) != prev_live_trig){
            if (scalers->GetScaler(i) < scaler_32.at(i)){
              scaler_32_overflows.at(i) += 1;
              std::cout << "incrementing overflow on channel " << i << " for event " << entry << "\n";
            }
            scaler_32.at(i) = scalers->GetScaler(i);
          }//if triggers are no longer chanigng, we stop.
        }//if raw clock = live clock, we're in the startup phase
      }//loop over scaler channels
      prev_raw_trig = scaler_32.at(9);
      prev_live_trig = scaler_32.at(10);
    }//size 32
    if (entry % 100000 == 0){
      std::cout << "Completed " << entry << " entries\n";
    }
  }//loop over tree

  for (unsigned int i = 0; i < scaler_32.size(); i++){
    scaler_32.at(i) += scaler_32_overflows.at(i)*pow(2.,24.);
  }
  int live_clock = scaler_32.at(12);
  int raw_clock = scaler_32.at(11);
  int live_trigger = scaler_32.at(10);
  int raw_trigger = scaler_32.at(9);

//live_clock     += scaler_32_overflows.at(12)*pow(2.,24.);
//raw_clock      += scaler_32_overflows.at(11)*pow(2.,24.);
//live_trigger   += scaler_32_overflows.at(10)*pow(2.,24.);
//raw_trigger    += scaler_32_overflows.at(9)*pow(2.,24.);
//second_source  += scaler_32_overflows.at(1)*pow(2.,24.);
  std::cout << "raw.clock\tlive.clock\traw.trigger\tlive.trigger" << std::endl;
  std::cout << raw_clock   << "\t" << live_clock   <<"\t"
            << raw_trigger << "\t" << live_trigger << "\n";

  for (unsigned int i = 0; i < scaler_32.size(); i++){
    std::cout << "Channel " << i << ": " << scaler_32.at(i) << "\n";
  }
}
#ifndef __CINT__

int main(int argc, char**argv){
   
  if (argc < 2){
    std::cout << "USAGE: getScalerCounts INPUT_FILE_NAME [ENTRY TO STOP AT]" << std::endl;
  }
  
  int final_entry = 0;
  if (argc == 3){
    final_entry = std::stoi(argv[2]);
  }
  getScalerCounts(argv[1], final_entry);
  return 0;
}

#endif
