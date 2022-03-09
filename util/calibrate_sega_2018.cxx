#include <algorithm>
#include <iostream>
#include <fstream>
#include <deque>

#include "TAxis.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "GH1.h"
#include "GH2.h"
#include "GH1D.h"
#include "GH2D.h"
#include "TSpectrum.h"

#include "TChannel.h"

const char* input_file = NULL;

int seg_fails = 0;

struct fit_result {
  bool success;
  double slope, offset;
};

std::ostream& operator<<(std::ostream& os, const fit_result& res){
  if(res.success){
    os << "Slope: " << res.slope
       << ", Offset: " << res.offset;
  } else {
    os << "Fit failed";
  }
  return os;
}

struct fit_point {
  fit_point(double x, double y, double xfit = 0)
    : x_spec(x), y_spec(y), x_fit(xfit) { }
  double x_spec;
  double y_spec;
  double x_fit;
};

fit_result Fit133Ba(GH1D* hist, std::ofstream &coefs, double sigma=7, double threshold = 0.05) {
  fit_result output;
  output.success = true;

  hist->GetXaxis()->SetRangeUser(1500,4500);
  
  // Initial search for the peaks, using TSpectrum
  TSpectrum spec;
  int nfound = spec.Search(hist, sigma, "", threshold);

  if(nfound < 4){
    std::cout << "Numpeaks was " << nfound << " for spectrum " << hist->GetName()
              << std::endl;
    hist->Write();
    output.success = false;
    coefs << 0 << "\n";
    coefs << 1 << "\n";
    seg_fails++;
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

  // Throw out all but the highest 4 energies
  while(points.size() > 4){
    points.pop_front();
  }
  /*
  std::cout << "TSpectrum Points: "
            << "\n" << points[0].x_spec
            << "\n" << points[1].x_spec
            << "\n" << points[2].x_spec
            << "\n" << points[3].x_spec
	    << std::endl;
  */
  
  // Pick initial values for a fit
  double spread = points[3].x_spec - points[0].x_spec;
  double xmin = points[0].x_spec - spread/4;
  double xmax = points[3].x_spec + spread/4;
  TF1* Ba133_fit = new TF1("Ba133","gaus(0) + gaus(3) + gaus(6) + gaus(9) + [12]*x + [13]", xmin, xmax);
  Ba133_fit->SetParameters(points[0].y_spec,points[0].x_spec,sigma,
			   points[1].y_spec,points[1].x_spec,sigma,
			   points[2].y_spec,points[2].x_spec,sigma,
			   points[3].y_spec
                          );
  Ba133_fit->SetParameter(10,points[3].x_spec);
  Ba133_fit->SetParameter(11,sigma);
  Ba133_fit->SetParameter(12,-0.022);
  Ba133_fit->SetParameter(13,63);
    

  // Use correct weights on each bin
  hist->Sumw2(true);
  hist->GetXaxis()->SetRangeUser(xmin, xmax);
  // Fit, q for quiet, R for range, goff to avoid drawing
  hist->Fit(Ba133_fit, "q", "Rgoff");
  std::vector<double> xpoints_fit;
  points[0].x_fit = (Ba133_fit->GetParameter(1)); //Mean of first gaus
  points[1].x_fit = (Ba133_fit->GetParameter(4)); //Mean of second gaus
  points[2].x_fit = (Ba133_fit->GetParameter(7)); //Mean of third gaus
  points[3].x_fit = (Ba133_fit->GetParameter(10)); //Mean of fourth gaus
  /*
  std::cout << "Fit Points: "
	    << "\n" << points[0].x_fit
            << "\n" << points[1].x_fit
            << "\n" << points[2].x_fit
            << "\n" << points[3].x_fit
            << std::endl;
  */

  for(auto& point : points){
    if(std::abs(point.x_spec - point.x_fit) > spread) {
      std::cout << "Fit failed for hist \"" << hist->GetName() << "\"" << std::endl;
      std::cout << "\tTSpectrum peak loc: " << point.x_spec << std::endl;
      std::cout << "\tGaus fit peak loc:  " << point.x_fit << std::endl;
      output.success = false;
      seg_fails++;
    }
  }

  // Use a linear fit to determine the slope/offset
  TGraph* linfit_graph = new TGraph(4);
  linfit_graph->SetNameTitle(Form("%s_graph",hist->GetName()),
                      Form("%s_graph",hist->GetName()));
  linfit_graph->SetPoint(0, points[0].x_fit, 276.4);
  linfit_graph->SetPoint(1, points[1].x_fit, 302.9);
  linfit_graph->SetPoint(2, points[2].x_fit, 356.0);
  linfit_graph->SetPoint(3, points[3].x_fit, 383.8);
  TF1* linfit = new TF1("linfit", "pol1(0)");
  linfit_graph->Fit(linfit,"q","goff");

  output.slope = linfit->GetParameter(1);
  output.offset = linfit->GetParameter(0);
  if(output.success) {
    coefs << output.offset << "\n";
    coefs << output.slope << "\n";
  }
  else {
    coefs << 0 << "\n";
    coefs << 1 << "\n";
  }
  //output.fwhm_1172 = output.slope*fwhm_fit[0];
  //output.fwhm_1332 = output.slope*fwhm_fit[1];

  /*
  if(std::abs(output.offset) > 10){
    std::cout << "Large offset parameter (" << output.offset << " keV)"
              << " for hist \"" << hist->GetName() << "\"" << std::endl;
  }
  */

  hist->Write();
  return output;
}

void FitCores(GH2* summary, std::ofstream &coefs) {
  for(int detnum=1; detnum<17; detnum++){
    GH1D* hist = (GH1D*)summary->ProjectionY(Form("charge_det%02d",detnum),detnum, detnum);
    fit_result res = Fit133Ba(hist,coefs);
    //std::cout << "Det " << detnum
    //        << ": " << res << std::endl;
  }
}

void FitSegments(GH2* summary, int detnum, std::ofstream &coefs) {
  for(int segnum=1; segnum<33; segnum++){
    GH1D* hist = (GH1D*)(summary->ProjectionY(Form("charge_det%02d_seg%02d",detnum,segnum),segnum, segnum));
    hist->Rebin(4);
    fit_result res = Fit133Ba(hist,coefs,5);
    //std::cout << "Det " << detnum << ", Seg " << segnum
    //           << ": " << res << std::endl;
  }
}

int main(int argc, char** argv) {

  if(argc < 2){
    std::cout << "Need a histogram file."<< std::endl;
    return -1;
  }

  input_file = argv[1];
  std::ofstream coef;
  coef.open("coefficients.txt");

  TFile* fin = new TFile(input_file);
  TFile fout("diagnostics.root","RECREATE");

  std::cout << "---------- Fitting Cores ------------" << std::endl;
  GH2* detsummary = (GH2*)fin->Get("sega_charge_summary");
  FitCores(detsummary,coef);

  
  for(int detnum=1; detnum<17; detnum++){
    std::cout << "---------- Fitting Det " << detnum
              << " Segments ------------"  << std::endl;
    std::string hist_name = Form("sega_det%02d_segsummary",detnum);
    GH2* segsummary = (GH2*)fin->Get(hist_name.c_str());
    if(segsummary){
      FitSegments(segsummary,detnum,coef);
    } else {
      std::cout << "Missing segment summary \"" << hist_name << "\"" << std::endl;
    }
  }

  coef.close();

  std:: cout << seg_fails << " failed segments" << std::endl;

  return 1;
  
}
