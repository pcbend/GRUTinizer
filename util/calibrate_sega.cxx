#include <iostream>

#include "TAxis.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TSpectrum.h"

#include "TChannel.h"

double sigma = 15;
double threshold = 0.5;
const char* input_file = NULL;
const char* input_channels = NULL;
const char* output_channels = NULL;

struct fit_result {
  bool success;
  double slope, offset;
  double fwhm_1172;
  double fwhm_1332;
};

std::ostream& operator<<(std::ostream& os, const fit_result& res){
  if(res.success){
    os << "Energy = " << res.slope << "*chan + " << res.offset << ", "
       << "FWHM@1172 = " << res.fwhm_1172 << " keV, "
       << "FWHM@1332 = " << res.fwhm_1332 << " keV";
  } else {
    os << "Fit failed";
  }
  return os;
}

unsigned int core_address(unsigned int detnum){
  return ( (1<<24) + //System
           (1<<16) + //Crate
           (2<<8)  + //Card
           (detnum-1) );  //Channel
}

fit_result FitCo60(TH1* hist, unsigned int address = -1) {
  fit_result output;
  output.success = true;

  // Skip first/last 40 bins, in case of noise at 0, especially in segments
  hist->GetXaxis()->SetRange(40,hist->GetXaxis()->GetNbins() - 40);

  // Initial search for the peaks, using TSpectrum
  TSpectrum spec;
  int nfound = spec.Search(hist, sigma, "goff nodraw", threshold);

  if(nfound != 2){
    std::cout << "Numpeaks was " << nfound << " for spectrum " << hist->GetName()
              << std::endl;
    output.success = false;
    return output;
  }

  // Extract parameters from TSpectrum
  std::vector<double> xpoints_spec;
  std::vector<double> ypoints_spec;
  for(int i=0; i<nfound; i++){
    xpoints_spec.push_back(spec.GetPositionX()[i]);
    ypoints_spec.push_back(spec.GetPositionY()[i]);
  }

  if(xpoints_spec[0] > xpoints_spec[1]){
    std::swap(xpoints_spec[0], xpoints_spec[1]);
    std::swap(ypoints_spec[0], ypoints_spec[1]);
  }

  // Pick initial values for a fit
  double spread = xpoints_spec[1] - xpoints_spec[0];
  double xmin = xpoints_spec[0]-spread/4;
  double xmax = xpoints_spec[1]+spread/4;
  TF1* co60_fit = new TF1("co60","gaus(0) + gaus(3)", xmin, xmax);
  co60_fit->SetParameters(
    ypoints_spec[0],
    xpoints_spec[0],
    sigma,
    ypoints_spec[1],
    xpoints_spec[1],
    sigma);

  // Use correct weights on each bin
  hist->Sumw2(true);
  // Fit, q for quiet, R for range, goff to avoid drawing
  hist->Fit(co60_fit, "q", "Rgoff");
  std::vector<double> xpoints_fit;
  std::vector<double> fwhm_fit;
  xpoints_fit.push_back(co60_fit->GetParameter(1)); //Mean of first gaus
  fwhm_fit.push_back(2.355*co60_fit->GetParameter(2));
  xpoints_fit.push_back(co60_fit->GetParameter(4)); //Mean of second gaus
  fwhm_fit.push_back(2.355*co60_fit->GetParameter(5));

  for(int i=0; i<nfound; i++){
    if(std::abs(xpoints_spec[i] - xpoints_fit[i]) > spread/4) {
      std::cout << "Fit failed for hist \"" << hist->GetName() << "\"" << std::endl;
      std::cout << "\tTSpectrum peak loc: " << xpoints_spec[i] << std::endl;
      std::cout << "\tGaus fit peak loc:  " << xpoints_fit[i] << std::endl;
      output.success = false;
    }
  }

  // Use a linear fit to determine the slope/offset
  TGraph* linfit_graph = new TGraph(2);
  linfit_graph->SetNameTitle(Form("%s_graph",hist->GetName()),
                      Form("%s_graph",hist->GetName()));
  linfit_graph->SetPoint(0, xpoints_fit[0], 1173.1);
  linfit_graph->SetPoint(1, xpoints_fit[1], 1332.5);
  TF1* linfit = new TF1("linfit", "pol1(0)");
  linfit_graph->Fit(linfit,"q","goff");

  output.slope = linfit->GetParameter(1);
  output.offset = linfit->GetParameter(0);
  output.fwhm_1172 = output.slope*fwhm_fit[0];
  output.fwhm_1332 = output.slope*fwhm_fit[1];

  if(std::abs(output.offset) > 10){
    std::cout << "Large offset parameter (" << output.offset << " keV)"
              << " for hist \"" << hist->GetName() << "\"" << std::endl;
  }

  if(output.success && address!=-1U) {
    TChannel* chan = TChannel::Get(address);
    std::vector<double> coefficients{output.offset, output.slope};
    chan->SetEnergyCoeff(coefficients);
  }

  return output;
}

void FitCores(TH2* summary) {
  for(int detnum=1; detnum<17; detnum++){
    TH1* hist = summary->ProjectionY(Form("charge_det%02d",detnum),
                                     detnum, detnum);
    fit_result res = FitCo60(hist, input_channels ? core_address(detnum) : -1);
    std::cout << "Det " << detnum
              << ": " << res << std::endl;
  }
}

void FitSegments(TH2* summary, int detnum) {
  for(int segnum=1; segnum<33; segnum++){
    TH1* hist = summary->ProjectionY(Form("charge_det%02d_seg%02d",detnum,segnum),
                                     segnum, segnum);
    fit_result res = FitCo60(hist);
    std::cout << "Det " << detnum << ", Seg " << segnum
              << ": " << res << std::endl;
  }
}



int main(int argc, char** argv) {

  if(argc < 2){
    std::cout << "Give me a histogram file" << std::endl;
    return -1;
  }

  input_file = argv[1];
  if(argc >= 3){
    input_channels = argv[2];
    TChannel::ReadCalFile(input_channels);
  }
  if(argc >= 4){
    output_channels = argv[3];
  }



  TFile* fin = new TFile(input_file);

  std::cout << "---------- Fitting Cores ------------" << std::endl;
  TH2* detsummary = (TH2*)fin->Get("sega_charge_summary");
  FitCores(detsummary);

  for(int detnum=1; detnum<17; detnum++){
    std::cout << "---------- Fitting Det " << detnum
              << " Segments ------------" << std::endl;
    std::string hist_name = Form("sega_det%02d_segsummary",detnum);
    TH2* segsummary = (TH2*)fin->Get(hist_name.c_str());
    if(segsummary){
      FitSegments(segsummary, detnum);
    } else {
      std::cout << "Missing segment summary \"" << hist_name << "\"" << std::endl;
    }
  }

  if(output_channels){
    TChannel::WriteCalFile(output_channels);
  }
}
