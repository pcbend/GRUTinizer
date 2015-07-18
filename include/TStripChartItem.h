#ifndef _STRIPCHARTITEM_H_
#define _STRIPCHARTITEM_H_

#include <string>

#include "TBits.h"
#include "TH1D.h"
#include "TVirtualPad.h"

class TStripChart;

class TStripChartItem : public TObject  {
public:
  TStripChartItem(){ }
  TStripChartItem(const std::string& title, TStripChart* parent, double bin_width = 1);

  void Fill(double value);
  void DrawCopy(Option_t* options = "sameL*");

  bool IsVisible()                     { return bitflags.TestBitNumber(kVisible); }
  void SetVisible(bool flag = true)    { bitflags.SetBitNumber(kVisible, flag); }

  bool IsAutoUpdate()                  { return bitflags.TestBitNumber(kAutoUpdate); }
  void SetAutoUpdate(bool flag = true) { bitflags.SetBitNumber(kAutoUpdate, flag); }

  int  GetColor()                      { return data->GetLineColor(); }
  void SetColor(int color)             { data->SetLineColor(color); }

  TH1* GetHist() { return data; }

  enum {kVisible, kAutoUpdate};

  ~TStripChartItem();

private:
  void ExtendAxis(int factor = 2);

  std::string title;
  std::string name;
  double bin_width;
  TBits bitflags;

  int nbins;
  double xmin, xmax;
  double ymin, ymax;

  TH1D* data;
  TStripChart* parent;

  ClassDef(TStripChartItem, 1)
};

#endif /* _STRIPCHARTITEM_H_ */
