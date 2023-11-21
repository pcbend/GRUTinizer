#include "TGretinaHit.h"

#include <algorithm>
#include <cmath>
#include <set>

#include <TRandom.h>

#include "GValue.h"
#include "TGEBEvent.h"
#include "TGretina.h"
#include "TS800.h"


TGretinaHit::TGretinaHit(){ Clear(); }

TGretinaHit::~TGretinaHit(){ }

bool TGretinaHit::fSmear = false;
double TGretinaHit::fSmearWidth = 0.2; // 2 mm
/*******************************************************************************/
/* Copies hit ******************************************************************/
/*******************************************************************************/
void TGretinaHit::Copy(TObject &rhs) const {
  TDetectorHit::Copy(rhs);
  ((TGretinaHit&)rhs).fWalkCorrection = fWalkCorrection;
  ((TGretinaHit&)rhs).fPad            = fPad;
  ((TGretinaHit&)rhs).fTOffset        = fTOffset;
  ((TGretinaHit&)rhs).fCrystalId      = fCrystalId;
  ((TGretinaHit&)rhs).fCoreEnergy     = fCoreEnergy;
  ((TGretinaHit&)rhs).fCoreCharge[0]  = fCoreCharge[0];
  ((TGretinaHit&)rhs).fCoreCharge[1]  = fCoreCharge[1];
  ((TGretinaHit&)rhs).fCoreCharge[2]  = fCoreCharge[2];
  ((TGretinaHit&)rhs).fCoreCharge[3]  = fCoreCharge[3];
  ((TGretinaHit&)rhs).fNumberOfInteractions = fNumberOfInteractions;
  ((TGretinaHit&)rhs).fSegments       = fSegments;
  ((TGretinaHit&)rhs).fAB	      = fAB;
  ((TGretinaHit&)rhs).fBaseline	      = fBaseline;
  ((TGretinaHit&)rhs).fPrestep	      = fPrestep;
  ((TGretinaHit&)rhs).fPoststep	      = fPoststep;
}

/*******************************************************************************/
/* Returns core energy from mode2 data and performs a calibration if ***********/
/* a channels.cal file is provided *********************************************/
/*******************************************************************************/
Float_t TGretinaHit::GetCoreEnergy() const {
  TChannel *channel = TChannel::GetChannel(Address());
  //printf("GetAddress() + i = 0x%08x\n",GetAddress()+i);
  if(!channel)
    return fCoreEnergy;
  return channel->CalEnergy(fCoreEnergy);
}

/*******************************************************************************/
/* Selects charge from different gain ranges and performs a calibration if *****/
/* a channels.cal file is provided *********************************************/
/*******************************************************************************/
Float_t TGretinaHit::GetCoreEnergy(int i) const {
  float charge = (float)GetCoreCharge(i) + gRandom->Uniform();
  TChannel *channel = TChannel::GetChannel(Address()+(i<<4));
  //printf("GetAddress() + i = 0x%08x\n",GetAddress()+i);
  if(!channel)
    return charge;
  return channel->CalEnergy(charge);
}

/*******************************************************************************/
/* Gets mnemonic for channel if a channels.cal file is provided ****************/
/*******************************************************************************/
const char *TGretinaHit::GetName() const {
  TChannel *channel = TChannel::GetChannel(Address());
  if(channel) return channel->GetName();
  return "";
}


/*******************************************************************************/
/* Returns position vector for the centre of the crystal ***********************/
/*******************************************************************************/
TVector3 TGretinaHit::GetCrystalPosition()  const {
  return TGretina::GetCrystalPosition(fCrystalId);
}

/*******************************************************************************/
/* Build TGretina events from mode2 data ***************************************/
/*******************************************************************************/
void TGretinaHit::BuildFrom(TSmartBuffer& buf){
  const TRawEvent::GEBBankType1& raw = *(const TRawEvent::GEBBankType1*)buf.GetData();

  //std::cout << "GretinaHit: " << raw << std::endl;
  //std::cout << raw << std::endl;
  Clear();

  SetTimestamp(raw.timestamp);
  fWalkCorrection = raw.t0;
  fCrystalId = raw.crystal_id;
  fCoreEnergy = raw.tot_e;
  fAB = 0;

  int board_id = ((fCrystalId/4) << 8) ;  //hole  number : 0x1f00
  board_id += ((fCrystalId%4) << 6) ;  //x-tal number : 0x00c0
  board_id += 9;                       //chan  number : 0x000f  information not available here(assume core).
  SetAddress((1<<24) + board_id);

  for(int i=0; i<4; i++){
    fCoreCharge[i] = raw.core_e[i];
  }
  fBaseline = raw.baseline;
  fPrestep = raw.prestep;
  fPoststep = raw.poststep;

  fNumberOfInteractions = raw.num;
  fPad = raw.pad;
  for(int i=0; i<fNumberOfInteractions; i++) {
    try {
      interaction_point pnt;
      pnt.fSeg = raw.intpts[i].seg;
      pnt.fX   = raw.intpts[i].x;
      pnt.fY   = raw.intpts[i].y;
      pnt.fZ   = raw.intpts[i].z;
      pnt.fFrac   = raw.intpts[i].e;
      pnt.fEng = raw.intpts[i].seg_ener;
      fSegments.push_back(pnt);
    } catch(...) {
      std::cout << "in try catch block!" << std::endl;
    }

  }
  fTOffset = raw.intpts[MAX_INTPTS-1].z;
  std::sort(fSegments.begin(),fSegments.end());
  //  Print("all");
}

/*******************************************************************************/
/* Returns position vector based on interaction point **************************/
/*******************************************************************************/
TVector3 TGretinaHit::GetIntPosition(unsigned int i) const {
  if(i<fSegments.size()){
    double xoffset = GValue::Value("GRETINA_X_OFFSET");
    if(std::isnan(xoffset))
      xoffset=0.00;
    double yoffset = GValue::Value("GRETINA_Y_OFFSET");
    if(std::isnan(yoffset))
      yoffset=0.00;
    double zoffset = GValue::Value("GRETINA_Z_OFFSET");
    if(std::isnan(zoffset))
      zoffset=0.00;
    TVector3 v = TGretina::CrystalToGlobal(fCrystalId,fSegments.at(i).fX + xoffset,
        fSegments.at(i).fY + yoffset,
        fSegments.at(i).fZ + zoffset);
    return v;
  } else {
    return TDetectorHit::BeamUnitVec;
  }
}
/*******************************************************************************/
/* Returns vector for loacl position within the crystal ************************/
/*******************************************************************************/
TVector3 TGretinaHit::GetLocalPosition(unsigned int i) const {
  if(i<fSegments.size()){
    return TVector3(fSegments.at(i).fX,
        fSegments.at(i).fY,
        fSegments.at(i).fZ);
  } else {
    return TVector3(0,0,1);
  }
}

/*******************************************************************************/
/* Returns doppler corrected energies based on highest energy interaction ******/
/* point, particle vectors and beam beta. Can select gain range otherwise ******/
/* uses mode2 energy ***********************************************************/
/*******************************************************************************/
double TGretinaHit::GetDoppler(double beta,const TVector3 *vec, int EngRange) const {
  if(Size()<1)
    return 0.0;
  if(vec==0) {
    vec = &BeamUnitVec;
  }
  double tmp = 0.0;
  double gamma = 1/(sqrt(1-pow(beta,2)));

  TVector3 gret_pos = GetPosition();
  double xoffset = GValue::Value("TARGET_X_OFFSET");
  if(std::isnan(xoffset)) xoffset=0.00;
  double yoffset = GValue::Value("TARGET_Y_OFFSET");
  if(std::isnan(yoffset)) yoffset=0.00;
  double zoffset = GValue::Value("TARGET_Z_OFFSET");
  if(std::isnan(zoffset)) zoffset=0.00;

  gret_pos.SetX(gret_pos.X() - xoffset);
  gret_pos.SetY(gret_pos.Y() - yoffset);
  gret_pos.SetZ(gret_pos.Z() - zoffset);

  if(EngRange>0) tmp = GetCoreEnergy(EngRange)*gamma *(1 - beta*TMath::Cos(gret_pos.Angle(*vec)));
  else tmp = fCoreEnergy*gamma *(1 - beta*TMath::Cos(gret_pos.Angle(*vec)));
  return tmp;
}

/*******************************************************************************/
/* Returns doppler corrected energies based on highest energy interaction ******/
/* point, particle vectors and beam beta. Corrects for non-dispersive position */
/* YTA, calculated using TS800 class. Can select gain range otherwise uses *****/
/* mode2 energy ****************************************************************/
/*******************************************************************************/
double TGretinaHit::GetDopplerYta(double beta, double yta, const TVector3 *vec, int EngRange) const {
  if(Size()<1)
    return 0.0;
  if(vec==0) {
    vec = &BeamUnitVec;
  }
  //Target offsets determine new reference point in lab frame
  double xoffset = GValue::Value("TARGET_X_OFFSET");
  if(std::isnan(xoffset)) xoffset=0.00;
  double yoffset = GValue::Value("TARGET_Y_OFFSET");
  if(std::isnan(yoffset)) yoffset=0.00;
  double zoffset = GValue::Value("TARGET_Z_OFFSET");
  if(std::isnan(zoffset)) zoffset=0.00;
  return GetDopplerYta(beta, yta, xoffset, yoffset, zoffset, vec, EngRange);
}

double TGretinaHit::GetDopplerYta(double beta, double yta, double xoffset, double yoffset, double zoffset, const TVector3 *vec, int EngRange) const {
  if(Size()<1)
    return 0.0;
  if(vec==0) {
    vec = &BeamUnitVec;
  }
  double tmp = 0.0;
  double gamma = 1./(sqrt(1.-pow(beta,2.)));
  TVector3 gret_pos = GetPosition();

  //Target offsets determine new reference point in lab frame
  gret_pos.SetX(gret_pos.X() - xoffset);
  gret_pos.SetY(gret_pos.Y() - (yoffset - yta));
  gret_pos.SetZ(gret_pos.Z() - zoffset);
  if(EngRange>0) tmp = GetCoreEnergy(EngRange)*gamma *(1 - beta*TMath::Cos(gret_pos.Angle(*vec)));
  else tmp = fCoreEnergy*gamma *(1 - beta*TMath::Cos(gret_pos.Angle(*vec)));
  return tmp;
}

/*******************************************************************************/
/* Legacy - No Longer used and may be removed **********************************/
/* Used by old Addback code to handle interaction points ***********************/
/*******************************************************************************/
void TGretinaHit::Add(const TGretinaHit& rhs) {

  // qStash all interaction points
  std::set<interaction_point> ips;
  // Copy other information to self if needed
  double my_core_energy = fCoreEnergy;
  if(fCoreEnergy < rhs.fCoreEnergy) {
    for(unsigned int i=0; i<rhs.fSegments.size(); i++){
      ips.insert(rhs.fSegments[i]);
    }

    for(unsigned int i=0; i<fSegments.size(); i++){
      ips.insert(fSegments[i]);
    }
    rhs.Copy(*this);
    fCoreEnergy += my_core_energy;
  } else {
    for(unsigned int i=0; i<fSegments.size(); i++){
      ips.insert(fSegments[i]);
    }
    for(unsigned int i=0; i<rhs.fSegments.size(); i++){
      ips.insert(rhs.fSegments[i]);
    }
    fCoreEnergy += rhs.fCoreEnergy;
  }

  // Fill all interaction points
  fNumberOfInteractions = 0;
  fSegments.clear();
  for(auto& point : ips){
    if(fNumberOfInteractions >= MAXHPGESEGMENTS){
      break;
    }
    fSegments.push_back(point);
    fNumberOfInteractions++;
  }
}

/*******************************************************************************/
/* Used by Addback code to handle interaction points ***************************/
/*******************************************************************************/
void TGretinaHit::NNAdd(const TGretinaHit& rhs) {
  //copy original hit into singles
  if (!fSetFirstSingles){
    TGretinaHit myCopy(*this);
    fSingles.push_back(myCopy);
    fSetFirstSingles = true;
  }
  fCoreEnergy += rhs.fCoreEnergy;
  fSingles.push_back(rhs);

  // S.G. - Does it make sense to add interactions points from other crystal?
  // Fill all interaction points
  for(unsigned int i=0; i<rhs.fSegments.size(); i++){
    if(fNumberOfInteractions >= MAXHPGESEGMENTS){
      break;
    }
    fSegments.push_back(rhs.fSegments[i]);
    fNumberOfInteractions++;
  }
}

/*******************************************************************************/
/* Returns position vector based on final interaction point ********************/
/*******************************************************************************/
TVector3 TGretinaHit::GetLastPosition() const {
  if(fSegments.size()<1)
    return TDetectorHit::BeamUnitVec;
  return GetIntPosition(fSegments.size()-1);
}

/*******************************************************************************/
/* Print function **************************************************************/
/*******************************************************************************/
void TGretinaHit::Print(Option_t *opt) const {

  std::cout << "TGretinaHit:" <<  std::endl;
  //std::cout << "\tAddress:        \t0x" << std::hex << fAddress << std::dec << std::endl;
  printf("\tAddress:        \t0x%08x\n",Address());
  std::cout << "\tHole:           \t" << GetHoleNumber()               << std::endl;
  std::cout << "\tCrystalNum      \t" << GetCrystalNumber()            << std::endl;
  std::cout << "\tCrystalId:      \t" << GetCrystalId()                << std::endl;

  std::cout << "\tLocal Timestamp:\t" << Timestamp()                   << std::endl;
  std::cout << "\tCorrected time: \t" << GetTime()                     << std::endl;
  std::cout << "\tDecomp Energy:  \t" << fCoreEnergy                   << std::endl;
  if(!strcmp(opt,"all")) {
    for(int i=0;i<4;i++)
      std::cout << "\tCharge[" << i << "]:       \t" << fCoreCharge[i] << std::endl;
  }
  std::cout << "\tErrorCode:       \t" << fPad                         << std::endl;
  std::cout << "\tInteractions:    \t" << fSegments.size()             << std::endl;

  if(!strcmp(opt,"all")) {
    for(int i=0;i<fNumberOfInteractions;i++) {
      printf("\t\t");
      fSegments.at(i).Print();
    }
  }
  std::cout << "------------------------------"  << std::endl;

}

/*******************************************************************************/
/* Clears hit ******************************************************************/
/*******************************************************************************/
void TGretinaHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
  fWalkCorrection = sqrt(-1);
  fCrystalId      = -1;
  fCoreEnergy     = sqrt(-1);
  fCoreCharge[0]  = -1;
  fCoreCharge[1]  = -1;
  fCoreCharge[2]  = -1;
  fCoreCharge[3]  = -1;
  fTOffset = sqrt(-1);
  fPad  = 0;
  fNumberOfInteractions = 0;
  fSegments.clear();
}


/*******************************************************************************/
/* Reduced fSegment size *******************************************************/
/* 0 - drop multiple ident int pnts ********************************************/
/* 1 - make into wedge data ***************************************************/
/*******************************************************************************/
void TGretinaHit::TrimSegments(int type) {
  if(type==0) {
    std::set<interaction_point,intpnt_compare> pset;
    for(auto x=fSegments.begin();x!=fSegments.end();x++) {
      pset.insert(*x);
    }
    fSegments.clear();
    for(auto x=pset.begin();x!=pset.end();x++) {
      fSegments.push_back(*x);
    }
    std::sort(fSegments.begin(),fSegments.end());
    fNumberOfInteractions = fSegments.size();
  } else if (type==1) {
    std::set<interaction_point,intpnt_compare_wedge> pset;
    for(auto x=fSegments.begin();x!=fSegments.end();x++) {
      pset.insert(*x);
    }
    fSegments.clear();
    for(auto x=pset.begin();x!=pset.end();x++) {
      fSegments.push_back(*x);
      fSegments.back().fSeg = (fSegments.back().fSeg%6);
    }
    std::sort(fSegments.begin(),fSegments.end());
    fNumberOfInteractions = fSegments.size();
  }
}

/*******************************************************************************/
/* Returns position vector to first interaction point **************************/
/* For simulations the position can be smeared with the functions **************/
/* SetSmear(true/false) and SetSmearWidth(width) width should be in cm *********/
/*******************************************************************************/
TVector3 TGretinaHit::GetPosition() const {
  TVector3 gret_pos = GetIntPosition(0);
  if(!fSmear) {
  } else {
    gret_pos.SetX(gret_pos.X() - gRandom->Gaus(0,fSmearWidth));
    gret_pos.SetY(gret_pos.Y() - gRandom->Gaus(0,fSmearWidth));
    gret_pos.SetZ(gret_pos.Z() - gRandom->Gaus(0,fSmearWidth));
  }
  return gret_pos;
}

/*******************************************************************************/
/* Gets Detector Number - Corresponds to Real Detetor number (Q1..Q12 etc.) ****/
/* Requires Calibration file ***************************************************/
/*******************************************************************************/
int TGretinaHit::GetDetnum() const {
  TChannel* chan = TChannel::GetChannel(fAddress);
  int output = -1;
  if(chan && fAddress!=-1){
    output = chan->GetArrayPosition();
  } else {
    output = -1;
  }

  if(output == -1 && chan){
    // std::cout << "Chan with det=-1: " << chan->GetName() << std::endl;
    // std::cout << "address: " << fAddress << std::endl;
  }
  return output;
}

