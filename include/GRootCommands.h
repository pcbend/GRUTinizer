#ifndef GROOTCOMMANDS__H
#define GROOTCOMMANDS__H

#include <string>
class TTree;
class TH1;
class GMarker;

#include "TDirectory.h"

int  LabelPeaks(TH1*,double,double,Option_t *opt="");
bool ShowPeaks(TH1**,unsigned int);
bool RemovePeaks(TH1**,unsigned int);

bool PhotoPeakFit(TH1*,double,double,Option_t *opt="");
bool GausFit(TH1*,double,double,Option_t *opt="");

std::string MergeStrings(const std::vector<std::string>& strings, char split='\n');

//bool PeakFit(TH1*,Double_t,Double_t,Option_t *opt="");

void Help();
void Commands();

#endif
