#include "TStripChart.h"

#include "TList.h"

TStyle* TStripChart::style = NULL;

TStripChart::TStripChart() {
  Init();
  Draw();
}

TStripChart::~TStripChart(){
  for(auto& chartval : charts){
    delete chartval.chart;
  }
}

void TStripChart::Init(){
  is_polling = false;

  if(style==NULL){
    style = new TStyle("OurVeryAwesomeStripChartStyle", "StripChartStyle");

    // style->SetCanvasBorderMode(0);
    // style->SetPadBorderMode(0);
    // style->SetPadColor(0);
    // style->SetCanvasColor(0);
    // style->SetTitleColor(0);
    // style->SetStatColor(0);
  }

  canvas.SetGrid();
  canvas.SetFillColor(42);
  canvas.SetFrameFillColor(33);
}

void TStripChart::Draw(Option_t* opt){
  TVirtualPad* gpad_bak = gPad;
  gPad = GetPad();
  hstack.Draw(opt);
  legend.Draw();
  gPad = gpad_bak;
}

void TStripChart::Update(){
  canvas.Modified();
  canvas.Update();
}

void TStripChart::SetRunning(bool flag) {
  if(!IsRunning() && flag){
    stopwatch.Start();
  }
  if(IsRunning() && !flag){
    stopwatch.Stop();
  }
  bitflags.SetBitNumber(kRunning, flag);
}

TVirtualPad* TStripChart::GetPad(){
  return &canvas;
}

double TStripChart::GetCurrentTime(){
  auto output = stopwatch.RealTime();
  if(IsRunning()){
    stopwatch.Continue();
  }
  return output;
}

void TStripChart::Attach(const char* title, double* value, int color){
  charts.emplace_back();
  ChartValues& chartval = charts.back();
  chartval.value = value;
  chartval.chart = new TStripChartItem(title, this);

  TStripChartItem& chart = *chartval.chart;
  if(color) {
    chart.SetColor(color);
  } else {
    chart.SetColor(charts.size());
  }
  chart.SetVisible(true);
  chart.SetAutoUpdate(true);

  hstack.Add(chart.GetHist());
  hstack.GetHists()->SetOwner(false);

  legend.AddEntry(chart.GetHist(), title, "lp");
}

void TStripChart::StartPoll(int period_milliseconds){
  is_polling = true;
  poll_thread = std::thread(&TStripChart::RunPoll, this, period_milliseconds);
}

void TStripChart::RunPoll(int period_milliseconds){
  while(is_polling){
    Fill();
    std::this_thread::sleep_for(std::chrono::milliseconds(period_milliseconds));
  }
}

void TStripChart::StopPoll(){
  is_polling = false;
}

void TStripChart::Fill() {
  SetRunning(true);
  for(auto& chartval : charts){
    chartval.chart->Fill(*chartval.value);
  }
}
