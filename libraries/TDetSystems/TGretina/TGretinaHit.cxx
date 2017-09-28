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

void TGretinaHit::Copy(TObject &rhs) const {
  TDetectorHit::Copy(rhs);
  ((TGretinaHit&)rhs).fWalkCorrection = fWalkCorrection;
  ((TGretinaHit&)rhs).fPad            = fPad;
  ((TGretinaHit&)rhs).fTOffset           = fTOffset;
  ((TGretinaHit&)rhs).fCrystalId      = fCrystalId;
  ((TGretinaHit&)rhs).fCoreEnergy     = fCoreEnergy;
  ((TGretinaHit&)rhs).fCoreCharge[0]  = fCoreCharge[0];
  ((TGretinaHit&)rhs).fCoreCharge[1]  = fCoreCharge[1];
  ((TGretinaHit&)rhs).fCoreCharge[2]  = fCoreCharge[2];
  ((TGretinaHit&)rhs).fCoreCharge[3]  = fCoreCharge[3];
  ((TGretinaHit&)rhs).fNumberOfInteractions = fNumberOfInteractions;
  ((TGretinaHit&)rhs).fSegments       = fSegments;
}

Float_t TGretinaHit::GetCoreEnergy(int i) const {
  float charge = (float)GetCoreCharge(i) + gRandom->Uniform();
  //board_id=; //card  number : 0x0030  information not available here.
  TChannel *channel = TChannel::GetChannel(Address()+(i<<4));
  //printf("GetAddress() + i = 0x%08x\n",GetAddress()+i); 
  if(!channel)
    return charge;
  return channel->CalEnergy(charge);
}

const char *TGretinaHit::GetName() const {
  TChannel *channel = TChannel::GetChannel(Address());
  if(channel) 
    return channel->GetName();
  std::string name = "";
  return name.c_str();
}


TVector3 TGretinaHit::GetCrystalPosition()  const { 
  return TGretina::GetCrystalPosition(fCrystalId); 
}

//TVector3 TGretinaHit::GetSegmentPosition(int i)  const { 
//  if(fSegmentNumber.size()) 
//    return TGretina::GetSegmentPosition(fCrystalId,fSegmentNumber.at(0));
//  else
//    return TGretina::GetSegmentPosition(fCrystalId,0);
//}



void TGretinaHit::BuildFrom(TSmartBuffer& buf){
  const TRawEvent::GEBBankType1& raw = *(const TRawEvent::GEBBankType1*)buf.GetData();

  //std::cout << "GretinaHit: " << raw << std::endl;
  //std::cout << raw << std::endl;
  Clear();

  SetTimestamp(raw.timestamp);
  fWalkCorrection = raw.t0;
  fCrystalId = raw.crystal_id;
  fCoreEnergy = raw.tot_e;

  //fAddress = (1<<24) + (fCrystalId<<16);
  //fAddress = (1<<24) + ( raw.board_id );
  int board_id = ((fCrystalId/4) << 8) ;  //hole  number : 0x1f00
  //    board_id =                       ;  //card  number : 0x0030  information not available here.
  board_id += ((fCrystalId%4) << 6) ;  //x-tal number : 0x00c0
  board_id += 9;                       //chan  number : 0x000f  information not available here(assume core).
  SetAddress((1<<24) + board_id);


  for(int i=0; i<4; i++){
    fCoreCharge[i] = raw.core_e[i];
  }

  fNumberOfInteractions = raw.num;
  fPad = raw.pad;

  //std::cout<< "pad\t" << fPad << "\tints\t" << fNumberOfInteractions << std::endl; 
  for(int i=0; i<fNumberOfInteractions; i++) {
    try {
      interaction_point pnt; 
      pnt.fSeg = raw.intpts[i].seg;
      pnt.fX   = raw.intpts[i].x;
      pnt.fY   = raw.intpts[i].y;
      pnt.fZ   = raw.intpts[i].z;
      pnt.fEng = raw.intpts[i].e;
      fSegments.push_back(pnt);
    } catch(...) {
      std::cout << "in try catch block!" << std::endl;
    }

  }
  fTOffset = raw.intpts[MAX_INTPTS-1].z;
  //std::cout << "[13].z :  " << raw.intpts[13].z << std::endl;
  //std::cout << "[14].z :  " << raw.intpts[14].z << std::endl;
  //std::cout << "[15].z :  " << raw.intpts[15].z << std::endl;
  //std::cout << "fTOffset :  " << fTOffset             << std::endl;

  std::sort(fSegments.begin(),fSegments.end());
  //  Print("all");

}

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

TVector3 TGretinaHit::GetLocalPosition(unsigned int i) const {
  if(i<fSegments.size()){
    return TVector3(fSegments.at(i).fX,
        fSegments.at(i).fY,
        fSegments.at(i).fZ);
  } else {
    return TVector3(0,0,1);
  }
}


double TGretinaHit::GetDopplerYta(double beta, double yta, const TVector3 *vec, int EngRange) const {
  if(Size()<1)
    return 0.0;
  if(vec==0) {
    vec = &BeamUnitVec;
  }
  double tmp = 0.0;
  double gamma = 1./(sqrt(1.-pow(beta,2.)));
  TVector3 gret_pos = GetPosition();
  //Target offsets determine new reference point in lab frame
  double xoffset = GValue::Value("TARGET_X_OFFSET");
  if(std::isnan(xoffset))
    xoffset=0.00;
  double yoffset = GValue::Value("TARGET_Y_OFFSET");
  if(std::isnan(yoffset))
    yoffset=0.00;
  double zoffset = GValue::Value("TARGET_Z_OFFSET");
  if(std::isnan(zoffset))
      zoffset=0.00; 


  gret_pos.SetX(gret_pos.X() - xoffset);
  gret_pos.SetY(gret_pos.Y() - (yoffset - yta));
  gret_pos.SetZ(gret_pos.Z() - zoffset);
  if(EngRange>0) 
    tmp = GetCoreEnergy(EngRange)*gamma *(1 - beta*TMath::Cos(gret_pos.Angle(*vec)));
  else
    tmp = fCoreEnergy*gamma *(1 - beta*TMath::Cos(gret_pos.Angle(*vec)));
  return tmp;
}
/*
   double TGretinaHit::GetDoppler_dB(double beta, const TVector3 *vec,double Dta){
   if(Size()<1)
   return 0.0;
   if(vec==0) {
   vec = &BeamUnitVec;
   }
   double tmp = 0.0;
   double gamma = 1.0/(sqrt(1.0-pow(beta,2.0)));
// Do beta correction here.
double dp_p = gamma/(1.0+gamma)*Dta;
beta *= (1.0+dp_p/(gamma*gamma));
double TheGamma = 1.0/TMath::Sqrt(1.0-beta*beta);
tmp = fCoreEnergy*TheGamma *(1.0 - beta*TMath::Cos(GetPosition().Angle(*vec)));
return tmp;
}
*/


/*
   double TGretinaHit::GetDoppler(const TS800 *s800,bool doDTAcorr,int EngRange) {
   if(!s800 || Size()<1)
   return 0.0;
   double beta  = GValue::Value("BETA");
   if(std::isnan(beta))
   return 0.0;
   double gata =  GValue::Value("GRETINA_ATA_OFFSET");
   if(std::isnan(gata))
   gata = 0.0;
   else 
   gata = gata*TMath::DegToRad();
   double gbta =  GValue::Value("GRETINA_BTA_OFFSET");
   if(std::isnan(gbta))
   gbta = 0.0;
   else 
   gbta = gata*TMath::DegToRad();
   if(doDTAcorr){
   double gamma = 1.0/(sqrt(1.-beta*beta));
   double dp_p = gamma/(1.+gamma) * s800->GetDta();
   beta *=(1.+dp_p/(gamma*gamma));
   }
   TVector3 track = s800->Track(gata,gbta);  //(TMath::Sin(s800->GetAta()),-TMath::Sin(s800->GetBta()),1);
   if(EngRange>-1)
   return GetDoppler(EngRange,beta,&track);
   return GetDoppler(beta,&track);
   }
   */


//TVector3 TGretinaHit::GetCrystalPosition(int i) const {
//  std::cerr << __PRETTY_FUNCTION__ << " NOT IMPLEMENTED YET" << std::endl;
//}

/*
   void TGretinaHit::SortHits(){
// sets are sorted, so this will sort all properties together.
//
// !! When multiple interactions are assigned to a single
//    segment, the first interaction is currenlty assigned
//    to that segment!
//

std::vector<interaction_point> ips;
for(int i=0; i<fNumberOfInteractions; i++){
ips.push_back(interaction_point(fSegmentNumber[i],
fGlobalInteractionPosition[i],
fLocalInteractionPosition[i],
fInteractionEnergy[i],
fInteractionFraction[i]));
}
//printf("ips.size() == %i\n",ips.size());
// Fill all interaction points
//
fSegmentNumber.clear();
fGlobalInteractionPosition.clear();
fLocalInteractionPosition.clear();
fInteractionEnergy.clear();
fInteractionFraction.clear();
//
fNumberOfInteractions = 0;

std::sort(ips.begin(), ips.end());

for(auto& point : ips){
if(fNumberOfInteractions >= MAXHPGESEGMENTS){
break;
}
fSegmentNumber.push_back(point.segnum);
fGlobalInteractionPosition.push_back(point.pos);
fLocalInteractionPosition.push_back(point.local_pos);
fInteractionEnergy.push_back(point.energy);
fInteractionFraction.push_back(point.energy_fraction);
fNumberOfInteractions++;
}

//Print("all");
// Because they are now sorted
fFirstInteraction = 0;
if(fNumberOfInteractions>1)
fSecondInteraction = 1;
}
*/

// TODO: Handle interactions points better
//       Right now, the "first interaction point" is the one with the highest energy,
//       and the "second" is the one with the second highest energy.
//       First and second may be assigned across crystal boundaries.
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
/*
   TGretinaHit& TGretinaHit::operator+=(const TGretinaHit& rhs) {
   AddToSelf(rhs);
   return *this;
   }

   TGretinaHiti& TGretinaHit::operator+(TGretinaHit lhs,const TGretinaHit& rhs) {

   lhs += rhs;
   return lhs;
   }
   */


TVector3 TGretinaHit::GetLastPosition() const {
  if(fSegments.size()<1)
    return TDetectorHit::BeamUnitVec;
  return GetIntPosition(fSegments.size()-1);
}


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


void TGretinaHit::TrimSegments(int type) {
  // 0: drop multiple ident int pnts.  
  // 1: make into wedge "data"
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

