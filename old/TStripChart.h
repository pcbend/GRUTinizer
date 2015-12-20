#ifndef _TSTRIPCHART_H_
#define _TSTRIPCHART_H_

#ifndef __CINT__
#  include <thread>
#  include <atomic>
#endif
#include <vector>

#include "TBits.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TLegend.h"
#include "TStopwatch.h"
#include "TStyle.h"

#include "TStripChartItem.h"



class TStripChart : public TObject {
public:
  TStripChart();
  ~TStripChart();

  void Update();
  void Draw(Option_t* opt = "nostack ][");

  TVirtualPad* GetPad();
  double GetCurrentTime();

  void SetPaused(bool flag = true) { bitflags.SetBitNumber(kPaused, flag); }
  bool IsPaused()                  { return bitflags.TestBitNumber(kPaused); }

  void SetRunning(bool flag = true);
  bool IsRunning()                 { return bitflags.TestBitNumber(kRunning); }

  void Attach(const char* title, double* value, int color = 0);

  size_t NumCharts(){ return charts.size(); }
  void Fill();

  void SetColor(size_t chartnum, int color) { GetChart(chartnum).SetColor(color); }

  TStripChartItem& GetChart(size_t chartnum) { return *charts.at(chartnum).chart; }

  TH1* GetHist(size_t chartnum) { return GetChart(chartnum).GetHist(); }

  void StartPoll(int period_milliseconds);
  void RunPoll(int period_milliseconds);
  void StopPoll();

private:

  enum {kPaused, kRunning};

  void Init();

  TBits bitflags;
  TCanvas canvas;
  TStopwatch stopwatch;
  TLegend legend;

  static TStyle* style;

  struct ChartValues{
    double* value;
    TStripChartItem* chart;
  };

  std::vector<ChartValues> charts;
  THStack hstack;

#ifndef __CINT__
  std::thread poll_thread;
  std::atomic_bool is_polling;
#endif

  ClassDef(TStripChart, 1)
};

#endif /* _TSTRIPCHART_H_ */
