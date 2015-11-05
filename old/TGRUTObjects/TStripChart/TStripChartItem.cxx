#include "TStripChartItem.h"
#include "TStripChart.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "TAxis.h"


ClassImp(TStripChartItem)

TStripChartItem::TStripChartItem(const std::string& title, TStripChart* parent, double bin_width)
: title(title), bin_width(bin_width),
  nbins(3600),
  ymin(NAN), ymax(NAN),
  parent(parent) {

  name = title;
  //std::replace(title.begin(), title.end(), ' ', '_');

  this->bin_width = std::max(0.5, this->bin_width);
  this->bin_width = floor(2*this->bin_width)/2.0;

  xmin = parent->GetCurrentTime();
  xmax = xmin + nbins*bin_width;
  data = new TH1D(name.c_str(), title.c_str(),
                  nbins, xmin, xmax);
  data->SetStats(0);
  data->SetDirectory(0);
}

TStripChartItem::~TStripChartItem(){
  delete data;
}

void TStripChartItem::ExtendAxis(int factor){
  TH1D* hist = new TH1D(Form("temp_%p",(void*)this),"temp",
                        factor*nbins, xmin, factor*(xmax-xmin) + xmin);
  hist->SetStats(0);
  hist->SetDirectory(0);

  hist->Set(nbins, data->GetArray());

  hist->SetNameTitle(name.c_str(), title.c_str());
  delete data;
  data = hist;

  xmin = data->GetXaxis()->GetXmin();
  xmax = data->GetXaxis()->GetXmax();
  nbins = data->GetXaxis()->GetNbins();
}

void TStripChartItem::Fill(double value){
  std::cout << "Filling with " << value << " at time " << parent->GetCurrentTime() << std::endl;

  while(value > xmax){
    std::cout << "Extending axes" << std::endl;
    ExtendAxis();
  }

  auto binnum = data->GetXaxis()->FindBin(parent->GetCurrentTime());
  std::cout << "binnum: " << binnum << std::endl;
  data->SetBinContent(binnum, value);

  ymin = std::min(ymin, value);
  ymax = std::max(ymax, value);
}

void TStripChartItem::DrawCopy(Option_t* options){
  if(!parent->GetPad()){
    return;
  }

  TVirtualPad* bak = gPad;
  gPad = parent->GetPad();
  data->DrawCopy(options);
  gPad = bak;
}
