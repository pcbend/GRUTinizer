#ifndef TGATES_H
#define TGATES_H

#include <iostream>
#include <math.h>
#include <string.h>
#include <iomanip>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

#include "TObject.h"
#include "TList.h"
#include "TCanvas.h"
#include "TCutG.h"

#include "TNucleus.h"
class TGates : public TObject{

 private:

 public:
  TGates();
  virtual ~TGates();

  bool LoadPIDFile(const char *);				//Loads PID Files
  void MakePIDFile(const char *);				//Creates a text file containing particle gates
  std::vector<std::pair<TNucleus*,TCutG*> > GetListOfGates() { return GateList; }

  std::string GetSymbol(int);
  int GateID(float, float);
  int Size() { return (int)GateList.size(); }
 private:
  int fNPid;
  int fNP;
  int fZ;
  int fA;
  float fX;
  float fY;

  std::vector<std::pair<TNucleus*,TCutG*> > GateList;

ClassDef(TGates,1);				// Creates a nucleus with corresponding nuclear information
};

#endif
