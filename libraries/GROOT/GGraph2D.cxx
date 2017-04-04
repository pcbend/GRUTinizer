
#include <TString.h>
#include <TAxis.h>

#include <GCanvas.h>
#include <GGraph2D.h>
#include <GGraph.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

GGraph2D::GGraph2D(const char *filename,Option_t *opt) { 
  SetNameTitle("GGraph2D",filename);
  
  SetLineColor(1);
  SetLineStyle(1);
  SetLineWidth(1);

  SetFillColor(1);
  SetFillStyle(1001);

  std::vector<double> vx,vy,vz,vdx,vdy,vdz;
  std::vector<double> values;
  values.reserve(6);
  std::ifstream infile(filename);
  if(!infile.is_open()) {
    MakeZombie();
    fprintf(stderr,"failed to open, %s!  I am now a zombie... mmmmm brains.",filename);
  }
  std::string line;
  while(getline(infile,line)) {
    if(!line.length())
      break;
    std::stringstream ss(line);
    std::string buffer;
    int count=0;
    int nps=0;
    std::fill(values.begin(),values.end(),0.00);  
    while(ss>>buffer) {
      std::stringstream(buffer)>>values[count++];;
    }
    SetPoint(nps,values.at(0),values.at(1),values.at(2));
    SetPointError(nps,values.at(5),values.at(4),values.at(3));
    nps++;
  }

}


GGraph *GGraph2D::ProjectionX(double low, double high,Option_t *opt) const {
  if(low>high) std::swap(low,high);
  TString sopt = opt;
  sopt.ToLower();
  std::vector<double> x;
  std::vector<double> dx;
  std::vector<double> z;
  std::vector<double> dz;
  for(int i=0;i<GetN();i++) { 
    if( ((*(GetY()+i))>=low) && ((*(GetY()+i))<=high) ) {
       x.push_back(*(GetX()+i));
       z.push_back(*(GetZ()+i));
      dx.push_back(*(GetEX()+i));
      dz.push_back(*(GetEZ()+i));
    }
  }
  
  GGraph *gr = new GGraph(x.size(),&(x[0]),&(z[0]),&(dz[0]),&(dx[0]));
  gr->SetNameTitle(Form("%s_projX",GetName()),Form("projX %.02f - %.02f",low,high));
  if(sopt.Contains("draw")) {
    new GCanvas;
    gr->Draw("AP");
    if(GetXaxis()) { gr->GetXaxis()->SetTitle(GetXaxis()->GetTitle()); }
  }
  return gr; 
}

GGraph *GGraph2D::ProjectionY(double low, double high,Option_t *opt) const {
  if(low>high) std::swap(low,high);
  TString sopt = opt;
  sopt.ToLower();
  std::vector<double> y;
  std::vector<double> dy;
  std::vector<double> z;
  std::vector<double> dz;
  for(int i=0;i<GetN();i++) { 
    if( ((*(GetX()+i))>=low) && ((*(GetX()+i))<=high) ) {
       y.push_back(*(GetY()+i));
       z.push_back(*(GetZ()+i));
      dy.push_back(*(GetEY()+i));
      dz.push_back(*(GetEZ()+i));
    }
  }
  
  GGraph *gr = new GGraph(y.size(),&(y[0]),&(z[0]),&(dz[0]),&(dy[0]));
  gr->SetNameTitle(Form("%s_projY",GetName()),Form("projY %.02f - %.02f",low,high));
  if(sopt.Contains("draw")) {
    new GCanvas;
    gr->Draw("AP");
    if(GetXaxis()) { gr->GetXaxis()->SetTitle(GetXaxis()->GetTitle()); }
  }
  return gr;
}


