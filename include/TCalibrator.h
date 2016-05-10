#ifndef _TCALIBRATORS_H_
#define _TCALIBRATORS_H_

#include <map>

#include <TNamed.h>
#include <TGraphErrors.h>

class TH1;

class TChannel;
class TNucleus;



class TCalibrator : public TNamed { 
public:
  TCalibrator();
  ~TCalibrator();

  virtual void Copy(TObject& obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");
  virtual void Draw(Option_t *option="");

  int GetFitOrder() const { return fit_order; }
  void SetFitOrder(int order) { fit_order = order; }

  TGraph& MakeCalibrationGraph(double min_figure_of_merit = 0.001);
  std::vector<double> Calibrate(double min_figure_of_merit = 0.001);

  int AddData(TH1* source_data, std::string source,
               double sigma=2.0,double threshold=0.05,double error=0.001);
 
  int AddData(TH1* source_data, TNucleus* source,
               double sigma=2.0,double threshold=0.05,double error=0.001);

  void UpdateTChannel(TChannel* channel);

  void Fit(int order=1); 
  double GetParameter(int i=0);


private:
  #ifndef __CINT__
  struct SingleFit {
    double max_error;
    std::string nucleus;
    std::map<double,double> data2source;
    TGraph graph;
  };

  std::map<std::string,SingleFit> all_fits;
  #endif

  TGraph graph_of_everything;
  TF1    *linfit;

  int fit_order;
  int total_points;


  void ResetMap(std::map<double,double> &inmap);
  void PrintMap(std::map<double,double> &inmap);
  bool CheckMap(std::map<double,double> inmap);

  ClassDef(TCalibrator,1)
};

#endif
