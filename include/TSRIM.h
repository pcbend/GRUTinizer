#ifndef TSRIMMANAGER_H
#define TSRIMMANAGER_H

#include <fstream>
#include <iostream>
#include <vector>

#include <map>
#include <utility>

#include "TGraph.h"
#include "TSpline.h"

class TSRIM	{
public:
  TSRIM();
  // looks like the emax-min inputs don't work right now for 94Sr and maybe other high mass nuclei
  TSRIM(const char* infilename, double emax = -1.0, double emin = 0.0, bool printfile = false);
  virtual ~TSRIM();

  void ReadEnergyLossFile(const char* filename, double emax = -1.0, double emin = 0.0,bool printfile = true);

  double GetAdjustedEnergy(double energy_keV, double distance_um, double stepsize = dx);
  double GetEnergyLost(double energy_keV, double distance_um, double stepsize = dx) {
    return energy_keV-GetAdjustedEnergy(energy_keV, distance_um, stepsize);
  }

  double GetEnergy(double energy_keV, double dist_um);
  double GetEnergyChange(double energy_keV, double dist_um) {
    return GetEnergy(energy_keV,dist_um) - energy_keV;
  }

  double GetEmax() { return Emax;};
  double GetEmin() { return Emin;};
  double GetXmax() { return Xmax;};
  double GetXmin() { return Xmin;};

  TGraph* GetEnergyLossGraph() {return fEnergyLoss;};
  TGraph* GetEvsXGraph() {return fXgetE;};
  TGraph* GetXvsEGraph() {return fEgetX;};
  TSpline3* GetEvsXSpline() {return sXgetE;};
  TSpline3* GetXvsESpline() {return sEgetX;};


private:
  std::vector<double> IonEnergy;
  std::vector<double> dEdX;
  TGraph* fEnergyLoss;
  TGraph* fEgetX;
  TGraph* fXgetE;
  TSpline3* sEnergyLoss;
  TSpline3* sEgetX;
  TSpline3* sXgetE;
  std::vector<double> E; // units of keV
  std::vector<double> X; // units of um
  double Emin, Emax, Xmin, Xmax;
  static const double dx; // um [sets accuracy of energy loss E vs X functions]

  std::map<std::pair<double,double>,double> AdjustedEnergyMap;

  ClassDef(TSRIM,0)
};

#endif
