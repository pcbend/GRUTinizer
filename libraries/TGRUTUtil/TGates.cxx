#include "TGates.h"

ClassImp(TGates)
/*******************************************************************************/
/* TGates  *********************************************************************/
/* Used to make a vector of <TNucleus, TCutG> which contain atomic data and ****/
/* gates used for making/filling histograms ************************************/
/*******************************************************************************/
TGates::TGates() {
}

TGates::~TGates() { }

/*******************************************************************************/
/* Loads a PID File ************************************************************/
/*******************************************************************************/
bool TGates::LoadPIDFile(const char * infile) {
  std::ifstream ifile(infile);
  if(!ifile.is_open()) {
    std::cout << "PID File " << infile << " not found" << std::endl;
    return false;
  }
  ifile >> fNPid;
  for(int i = 0; i < fNPid; i++) {
    TCutG *tmpG = new TCutG();
    ifile >> fZ >> fA;
    TNucleus *tmpN = new TNucleus(fZ, fA - fZ);
    ifile >> fNP;
    for(int j = 0; j < fNP; j++) {
      ifile >> fX >> fY;
      tmpG->SetPoint(j, fX, fY);
    }
    tmpG->SetName(Form("%s",tmpN->GetSymbol()));
    GateList.push_back(std::make_pair(tmpN, tmpG));
  }
  return true;
}

/*******************************************************************************/
/* Used in interactive mode to draw one or more TCutGs and create a .pid file **/
/*******************************************************************************/
void TGates::MakePIDFile(const char * filename) {
  std::vector<std::string> data;
  int NPID = 0;
  while(1) {
    TCutG *cut1 = (TCutG*)gPad->WaitPrimitive("CUTG","CutG");
    cut1->SetName("cut1");
    cut1->Print();
    int n = cut1->GetN();
    double *x = cut1->GetX();
    double *y = cut1->GetY();
    int Z, A;
    std::cout << "Z A?" << std::endl;
    std::cin >> Z >> A;
    data.push_back(Form("%i %i",Z,A));
    data.push_back(Form("%i",n));
    for (int i = 0; i < n; i++) {
      data.push_back(Form("%f %f",*x++,*y++));
    }
    delete cut1;
    std::string con;
    std::cout << "Continue (y/n)" << std::endl;
    std::cin >> con;
    NPID++;
    if(con != "y") break;
  }
  std::ofstream outfile;
  std::string fn = filename;
  if(fn.substr(fn.find_last_of(".") + 1) == "pid") {
    outfile.open(Form("%s",filename));
  } else {
    outfile.open(Form("%s.pid",filename));
  }

  outfile << NPID << std::endl;
  for(unsigned int i = 0; i < data.size();i++) {
    outfile << data.at(i) << std::endl;
  }
  outfile.close();
  return;
}

/*******************************************************************************/
/* Used in interactive mode to draw a 1D Gate and create a .pid file ***********/
/*******************************************************************************/
void TGates::Make1DPIDFile(const char * filename) {
  std::vector<std::string> data;
  int NPID = 0;

  TLine *lin[2];
  for(int i = 0; i < 2; i++ ) {
    TMarker *mark = (TMarker*)gPad->WaitPrimitive("TMarker","TMarker");
    lin[i] = new TLine(mark->GetX(), gPad->GetUymin(), mark->GetX(), gPad->GetUymax());
    lin[i]->Draw("same");
    mark->Delete();
  }
  float xmin = lin[0]->GetX1();
  float xmax = lin[1]->GetX1();
  if(xmax < xmin) std::swap(xmin, xmax);
  int Z,A;
  std::cout << "Z A?" << std::endl;
  std::cin >> Z >> A;
  data.push_back(Form("%i %i", Z, A));
  data.push_back(Form("%i", 1));
  data.push_back(Form("%f %f", xmin, xmax));
  NPID++;

  std::ofstream outfile;

  std::string fn = filename;
  if(fn.substr(fn.find_last_of(".") + 1) == "pid") {
    outfile.open(Form("%s",filename));
  } else {
    outfile.open(Form("%s.pid",filename));
  }

  outfile << NPID << std::endl;
  for(unsigned int i = 0; i < data.size();i++) {
    outfile << data.at(i) << std::endl;
  }
  outfile.close();
  return;
}

/*******************************************************************************/
/* Gets the atomic symbol of a TGate, primarily for naming Histograms **********/
/*******************************************************************************/
std::string TGates::GetSymbol(int ID) {
  if(ID > (int)GateList.size() - 1) return "NA";
  else return GateList.at(ID).first->GetSymbol();

}

/*******************************************************************************/
/* Used for Gating purposes, returns poisiton in the vecotr GateList ***********/
/* For a 1D gate variable y must be defined, but is not used *******************/
/*******************************************************************************/
int TGates::GateID(float x, float y) {
  int i = 0;
  for(auto &iter: GateList) {
    //Check if 1D Gate
    if(iter.second->GetN() == 1) {
      if((x > iter.second->GetPointX(0)) && (x < iter.second->GetPointY(0))) return i;
    } else if(iter.second->IsInside(x,y)) return i;
    i++;
  }
  return -1;
}

/*******************************************************************************/
/* Gets the mass number a TGate ************************************************/
/*******************************************************************************/
int TGates::GetA(int ID) {
  if(ID > (int)GateList.size() - 1) return -1;
  else return GateList.at(ID).first->GetA();

}

/*******************************************************************************/
/* Gets the proton number a TGate **********************************************/
/*******************************************************************************/
int TGates::GetZ(int ID) {
  if(ID > (int)GateList.size() - 1) return -1;
  else return GateList.at(ID).first->GetZ();

}
/*******************************************************************************/
/* Gets the neutron number a TGate *********************************************/
/*******************************************************************************/
int TGates::GetN(int ID) {
  if(ID > (int)GateList.size() - 1) return -1;
  else return GateList.at(ID).first->GetN();

}

/*******************************************************************************/
/* Gets the mass a TGate in MeV ************************************************/
/*******************************************************************************/
double TGates::GetMass(int ID) {
  if(ID > (int)GateList.size() - 1) return -1;
  else return GateList.at(ID).first->GetMass();
}

/*******************************************************************************/
/* Loads a Gate File ***********************************************************/
/* Gate files do not contain TNucleus information  *****************************/
/*******************************************************************************/
bool TGates::LoadGateFile(const char * infile) {
  std::ifstream ifile(infile);
  if(!ifile.is_open()) {
    std::cout << "Gate File " << infile << " not found" << std::endl;
    return false;
  }

  gate2D = new TCutG();
  ifile >> fNP;
  for(int j = 0; j < fNP; j++) {
    ifile >> fX >> fY;
    gate2D->SetPoint(j, fX, fY);
  }
  gate2D->SetName("gate2d");

  return true;
}

/*******************************************************************************/
/* Draws a Gate File ***********************************************************/
/* At present supports 2D gates only *******************************************/
/*******************************************************************************/
void TGates::MakeGateFile(const char * filename) {
  std::vector<std::string> data;
  while(1) {
    TCutG *cut1 = (TCutG*)gPad->WaitPrimitive("CUTG","CutG");
    cut1->SetName("cut1");
    cut1->Print();
    int n = cut1->GetN();
    double *x = cut1->GetX();
    double *y = cut1->GetY();
    data.push_back(Form("%i",n));
    for (int i = 0; i < n; i++) {
      data.push_back(Form("%f %f",*x++,*y++));
    }
    delete cut1;
    std::string con;
    std::cout << "Redraw (y/n)" << std::endl;
    std::cin >> con;
    if(con != "y") break;
    else data.clear();
  }
  std::ofstream outfile;

  std::string fn = filename;
  if(fn.substr(fn.find_last_of(".") + 1) == "gate") {
    outfile.open(Form("%s",filename));
  } else {
    outfile.open(Form("%s.gate",filename));
  }
  for(unsigned int i = 0; i < data.size();i++) {
    outfile << data.at(i) << std::endl;
  }
  outfile.close();
  return;
}

/*******************************************************************************/
/* Checks if gate codition is met **********************************************/
/*******************************************************************************/
bool TGates::InGate(float X, float Y) {
  if(gate2D->IsInside(X, Y)) return true;
  else return false;
}
