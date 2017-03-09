#include <iostream>

#include "TTree.h"
#include "TFile.h"

#include "TNSCLScalers.h"
#include "TDetector.h"

void getScalerCounts(char *input_root_file_name){
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


//int s800_raw_trigger_index = 0;
//int s800_live_trigger_index = 1;

  //These 2 are actually raw/live trigger 

//  const int NUM_SCALERS = 20;
  TNSCLScalers *scalers= 0; 
  in_tree->SetBranchAddress("TNSCLScalers", &scalers);
  std::vector<int> scaler_250(250);//incremental scalers
  std::vector<int> scaler_32(32);//periodic scaler
  std::vector<int> scaler_32_overflows(32);//periodic scaler overflow check. adds 2^24 to result

  int n_entries = in_tree->GetEntries();

  for (int entry = 0; entry < n_entries; entry++){
    scalers->Clear();
    in_tree->GetEntry(entry);

    
//    s800_raw_triggers += scalers->GetScaler(s800_raw_trigger_index); 
//    s800_live_triggers += scalers->GetScaler(s800_live_trigger_index); 
    if (scalers->Size() == 250){
      for (int i = 0; i < 250;i++){
        scaler_250.at(i) += scalers->GetScaler(i);
      }
    }
    if (scalers->Size() == 32){
      for (int i = 0; i < 32;i++){
        if (scalers->GetScaler(i) < scaler_32.at(i)){
          scaler_32_overflows.at(i) += 1;
        }
        scaler_32.at(i) = scalers->GetScaler(i);
      }
    }
  }//loop over tree

  int live_clock = scaler_32.at(12);
  int raw_clock = scaler_32.at(11);
  int live_trigger = scaler_32.at(10);
  int raw_trigger = scaler_32.at(9);
  int second_source = scaler_32.at(1);

  live_clock     += scaler_32_overflows.at(12)*pow(2.,24.);
  raw_clock      += scaler_32_overflows.at(11)*pow(2.,24.);
  live_trigger   += scaler_32_overflows.at(10)*pow(2.,24.);
  raw_trigger    += scaler_32_overflows.at(9)*pow(2.,24.);
  second_source  += scaler_32_overflows.at(1)*pow(2.,24.);
  std::cout << "raw.clock\tlive.clock\traw.trigger\tlive.trigger" << std::endl;
  std::cout << raw_clock   << "\t" << live_clock   <<"\t"
            << raw_trigger << "\t" << live_trigger 
            << std::endl;
}
#ifndef __CINT__

int main(int argc, char**argv){
   
  if (argc < 2){
    std::cout << "USAGE: getScalerCounts INPUT_FILE_NAME" << std::endl;
  }
  getScalerCounts(argv[1]);
  return 0;
}

#endif
