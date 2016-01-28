#include <algorithm>
#include <iostream>
#include <deque>

#include "TAxis.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TSpectrum.h"

#include "TChannel.h"

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

unsigned int segment_address(unsigned int detnum, unsigned int segnum){
  return -1;
}

struct fit_point {
  fit_point(double x, double y, double xfit = 0)
    : x_spec(x), y_spec(y), x_fit(xfit) { }
  double x_spec;
  double y_spec;
  double x_fit;
};

fit_result FitCo60(TH1* hist, unsigned int address = -1, double sigma=15, double threshold = 0.5) {
  fit_result output;
  output.success = true;

  // Skip first/last 500 bins, in case of noise at 0 in segments
  hist->GetXaxis()->SetRangeUser(500,hist->GetXaxis()->GetNbins() - 500);

  // Initial search for the peaks, using TSpectrum
  TSpectrum spec;
  int nfound = spec.Search(hist, sigma, "", threshold);

  if(nfound < 2){
    std::cout << "Numpeaks was " << nfound << " for spectrum " << hist->GetName()
              << std::endl;
    hist->Write();
    output.success = false;
    return output;
  }

  // Extract parameters from TSpectrum
  std::deque<fit_point> points;
  for(int i=0; i<nfound; i++){
    points.emplace_back(spec.GetPositionX()[i], spec.GetPositionY()[i]);
  }

  // Sort in increasing energy
  std::sort(points.begin(), points.end(),
            [](const fit_point& a, const fit_point& b) { return a.x_spec < b.x_spec; }
  );

  // Throw out all but the highest 2 energies
  while(points.size() > 2){
    points.pop_front();
  }

  // Pick initial values for a fit
  double spread = points[1].x_spec - points[0].x_spec;
  double xmin = points[0].x_spec - spread/4;
  double xmax = points[1].x_spec + spread/4;
  TF1* co60_fit = new TF1("co60","gaus(0) + gaus(3)", xmin, xmax);
  co60_fit->SetParameters(
    points[0].y_spec,
    points[0].x_spec,
    sigma,
    points[1].y_spec,
    points[1].x_spec,
    sigma);

  // Use correct weights on each bin
  hist->Sumw2(true);
  hist->GetXaxis()->SetRangeUser(xmin, xmax);
  // Fit, q for quiet, R for range, goff to avoid drawing
  hist->Fit(co60_fit, "q", "Rgoff");
  std::vector<double> xpoints_fit;
  std::vector<double> fwhm_fit;
  points[0].x_fit = (co60_fit->GetParameter(1)); //Mean of first gaus
  fwhm_fit.push_back(2.355*co60_fit->GetParameter(2));
  points[1].x_fit = (co60_fit->GetParameter(4)); //Mean of second gaus
  fwhm_fit.push_back(2.355*co60_fit->GetParameter(5));

  for(auto& point : points){
    if(std::abs(point.x_spec - point.x_fit) > spread) {
      std::cout << "Fit failed for hist \"" << hist->GetName() << "\"" << std::endl;
      std::cout << "\tTSpectrum peak loc: " << point.x_spec << std::endl;
      std::cout << "\tGaus fit peak loc:  " << point.x_fit << std::endl;
      output.success = false;
    }
  }

  // Use a linear fit to determine the slope/offset
  TGraph* linfit_graph = new TGraph(2);
  linfit_graph->SetNameTitle(Form("%s_graph",hist->GetName()),
                      Form("%s_graph",hist->GetName()));
  linfit_graph->SetPoint(0, points[0].x_fit, 1173.1);
  linfit_graph->SetPoint(1, points[1].x_fit, 1332.5);
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

  hist->Write();
  return output;
}

void FitCores(TH2* summary) {
  for(int detnum=1; detnum<17; detnum++){
    TH1* hist = summary->ProjectionY(Form("charge_det%02d",detnum),
                                     detnum, detnum);
    fit_result res = FitCo60(hist, input_channels ? core_address(detnum) : -1,
                             15, 0.5);
    std::cout << "Det " << detnum
              << ": " << res << std::endl;
  }
}

void FitSegments(TH2* summary, int detnum) {
  for(int segnum=1; segnum<33; segnum++){
    TH1* hist = summary->ProjectionY(Form("charge_det%02d_seg%02d",detnum,segnum),
                                     segnum, segnum);
    hist->Rebin(2);
    fit_result res = FitCo60(hist, input_channels ? segment_address(detnum,segnum) : -1,
                             70, 0.15);
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
  TFile fout("diagnostics.root","RECREATE");

  std::cout << "---------- Fitting Cores ------------" << std::endl;
  TH2* detsummary = (TH2*)fin->Get("sega_charge_summary");
  FitCores(detsummary);

  for(int detnum=1; detnum<2; detnum++){
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
