#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <glob.h>

#include "TF1.h"
#include "FitDoubleExpo_Gaus.C"
#include "TEnv.h"
#include "TH1.h"
#include "TH1F.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TSpline.h"
#include "TCutG.h"
#include "TStyle.h"
#include "TChannel.h"
#include "GValue.h"
#include "TLegend.h"
#include "TPaveText.h"

#include "TCaesarHit.h"
#include "TS800Hit.h"
#include "TCaesar.h"
#include "TS800.h"

#include "GH1D.h"
//This program takes as input a config file containing:
//    -  directory containing necessary input root tree files (INPUT_DIR)
//    -  number of parameters to use in the fit (NUM_PARS)
//    -  set of initial parameters for the fit (PAR.#) where # starts with 0
//    -  file containing cuts (CUT_FILE_NAME)
//    -  Ranges for histogramming (DATA_LOW_X, DATA_HIGH_X)
//    -  Ranges for fitting       (FIT_LOW_X,  FIT_HIGH_X)
//    -  Binning (KEV_PER_BIN)

//All files must contain a tree called "EventTree", i.e. they must be made by GRUTinizer.
//Usage: fitCoulex [CONFIG FILE]



void getFileList(std::string &search_string, std::vector<std::string> &file_list){
  glob_t glob_result; 

  if (glob(search_string.c_str(),GLOB_ERR,NULL,&glob_result) == GLOB_NOMATCH){
    std::cout << "GLOB failed to find pattern matching: " << search_string << std::endl;
    return;
  }

  std::cout << "Found " << glob_result.gl_pathc << " files matching " << search_string << ":"<<  std::endl;
  for (unsigned int i = 0; i < glob_result.gl_pathc; ++i){
    file_list.push_back( glob_result.gl_pathv[i] ); 
    std::cout << "\t" << glob_result.gl_pathv[i] << std::endl;
  }
 
  return;
}

void getHistograms(std::vector<TH1F*> &data_hists, std::vector<double> &angles,
                   TFile *&input_file, const double START_ANGLE, 
                   const double FINAL_ANGLE,  const double ANGLE_STEPS,
                   int data_low_x, int data_high_x, int kev_per_bin){
  const int TOTAL_ANGLES = (FINAL_ANGLE-START_ANGLE)/ANGLE_STEPS + 1;
  double temp_angle = START_ANGLE;
  for (int i = 0; i < TOTAL_ANGLES; i++){
    std::stringstream ss;
    ss << "fit_angle_" << temp_angle;
    std::string data_hist_name = ss.str();
    input_file->cd("Fits");
//    input_file->Print();
    std::cout << "Getting histogram: " << data_hist_name << std::endl;
    std::cout << (TH1F*)gDirectory->Get(data_hist_name.c_str()) << std::endl;
    data_hists.push_back((TH1F*)gDirectory->Get(data_hist_name.c_str()));
    if (data_hists.at(i)->GetNbinsX() != data_hists.at(i)->GetXaxis()->GetXmax()){
      std::cout << "Number of bins ("<<data_hists.at(i)->GetNbinsX()<<") does not equal x-axis length ("
                << data_hists.at(i)->GetXaxis()->GetXmax()<<") so should not rebin!" << std::endl;
      exit(1);
    }
    data_hists.at(i)->GetXaxis()->SetRangeUser(data_low_x,data_high_x);
    data_hists.at(i)->Rebin(kev_per_bin);
    angles.push_back(temp_angle);
    temp_angle += ANGLE_STEPS;
  }
}

void getGeantHistograms(std::vector<TH1F*> &geant_hists, std::vector<std::string> &geant_file_names){
  for (unsigned int i = 0; i < geant_file_names.size(); i++){
    TFile *geant_file = new TFile(geant_file_names.at(i).c_str(),"read");
    if (!geant_file){
      std::cout << "Failed loading geant file: " << geant_file_names.at(i) << std::endl;
      return;
    }
    std::string geant_hist_name("all_dop");
    std::string geant_fep_hist_name("all_dop_fep");

    TH1F *geant_fep_hist = (TH1F*)geant_file->Get(geant_fep_hist_name.c_str());
//    TH1F *geant_compton = (TH1F*)geant_hists.at(i*2)->Clone("geant_compton");
    TH1F *geant_compton = (TH1F*)(((TH1F*)geant_file->Get(geant_hist_name.c_str()))->Clone("geant_compton"));
    if (i == 0){
      geant_hists.push_back( (TH1F*)geant_file->Get(geant_fep_hist_name.c_str()));
    }
//  else if (i == 1){
//    geant_hists.push_back( (TH1F*)geant_file->Get(geant_hist_name.c_str()));
//  }
    else{
      geant_hists.push_back( (TH1F*)geant_file->Get(geant_hist_name.c_str()));
    }
    geant_compton->Add(geant_fep_hist,-1);
    geant_hists.push_back(geant_compton);
  }
  return;
}

void getFiles(std::vector<TFile*> &files, std::vector<std::string> &file_list){
  for (unsigned int i = 0; i < file_list.size(); i++){
    files.push_back(new TFile(file_list.at(i).c_str(),"read"));
  }
  if (files.size() != file_list.size()){
    std::cout << "Unexpected number of trees: "   << files.size() 
              << " compared to number of files: " << file_list.size() << std::endl;
  }
  return;
}
int fitCoulex(const char *cfg_file_name){
   
  //////////////////////////////////////////////////
  //All Variables
  const int MAX_PARS = 15;
  //These angles determine the maximum angle cut that will be used
//const double START_ANGLE = 2.4;
//const double FINAL_ANGLE = 2.4;
  const double START_ANGLE = 180;
  const double FINAL_ANGLE = 180;
  const double ANGLE_STEPS = 0.1;
  const int TOTAL_ANGLES = (FINAL_ANGLE-START_ANGLE)/ANGLE_STEPS + 1;

  double init_pars[MAX_PARS];
  for (int i = 0; i < MAX_PARS; i++){
    init_pars[i] = 0;
  }

  TEnv *cfg = new TEnv(cfg_file_name);
  std::vector<std::string> file_list;
  std::ofstream out_file;

  std::vector<TFile*> files;
  std::vector<TH1F*> data_hists;
  //the structure of the geant vector will be:
  //geant_hists.at(0) = fep geant hist for main peak
  //geant_hists.at(1) = compton only (full - fep)
  //geant_hists.at(2) = full geant hist for 2nd peak
  //geant_hists.at(3) = compton for that 2nd peak
  std::vector<TH1F*> geant_hists;
  std::vector<double> angles;
  data_hists.reserve(TOTAL_ANGLES);//Want one histogram for every angle cut
  angles.reserve(TOTAL_ANGLES);

  //Fitting Arrays
  double hist_constant[TOTAL_ANGLES][6];
  double fit_error[TOTAL_ANGLES];
  double chi_squared[TOTAL_ANGLES];

  //Config File Settings
  std::string isotope_name;
  std::string input_file_name;
  std::string out_file_name;
  std::vector<std::string> geant_file_names;

  int data_low_x;
  int data_high_x;
  int peak_low_x;
  int peak_high_x;
  int fit_low_x;
  int fit_high_x;
  
  int kev_per_bin;
  int num_pars;
  int num_geant_files;

  //////////////////////////////////////////////////
  //Get configuration values from file
  isotope_name     = cfg->GetValue("ISOTOPE_NAME", "");
  if (isotope_name.empty()){
    std::cout << "ERROR: Failed to get ISOTOPE_NAME from cfg file: " << cfg_file_name << std::endl;
    return -1;
  }
  input_file_name     = cfg->GetValue("INPUT_FILE", "");
  if (input_file_name.empty()){
    std::cout << "ERROR: Failed to get INPUT_FILE from cfg file: " << cfg_file_name << std::endl;
    return -1;
  }

  out_file_name     = cfg->GetValue("OUT_FILE_NAME", "");
  if (out_file_name.empty()){
    std::cout << "ERROR: Failed to get OUT_FILE_NAME from cfg file: " << cfg_file_name << std::endl;
    return -1;
  }

  num_geant_files = cfg->GetValue("NUM_GEANT_FILES", 0);
  if (num_geant_files > 5 || num_geant_files < 1){
    std::cout << "ERROR: Failed to get correct number of geant files from cfg file: " << cfg_file_name << std::endl;
    return -1;
  }

  for (int i = 0; i < num_geant_files; i++){
    std::string geant_file_name = cfg->GetValue(Form("GEANT_FILE_NAME_%d",i), "");
    if (geant_file_name.empty()){
      std::cout << "ERROR: Failed to get GEANT_FILE_NAME_"<<i<<" from cfg file: " << cfg_file_name << std::endl;
      return -1;
    }
    geant_file_names.push_back(geant_file_name);
  }
  
  data_low_x = cfg->GetValue("DATA_LOW_X", -1);
  if (data_low_x == -1){
    std::cout << "ERROR: Bad data_low_x Value : " << data_low_x << std::endl;
    return -1;
  }
  data_high_x = cfg->GetValue("DATA_HIGH_X", 0);
  if (data_high_x == 0){
    std::cout << "ERROR: Bad data_high_x Value : " << data_high_x << std::endl;
    return -1;
  }
  peak_low_x = cfg->GetValue("PEAK_LOW_X", -1);
  if (peak_low_x == -1){
    std::cout << "ERROR: Bad PEAK_LOW_X Value : " << peak_low_x << std::endl;
    return -1;
  }
  peak_high_x = cfg->GetValue("PEAK_HIGH_X", 0);
  if (peak_high_x == 0){
    std::cout << "ERROR: Bad PEAK_HIGH_X Value : " << peak_high_x << std::endl;
    return -1;
  }
  fit_low_x = cfg->GetValue("FIT_LOW_X", -1);
  if (fit_low_x == -1){
    std::cout << "ERROR: Bad fit_low_x Value : " << fit_low_x << std::endl;
    return -1;
  }
  fit_high_x = cfg->GetValue("FIT_HIGH_X", 0);
  if (fit_high_x == 0){
    std::cout << "ERROR: Bad fit_high_x Value : " << fit_high_x << std::endl;
    return -1;
  }
  kev_per_bin = cfg->GetValue("KEV_PER_BIN", 0);
  if (kev_per_bin == 0){
    std::cout << "ERROR: Bad kev_per_bin Value : " << kev_per_bin << std::endl;
    return -1;
  }

  num_pars = cfg->GetValue("NUM_PARS",0);
  if (num_pars > 11 || num_pars < 8){
    std::cout << "ERROR: Bad NUM_PARS Value : " << num_pars << std::endl;
    return -1;
  }
  
  std::cout << "Using " << num_pars << " parameters: " << std::endl;
  for (int i = 0; i < num_pars; i++){
    init_pars[i] = cfg->GetValue(Form("PAR.%d", i), 0.0);
    std::cout << "Parameter " << i << " = " << init_pars[i] << std::endl;
    if (TMath::Abs(init_pars[i]) < 1e-08){
      std::cout << "ERROR: Failed to get parameter = " << i 
                << " from cfg file with num_pars =  " << num_pars << std::endl;
      return -1;
    }
  }

  //Finished parsing cfg file
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  //Open file containing histograms 
  std::cout << "Opening input file: " << input_file_name.c_str() << std::endl;
  TFile *input_file = new TFile(input_file_name.c_str(), "read");
  if (!input_file){
    std::cout << "ERROR: Failed to open input file." << std::endl;
    return -1;
  }
  
  //Open Output file which will contain the histogram results
  std::cout << "Opening output file: " << out_file_name.c_str() << std::endl;
  out_file.open(out_file_name.c_str());
  if (!out_file){
    std::cout << "ERROR: Failed to open out_file!" << std::endl;
    return -1;
  }
  if (num_geant_files == 1){
    out_file << "Angle\tFEP Scaling\tCompton Scaling\tFit Error\tChi Squared\tResidual Sum\tResSumInPeak\tPeakSum" << std::endl;
  }
  else if (num_geant_files == 2){
    out_file << "Angle\t Fep Scaling\t Compton Scaling\t 2nd-2Plus Scaling\t FitError\t ChiSquared\t ResSum\t ResSumInPeak\t PeakSum" << std::endl;
  }
  else if (num_geant_files == 3){
    out_file << "Angle\t Fep Scaling\t Compton Scaling\t655 Scaling\t1362 Scaling\tFitError\t ChiSquared\t ResSum\t ResSumInPeak\t PeakSum" << std::endl;
  }
  else if (num_geant_files == 4){
    out_file << "Angle\t Fep Scaling\t Compton Scaling\t655/1365 Scaling\t1362 Rel. Scaling\t1542 Scaling\tFitError\t ChiSquared\t ResSum\t ResSumInPeak\t PeakSum" << std::endl;
  }
  else if (num_geant_files == 5){
    out_file << "Angle\tFep Scaling\tCompton Scaling\t802 Scaling\t1440 Scaling"
             << "\t1577 Scaling\t1644 Scaling\t FitError\t ChiSquared\t ResSum"
             << "\t ResSumInPeak\t PeakSum" << std::endl;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //GET GEANT HISTOGRAMS

  std::cout << "Getting Geant Histograms"  << std::endl;
  getGeantHistograms(geant_hists,geant_file_names);
  if (geant_hists.size() != (unsigned int)num_geant_files*2){
    std::cout << "ERROR: Incorrect number of histograms ("<<geant_hists.size()
              <<") added to vector! Expected " << num_geant_files*2 <<  std::endl;
    return -1 ;
  }
  //Need to rebin these to match the binning from my filled histograms
  for (int geant_file = 0; geant_file < num_geant_files*2; geant_file+=2){
    geant_hists.at(geant_file)->GetXaxis()->SetRangeUser(data_low_x,data_high_x);
    geant_hists.at(geant_file)->Rebin(kev_per_bin);
    geant_hists.at(geant_file+1)->GetXaxis()->SetRangeUser(data_low_x,data_high_x);
    geant_hists.at(geant_file+1)->Rebin(kev_per_bin);
  }
  //DONE GETTING GEANT HISTOGRAMS
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  std::cout << "Getting data histograms.." << std::endl;
  getHistograms(data_hists, angles, input_file, START_ANGLE, FINAL_ANGLE,ANGLE_STEPS, 
                data_low_x, data_high_x, kev_per_bin);
  std::cout << "Done getting histograms." << std::endl;
  
  ////////////////////////////////////////////////////
  //Now Starting Fitting Routine
  int num_res_points = (fit_high_x - fit_low_x)/kev_per_bin;
  std::vector<double> residuals;
  std::vector<double> normed_residuals;
  std::vector<double> bin_centers;
  std::vector<double> peak_sum;
  residuals.resize(num_res_points);
  normed_residuals.resize(num_res_points);
  bin_centers.resize(num_res_points);
  peak_sum.resize(TOTAL_ANGLES);

  TF1 *used_fit_function = new TF1("double_expo_fit", 
                                   "[0]*([1]*TMath::Exp([2]*x)+[3]*TMath::Exp([4]*x))",
                                   data_low_x,data_high_x);

  //Setting parameters for double exponential!
  for (int i = 1; i <= 4; i++){
    used_fit_function->SetParameter(i, init_pars[num_geant_files+i+1]);
  }
//if (num_geant_files ==1){
//  used_fit_function->SetParameter(1,init_pars[3]);
//  used_fit_function->SetParameter(2,init_pars[4]);
//  used_fit_function->SetParameter(3,init_pars[5]);
//  used_fit_function->SetParameter(4,init_pars[6]);
//}
//if (num_geant_files ==2){
//  used_fit_function->SetParameter(1,init_pars[4]);
//  used_fit_function->SetParameter(2,init_pars[5]);
//  used_fit_function->SetParameter(3,init_pars[6]);
//  used_fit_function->SetParameter(4,init_pars[7]);
//}
  TFile *out_hist_file = new TFile("output_fit_hists.root","recreate");
  for (int angle_index = 0; angle_index < TOTAL_ANGLES; angle_index++){
    peak_sum[angle_index] = 0;
    TF1 *fit_func = 0;
    if (num_geant_files == 1){
      fit_func = FitDoubleExpTwoHist(data_hists.at(angle_index), geant_hists.at(0), 
          geant_hists.at(1), fit_low_x, fit_high_x, init_pars);
    }
    else if (num_geant_files == 2){
      fit_func = FitDoubleExpThreeHist(data_hists.at(angle_index), geant_hists.at(0), 
          geant_hists.at(1), geant_hists.at(2), fit_low_x, fit_high_x, init_pars);
    }
    else if (num_geant_files == 3){
      fit_func = FitDoubleExpFourHist(data_hists.at(angle_index), geant_hists.at(0), 
          geant_hists.at(1), geant_hists.at(2), geant_hists.at(4), fit_low_x, 
          fit_high_x, init_pars);
    }
    else if (num_geant_files == 4){
      fit_func = FitDoubleExpFiveHist(data_hists.at(angle_index), geant_hists.at(0), 
          geant_hists.at(1), geant_hists.at(2), geant_hists.at(4), geant_hists.at(6), 
          fit_low_x, fit_high_x, init_pars);
    }
    else if (num_geant_files == 5){
      fit_func = FitDoubleExpSixHist(data_hists.at(angle_index), geant_hists.at(0), 
          geant_hists.at(1), geant_hists.at(2), geant_hists.at(4),  geant_hists.at(6), 
          geant_hists.at(8), fit_low_x, fit_high_x, init_pars);
    }
    else {
      std::cout << "Unknown reason for num_geant_files = " << num_geant_files << std::endl;
    }

    //GH1D *fit_func_hist = new GH1D(*fit_func, 512,0,8192);
    GH1D *fit_func_hist = new GH1D(*fit_func, 8192./kev_per_bin,0,8192);
    //fit_func_hist->Rebin(kev_per_bin);
    TSpline3 *fit_func_spline = new TSpline3(fit_func_hist);
//  std::cout << "Parameters from Fit Function are: " << std::endl;
//  fit_func->Print();
    data_hists.at(angle_index)->SetTitle(Form("Fit to %s with Scatter Cut (< %3.3f)", isotope_name.c_str(),angles.at(angle_index)));
    data_hists.at(angle_index)->GetXaxis()->SetTitle("Energy (keV)");
    data_hists.at(angle_index)->GetYaxis()->SetTitle(Form("Counts / %d keV", kev_per_bin));

    fit_error[angle_index]     = fit_func->GetParError(0); 
    chi_squared[angle_index]   = fit_func->GetChisquare();

    hist_constant[angle_index][0] = fit_func->GetParameter(0);
    hist_constant[angle_index][1] = fit_func->GetParameter(1);
    hist_constant[angle_index][2] = 0;
    hist_constant[angle_index][3] = 0;
    hist_constant[angle_index][4] = 0;
    hist_constant[angle_index][5] = 0;
    if (num_geant_files == 2){
      hist_constant[angle_index][2] = fit_func->GetParameter(2);
    }
    if (num_geant_files == 3){
      hist_constant[angle_index][2] = fit_func->GetParameter(2);
      hist_constant[angle_index][3] = fit_func->GetParameter(3);
    }
    if (num_geant_files == 4){
      hist_constant[angle_index][2] = fit_func->GetParameter(2);
      hist_constant[angle_index][3] = fit_func->GetParameter(3);
      hist_constant[angle_index][4] = fit_func->GetParameter(4);
    }
    else if (num_geant_files == 5){
      hist_constant[angle_index][2] = fit_func->GetParameter(2);
      hist_constant[angle_index][3] = fit_func->GetParameter(3);
      hist_constant[angle_index][4] = fit_func->GetParameter(4);
      hist_constant[angle_index][5] = fit_func->GetParameter(5);
    }
    //Now we get residuals
    double res_sum = 0;
    double res_sum_in_peak = 0;

    int start_bin = data_hists[angle_index]->FindBin(fit_low_x);
    int end_bin = data_hists[angle_index]->FindBin(fit_high_x);
    used_fit_function->SetParameter(0, fit_func->GetParameter(num_geant_files+1));//set double exponential scaling factor
    
    for (int bin = start_bin; bin < end_bin; bin++){
      bin_centers[bin-start_bin] = data_hists.at(angle_index)->GetBinCenter(bin);
      residuals[bin-start_bin]   = data_hists.at(angle_index)->GetBinContent(bin) - 
                                   fit_func->Eval(bin_centers[bin-start_bin]);
      res_sum += residuals[bin-start_bin];
      normed_residuals[bin-start_bin] = residuals[bin-start_bin]/TMath::Sqrt(data_hists[angle_index]->GetBinContent(bin));

      if (bin_centers[bin-start_bin] >= peak_low_x  && bin_centers[bin-start_bin] <= peak_high_x){
        TH1D geant_compton_hist1;
        geant_compton_hist1 = *((TH1D*)geant_hists.at(1)->Clone("geant_compton_hist1"));
        geant_compton_hist1.Scale(hist_constant[angle_index][1]*hist_constant[angle_index][0]);

        peak_sum[angle_index] += (fit_func->Eval(bin_centers[bin-start_bin])
                                  - used_fit_function->Eval(bin_centers[bin-start_bin]) 
                                  - geant_compton_hist1.GetBinContent(bin));
        if (num_geant_files == 2){
          TH1D geant_compton_hist2;
          geant_compton_hist2 = *((TH1D*)geant_hists.at(3)->Clone("geant_compton_hist2"));
          geant_compton_hist2.Scale(hist_constant[angle_index][2]);
          peak_sum[angle_index] -= geant_compton_hist2.GetBinContent(bin);
        }//2 geant files
        else if (num_geant_files == 3){
          TH1D geant_compton_hist2;
          geant_compton_hist2 = *((TH1D*)geant_hists.at(3)->Clone("geant_compton_hist2"));
          geant_compton_hist2.Scale(hist_constant[angle_index][2]);
          peak_sum[angle_index] -= geant_compton_hist2.GetBinContent(bin);

          TH1D geant_compton_hist3;
          geant_compton_hist3 = *((TH1D*)geant_hists.at(5)->Clone("geant_compton_hist3"));
          geant_compton_hist3.Scale(hist_constant[angle_index][3]);
          peak_sum[angle_index] -= geant_compton_hist3.GetBinContent(bin);
        }//2 geant files
        else if (num_geant_files == 4){
          TH1D geant_compton_hist2;
          geant_compton_hist2 = *((TH1D*)geant_hists.at(3)->Clone("geant_compton_hist2"));
          geant_compton_hist2.Scale(hist_constant[angle_index][2]*0.68);
          peak_sum[angle_index] -= geant_compton_hist2.GetBinContent(bin);

          TH1D geant_compton_hist3;
          geant_compton_hist3 = *((TH1D*)geant_hists.at(5)->Clone("geant_compton_hist3"));
          geant_compton_hist3.Scale(hist_constant[angle_index][2]);
          peak_sum[angle_index] -= geant_compton_hist3.GetBinContent(bin);

          TH1D geant_compton_hist4;
          geant_compton_hist4 = *((TH1D*)geant_hists.at(7)->Clone("geant_compton_hist4"));
          geant_compton_hist4.Scale(hist_constant[angle_index][4]);
          peak_sum[angle_index] -= geant_compton_hist4.GetBinContent(bin);
        }//2 geant files
        else if (num_geant_files == 5){
          TH1D geant_compton_hist2;
          geant_compton_hist2 = *((TH1D*)geant_hists.at(3)->Clone("geant_compton_hist2"));
          geant_compton_hist2.Scale(hist_constant[angle_index][2]);
          peak_sum[angle_index] -= geant_compton_hist2.GetBinContent(bin);

          TH1D geant_compton_hist3;
          geant_compton_hist3 = *((TH1D*)geant_hists.at(5)->Clone("geant_compton_hist3"));
          geant_compton_hist3.Scale(hist_constant[angle_index][3]);
          peak_sum[angle_index] -= geant_compton_hist3.GetBinContent(bin);

          TH1D geant_compton_hist4;
          geant_compton_hist4 = *((TH1D*)geant_hists.at(7)->Clone("geant_compton_hist4"));
          geant_compton_hist4.Scale(hist_constant[angle_index][4]);
          peak_sum[angle_index] -= geant_compton_hist4.GetBinContent(bin);

          TH1D geant_compton_hist5;
          geant_compton_hist5 = *((TH1D*)geant_hists.at(9)->Clone("geant_compton_hist5"));
          geant_compton_hist5.Scale(hist_constant[angle_index][5]);
          peak_sum[angle_index] -= geant_compton_hist5.GetBinContent(bin);
        }//5 geant files
        res_sum_in_peak += residuals[bin-start_bin];
      }
    }//bin loop


    TGraph *residual_plot = new TGraph(num_res_points, &bin_centers[0], &residuals[0]);
    residual_plot->SetName(Form("res_plot_angle_%f", angles.at(angle_index)));
    TGraph *norm_residual_plot = new TGraph(num_res_points, &bin_centers[0], &normed_residuals[0]);
    norm_residual_plot->SetName(Form("norm_res_plot_angle_%f", angles.at(angle_index)));

    out_file <<  angles[angle_index] <<"\t"<<hist_constant[angle_index][0] << "\t" << hist_constant[angle_index][1] <<  "\t";
    if (num_geant_files == 2){
      out_file << hist_constant[angle_index][2] << "\t" ;
    }
    else if (num_geant_files == 3){
      out_file << hist_constant[angle_index][2] << "\t" ;
      out_file << hist_constant[angle_index][3] << "\t" ;
    }
    else if (num_geant_files == 4){
      out_file << hist_constant[angle_index][2] << "\t" ;
      out_file << hist_constant[angle_index][3] << "\t" ;
      out_file << hist_constant[angle_index][4] << "\t" ;
    }
    else if (num_geant_files == 5){
      out_file << hist_constant[angle_index][2] << "\t" ;
      out_file << hist_constant[angle_index][3] << "\t" ;
      out_file << hist_constant[angle_index][4] << "\t" ;
      out_file << hist_constant[angle_index][5] << "\t" ;
    }
    out_file << fit_error[angle_index] << "\t" 
             << chi_squared[angle_index] << "\t" << res_sum << "\t" << res_sum_in_peak 
             << "\t" << peak_sum[angle_index] << "\t" << std::endl;

        

    out_hist_file->cd();
    residual_plot->Write();
    norm_residual_plot->Write();
    data_hists[angle_index]->Write();

    TCanvas *disentangled_can = new TCanvas(Form("dis_can_%1.1f",angles[angle_index]),Form("dis_can_%f",angles[angle_index]), 800,600);
    disentangled_can->cd();
    data_hists[angle_index]->Draw("HIST");
    data_hists[angle_index]->SetLineWidth(2);
    TLine *fit_low = new TLine(fit_low_x, 0, fit_low_x, 780);
    TLine *fit_high = new TLine(fit_high_x, 0, fit_high_x, 780);
    TLine *peak_low = new TLine(peak_low_x, 0, peak_low_x, 780);
    TLine *peak_high = new TLine(peak_high_x, 0, peak_high_x, 780);
    fit_low->SetLineStyle(2);
    fit_high->SetLineStyle(2);
    peak_low->SetLineStyle(3);
    peak_high->SetLineStyle(3);
    fit_low->Draw();
    fit_high->Draw();
    peak_low->Draw();
    peak_high->Draw();

    geant_hists.at(0)->SetLineColor(kGreen+2);
    geant_hists.at(1)->SetLineColor(kRed);
    if (num_geant_files ==2 ){
      geant_hists.at(2)->SetLineColor(kMagenta);
    }
    used_fit_function->SetLineColor(kBlack);
    fit_func_spline->SetLineColor(kCyan+2);

    geant_hists.at(0)->SetLineWidth(3);
    geant_hists.at(1)->SetLineWidth(3);
    if (num_geant_files ==2 ){
      geant_hists.at(2)->SetLineWidth(3);
    }
    used_fit_function->SetLineWidth(3);
    fit_func_spline->SetLineWidth(3);

    TH1D fep_hist = *((TH1D*)geant_hists.at(0)->Clone("fep_hist"));
    fep_hist.Scale(hist_constant[angle_index][0]);
    fep_hist.Draw("same");

    TH1D compton_hist = *((TH1D*)geant_hists.at(1)->Clone("compton_hist"));
    compton_hist.Scale(hist_constant[angle_index][1]*hist_constant[angle_index][0]);
    compton_hist.Draw("same");

    TH1D second_2plus_hist;

    TH1D hist_1362;
    TH1D hist_655;
    TH1D hist_802;
    TH1D hist_1440;
    TH1D hist_1542;
    TH1D hist_1577;
    TH1D hist_1644;

    if (num_geant_files ==2 ){
      geant_hists.at(2)->SetLineWidth(3);
      second_2plus_hist = *((TH1D*)geant_hists.at(2)->Clone("second_2plus_hist"));
      second_2plus_hist.Scale(hist_constant[angle_index][2]);
      second_2plus_hist.Draw("same");
    }
    else if (num_geant_files ==3){
      geant_hists.at(2)->SetLineWidth(3);
      hist_655  = *((TH1D*)geant_hists.at(2)->Clone("hist_655"));
      hist_655.Scale(hist_constant[angle_index][2]);
      hist_655.Draw("same");
      geant_hists.at(4)->SetLineWidth(3);
      hist_1362  = *((TH1D*)geant_hists.at(4)->Clone("hist_1362"));
      hist_1362.Scale(hist_constant[angle_index][3]);
      hist_1362.Draw("same");
    }
    else if (num_geant_files ==4){
      geant_hists.at(2)->SetLineWidth(3);
      hist_655  = *((TH1D*)geant_hists.at(2)->Clone("hist_655"));
      hist_655.Scale(hist_constant[angle_index][2]*0.68);
      hist_655.Draw("same");
      geant_hists.at(4)->SetLineWidth(3);
      hist_1362  = *((TH1D*)geant_hists.at(4)->Clone("hist_1362"));
      hist_1362.Scale(hist_constant[angle_index][2]);
      hist_1362.Draw("same");
      geant_hists.at(6)->SetLineWidth(3);
      hist_1542  = *((TH1D*)geant_hists.at(6)->Clone("hist_1542"));
      hist_1542.Scale(hist_constant[angle_index][4]);
      hist_1542.Draw("same");
    }
    else if (num_geant_files == 5){
      geant_hists.at(2)->SetLineWidth(3);
      hist_802  = *((TH1D*)geant_hists.at(2)->Clone("hist_802"));
      hist_802.Scale(hist_constant[angle_index][2]);
      hist_802.Draw("same");
      geant_hists.at(4)->SetLineWidth(3);
      hist_1440  = *((TH1D*)geant_hists.at(4)->Clone("hist_1440"));
      hist_1440.Scale(hist_constant[angle_index][3]);
      hist_1440.Draw("same");
      geant_hists.at(6)->SetLineWidth(3);
      hist_1577  = *((TH1D*)geant_hists.at(6)->Clone("hist_1577"));
      hist_1577.Scale(hist_constant[angle_index][4]);
      hist_1577.Draw("same");
      geant_hists.at(8)->SetLineWidth(3);
      hist_1644  = *((TH1D*)geant_hists.at(8)->Clone("hist_1644"));
      hist_1644.Scale(hist_constant[angle_index][5]);
      hist_1644.Draw("same");
    }
    if (!used_fit_function){
      std::cout << "Fit function doesn't exist." << std::endl;
    }
    else{
      used_fit_function->Draw("same");
    }
//    fit_func->Draw("same");
    fit_func_spline->SetNpx(1000);
    fit_func_spline->Draw("same");

    out_hist_file->cd();
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);
    disentangled_can->Write();


    TCanvas *residual_with_hists_can = new TCanvas(Form("res_can_%1.1f",angles[angle_index]),Form("res_can_%1.1f",angles[angle_index]), 800,600);
    residual_with_hists_can->Divide(1,2);
    residual_with_hists_can->cd(1);
    data_hists[angle_index]->SetLineWidth(3);
    fit_func_hist->SetLineWidth(3);
    data_hists[angle_index]->SetLineColor(kBlue);
    fit_func_hist->SetLineColor(kRed);
    data_hists[angle_index]->Draw("HIST");
    TLegend leg(0.5,0.5,0.8,0.7);
    leg.AddEntry(fit_func_hist, "Fit Function", "l");
    leg.AddEntry(data_hists[angle_index], "Data", "l");
    leg.Draw();
    fit_func_hist->Draw("same");
    
    
    residual_with_hists_can->cd(2);
    TH1D *res_sub_hist = (TH1D*)data_hists[angle_index]->Clone("res_sub_hist");
    res_sub_hist->Add(fit_func_hist,-1);
    res_sub_hist->Draw("HIST");
    res_sub_hist->SetTitle("Residuals"); 
    TPaveText *pt = new TPaveText(0.65,0.15,0.85,0.30, "NDC");
    TLine *peak_low_ressub = new TLine(peak_low_x, -50, peak_low_x, 50);
    TLine *peak_high_ressub = new TLine(peak_high_x, -50, peak_high_x, 50);
    peak_low_ressub->SetLineStyle(3);
    peak_high_ressub->SetLineStyle(3);
    peak_low_ressub->SetLineWidth(3);
    peak_high_ressub->SetLineWidth(3);
    peak_low_ressub->Draw();
    peak_high_ressub->Draw();

    int peak_low_bin = res_sub_hist->FindBin(peak_low_x);
    int peak_high_bin = res_sub_hist->FindBin(peak_high_x);
    pt->AddText(Form("Residual Sum in Peak: %1.1f",res_sub_hist->Integral(peak_low_bin,peak_high_bin-1)));
    pt->AddText(Form("Total Peak Sum: %1.1f",peak_sum.at(angle_index)));
    pt->Draw();

    residual_with_hists_can->Write();

    delete residual_plot;
    delete norm_residual_plot;
    delete fit_func;
    delete fit_func_spline;
    delete fit_func_hist;
    delete disentangled_can;
    delete residual_with_hists_can;
    delete pt;
    if (angle_index % 10 == 0){
      std::cout << " Completed Angle " << angles[angle_index] << std::endl; 
    }
  }
  out_file.close();
  //END OF OLD FILE
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  return 0;
}//End of Function

#ifndef __CINT__
int main(int argc, char **argv){
  if (argc != 2){
    std::cout << "Error! Incorrect number of arguments: " << argc << std::endl;
    std::cout << "Usage: fitCoulex CFG_FILE";
    return 1;
  }

  fitCoulex(argv[1]);
}
#endif
