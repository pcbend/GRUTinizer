
#include <TCalibrator.h>

#include <cmath>
#include <cstdio>
#include <vector>
#include <map>
#include <algorithm>

#include <TH1.h>
#include <TSpectrum.h>

#include <TChannel.h>
#include <TNucleus.h>
#include <TTransition.h>

#include <GRootFunctions.h>
#include <GRootCommands.h>
#include <GGaus.h>
#include <Globals.h>

ClassImp(TCalibrator)

TCalibrator::TCalibrator() { 
  linfit=0;
  Clear(); 
}

TCalibrator::~TCalibrator() { 
  if(linfit) delete linfit;
}

void TCalibrator::Copy(TObject &obj) const { }

void TCalibrator::Print(Option_t *opt) const {
  for(auto it:all_fits) {
    printf("Calibration for %s\tmax error: %.04f\n",it.first.c_str(),it.second.max_error);
    int counter = 0;
    printf("\t\t energy\tchannel\n");
    for(auto it2:it.second.data2source) {
      printf("\t%i\t%.1f\t%.1f\n",counter++,it2.second,it2.first);
    }
    printf("------------------\n");
  } 
  if(linfit) linfit->Print();
}

void TCalibrator::Clear(Option_t *opt) { 
  graph_of_everything.Clear(opt);
  all_fits.clear();
  total_points=0;
}

void TCalibrator::Draw(Option_t *opt) { 
  if((graph_of_everything.GetN()<1) &&
     (all_fits.size()>0))
    MakeCalibrationGraph();
  Fit();
  graph_of_everything.Draw("AC*");
}

void TCalibrator::Fit(int order) {
  if((graph_of_everything.GetN()<1) &&
     (all_fits.size()>0))
    MakeCalibrationGraph();
  if(graph_of_everything.GetN()<1)
    return;
  if(order==1) {
    linfit = new TF1("linfit",GRootFunctions::LinFit,0,1,2);
    linfit->SetParameter(0,0.0);
    linfit->SetParameter(1,1.0);
    linfit->SetParName(0,"intercept");
    linfit->SetParName(1,"slope");
  } else if(order==2) {
    linfit = new TF1("linfit",GRootFunctions::QuadFit,0,1,3);
    linfit->SetParameter(0,0.0);
    linfit->SetParameter(1,1.0);
    linfit->SetParameter(2,0.0);
    linfit->SetParName(0,"A");
    linfit->SetParName(1,"B");
    linfit->SetParName(2,"C");
  }
  graph_of_everything.Fit(linfit);
}


TGraph &TCalibrator::MakeCalibrationGraph(double min_fom) { 
  std::vector<double> xvalues;
  std::vector<double> yvalues;
  //std::vector<double> xerrors;
  //std::vector<double> yerrors;

  for(auto it:all_fits) {
    for(auto it2:it.second.data2source) {
      xvalues.push_back(it2.first);
      yvalues.push_back(it2.second);
    }
  }  
  graph_of_everything.Clear();
  graph_of_everything = TGraph(xvalues.size(),xvalues.data(),yvalues.data());
 
  return graph_of_everything; 
}

std::vector<double> TCalibrator::Calibrate(double min_fom) { std::vector<double> vec; return vec; }


void TCalibrator::AddData(TH1 *data,std::string source, double sigma,double threshold,double error) { 
  if(!data || !source.length()) 
    return;
  TNucleus n(source.c_str());
  AddData(data,&n,sigma,threshold,error);
}

void TCalibrator::AddData(TH1 *data,TNucleus *source, double sigma,double threshold,double error) { 
  if(!data || !source)
    return;

  int actual_x_max = std::floor(data->GetXaxis()->GetXmax());
  int actual_x_min = std::floor(data->GetXaxis()->GetXmax());
  int displayed_x_max = std::floor(data->GetXaxis()->GetBinUpEdge(data->GetXaxis()->GetLast()));
  int displayed_x_min = std::floor(data->GetXaxis()->GetBinLowEdge(data->GetXaxis()->GetFirst()));
  
  std::string name;
  if((actual_x_max==displayed_x_max) && (actual_x_min==displayed_x_min))
    name = source->GetName();
  else
    name = Form("%s_%i_%i",source->GetName(),displayed_x_min,displayed_x_max);

  
  
  TIter iter(source->GetTransitionList());
  std::vector<double> source_energy;
  while(TTransition *transition = (TTransition*)iter.Next()) {
    source_energy.push_back(transition->GetEnergy());
  }
  std::sort(source_energy.begin(),source_energy.end());

  TSpectrum spectrum;
  spectrum.Search(data,sigma,"",threshold);
  //std::vector<double> data_channels;
  std::map<double,double> datatosource; 
  for(int x=0;x<spectrum.GetNPeaks();x++) {
    GGaus *fit = GausFit(data,spectrum.GetPositionX()[x]-5,spectrum.GetPositionX()[x]+5);
    //data_channels
    //data_channels.push_back(fit.GetCentroid());
    datatosource[fit->GetCentroid()] = sqrt(-1);
    data->GetListOfFunctions()->Remove(fit);
  }

    
  double max_err = 0.0000;
  for(auto source_a:source_energy) {
    for(auto source_b:source_energy) {
      if(source_a==source_b) continue;
      int counter2 =0;
      for(auto &data_ita: datatosource) {
        for(auto &data_itb: datatosource) {
          if(data_ita==data_itb) continue;
          double dratio = data_ita.first/data_itb.first;
          double sratio = source_a/source_b;
          if( fabs(dratio-sratio) <=error) {
            printf(BLUE "\t%i\t%.02f\t%.02f\t" RESET_COLOR "\n",counter2++,dratio,sratio);
            if(fabs(dratio-sratio)>max_err)
              max_err = fabs(dratio-sratio);
            if(std::isnan(data_ita.second))
              data_ita.second = source_a;
            if(std::isnan(data_itb.second))
              data_itb.second = source_b;
            break;
          } 
        }
      }
    }
    printf("\n");
    if(CheckMap(datatosource))
      break;
    ResetMap(datatosource);
    Print();
  }

  if(CheckMap(datatosource)) {
    all_fits[name].data2source = datatosource;
    all_fits[name].nucleus     = source->GetName();
    all_fits[name].max_error   = max_err;
    total_points += datatosource.size();
  }
}

void TCalibrator::ResetMap(std::map<double,double> &inmap) {
  for(auto &it:inmap) 
    it.second = sqrt(-1);
}

bool TCalibrator::CheckMap(std::map<double,double> inmap) {
  for(auto it:inmap) {
    if(std::isnan(it.second)) {
      return false;
    }
  }
  return true;
}



void TCalibrator::UpdateTChannel(TChannel *channel) { } 


