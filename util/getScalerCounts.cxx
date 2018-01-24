#include <iostream>

#include "TTree.h"
#include "TFile.h"

#include "TS800Scaler.h"
#include "TDetector.h"


void initialize_map(std::map<int, std::string> &chan_map){
  chan_map[0] = "S800.Source";
  chan_map[1] = "Second.Source";
  chan_map[2] = "Ext1.Source";
  chan_map[3] = "Ext2.Source";
  chan_map[4] = "S800.Trigger";
  chan_map[5] = "Coinc.Trigger";
  chan_map[6] = "Ext1.Trigger";
  chan_map[7] = "Ext2.Trigger";
  chan_map[8] = "Second.Trigger";
  chan_map[9] = "Raw.Trigger";
  chan_map[10] = "Live.Trigger";
  chan_map[11] = "Raw.Clock";
  chan_map[12] = "Live.Clock";
  chan_map[14] = "10Hz";
  chan_map[15] = "1Hz";
  chan_map[16] = "E1.Up";
  chan_map[17] = "E1.Down";
  chan_map[18] = "XFP.Scint";
  chan_map[19] = "OBJ.Scint";
  chan_map[21] = "Galotte";
  chan_map[22] = "CRDC1.Anode";
  chan_map[23] = "CRDC2.Anode";
  chan_map[24] = "XFP.classic";
  chan_map[25] = "OBJ.classic";
  chan_map[28] = "Hodo";
}

void getScalerCounts(const char *input_root_file_name, const int final_entry){
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
  std::vector<int> scaler_32(32);//periodic scaler
  std::vector<int> scaler_32_overflows(32);//periodic scaler overflow check. adds 2^24 to result
  std::vector<int> init_scaler_32(32);

  std::map<int, std::string> chan_map;
  initialize_map(chan_map);

  int n_entries = in_tree->GetEntries();
  if (final_entry != 0){
    n_entries = final_entry;//forces readout to stop wherever final entry is
  }

  double prev_live_trig = 0;
  double prev_raw_trig  = 0;
  double prev_10hz = 0;//handles initialization packets being out of order

  bool found_init = false;//looking for i
  for (int entry = 0; entry < n_entries; entry++){
    scalers->Clear("");
    in_tree->GetEntry(entry);

    if (scalers->Size() == 250){
      //ignoring incremental scalers for now
      continue;
    }

    if (scalers->Size() == 32){
      for (int i = 31; i >= 0;i--){
        if (scalers->GetScaler(12) != scalers->GetScaler(11)){
          if (scalers->GetScaler(9) != prev_raw_trig || scalers->GetScaler(10) != prev_live_trig){
            if (scalers->GetScaler(i) < scaler_32.at(i)){
              scaler_32_overflows.at(i) += 1;
            }
            scaler_32.at(i) = scalers->GetScaler(i);
          }//if triggers are no longer changing, we stop.
        }//if raw clock == live clock, we're in the startup phase
      }//loop over scaler channels
      if (!found_init && prev_live_trig == 0 && scaler_32.at(10) != 0){
        //initialization events can be in middle of run file. This occurs
        //because the scaler packets at the start of a run are assigned
        //timestamp 0 during initialization, but packets with timestamp 0 are
        //assigned a timestamp when they enter the event builder.  The events
        //in the last initialization packet must be subtracted from the others.
        std::cout << "Found final initialization packets at entry " << entry << "\n";
        found_init = true;
      }
      prev_raw_trig = scaler_32.at(9);
      prev_live_trig = scaler_32.at(10);
      if (!found_init && scalers->GetScaler(14) > prev_10hz){
        for (unsigned int i = 0; i < 32; i++){
          init_scaler_32[i] = scalers->GetScaler(i);
        }
        prev_10hz = scalers->GetScaler(14);
      }
    }//size 32
    if (entry % 100000 == 0){
      std::cout << "Completed " << entry << " entries\r" << std::flush;
    }
  }//loop over tree

  std::cout << "============================Init Scalers======================\n";
  for (std::map<int, std::string>::iterator iter = chan_map.begin(); iter != chan_map.end(); ++iter){
    std::cout << iter->second << ": " << init_scaler_32.at(iter->first) << "\n";
  }
  std::cout << "==========================End Init Schalers===================\n";

  for (unsigned int i = 0; i < scaler_32.size(); i++){
    scaler_32.at(i) += scaler_32_overflows.at(i)*pow(2.,24.);
    scaler_32.at(i) -= init_scaler_32.at(i);
  }

  std::cout << "===========================Final Scalers=======================\n";
  for (std::map<int, std::string>::iterator iter = chan_map.begin(); iter != chan_map.end(); ++iter){
    std::cout << iter->second << ": " << scaler_32.at(iter->first) << "\n";
  }
  std::cout << "==========================End Final Schalers===================\n";
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
