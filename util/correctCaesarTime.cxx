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

#define MAX_CHN 16
#define MAX_VSN 12  
#define N_RINGS 10 //total number of rings
#define N_DETS 192 //total number of detectors

void readVSNMap(std::string in_file_name, int vsnchn_ring_map_energy[MAX_VSN][MAX_CHN], 
                                          int vsnchn_det_map_energy[MAX_VSN][MAX_CHN], 
                                          int vsnchn_ring_map_time[MAX_VSN][MAX_CHN], 
                                          int vsnchn_det_map_time[MAX_VSN][MAX_CHN]){
  //Note that in the VSN mapping file, VSN is referenced from 1 while the channel
  //is referenced from 0.
  
  int  const det_per_ring[] = {10,14,24,24,24, 24, 24, 24, 14, 10};
  char const ring_names[] = {'a','b','c','d','e','f','g','h', 'i','j'};
  std::cout << "Mapping detectors <-> FERA modules"<< std::endl;
  
  for(int vsn=0; vsn<MAX_VSN; vsn++){
    for(int chn=0; chn<MAX_CHN; chn++){
      vsnchn_ring_map_energy[vsn][chn] = -1;
      vsnchn_det_map_energy [vsn][chn] = -1;
      vsnchn_ring_map_time[vsn][chn] = -1;
      vsnchn_det_map_time [vsn][chn] = -1;
    }
  }

  TEnv *map = new TEnv(in_file_name.c_str()); 
  for(int ring=0; ring < N_RINGS; ring++){
    for(int det=1; det <= det_per_ring[ring]; det++){
      //cout << Form("Fera.Ring.%c.chn_en.%d",ring_names[ring],det) << "\t";
      //cout << map->GetValue(Form("Fera.Ring.%c.chn_en.%d",ring_names[ring],det),-1) << endl;
      
      int vsn_e = map->GetValue(Form("Fera.Ring.%c.vsn_en.%d",ring_names[ring],det),-1);
      int vsn_t = map->GetValue(Form("Fera.Ring.%c.vsn_ti.%d",ring_names[ring],det),-1);
      int chn_e = map->GetValue(Form("Fera.Ring.%c.chn_en.%d",ring_names[ring],det),-1);
      int chn_t = map->GetValue(Form("Fera.Ring.%c.chn_ti.%d",ring_names[ring],det),-1);
      
      
      if((vsn_e != vsn_t) || (chn_e != chn_t))
        std::cout << "  >>>>> WARNING: ring " << ring_names[ring] << " det "  << det << " has not equal mapping for time AND energy" << std::endl;
      
      vsn_e--;//forces vsn_e to be referennced from 0
      if(vsn_e>-1 && chn_e>-1){
        if(vsn_e < MAX_VSN &&  chn_e< MAX_CHN){
          if(vsnchn_ring_map_energy[vsn_e][chn_e] == -1 || vsnchn_det_map_energy [vsn_e][chn_e] == -1){ // was not yet set
            vsnchn_ring_map_energy[vsn_e][chn_e] = ring;
            vsnchn_det_map_energy [vsn_e][chn_e] = det-1;//det runs now from 0
          }
          else{
            std::cout << "  >>>>> Error of mapping detectors to VSN/CHN for energy!!!" << std::endl
                 << "Ring " << ring_names[ring] << " "
                 << "Det " << det << " "
                 << "is assigned to VSN " << vsn_e+1 << " Chn " << chn_e
                 << std::endl
                 << "but this combination is already occupied by "
                 << ring_names[vsnchn_ring_map_energy[vsn_e][chn_e]]
                 << " Det " << vsnchn_det_map_energy [vsn_e][chn_e]
                 << std::endl
                 << "  >>>>> THIS DETECTOR IS OMITTED"
                 << std::endl;
          }
        }// < max vsn/ch
        else{
          std::cout << "  >>>>> Error of mapping detectors to VSN/CHN for energy!!!" << std::endl
               << "Ring " << ring_names[ring] 
               << "Det " << det
               << "is assigned to VSN " << vsn_e+1 << " Chn " << chn_e
               << " exceeds limits VSN/chn of " << MAX_VSN << "/" 
               << MAX_CHN << std::endl;

        }
      }//read a value
      
     vsn_t--;
      if(vsn_t>-1 && chn_t>-1){
        if(vsn_t < MAX_VSN &&  chn_t< MAX_CHN){
          if(vsnchn_ring_map_time[vsn_t][chn_t] == -1 || vsnchn_det_map_time [vsn_t][chn_t] == -1){ // was not yet set
            vsnchn_ring_map_time[vsn_t][chn_t] = ring;
            vsnchn_det_map_time [vsn_t][chn_t] = det-1;//det runs now from 0
          }
          else{
            std::cout << "  >>>>> Error of mapping detectors to VSN/CHN for energy!!!" << std::endl
                 << "Ring " << ring_names[ring] << " "
                 << "Det " << det << " "
                 << "is assigned to VSN " << vsn_t+1 << " Chn " << chn_t
                 << std::endl
                 << "but this combination is already occupied by "
                 << ring_names[vsnchn_ring_map_time[vsn_t][chn_t]]
                 << " Det " << vsnchn_det_map_time [vsn_t][chn_t]
                 << std::endl
                 << "  >>>>> THIS DETECTOR IS OMITTED"
                 << std::endl;
          }
        }// < max vsn/ch
        else{
          std::cout << "  >>>>> Error of mapping detectors to VSN/CHN for energy!!!" << std::endl
               << "Ring " << ring_names[ring] 
               << "Det " << det
               << "is assigned to VSN " << vsn_t+1 << " Chn " << chn_t
               << " exceeds limits VSN/chn of " << MAX_VSN << "/" 
               << MAX_CHN << std::endl;

        }
      }//read a value
      
    }//det
  }//rings
  return;
}

//correctCaesarTime cycles through all the bins of the 
//"time2d" histogram whose x-axis is the absolute detector number (see 
//GetAbsoluteDetectorNumber()) and whose y-axis is the CAESAR raw times. 
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
  //Note: we need the channels + vsn mapping to correctly cycle through addresses
  //and to determine the relationship between absolute detector number and address
  std::string vsn_map_filename      = std::string(getenv("GRUTSYS")) + "/config/caesar/VSNMap.dat";
  
  int vsnchn_ring_map_energy[MAX_VSN][MAX_CHN];
  int vsnchn_det_map_energy[MAX_VSN][MAX_CHN];
  int vsnchn_ring_map_time[MAX_VSN][MAX_CHN];
  int vsnchn_det_map_time[MAX_VSN][MAX_CHN];
  readVSNMap(vsn_map_filename, vsnchn_ring_map_energy, vsnchn_det_map_energy,
                                vsnchn_ring_map_time,   vsnchn_det_map_time);

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
    //Setting axis range allows us to avoid including the overflow peak
    hist_1d->Rebin(4);
    hist_1d->GetXaxis()->SetRangeUser(150,350);
    peakfinder.Search(hist_1d);
    hist_1d->Fit("gaus","M","",peakfinder.GetPositionX()[0] - FIT_PADDING, 
                               peakfinder.GetPositionX()[0] + FIT_PADDING);

    //First parameter in "gaus" is the mean
    centroids[bin-1] = ((TF1*)hist_1d->FindObject("gaus"))->GetParameter(1);
    delete hist_1d;
  }

  //Now cycle through cal file and enter the offsets!
  for (int vsn = 0; vsn < MAX_VSN; vsn++){
    for (int chan = 0; chan < MAX_CHN; chan++){
      time_coeff.clear();
      int ring = vsnchn_ring_map_energy[vsn][chan];
      int det  = vsnchn_det_map_energy[vsn][chan];
      double offset = 0;
      unsigned int address = ((37<<24) + (vsn << 16)+ chan);
      int absolute_detector_number = det+TOTAL_DET_IN_PREV_RINGS[ring];

      TChannel *tchan = TChannel::Get(address);
      time_coeff = tchan->GetTimeCoeff();

      offset =  centroids[comp_det] - centroids[absolute_detector_number];

      if (subtract_current_offset){
        time_coeff[0] = offset+time_coeff.at(0);
      }
      else{
        time_coeff[0] = offset;
      }
      tchan->SetTimeCoeff(time_coeff);
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
  if (argc != 6) {
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
  bool subtract_current_offset = (atoi(argv[5]) != 0);
  std::string cal_file_name(argv[4]);
  
  correctCaesarTime(time2d_hist, comp_det, cal_file_name, subtract_current_offset);
}
#endif

