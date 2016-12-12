#define N_RINGS 10
#define MAX_DETS 24

//CHANGE THIS
#define NUM_FILES 4 
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "TTree.h"
#include "TCaesar.h"
#include "TCaesarHit.h"
#include "GValue.h"
#include "TChannel.h"
#include "TFile.h"
#include "TH1D.h"
#include "TCutG.h"

//Usage: makeCalibrationHists input_file_list.dat output_hists.root
//input_file_list should contain the list of files that will be used for the calibration

//NUM_FILES, run_numbers, and source_names must be changed by hand! 
int main(int argc, char *argv[]){
  if (argc < 3){
    std::cout << "Usaged: MakeCalibrationHists input_filelist.dat output_file.root" << std::endl;
    return -1;
  }

  //CHANGE THIS
  std::string source_name[NUM_FILES] = {
    "na22",
    "y88",
    "co60",
    "cs137"
  };

  //CHANGE THIS
  int run_numbers[NUM_FILES] = {
    363,362,366,365 
  };
  int det_per_ring[N_RINGS] = {
    10, 14, 24, 24, 24,
    24, 24, 24, 14, 10
  };

  std::vector<std::string> input_file_names;
  std::vector<TFile*> files;
  std::string line;
  std::ifstream input_file;

  TH1D* output_hists[NUM_FILES][N_RINGS][MAX_DETS];
  TFile *out_file = new TFile(argv[2], "recreate");

  TCaesar *caesar = 0;
  
  Int_t det;
  Int_t ring;
  Int_t n_gamma;
  Int_t n_entries;
  Double_t charge;
  Double_t time;
  
  Int_t hist_high_x = 2048;
  Int_t hist_low_x = 0;
  Int_t hist_n_bins_x = hist_high_x;


  //Open cut file for TCut
  TFile *cut_file = new TFile("/mnt/analysis/pecan-gade/elman/source_runs_endofexperiment/tcut_file_endofexp.root","read");
  if (!cut_file){
    std::cout << "Failed to open cut file" << std::endl;
    return 1;
  }
  TCutG *tcut = (TCutG*)cut_file->Get("sample_tcut");
  if (!tcut){
    std::cout << "Failed to get tcut file" << std::endl;
    return 1;
  }

  //Open list of files to parse
  input_file.open(argv[1]);
  if (!input_file.is_open()){
    std::cout << "Failed opening file!" << std::endl;
    return 1;
  }

  while (std::getline(input_file, line)){
    input_file_names.push_back(line);
  }
  input_file.close();

  for (unsigned int i = 0; i < input_file_names.size(); i++){
    files.push_back(new TFile(input_file_names.at(i).c_str(),"read")); 
  }
  if (files.size() != NUM_FILES){
    std::cout << "ERROR: Found incorrect number of files ("<<files.size()<<")!" << std::endl;
  }
  
  //Make all histograms with format:
  //sourcename_ring_#_det_#
  for (int file = 0; file < NUM_FILES; file++){
    for (int ring = 0; ring < N_RINGS; ring++){
      for (int det = 0; det < det_per_ring[ring]; det++){
        output_hists[file][ring][det] = new TH1D(Form("%s_run_%d_ring_%d_det_%d",source_name[file].c_str(), run_numbers[file],ring,det),
                                                 Form("%s_run_%d_ring_%d_det_%d",source_name[file].c_str(), run_numbers[file],ring,det), hist_n_bins_x, hist_low_x, hist_high_x);
        output_hists[file][ring][det]->SetDirectory(out_file);
      } 
    }
  }


  Int_t cur_source = 0;
  for (unsigned int file = 0; file < NUM_FILES; file++){
    std::cout << "Currently parsing tree from: " << input_file_names.at(file) << std::endl;
    TTree *cur_tree = (TTree*)files.at(file)->Get("EventTree");
    cur_tree->SetBranchAddress("TCaesar", &caesar);

//  GValue *gvalues = (GValue*)files.at(file)->Get("GValue");
    TChannel *tchannels = (TChannel*)files.at(file)->Get("TChannel");
    if (!tchannels){
      std::cout << "Missing GValue or TChannel for file = " << file << std::endl;
    }

    n_entries = cur_tree->GetEntries();
    for (int entry = 0; entry < n_entries; entry++){
      caesar->Clear();
      cur_tree->GetEvent(entry);
      if (!caesar){
        continue;
      }
      n_gamma = caesar->Size();
      for (int gamma = 0; gamma < n_gamma; gamma++){
        TCaesarHit hit = caesar->GetCaesarHit(gamma);
        if (!hit.IsValid() || hit.IsOverflow()){
          continue;
        }
        charge = hit.Charge();//raw data, no calibrations
        time = hit.GetTime();
        det = hit.GetDetectorNumber();
        ring = hit.GetRingNumber();
        if (tcut->IsInside(time, charge)){
          output_hists[cur_source][ring][det]->Fill(charge);
        }
      }//cycle through gamma multiplicity
    }//cycle through tree entries
    std::cout << "Finished with file: " << input_file_names.at(file) << std::endl;
    cur_source += 1;
    delete cur_tree;
  }//cycle through files in chain
 
  out_file->Write();
  out_file->Close();
}

