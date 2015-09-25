#ifndef GROOTFUNCTIONS__H
#define GROOTFUNCTIONS__H

class TH1;

#include "TDirectory.h"

int  LabelPeaks(TH1*,double,double,Option_t *opt="");
bool ShowPeaks(TH1**,unsigned int);
bool RemovePeaks(TH1**,unsigned int);

void Help();
void Commands();

#endif
