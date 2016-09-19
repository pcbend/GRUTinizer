#include <iostream>
#include <fstream>
#include <cstdlib> //for getenv and atoi
#include <vector>
#include <string>

#include "TChannel.h"
#include "TEnv.h"
#include "TH2D.h"
#include "TFile.h"
#include "TF1.h"
#include "TSpectrum.h"
#include "TCaesar.h"

#define MAX_CHN 16
#define MAX_VSN 12  
#define N_RINGS 10 //total number of rings
#define N_DETS 192 //total number of detectors

//correctCaesarTime cycles through all the bins of the 
//"time2d" histogram whose x-axis is the absolute detector number (see 
//TCaesarHit::GetAbsoluteDetectorNumber()) and whose y-axis is the CAESAR raw times. 
//It finds the centroid of the peak of the 1d projection and stores it in
//a file called "centroids.dat". It then computes the difference between 
//all the centroids and the centroid of a detector that one wants to compare 
//to, and gives the offsets in an output cal file. The offsets can either be 
//relative to 0 or relative to the current offset in the input cal file by
//setting add_cur_offset to false or true, respectively. 
//
//i.e. the reported offset for detector i will either be:
// centroid_compdet-centroid_i or (centroid_compdet-centroid_i)+TIMECOEFF_OFFSET
//
//
// INPUT:
//
//
// OUTPUT:
//
//

void correctCaesarTime(TH2D* time2d, int comp_det, std::string cal_file_name, 
                       bool subtract_current_offset=true){

  TCaesar *caesar = new TCaesar();//this automatically fills the VSN mapping!

  TSpectrum peakfinder(1);
  const int TOTAL_DET_IN_PREV_RINGS[10] = {0,10,24,48,72,96,120,144,168,182};
  const int FIT_PADDING = 30; //determines distance to left/right of peak for fitting
  double centroids[N_DETS];
  std::string out_chan_filename("correctTimeOffsets.cal");

  std::vector<double> time_coeff;//for storing offsets before saving to file
  if (!cal_file_name.empty()){
    TChannel::ReadCalFile(cal_file_name.c_str());
  }

  //First cycle through histogram and determine centroids
  for (int  bin= 1; bin <= N_DETS; bin++){
    TH1D *hist_1d = (TH1D*)time2d->ProjectionY("name",bin,bin);
    if (!hist_1d){
      std::cout << "ERROR: Failed to get projection for bin " << bin << std::endl;
      return;
    }
    //Setting axis range allows us to avoid including the overflow peak
    hist_1d->GetXaxis()->SetRangeUser(50,800);
    peakfinder.Search(hist_1d);
    hist_1d->Fit("gaus","M","",peakfinder.GetPositionX()[0] - FIT_PADDING, 
                               peakfinder.GetPositionX()[0] + FIT_PADDING);

    //Parameter 1 in "gaus" is the mean
    centroids[bin-1] = ((TF1*)hist_1d->FindObject("gaus"))->GetParameter(1);
    delete hist_1d;
  }

  //Now cycle through cal file and enter the offsets!
  for (int vsn = 0; vsn < MAX_VSN; vsn++){
    for (int chan = 0; chan < MAX_CHN; chan++){
      time_coeff.clear();
      int ring = caesar->vsnchn_ring_map_energy[vsn][chan];
      int det  = caesar->vsnchn_det_map_energy[vsn][chan];
      double offset = 0;
      unsigned int address = ((37<<24) + (vsn << 16)+ chan);
      int absolute_detector_number = det+TOTAL_DET_IN_PREV_RINGS[ring];

      TChannel *tchan;
      //std::cout << "TChan : " << tchan <<std::endl;
      offset =  centroids[comp_det] - centroids[absolute_detector_number];

      if (subtract_current_offset){
        tchan = TChannel::Get(address);
        time_coeff = tchan->GetTimeCoeff();
        time_coeff[0] = offset+time_coeff.at(0);
        tchan->SetTimeCoeff(time_coeff);
      }//update current channel
      else{
        tchan = new TChannel(Form("RING%02dDET%02d",ring,det), address);
        time_coeff.push_back(offset);
        time_coeff.push_back(1);//we don't want to change slope!
        tchan->SetTimeCoeff(time_coeff);
        TChannel::AddChannel(tchan);
      }//no channel exists so add one to TChannel!
    }//loop over channels
  }//loop over VSN

  TChannel::WriteCalFile(out_chan_filename);
  return;
}

#ifndef __CINT__
int main(int argc, char **argv){
  std::string usage("USAGE: correctCaesarTime [hist_file] [hist_name] ");
              usage += "[Detector to Compare To] [Cal file name] ";
              usage += "[Subtract Curr Offset?]";
  if (argc < 4 || argc > 6) {
    std::cout << "incorrect number of arguments: " << argc << std::endl;
    std::cout << usage << std::endl;
    return -1;
  }

 
  TFile *input_file = new TFile(argv[1], "read");
  if (!input_file){
    std::cout << "Failed to open input file with name " << argv[1] << std::endl;
    return -1;
  }
  TH2D *time2d_hist = (TH2D*)input_file->Get(argv[2]); 
  if (!time2d_hist){
    std::cout << "Failed to open input file with name " << argv[1] << std::endl;
    return -1;
  }

  int comp_det = atoi(argv[3]);
 
  if (argc == 6){
    std::string cal_file_name(argv[4]);
    bool subtract_current_offset = (atoi(argv[5]) != 0);
    correctCaesarTime(time2d_hist, comp_det, cal_file_name, subtract_current_offset);
  }
  if (argc == 5){
    std::string cal_file_name(argv[4]);
    correctCaesarTime(time2d_hist, comp_det, cal_file_name, false);
  }
  if (argc == 4){
    correctCaesarTime(time2d_hist, comp_det, "", false);
  }
}
#endif

