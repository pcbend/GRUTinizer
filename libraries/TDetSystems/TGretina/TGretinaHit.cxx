#include "TGretinaHit.h"

#include <cmath>
#include <set>

#include "TGretina.h"

struct interaction_point {
  interaction_point(int segnum, TVector3 pos,TVector3 loc,float energy)
    : segnum(segnum), pos(pos),local_pos(loc), energy(energy) { }

  int segnum;
  TVector3 pos;
  TVector3 local_pos;
  float energy;
  float energy_fraction;

  bool operator<(const interaction_point& other) const {
    return energy > other.energy;
  }
};

TGretinaHit::TGretinaHit(){ Clear(); }

TGretinaHit::~TGretinaHit(){ }

void TGretinaHit::Copy(TObject &rhs) const {
  TDetectorHit::Copy(rhs);
  ((TGretinaHit&)rhs).fTimeStamp      = fTimeStamp;
  ((TGretinaHit&)rhs).fWalkCorrection = fWalkCorrection;
  ((TGretinaHit&)rhs).fAddress        = fAddress;
  ((TGretinaHit&)rhs).fPad            = fPad;
  ((TGretinaHit&)rhs).fCrystalId      = fCrystalId;
  ((TGretinaHit&)rhs).fCoreEnergy     = fCoreEnergy;
  ((TGretinaHit&)rhs).fCoreCharge[0]  = fCoreCharge[0];
  ((TGretinaHit&)rhs).fCoreCharge[1]  = fCoreCharge[1];
  ((TGretinaHit&)rhs).fCoreCharge[2]  = fCoreCharge[2];
  ((TGretinaHit&)rhs).fCoreCharge[3]  = fCoreCharge[3];
  ((TGretinaHit&)rhs).fFirstInteraction  = fFirstInteraction;
  ((TGretinaHit&)rhs).fSecondInteraction = fSecondInteraction;
  ((TGretinaHit&)rhs).fNumberOfInteractions = fNumberOfInteractions;
  //for(int i=0;i<fNumberOfInteractions;i++) {
    ((TGretinaHit&)rhs).fSegmentNumber = fSegmentNumber;
    ((TGretinaHit&)rhs).fGlobalInteractionPosition = fGlobalInteractionPosition;
    ((TGretinaHit&)rhs).fLocalInteractionPosition  = fLocalInteractionPosition;
    ((TGretinaHit&)rhs).fInteractionEnergy = fInteractionEnergy;
    ((TGretinaHit&)rhs).fInteractionFraction = fInteractionFraction;
  //}
}


void TGretinaHit::BuildFrom(const TRawEvent::GEBBankType1& raw){
  //SetAddress(kDetectorSystems::GRETINA, 1, raw.crystal_id);
  //                     HOLE          CRYSTAL     SEGMENT
  //SetAddress(kDetectorSystems::GRETINA, 1, raw.crystal_id);
  
  //std::cout << "-------------------------------------------------" << std::endl;
  //std::cout << raw << std::endl;
  Clear();

  fTimeStamp = raw.timestamp;
  fWalkCorrection = raw.t0;
  fCrystalId = raw.crystal_id;
  fCoreEnergy = raw.tot_e;

  fAddress = (1<<24) + (fCrystalId<<16);

  for(int i=0; i<4; i++){
    fCoreCharge[i] = raw.core_e[i];
  }

  fNumberOfInteractions = raw.num;
  fPad = raw.pad;

  float first_interaction_value  = sqrt(-1);
  float second_interaction_value = sqrt(-1);

  for(int i=0; i<fNumberOfInteractions; i++) {
    //fSegmentNumber[i] = 36*fCrystalId + raw.intpts[i].seg;
    //printf("[%02i] : seg[%02i] = %.02f\n",i,raw.intpts[i].seg,raw.intpts[i].seg_ener); fflush(stdout);
    fSegmentNumber.push_back(raw.intpts[i].seg);
    fGlobalInteractionPosition.push_back(TGretina::CrystalToGlobal(fCrystalId,
                                                              raw.intpts[i].x,
                                                              raw.intpts[i].y,
                                                              raw.intpts[i].z));
    fLocalInteractionPosition.push_back(TVector3(raw.intpts[i].x,
                                                 raw.intpts[i].y,
                                                 raw.intpts[i].z));

    float seg_ener = raw.intpts[i].seg_ener;
    float seg_frac = raw.intpts[i].e;
    fInteractionEnergy.push_back(seg_ener);
    fInteractionFraction.push_back(seg_frac);

    if(seg_ener >= first_interaction_value){
      second_interaction_value = first_interaction_value;
      first_interaction_value = seg_ener;
      fSecondInteraction = fFirstInteraction;
      fFirstInteraction = i;
    } else if(seg_ener > second_interaction_value){
      fSecondInteraction = i;
      second_interaction_value = seg_ener;
    }
  }
  //Print("all");
  SortHits();

}

TVector3 TGretinaHit::GetInteractionPosition(int i) const {
  if(i>=0 && i<fNumberOfInteractions){
    return fGlobalInteractionPosition[i];
  } else {
    return TVector3(0,0,1);
  }
}

TVector3 TGretinaHit::GetLocalPosition(int i) const {
  if(i>=0 && i<fNumberOfInteractions){
    return fLocalInteractionPosition[i];
  } else {
    return TVector3(0,0,1);
  }
}

double TGretinaHit::GetDoppler_dB(double beta, const TVector3 *vec,double Dta){
  if(Size()<1)
    return 0.0;
  bool madevec = false;
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




//TVector3 TGretinaHit::GetCrystalPosition(int i) const {
//  std::cerr << __PRETTY_FUNCTION__ << " NOT IMPLEMENTED YET" << std::endl;
//}

bool TGretinaHit::CheckAddback(const TGretinaHit& rhs) const {
  if(fNumberOfInteractions && rhs.fNumberOfInteractions)
    return false;

  TVector3 dist= GetPosition() - rhs.GetPosition();
  double dtime = std::abs(GetTime() - rhs.GetTime());

  return ((dist.Mag()<250.0) && (dtime<20.0));
}

void TGretinaHit::SortHits(){
  // sets are sorted, so this will sort all properties together.
  std::set<interaction_point> ips;
  for(int i=0; i<fNumberOfInteractions; i++){
    ips.insert(interaction_point(fSegmentNumber[i],
                                 fGlobalInteractionPosition[i],
                                 fLocalInteractionPosition[i],
                                 fInteractionEnergy[i]));
  }
  printf("ips.size() == %i\n",ips.size());
  // Fill all interaction points
  fNumberOfInteractions = 0;
  for(auto& point : ips){
    if(fNumberOfInteractions >= MAXHPGESEGMENTS){
      break;
    }

    fSegmentNumber[fNumberOfInteractions] = point.segnum;
    fGlobalInteractionPosition[fNumberOfInteractions] = point.pos;
    fLocalInteractionPosition[fNumberOfInteractions] = point.local_pos;
    fInteractionEnergy[fNumberOfInteractions] = point.energy;
    fInteractionFraction[fNumberOfInteractions] = point.energy_fraction;
    fNumberOfInteractions++;
  }
  //Print("all");
  // Because they are now sorted
  fFirstInteraction = 0;
  fSecondInteraction = 1;
}

// TODO: Handle interactions points better
//       Right now, the "first interaction point" is the one with the highest energy,
//       and the "second" is the one with the second highest energy.
//       First and second may be assigned across crystal boundaries.
void TGretinaHit::AddToSelf(const TGretinaHit& rhs, double& max_energy) {

  // Stash all interaction points
  std::set<interaction_point> ips;
  for(int i=0; i<fNumberOfInteractions; i++){
    ips.insert(interaction_point(fSegmentNumber[i],
                                 fGlobalInteractionPosition[i],
                                 fLocalInteractionPosition[i],
                                 fInteractionEnergy[i]));
  }
  for(int i=0; i<rhs.fNumberOfInteractions; i++){
    ips.insert(interaction_point(fSegmentNumber[i],
                                 fGlobalInteractionPosition[i],
                                 fLocalInteractionPosition[i],
                                 fInteractionEnergy[i]));
  }

  // Copy other information to self if needed
  double my_core_energy = fCoreEnergy;
  if(fCoreEnergy < rhs.fCoreEnergy) {
    rhs.Copy(*this);
    max_energy = fCoreEnergy;
    fCoreEnergy += my_core_energy;
  } else {
    fCoreEnergy += rhs.fCoreEnergy;
  }

  // Fill all interaction points
  fNumberOfInteractions = 0;
  fSegmentNumber.clear();
  fGlobalInteractionPosition.clear();
  fLocalInteractionPosition.clear();
  fInteractionEnergy.clear();
  fInteractionFraction.clear();
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

  // Because they are now sorted
  fFirstInteraction = 0;
  fSecondInteraction = 1;
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












TVector3 TGretinaHit::GetFirstIntPosition() const {
   if(GetFirstIntPoint()>-1)
     return GetInteractionPosition(GetFirstIntPoint());
   return TDetectorHit::BeamUnitVec;
}

TVector3 TGretinaHit::GetSecondIntPosition() const {
   if(GetSecondIntPoint()>-1)
     return GetInteractionPosition(GetSecondIntPoint());
   return TDetectorHit::BeamUnitVec;
}

void TGretinaHit::Print(Option_t *opt) const {

  std::cout << "TGretinaHit:" <<  std::endl;
  //std::cout << "\tAddress:        \t0x" << std::hex << fAddress << std::dec << std::endl;
  printf("\tAddress:        \t0x%08x\n",fAddress);
  std::cout << "\tHole:           \t" << GetHoleNumber()               << std::endl;
  std::cout << "\tCrystalNum      \t" << GetCrystalNumber()            << std::endl;
  std::cout << "\tCrystalId:      \t" << GetCrystalId()                << std::endl;

  std::cout << "\tLocal Timestamp:\t" << fTimeStamp                   << std::endl;
  std::cout << "\tCorrected time: \t" << fTimeStamp - fWalkCorrection << std::endl;
  std::cout << "\tDecomp Energy:  \t" << fCoreEnergy                   << std::endl;
  if(!strcmp(opt,"all")) {
    for(int i=0;i<4;i++)
      std::cout << "\tCharge[" << i << "]:       \t" << fCoreCharge[i] << std::endl;
  }
  std::cout << "\tErrorCode:       \t" << fPad                         << std::endl;
  std::cout << "\tInteractions:    \t" << fNumberOfInteractions        << std::endl;
  std::cout << "\tFirst Int:       \t" << fFirstInteraction            << std::endl;
  std::cout << "\tSecond Int:      \t" << fSecondInteraction           << std::endl;

  if(!strcmp(opt,"all")) {
    for(int i=0;i<fNumberOfInteractions;i++) {
      printf("\t\tSeg[%02i]\tEng: % 4.2f / % 4.2f  \t(R,T,P) % 3.2f % 3.2f % 3.2f\n",fSegmentNumber[i],fInteractionEnergy[i],fInteractionFraction[i],
                                                                          fGlobalInteractionPosition[i].Mag(),
                                                                          fGlobalInteractionPosition[i].Theta()*TMath::RadToDeg(),
                                                                          fGlobalInteractionPosition[i].Phi()*TMath::RadToDeg() ); fflush(stdout);
      //std::cout << "\t\tSe[" << i << "]Seg Num:      \t" << fSegmentNumber[i]     << std::endl
      //          << "\t\t[" << i << "]Seg Eng:      \t" << fInteractionEnergy[i] << std::endl
      //                   << "\t\t[" << i << "]Seg Pos(R,T,P)\t" << fGlobalInteractionPosition[i].Mag()
      //                                                 << "  "
      //                                                 << fGlobalInteractionPosition[i].Theta()*TMath::RadToDeg()
      //                                                 << "  "
      //                                                 << fGlobalInteractionPosition[i].Phi()*TMath::RadToDeg()
      //                                                 << std::endl;
    }
  }
  std::cout << "------------------------------"  << std::endl;

}

void TGretinaHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);

  fTimeStamp      = -1;
  fWalkCorrection = sqrt(-1);

  fAddress        = -1;
  fCrystalId      = -1;
  fCoreEnergy     = sqrt(-1);
  fCoreCharge[0]  = -1;
  fCoreCharge[1]  = -1;
  fCoreCharge[2]  = -1;
  fCoreCharge[3]  = -1;

  fFirstInteraction  = -1;
  fSecondInteraction = -1;

  fPad = 0;

  fNumberOfInteractions = 0;

  fSegmentNumber.clear();
  fGlobalInteractionPosition.clear();
  fLocalInteractionPosition.clear();
  fInteractionEnergy.clear();
  fInteractionFraction.clear();
/*
  for(int i=0;i<MAXHPGESEGMENTS;i++) {
    fSegmentNumber[i]             = -1 ;
    fGlobalInteractionPosition[i].Clear(opt);
    fLocalInteractionPosition[i].Clear(opt);
    fInteractionEnergy[i]         = -1 ;
    fInteractionFraction[i]         = -1 ;
  
  }
  */
}
