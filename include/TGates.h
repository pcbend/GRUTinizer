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

  bool LoadGateFile(const char *);				//Loads 2d Gate from File
  void MakeGateFile(const char *);				//Creates a text file containing a 2d gate
  bool InGate(float, float);					//Checks if inside gate

  std::string GetSymbol(int);					//Returns name of beam in GateList
  int    GetA(int);						//Returns A of beam in GateList
  int    GetZ(int);						//Returns Z of beam in GateList
  int    GetN(int);						//Returns N of beam in GateList
  double GetMass(int);						//Returns Mass of beam in GateList

  int GateID(float, float);					//Returns position in vector
  int Size() { return (int)GateList.size(); }
 private:
  int fNPid;
  int fNP;
  int fZ;
  int fA;
  float fX;
  float fY;

  TCutG*  gate2D;
  std::vector<std::pair<TNucleus*,TCutG*> > GateList;

ClassDef(TGates,1);				// Creates a nucleus with corresponding nuclear information
};

#endif
