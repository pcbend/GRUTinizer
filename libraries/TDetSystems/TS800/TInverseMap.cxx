
#include <TInverseMap.h>

#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <sstream>


#include <TS800.h>
#include <TGRUTOptions.h>

TInverseMap *TInverseMap::fInverseMap=0;

TInverseMap::TInverseMap(const char* filename) : TNamed("InverseMap",filename) {
  ReadMapFile(filename);
}

TInverseMap::~TInverseMap() { }


TInverseMap *TInverseMap::Get(const char *filename) {
  if(fInverseMap)
    return fInverseMap;
  if(strlen(filename)==0 || access(filename,F_OK)==-1) {
    printf("no inverse map loaded and file \"%s\" not found.\n",filename);
    return 0;
  }
  fInverseMap = new TInverseMap(filename);
  return fInverseMap;
}


bool TInverseMap::ReadMapFile(const char *filename) {
  std::string mapfile = filename;
  if(mapfile.length()==0)
    mapfile = TGRUTOptions::Get()->S800InverseMapFile();
  if(mapfile.length()==0 || access(mapfile.c_str(),F_OK)==-1) {
    printf("no inverse map loaded and file \"%s\" not found.\n",mapfile.c_str());
    return false;
  }
  //static std::mutex inv_map_mutex;

  std::ifstream infile;
  infile.open(mapfile.c_str());
  std::string line;
  getline(infile,info);
  sscanf(info.c_str(),"S800 inverse map - Brho=%g - M=%d - Q=%d", &fBrho, &fMass, &fCharge);
  
  int par =0;
  while(getline(infile,line)) {
    if(line.find("----------")!=std::string::npos)
      continue;
    if(line.find("COEFFICIENT")!=std::string::npos){
      par++;
      continue;
    }
    unsigned int index;
    InvMapRow invrow;
    std::stringstream ss(line);
    ss >> index;
    if((index-1) != fMap[par-1].size()) { 
      //problems. 
    }
    {
      std::string temp;
      ss >> temp;
      invrow.coefficient = std::atof(temp.c_str());
    }
//    ss >> invrow.coefficient;
    ss >> invrow.order;
    ss >> invrow.exp[0];
    ss >> invrow.exp[1];
    ss >> invrow.exp[2];
    ss >> invrow.exp[3];
    ss >> invrow.exp[4];
    ss >> invrow.exp[5];
    
    fMap[par-1].push_back(invrow);

    //printf("%i\t%s\n",index,line.c_str());

  }
  return true;
}

void TInverseMap::Print(Option_t *opt) const {
  printf("%s\n",info.c_str());
  printf("\tBrho = %.04f\t",fBrho);
  printf("Mass = %i\t",fMass);
  printf("Q    = %i\n",fCharge);
  for(auto it1 : fMap) {
    printf("----------- par: %i ---------------\n",it1.first);
    int counter =1;
    for(unsigned int i=0;i< it1.second.size();i++) {
      printf("\t%i\t%.04f\t\t%i\t%i %i %i %i %i %i\n",
          counter++,it1.second.at(i).coefficient,it1.second.at(i).order,
          it1.second.at(i).exp[0],
          it1.second.at(i).exp[1],
          it1.second.at(i).exp[2],
          it1.second.at(i).exp[3],
          it1.second.at(i).exp[4],
          it1.second.at(i).exp[5]);
    }
  }
}

//Parameter def.               |    
// 0 == ATA                    |    
// 1 == YTA                    |    
// 2 == BTA                    |    
// 3 == DTA                    |    

//Input def.
// 0 == Crdc 0 -x in meters.   |    
// 1 == - Afp in radians.      |    
// 2 == Crdc 0 +y in meters.   |    
// 3 == + Bfp in radians.      |   
// 4 == 0;                     |    
// 5 == 0;                     |    

// S800 angles
// S800 system (looking dwnstream)
// positive ata: dispersive (aka: from x-axis) angle pointing down
// positive bta: none-dispersive (aka: from y-axis)  angle pointing right
// yta: y position in [m], along none-dispersive angle, positive right
// 
// GRETINA:
// z: beamaxis
// x: down (=S800 neg. dispersive direction)
// Y: left
// Opening for gate valve points to the floor (x positive)
// 
// We transform S800 angle into GRETINA.

float TInverseMap::Ata(int order, double xfp, double afp, double yfp, double bfp) const {
  float input[6];
  input[0]  = - xfp / 1000.0;
  input[1]  = - afp;
  input[2]  =   yfp / 1000.0;
  input[3]  =   bfp;
  input[4]  =   0.0;
  input[5]  =   0.0;
  return MapCalc(order, 0, input);
}

float TInverseMap::Bta(int order, double xfp, double afp, double yfp, double bfp) const {
  float input[6];
  input[0]  = - xfp / 1000.0;
  input[1]  = - afp;
  input[2]  =   yfp / 1000.0;
  input[3]  =   bfp;
  input[4]  =   0.0;
  input[5]  =   0.0;
  return MapCalc(order, 2, input);
}

float TInverseMap::Yta(int order, double xfp, double afp, double yfp, double bfp) const {
  float input[6];
  input[0]  = - xfp / 1000.0;
  input[1]  = - afp;
  input[2]  =   yfp / 1000.0;
  input[3]  =   bfp;
  input[4]  =   0.0;
  input[5]  =   0.0;
  return MapCalc(order, 1, input);
}

float TInverseMap::Dta(int order, double xfp, double afp, double yfp, double bfp) const {
  float input[6];
  input[0]  = - xfp / 1000.0;
  input[1]  = - afp;
  input[2]  =   yfp / 1000.0;
  input[3]  =   bfp;
  input[4]  =   0.0;
  input[5]  =   0.0;
  return MapCalc(order, 3, input);
}

float TInverseMap::Ata(int order, const TS800 *s800) { 
  float input[6];
  input[0]  = - s800->GetXFP() / 1000.0;
  input[1]  = - s800->GetAFP();
  input[2]  =   s800->GetYFP() / 1000.0;
  input[3]  =   s800->GetBFP();
  input[4]  =   0.0;
  input[5]  =   0.0;
  int par = 0; // ATA
  return MapCalc(order,par,input);
}

float TInverseMap::Bta(int order, const TS800 *s800) { 
  float input[6];
  input[0]  = - s800->GetXFP() / 1000.0;
  input[1]  = - s800->GetAFP();
  input[2]  =   s800->GetYFP() / 1000.0;
  input[3]  =   s800->GetBFP();
  input[4]  =   0.0;
  input[5]  =   0.0;
  int par = 2; // BTA
  return MapCalc(order,par,input);
}

float TInverseMap::Yta(int order, const TS800 *s800) { 
  float input[6];
  input[0]  = - s800->GetXFP() / 1000.0;
  input[1]  = - s800->GetAFP();
  input[2]  =   s800->GetYFP() / 1000.0;
  input[3]  =   s800->GetBFP();
  input[4]  =   0.0;
  input[5]  =   0.0;
  int par = 1; // YTA
  // *1000, because the map returns the value in meters.
  return MapCalc(order,par,input)*1000;
}

float TInverseMap::Dta(int order, const TS800 *s800) { 
  float input[6];
  input[0]  = - s800->GetXFP() / 1000.0;
  input[1]  = - s800->GetAFP();
  input[2]  =   s800->GetYFP() / 1000.0;
  input[3]  =   s800->GetBFP();
  input[4]  =   0.0;
  input[5]  =   0.0;
  int par = 3; // DTA
  return MapCalc(order,par,input);
}

float TInverseMap::MapCalc(int order,int par,float *input) const { 
  float cumul         = 0.0;
  float multiplicator = 0.0;
  std::vector<InvMapRow> vec = fMap.at(par);
  for(unsigned int x=0; x<vec.size();x++) {
    if(order<vec.at(x).order) break;
    multiplicator = 1.0;
    for(int y=0;y<6;y++) {
      if(vec.at(x).exp[y]!=0)
        multiplicator *= pow(input[y],vec.at(x).exp[y]);
    }
    cumul += multiplicator*vec.at(x).coefficient;
  }
  return cumul;
}









