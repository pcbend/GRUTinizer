#define VERBOSE 0
#define N_KNOWN_SOURCES 6    // Number of Known Sources to be used in Source Class
#define N_RINGS  10          // Number of Caesar Rings
#define MAX_DETS 24          // Maximum number of detectors in a ring
#define MAX_PEAKS 2          // Maximum number of peaks in a source spectrum
#define TOTAL_PEAKS_TO_FIT 8 // Maximum number of peaks to find for each detector
#define FIT_PADDING 15       // Distance to go to left and right of peak for fitting with gaussian
#define FIND_PADDING 100     // for ensuring correct energy is found from rough estimate
#define FIT_ORDER 2

#define HIGH_CH_LIMIT_Y88_PEAK_1 330 //Helps fit y88 by checking whether found peaks are in correct
#define LOW_CH_LIMIT_Y88_PEAK_2  400  //range

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include "TH1D.h"
#include "TFile.h"
#include "TF1.h"
#include "TSpectrum.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCaesar.h"

#include "TChannel.h"

//The Source class allows easy organization of different source runs by source
//name and number without having to always explicitly give the energies, etc. 
//of the sources.
class Source {
  public:
    Source(std::string in_name, int in_run_number, int in_n_peaks, double in_peak_finder_threshold, 
           double in_energies[MAX_PEAKS], double in_peak_finder_sigma, int in_low_ch_limit, int in_high_ch_limit);//Full constructor
    
    //For known sources should have constructor that automatically adds energies
    //and number of peaks.
    Source(std::string name, double run_number, double peak_finder_threshold,
           double peak_finder_sigma);//Constructor for known sources

    ~Source(){
    }

    bool isValid(){
      return is_valid;
    }

    std::string name;
    int n_peaks; //number of expected gamma peaks for source
    double peak_finder_threshold;//Thresholds for using TSpectrum
    double energies[MAX_PEAKS];
    double peak_finder_sigma;//Determined from preliminary gauss fit to data
    int run_number;//number of run from which the source came
    int low_ch_limit;
    int high_ch_limit;

  private:
    std::vector<std::string> known_source_names;
    double known_energies[N_KNOWN_SOURCES][MAX_PEAKS];
    int known_n_peaks[N_KNOWN_SOURCES];
    int known_ranges[N_KNOWN_SOURCES][2];//Expected Low/High Limis for Fits
    bool is_valid;

    void fillKnownSourceNames(){
      this->known_source_names.push_back("y88");
      this->known_source_names.push_back("co60");
      this->known_source_names.push_back("na22");
      this->known_source_names.push_back("cs137");
      this->known_source_names.push_back("ba133");
      this->known_source_names.push_back("bg");
    }
    void fillKnownEnergies(){
      this->known_energies[0][0] = 898.042;
      this->known_energies[0][1] = 1836.063; //88Y

      this->known_energies[1][0] = 1173.228;
      this->known_energies[1][1] = 1332.492; //60Co

      this->known_energies[2][0] = 511.0;
      this->known_energies[2][1] = 1274.537; //22Na

      this->known_energies[3][0] = 661.657;  //137Cs (1 peak)
      this->known_energies[3][1] = 0; 

//      this->known_energies[4][0] = 80.9979; //Might be unable to fit this one
      this->known_energies[4][0] = 356.0129; //133Ba
      this->known_energies[4][1] = 0; //133Ba

      this->known_energies[5][0] = 1460.822; //40K
      this->known_energies[5][1] = 0;//keep failing to find this peak
//      this->known_energies[5][1] = 2614.511; //208Tl
    }
    void fillKnownNPeaks(){
      known_n_peaks[0] = 2;//Y88
      known_n_peaks[1] = 2;//60Co
      known_n_peaks[2] = 2;//22Na
      known_n_peaks[3] = 1;//137Cs
      known_n_peaks[4] = 1;//133Ba
      known_n_peaks[5] = 1;//Bg
    }
    void fillKnownRanges(){ //Low/High X-Axis Values for Ease of Finding Peaks
      known_ranges[0][0]  = 150;
      known_ranges[0][1]  = 2000;

      known_ranges[1][0]  = 150;
      known_ranges[1][1]  = 2000;

      known_ranges[2][0]  = 80;
      known_ranges[2][1]  = 2000;

      known_ranges[3][0]  = 150;
      known_ranges[3][1]  = 2000;

      known_ranges[4][0]  = 150;
      known_ranges[4][1]  = 2000;

      known_ranges[5][0]  = 150;
      known_ranges[5][1]  = 2000;
    }
};

Source::Source(std::string in_name, int in_run_number, int in_n_peaks, double in_peak_finder_threshold, 
           double in_energies[MAX_PEAKS], double in_peak_finder_sigma, int in_low_ch_limit, int in_high_ch_limit){
      this->is_valid = true;
      this->name = in_name;
      this->n_peaks = in_n_peaks;
      this->peak_finder_threshold = in_peak_finder_threshold;
      this->run_number = in_run_number;
      for (int i = 0; i < MAX_PEAKS; i++){
        this->energies[i] = in_energies[i];
      }
      this->peak_finder_sigma = in_peak_finder_sigma;
      this->low_ch_limit = in_low_ch_limit;
      this->high_ch_limit = in_high_ch_limit;
    }//Full constructor
    
    //For known sources should have constructor that automatically adds energies
    //and number of peaks.
    
Source::Source(std::string name, double run_number, double peak_finder_threshold,
           double peak_finder_sigma){
      this->fillKnownSourceNames();
      this->fillKnownEnergies();
      this->fillKnownNPeaks();
      this->fillKnownRanges();
      //Find location of source name in known source list
      std::vector<std::string>::iterator string_iter = std::find(known_source_names.begin(),  
                                                    known_source_names.end(), 
                                                    name);
      if (string_iter != known_source_names.end()){
        std::cout << "Found source name: " << name << std::endl;
        this->is_valid = true;
        int index = std::distance(known_source_names.begin(), string_iter); 
//      Source(name, run_number, known_n_peaks[index], peak_finder_threshold, 
//             known_energies[index], peak_finder_sigma, known_ranges[index][0],
//             known_ranges[index][1]);
        this->name = name;
        this->n_peaks = known_n_peaks[index];
        this->peak_finder_threshold = peak_finder_threshold;
        this->run_number = run_number;
        for (int i = 0; i < MAX_PEAKS; i++){
          this->energies[i] = known_energies[index][i];
        }
        this->peak_finder_sigma = peak_finder_sigma;
        this->low_ch_limit = known_ranges[index][0];
        this->high_ch_limit = known_ranges[index][1];
      }
      else{
        std::cout << "Failed to find source " << name << "!" << std::endl;
        this->is_valid = false;
      }
    }//Constructor for known sources


double getEnergy(double ch, double cal_par[4]){
  //Convert a channel value to an energy value
  double en = cal_par[0];
  en += cal_par[1] * ch;
  en += cal_par[2] * ch*ch;
  en += cal_par[3] * ch*ch*ch;
  return en;
}
void saveCalibration(double data_to_save[N_RINGS][MAX_DETS][4], char *out_cal_file_name){
  char ring_name[N_RINGS][100] = {
    "Caesar.Ring.a",
    "Caesar.Ring.b",
    "Caesar.Ring.c",
    "Caesar.Ring.d",
    "Caesar.Ring.e",
    "Caesar.Ring.f",
    "Caesar.Ring.g",
    "Caesar.Ring.h",
    "Caesar.Ring.i",
    "Caesar.Ring.j"
  };
  int det_per_ring[N_RINGS] = {
    10, 14, 24, 24, 24,
    24, 24, 24, 14, 10
  };

  std::ofstream out_file;
  out_file.open(out_cal_file_name);
  for (int ring = 0; ring < N_RINGS; ring++){
    for(int det = 0; det < det_per_ring[ring]; det++){
      out_file << ring_name[ring] << ".e_a0." << det+1 << ":  " << data_to_save[ring][det][0] << std::endl;
      out_file << ring_name[ring] << ".e_a1." << det+1 << ":  " << data_to_save[ring][det][1] << std::endl;
      out_file << ring_name[ring] << ".e_a2." << det+1 << ":  " << data_to_save[ring][det][2] << std::endl;
      out_file << ring_name[ring] << ".e_a3." << det+1 << ":  " << data_to_save[ring][det][3] << std::endl;
    }
  }
  out_file.close();
}

//saveChannels will create a TChannel file for GRUTinizer Calibration purposes
//If current cal file name is passed, the cal file will be opened and energy coefficient
//data inside will be overwritten with the fitted data (useful when you have already done
//a time calibration!)

void saveChannels(double data_to_save[N_RINGS][MAX_DETS][4], char *out_cal_file_name, std::string cal_file_name =""){
  if (!cal_file_name.empty()){
    TChannel::ReadCalFile(cal_file_name.c_str());
  }
  TCaesar *caesar = new TCaesar(); //gets us access to VSN mapping

  TChannel *tchan;
  std::vector<double> energy_coeff;

  
  for (int vsn = 0; vsn < MAX_VSN; vsn++){
    for (int chan = 0; chan < MAX_CHN; chan++){
      energy_coeff.clear();
      int ring = caesar->vsnchn_ring_map_energy[vsn][chan];
      int det  = caesar->vsnchn_det_map_energy[vsn][chan];
      unsigned int address = ((37<<24) + (vsn << 16)+ chan);

      for (int par = 0; par < 4; par++){
        energy_coeff.push_back(data_to_save[ring][det][par]);
      }

      if (!cal_file_name.empty()){
        tchan = TChannel::Get(address);
        tchan->SetEnergyCoeff(energy_coeff);
      }
      else{
        tchan = new TChannel(Form("RING%02dDET%02d",ring,det), address);
        tchan->SetEnergyCoeff(energy_coeff);
        TChannel::AddChannel(tchan);
      }
    }//loop over channels
  }//loop over vsn

  TChannel::WriteCalFile(out_cal_file_name);
}

void readInitialCalibration(char *in_par_file_name, double cal_par[N_RINGS][MAX_DETS][4]){
  std::ifstream in_par_file;
  in_par_file.open(in_par_file_name);
  if (!in_par_file){
    std::cout << "Failed to open input calibrations!" << std::endl;
    exit(5); 
  }

  std::string line;
  char ring_name;
  int fit_order;
  int det_id;
  int ring_id;
  double cal_par_val;//the actual calibration parameter value
  while (std::getline(in_par_file,line)){ 
      sscanf(line.c_str(), "Caesar.Ring.%c.e_a%d.%d:  %lf", &ring_name, &fit_order, &det_id, &cal_par_val);
      det_id = det_id -1; //want det_id to start from 0
      ring_id = ring_name - 'a';//forces ring_id to start from 0
      cal_par[ring_id][det_id][fit_order] = cal_par_val;
  }
  in_par_file.close();
  return;
}
void readInitialCalibration_GRUTinizer(char *in_cal_file_name, double cal_par[N_RINGS][MAX_DETS][4]){
  
  TChannel::ReadCalFile(in_cal_file_name);
  if(!TChannel::Size()){
    std::cout << "Failed to get channels from in_cal_file_name: " << in_cal_file_name << std::endl;
    return;
  }
  std::vector<double> energy_coeff;
  TChannel *tchan;
  TCaesar *caesar = new TCaesar();
  for (int vsn = 0; vsn < MAX_VSN; vsn++){
    for (int chan = 0; chan < MAX_CHN; chan++){
      energy_coeff.clear();
      int ring = caesar->vsnchn_ring_map_energy[vsn][chan];
      int det  = caesar->vsnchn_det_map_energy[vsn][chan];
      unsigned int address = ((37<<24) + (vsn << 16)+ chan);

      tchan = TChannel::Get(address);
      energy_coeff = tchan->GetEnergyCoeff();

      for (unsigned int par = 0; par < energy_coeff.size(); par++){
        cal_par[ring][det][par] = energy_coeff.at(par);
      }
    }//loop over channels
  }//loop over VSN

  delete caesar;
  return;
}
//Used as follows:
//(Note: All ring/det #'s start from 0!)
//in_hist_file_name is a file containing the energy histograms for the sources organized by the names
//  sourcename_run_#_ring_#_det_# (e.g., y88_run_76__ring_1_det_8) 
//
//in_source_list is a flat text file containing informations about the sources in the form
//    source_name run_# peak_finder_threshold peak_finder_sigma
//    (e.g. y88 76 12.5 5.5)
//    For more information about threshold/sigma parameters, see the TSpectrum webpage

//in_par_file is a file containing an intial guess at the calibration where each line is in the form:
//Caesar.Ring.a.e_a0.1: #, where 1 is the det_id (1,..,24),  a is the ring name, and E = a0+a1*E_raw+a2*E_raw^2
//or as a tchannel file!
//
//in_cal_file_name is the file containing the time calibration that you want to append the energy
//coefficients to
void CalibrateCaesar(char *in_hist_file_name, char *in_source_list, char *out_cal_file_name, 
                     char *out_hist_file_name, char *in_cal_file_name = NULL, char *in_par_file_name =NULL){
  unsigned int det_per_ring[N_RINGS] = {
    10, 14, 24, 24, 24,
    24, 24, 24, 14, 10
  };

  std::vector<Source> source_list;//Sources to be read in from file
  double cal_par[N_RINGS][MAX_DETS][4]; //calibration parameters for each ring & detector
  double peak_locations[10]; //locations of peaks found by TSpectrum
  double peak_energy; //energy of peak location from two-point fit
  bool found_y88_peaks;//if not found, cannot continue calibration
  bool failed_calibration;//if failed calibration for non-Y88 peaks
  int peaks_found; //# peaks found by tspectrum; not all will be used
  int peaks_fit;//# of found peaks that have been fit
  std::vector<double> fit_means;
  std::vector<double> expected_energy;
  std::vector<unsigned int> y88_indices;//allows easy finding of y88 for 2-Point Calibration
  const int REBINNING_FACTOR = 4;


  //Initialize array to 0
  for (int ring = 0; ring < N_RINGS; ring++){
    for(int det = 0; det < MAX_DETS; det++){
      cal_par[ring][det][0] = 0;
      cal_par[ring][det][1] = 1;
      cal_par[ring][det][2] = 0;
      cal_par[ring][det][3] = 0;
    }
  }

  //Read in source list
  std::string line;//for parsing file
  int lines_read = 0;
  std::ifstream source_list_file;
  source_list_file.open(in_source_list);

  if (!source_list_file.is_open()){
    std::cout << "Failed opening file!" << std::endl;
    return;
  }

  //Each line of source list file should be in form 
  
  while(std::getline(source_list_file,line)){
    std::string source_name;
    int run_number;
    double peak_finder_threshold;
    double peak_finder_sigma;

    std::stringstream ss(line);
    ss >> source_name; 
    ss >> run_number; 
    ss >> peak_finder_threshold;
    ss >> peak_finder_sigma;
    source_list.push_back(Source(source_name, run_number, peak_finder_threshold, 
                                 peak_finder_sigma));
    if (source_name == "y88"){
      y88_indices.push_back(lines_read); 
    }

    if (!source_list[lines_read].isValid()){
      std::cout << source_name << " " << run_number << " " << peak_finder_threshold 
                << " " << peak_finder_sigma << std::endl;
      std::cout << "Failed reading source list for this line! Exiting... " << std::endl;
      source_list_file.close();
      return;
    }
    lines_read++;
  }
  source_list_file.close();


  //Open file containing histograms in form sourcename_ring_#_det_#
  TFile *in_hist_file = new TFile(in_hist_file_name, "read"); 

  //Create an output file will store all histograms and graphs for inspection of fits
  TFile *out_hist_file = new TFile(out_hist_file_name, "recreate"); 
  
  TSpectrum y88_peak_finder(2); //Used just to find peaks for initial 2-point fit
  TSpectrum peak_finder; //Finds all peaks in a spectrum

  TH1D *in_hist;//Histogram grabbed from file
  std::vector<TH1D*> out_hists;//Histogram for sanity check of fits
  std::vector<TGraph*> out_graphs;

  //First, do a 2-point fit on 88-yttrium to get a base calibration, unless
  //initial calibration is given.
  //Then, using this very rough approximation, we can distinguish between
  //the peaks TSpectrum finds to determine the correct peak locations for 
  //fitting.

  TF1 *gaus_fit_1 = new TF1("gaus_fit_1", "gaus"); //need two gaussian functions defined
                                                   //because TH1::Fit overwrites the function
                                                   //with the given name, and need to retain
                                                   //the parameters

  if (in_par_file_name != NULL){
    //readInitialCalibration(in_par_file_name, cal_par);
    readInitialCalibration_GRUTinizer(in_par_file_name, cal_par);
  }

  for (unsigned int ring = 0; ring < N_RINGS; ring++){
    for (unsigned int det = 0; det < det_per_ring[ring]; det++){
      failed_calibration = false;
      found_y88_peaks = false;
      if (in_par_file_name == NULL){
        //Find all y88 runs for the first two point fit
        for (unsigned int i = 0; i < y88_indices.size(); i++){
          in_hist = (TH1D*)in_hist_file->Get(Form("%s_run_%d_ring_%d_det_%d", source_list[y88_indices[i]].name.c_str(),
                source_list[y88_indices[i]].run_number, ring, det));
          in_hist->Rebin(REBINNING_FACTOR);
          if (in_hist == NULL){
            std::cout <<  "Histogram: " <<  (source_list[y88_indices[i]].name).c_str() << "_run_" <<  
              source_list[y88_indices[i]].run_number << "_ring_" <<  ring <<  
              "_det_" << det << " not found!" << std::endl;
            failed_calibration = true;
            break;
          }

          if (in_hist->Integral() == 0){
            std::cout << "Histogram is empty for ring " << ring << " and det " << det << std::endl;
            failed_calibration = true;
            break;
          }

          in_hist->GetXaxis()->SetRangeUser(source_list[y88_indices[i]].low_ch_limit, source_list[y88_indices[i]].high_ch_limit);
          //      std::cout << "low_ch_limit: " << source_list[y88_indices[i]].low_ch_limit << std::endl;
          //      std::cout << "high_ch_limit: " << source_list[y88_indices[i]].high_ch_limit << std::endl;
          //      std::cout << "peak_finder_sigma: " << source_list[y88_indices[i]].peak_finder_sigma << std::endl;
          //      std::cout << "peak_finder_threshold: " << source_list[y88_indices[i]].peak_finder_threshold << std::endl;
          y88_peak_finder.Search(in_hist, source_list[y88_indices[i]].peak_finder_sigma,
              "nodraw", source_list[y88_indices[i]].peak_finder_threshold);

          if (y88_peak_finder.GetPositionX()[0] > source_list[y88_indices[i]].low_ch_limit &&
              y88_peak_finder.GetPositionX()[0] < HIGH_CH_LIMIT_Y88_PEAK_1 &&
              y88_peak_finder.GetPositionX()[1] > LOW_CH_LIMIT_Y88_PEAK_2 &&
              y88_peak_finder.GetPositionX()[1] < source_list[y88_indices[i]].high_ch_limit){

            found_y88_peaks = true; 

            in_hist->Fit(gaus_fit_1,"Q+","", y88_peak_finder.GetPositionX()[0] - FIT_PADDING, 
                y88_peak_finder.GetPositionX()[0] + FIT_PADDING);
            fit_means.push_back(in_hist->GetFunction("gaus_fit_1")->GetParameter(1));
            expected_energy.push_back(source_list[y88_indices[i]].energies[0]);

            in_hist->Fit("gaus","Q+","", y88_peak_finder.GetPositionX()[1] - FIT_PADDING, 
                y88_peak_finder.GetPositionX()[1] + FIT_PADDING);
            fit_means.push_back(in_hist->GetFunction("gaus")->GetParameter(1));
            expected_energy.push_back(source_list[y88_indices[i]].energies[1]);

            out_hists.push_back((TH1D*)in_hist->Clone(Form("%s_run_%d_ring_%d_det_%d_fitted", source_list[y88_indices[i]].name.c_str(),
                    source_list[y88_indices[i]].run_number, ring, det)));
            out_hists.back()->SetDirectory(out_hist_file);
          }//Found correct peaks for y88
          else{
                std::cout << "\tlow_ch_limit: " << source_list[y88_indices[i]].low_ch_limit << std::endl;
                std::cout << "\thigh_ch_limit: " << source_list[y88_indices[i]].high_ch_limit << std::endl;
                std::cout << "HIGH_CH_LIMIT_Y88_PEAK_1: " << HIGH_CH_LIMIT_Y88_PEAK_1 << std::endl;
                std::cout << "LOW_CH_LIMIT_Y88_PEAK_2: " << LOW_CH_LIMIT_Y88_PEAK_2 << std::endl;
                std::cout << "Found positions: " << y88_peak_finder.GetPositionX()[0] << " and " << y88_peak_finder.GetPositionX()[1] << std::endl;

          }
          
        }//Loop over Y88 Indices

        if (!found_y88_peaks){
          std::cout << "Failed calibration for Y88 for ring " << ring << " and det " << det << std::endl;
          break;
        }

        TGraph y88_graph(fit_means.size(), &fit_means[0], &expected_energy[0]);
        TF1 *test_poly1_fit = new TF1(Form("test_poly_fit_ring_%d_det_%d", ring, det), Form("pol%d",1));
        test_poly1_fit->SetParameter(0,30);
        test_poly1_fit->SetParameter(1,4);
        test_poly1_fit->SetParLimits(0,-60,60);
        test_poly1_fit->SetParLimits(1,3,4);
        y88_graph.Fit(test_poly1_fit, "QME");
        cal_par[ring][det][0] = y88_graph.GetFunction(Form("test_poly_fit_ring_%d_det_%d",ring,det))->GetParameter(0);
        cal_par[ring][det][1] = y88_graph.GetFunction(Form("test_poly_fit_ring_%d_det_%d",ring,det))->GetParameter(1);
      }//if there is no in_par_file
      else {
        found_y88_peaks = true;
      }//get input parameters from file

      //Now that we have initial fit, we can fit the other sources
      for (unsigned int source = 0; source < source_list.size(); source++){ //Loop over non-Y88 sources!
        if (in_par_file_name == NULL){
          if (std::find(y88_indices.begin(), y88_indices.end(), source) != y88_indices.end()){
            continue;//skip y88, if it's already been calibrated
          }
        }

        in_hist = (TH1D*)in_hist_file->Get(Form("%s_run_%d_ring_%d_det_%d", source_list[source].name.c_str(),
                                                                            source_list[source].run_number, ring, det));
        if (in_hist == NULL){
          std::cout << "Histogram: " << source_list[source].name << "_run_" << source_list[source].run_number <<
                       "_ring_" << ring << "_det_" << det << " not found!" << std::endl;
          failed_calibration = true;
          break;
        }
        in_hist->Rebin(REBINNING_FACTOR);

        if (in_hist->Integral() == 0){
          std::cout << "Histogram: " << source_list[source].name << "_run_" << source_list[source].run_number <<
                       "_ring_" << ring << "_det_" << det << " is empty!" << std::endl;
          failed_calibration = true;
          break;
        }  
        in_hist->GetXaxis()->SetRangeUser(source_list[source].low_ch_limit, source_list[source].high_ch_limit);
        peak_finder.Search(in_hist, source_list[source].peak_finder_sigma,
                           "nodraw", source_list[source].peak_finder_threshold);
        peaks_found = peak_finder.GetNPeaks();
        for (int peak = 0; peak < peaks_found; peak++){
          peak_locations[peak] = peak_finder.GetPositionX()[peak];
        }
        peaks_fit = 0;

        for (int i = 0; i < peaks_found; i++){
          //Use simple fit to get approximate energy to compare to expected energies!
          peak_energy = getEnergy(peak_locations[i],cal_par[ring][det]);

          if (peak_energy < source_list[source].energies[0] + FIND_PADDING &&
              peak_energy > source_list[source].energies[0] - FIND_PADDING){
            in_hist->Fit(gaus_fit_1,"Q+","",peak_locations[i] - FIT_PADDING, peak_locations[i] + FIT_PADDING);
            fit_means.push_back(in_hist->GetFunction("gaus_fit_1")->GetParameter(1));
            expected_energy.push_back(source_list[source].energies[0]);
            peaks_fit++;
          }//is peak 1
          else if (peak_energy < source_list[source].energies[1] + FIND_PADDING &&
                     peak_energy > source_list[source].energies[1] - FIND_PADDING){
            in_hist->Fit("gaus","Q+","",peak_locations[i] - FIT_PADDING, peak_locations[i] + FIT_PADDING);
            fit_means.push_back(in_hist->GetFunction("gaus")->GetParameter(1));
            expected_energy.push_back(source_list[source].energies[1]);
            peaks_fit++;
          }//is peak 2
          if (i == peaks_found-1 && peaks_fit != source_list[source].n_peaks){//failed to fit all peaks
            std::cout << "Failed calibration for source " << source_list[source].name << " with " << peaks_fit << " peaks fit" << std::endl;
            std::cout << "out of " << peaks_found << " peaks found " << "for ring: " << ring << " det: " << det << " run number: " 
                      << source_list[source].run_number << std::endl;
            std::cout << "Found peak_energy = " << peak_energy << std::endl;
          }//peaks fit not enough
        }//loop over peaks_found
        if (peaks_fit != source_list[source].n_peaks){
            failed_calibration = true; //forces to skip to next detector
            break;
        }
        out_hists.push_back((TH1D*)in_hist->Clone(Form("%s_run_%d_ring_%d_det_%d_fitted", source_list[source].name.c_str(),
                                                  source_list[source].run_number, ring, det)));
        (out_hists.back())->SetDirectory(out_hist_file);
      }//loop over non-y88 sources

      if (!found_y88_peaks || failed_calibration){
        continue; //Have to do this detector by hand; either
                  //failed y88 or other source calibration
      }//if failed calibration

      //Now do the full fit for this detector
      TGraph fit_graph(fit_means.size(), &fit_means[0], &expected_energy[0]);
      TF1 *poly_fit = new TF1(Form("poly_fit_ring_%d_det_%d", ring, det), Form("pol%d",FIT_ORDER));
      for (int par = 0; par < FIT_ORDER+1; par++){
        poly_fit->SetParameter(par, cal_par[ring][det][par]);
      }
      poly_fit->SetParameter(1,4);
      poly_fit->SetParameter(2,-0.0003);
//    poly_fit->SetParameter(1,4);
//    poly_fit->SetParLimits(0,-60,60);
//      poly_fit->SetParLimits(1,3,5);
      fit_graph.Fit(poly_fit, "Q");
      //fit_graph.Fit(Form("pol%d", FIT_ORDER), "Q");
      for (int par = 0; par < FIT_ORDER+1; par++){
//        std::cout << "Before Fit: a" << par << " = " << cal_par[ring][det][par] << std::endl;
        cal_par[ring][det][par] = fit_graph.GetFunction(Form("poly_fit_ring_%d_det_%d",ring,det))->GetParameter(par);
//       std::cout << "After Fit : a" << par << " = " << cal_par[ring][det][par] << std::endl;
      }
      if (FIT_ORDER == 1){
        cal_par[ring][det][2] = 0;
        cal_par[ring][det][3] = 0;
      }
      out_graphs.push_back((TGraph*)fit_graph.Clone(Form("ring_%d_det_%d_polyfit", ring,det)));
      (out_graphs.back())->Write();
      delete poly_fit;
      fit_means.clear();
      expected_energy.clear();
    }//loop over det
  }//loop over rings

//  saveCalibration(cal_par, out_cal_file_name);
  saveChannels(cal_par, out_cal_file_name, in_cal_file_name);
  
  out_hist_file->Write();
  out_hist_file->Close();
  in_hist_file->Close();
 
  
}//end of function


#ifndef __CINT__

int main(int argc, char**argv){
  if (argc < 5 || argc > 7){
    std::cout << "Incorrect number of arguements!" << std::endl;
    std::cout << "USAGE: CalibrateCaesar in_hists.root in_source_lists.dat out_cal_file_name.dat out_fitted_hists.root [tcal file] [starting_pars.dat]" << std::endl;
    return -1;
  }
  if (argc == 7){
    CalibrateCaesar(argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);
  }
  if (argc == 6){
    CalibrateCaesar(argv[1],argv[2],argv[3],argv[4],argv[5]);
  }
  if (argc == 5){
    CalibrateCaesar(argv[1],argv[2],argv[3],argv[4]);
  }
  return 0;
}

#endif
