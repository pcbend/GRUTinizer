#include "TGretinaHit.h"

#include <algorithm>
#include <cmath>
#include <set>

#include <TRandom.h>

#include "GValue.h"
#include "TGEBEvent.h"
#include "TGretina.h"
//#include "TS800.h"

/*
struct interaction_point {
  interaction_point(int segnum, TVector3 pos,TVector3 loc,float energy,float fraction,float assigned)
    : segnum(segnum), pos(pos),local_pos(loc), energy(energy), energy_fraction(fraction), energy_assigned(assigned) { }

  int segnum;
  TVector3 pos;
  TVector3 local_pos;
  float energy;
  float energy_fraction;
  float energy_assigned;

  bool operator<(const interaction_point& other) const {
    if (energy_assigned != other.energy_assigned) {
      return energy_assigned > other.energy_assigned;
    }

    //if(segnum == other.segnum) {
    //  return energy_fraction > other.energy_fraction;
    //}

    return segnum < other.segnum;
  }
};
*/

TInteractionPoint::TInteractionPoint(const TInteractionPoint &IP) {                       
  this->Copy(IP);
}

void TInteractionPoint::Copy(const TInteractionPoint &IP) {
  fSegNum      = IP.GetSegNum();
  fEng         = IP.GetPreampE();
  fDecompEng   = IP.GetDecompE();
  fAssignedEng = IP.GetAssignE();
  fOrder       = IP.GetOrder();
  fLPosition   = IP.GetLocalPosition();
}

TVector3 TInteractionPoint::GetPosition(int xtal) const { return TGretina::CrystalToGlobal(xtal,
                                                                            fLPosition.X(),
                                                                            fLPosition.Y(),
                                                                            fLPosition.Z()); }


void TInteractionPoint::Print(Option_t *opt) const { 
  TVector3 lv = GetLocalPosition();
  printf("seg[%02i] wedge[%i]    %.1f / %.1f   [ %.1f, %.1f. %.1f] \n",
          GetSegNum(),Wedge(),GetAssignE(),GetPreampE(),lv.X(),lv.Y(),lv.Z());
}


void TInteractionPoint::Clear(Option_t *opt) {
    fSegNum      = -1;
    fEng         = sqrt(-1);    
    fDecompEng   = sqrt(-1);    
    fAssignedEng = sqrt(-1);;   
    fOrder       = -1;          
    fLPosition.SetXYZ(0,0,1);
}







TGretinaHit::TGretinaHit(){ Clear(); }

TGretinaHit::~TGretinaHit(){ }

void TGretinaHit::Copy(TObject &rhs) const {
  TDetectorHit::Copy(rhs);
  //((TGretinaHit&)rhs).fTimeStamp      = fTimeStamp;
  ((TGretinaHit&)rhs).fT0 = fT0;
  ((TGretinaHit&)rhs).fAddress        = fAddress;
  ((TGretinaHit&)rhs).fPad            = fPad;
  ((TGretinaHit&)rhs).fCrystalId      = fCrystalId;
  ((TGretinaHit&)rhs).fCoreEnergy     = fCoreEnergy;
  ((TGretinaHit&)rhs).fCoreCharge[0]  = fCoreCharge[0];
  ((TGretinaHit&)rhs).fCoreCharge[1]  = fCoreCharge[1];
  ((TGretinaHit&)rhs).fCoreCharge[2]  = fCoreCharge[2];
  ((TGretinaHit&)rhs).fCoreCharge[3]  = fCoreCharge[3];
  
  ((TGretinaHit&)rhs).fNumberOfInteractions = fNumberOfInteractions;
  ((TGretinaHit&)rhs).fInteractions = fInteractions;
  /*
  ((TGretinaHit&)rhs).fFirstInteraction  = fFirstInteraction;
  ((TGretinaHit&)rhs).fSecondInteraction = fSecondInteraction;
  ((TGretinaHit&)rhs).fNumberOfInteractions = fNumberOfInteractions;
  ((TGretinaHit&)rhs).fSegmentNumber = fSegmentNumber;
  ((TGretinaHit&)rhs).fGlobalInteractionPosition = fGlobalInteractionPosition;
  ((TGretinaHit&)rhs).fLocalInteractionPosition  = fLocalInteractionPosition;
  ((TGretinaHit&)rhs).fInteractionEnergy = fInteractionEnergy;
  ((TGretinaHit&)rhs).fInteractionFraction = fInteractionFraction;
  */
}

Float_t TGretinaHit::GetCoreEnergy(int i) const {
  float charge = (float)GetCoreCharge(i) + gRandom->Uniform();
  //board_id=; //card  number : 0x0030  information not available here.
  TChannel *channel = TChannel::GetChannel(GetAddress()+(i<<4));
  //printf("GetAddress() + i = 0x%08x\n",GetAddress()+i); 
  if(!channel)
    return charge;
  return channel->CalEnergy(charge);
}

//const char *TGretinaHit::GetName() const {
//  std::string name = "";
//  TChannel *channel = TChannel::GetChannel(GetAddress());
//  if(!channel)
//    return name.c_str();
//  return channel->GetName();
//}


//TVector3 TGretinaHit::GetCrystalPosition()  const { return TGretina::GetCrystalPosition(fCrystalId); }
//TVector3 TGretinaHit::GetSegmentPosition()  const { if(fSegmentNumber.size()) 
//  return TGretina::GetSegmentPosition(fCrystalId,fSegmentNumber.at(0));
//  else
//    return TGretina::GetSegmentPosition(fCrystalId,0);
//}



void TGretinaHit::BuildFrom(TSmartBuffer& buf){
  const TRawEvent::GEBBankType1& raw = *(const TRawEvent::GEBBankType1*)buf.GetData();

  //std::cout << "GretinaHit: " << raw << std::endl;
  //SetAddress(kDetectorSystems::GRETINA, 1, raw.crystal_id);
  //                     HOLE          CRYSTAL     SEGMENT
  //SetAddress(kDetectorSystems::GRETINA, 1, raw.crystal_id);
  //std::cout << "-------------------------------------------------" << std::endl;
  //std::cout << raw << std::endl;
  Clear();
 
  SetTimestamp(raw.timestamp);
  fT0 = raw.t0;
  fCrystalId = raw.crystal_id;

  int board_id = ((fCrystalId/4) << 8) ;  //hole  number : 0x1f00
  //    board_id =                       ;  //card  number : 0x0030  information not available here.
  board_id += ((fCrystalId%4) << 6) ;  //x-tal number : 0x00c0
  board_id += 9;                       //chan  number : 0x000f  information not available here(assume core).
  fAddress = (1<<24) + board_id;

  TChannel *ch = TChannel::GetChannel(fAddress);
  if(!ch)
    fCoreEnergy = raw.tot_e;
  else
    fCoreEnergy = ch->CalEnergy(raw.tot_e);


  for(int i=0; i<4; i++){
    fCoreCharge[i] = raw.core_e[i];
  }

  fNumberOfInteractions = raw.num;
  fPad = raw.pad;

  //float first_interaction_value  = fFirstInteraction;//sqrt(-1);
  //float second_interaction_value = sqrt(-1);

  int order_counter=0;
  float decomp_sum =0.0;
  for(int i=0; i<fNumberOfInteractions; i++) {
    int   seg  = raw.intpts[i].seg;
    float eng  = raw.intpts[i].seg_ener;
    float frac = raw.intpts[i].e;
    
    TVector3 lpos = TVector3(raw.intpts[i].x,raw.intpts[i].y,raw.intpts[i].z);
    
    TInteractionPoint intpt(seg,eng,frac,lpos);
    
    intpt.SetOrder(order_counter++);
    decomp_sum += intpt.GetDecompE();
    fInteractions.push_back(intpt);
  }
  //now that the interactions are set, assign energy fractions based of of core - 
  for(int i=0;i<fNumberOfInteractions;i++) {
    //fInteractions[i].SetAssignE((fInteractions[i].GetDecompE()/decomp_sum)*GetCoreEnergy());
    double energy = (fInteractions[i].GetDecompE()/decomp_sum)*GetCoreEnergy();
    if(energy>0.0) {
      fInteractions[i].SetAssignE((fInteractions[i].GetDecompE()/decomp_sum)*GetCoreEnergy());
    } else {   // go home antiphoton, you're drunk.
      fInteractions[i].SetAssignE(0.00);
    }
  }
  
  SortInts();
}

void TInteractionPoint::Add(TInteractionPoint &other) {
  ///TODO:
  //  - check that they are the same segment. 
  //  - recontruct the XYZ position as a weighted sum.
  if(GetDecompE()>other.GetDecompE()) {
    SetAssignE(GetAssignE()+other.GetAssignE());
  } else {
    double eng = GetAssignE();
    Copy(other);
    SetAssignE(GetAssignE()+eng);
  }
}



//TVector3 TGretinaHit::GetInteractionPosition(int i) const {
//  if(i>=0 && i<fNumberOfInteractions){
//    return fGlobalInteractionPosition[i];
//  } else {
//    return TVector3(0,0,1);
//  }
//}

TVector3 TGretinaHit::GetPosition() const {
  if(fNumberOfInteractions>0){
    //double xoffset = GValue::Value("GRETINA_X_OFFSET");
    //if(std::isnan(xoffset))
    //  xoffset=0.00;
    //double yoffset = GValue::Value("GRETINA_Y_OFFSET");
    //if(std::isnan(yoffset))
    //  yoffset=0.00;
    //double zoffset = GValue::Value("GRETINA_Z_OFFSET");
    //if(std::isnan(zoffset))
    //  zoffset=0.00;
    //TVector3 offset(xoffset,yoffset,zoffset);
    return GetInteractionPoint(0).GetPosition(GetCrystalId());// + offset;
  } else {
    return TDetectorHit::BeamUnitVec;
  }
}

//TVector3 TGretinaHit::GetLocalPosition(int i) const {
//  if(i>=0 && i<fNumberOfInteractions){
//    return fLocalInteractionPosition[i];
//  } else {
//    return TVector3(0,0,1);
//  }
//}

double TGretinaHit::GetDoppler(double beta,const TVector3 *vec) {
  if(Size()<1)
    return 0.0;
  if(vec==0) {
    vec = &BeamUnitVec;
  }
  double tmp = 0.0;
  double gamma = 1/(sqrt(1-pow(beta,2)));
  tmp = fCoreEnergy*gamma *(1 - beta*TMath::Cos(GetPosition().Angle(*vec)));
  return tmp;
} 

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


int TGretinaHit::CleanInteractions() {
  std::map<int,TInteractionPoint> int_map;
  std::vector<TInteractionPoint>::iterator it;
  for(it=fInteractions.begin();it!=fInteractions.end();it++) {
    if(int_map.count(it->GetSegNum())>0) {
      int_map[it->GetSegNum()].Add(*it);
    } else {
      int_map[it->GetSegNum()]=*it;
    }
  }
  fInteractions.clear();
  std::map<int,TInteractionPoint>::iterator it2;
  for(it2=int_map.begin();it2!=int_map.end();it2++) {
    fInteractions.push_back(it2->second);
  }
  return fInteractions.size();
}

TVector3 TGretinaHit::GetCrystalPosition()  const { return TGretina::GetCrystalPosition(fCrystalId); }


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
          fInteractionFraction[i])
          GetInteractionPercentage(i));
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

void TGretinaHit::SortInts(){
  std::sort(fInteractions.begin(),fInteractions.end());
  // sets are sorted, so this will sort all properties together.
  //
  // !! When multiple interactions are assigned to a single
  //    segment, the first interaction is currenlty assigned
  //    to that segment!
  //
  /*
  std::vector<interaction_point> ips;
  for(int i=0; i<fNumberOfInteractions; i++){
    ips.push_back(interaction_point(fSegmentNumber[i],
          fGlobalInteractionPosition[i],
          fLocalInteractionPosition[i],
          fInteractionEnergy[i],
          fInteractionFraction[i],
          GetInteractionPercentage(i)));
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
  //fNumberOfInteractions = 0;

  std::sort(ips.begin(), ips.end());
  */
  /*&
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
  */
}











/*

// TODO: Handle interactions points better
//       Right now, the "first interaction point" is the one with the highest energy,
//       and the "second" is the one with the second highest energy.
//       First and second may be assigned across crystal boundaries.
void TGretinaHit::AddToSelf(const TGretinaHit& rhs) {

  // qStash all interaction points
  std::set<interaction_point> ips;
  for(int i=0; i<fNumberOfInteractions; i++){
    ips.insert(interaction_point(fSegmentNumber[i],
          fGlobalInteractionPosition[i],
          fLocalInteractionPosition[i],
          fInteractionEnergy[i]));
  }
  for(int i=0; i<rhs.fNumberOfInteractions; i++){
    ips.insert(interaction_point(rhs.fSegmentNumber[i],
          rhs.fGlobalInteractionPosition[i],
          rhs.fLocalInteractionPosition[i],
          rhs.fInteractionEnergy[i]));
  }

  // Copy other information to self if needed
  double my_core_energy = fCoreEnergy;
  if(fCoreEnergy < rhs.fCoreEnergy) {
    rhs.Copy(*this);
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
*/


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











/*
TVector3 TGretinaHit::GetFirstIntPosition() const {
  double xoffset = GValue::Value("GRETINA_X_OFFSET");
  if(std::isnan(xoffset))
    xoffset=0.00;
  double yoffset = GValue::Value("GRETINA_Y_OFFSET");
  if(std::isnan(yoffset))
    yoffset=0.00;
  double zoffset = GValue::Value("GRETINA_Z_OFFSET");
  if(std::isnan(zoffset))
    zoffset=0.00;

  TVector3 offset(xoffset,yoffset,zoffset);

  if(GetFirstIntPoint()>-1){
    TVector3 IntPos = GetInteractionPosition(GetFirstIntPoint());
    IntPos.SetX(IntPos.X()+xoffset);
    IntPos.SetY(IntPos.Y()+yoffset);
    IntPos.SetZ(IntPos.Z()+zoffset);

    return IntPos;
  }
  return TDetectorHit::BeamUnitVec;
}

TVector3 TGretinaHit::GetLastPosition() const {
  double xoffset = GValue::Value("GRETINA_X_OFFSET");
  if(std::isnan(xoffset))
    xoffset=0.00;
  double yoffset = GValue::Value("GRETINA_Y_OFFSET");
  if(std::isnan(yoffset))
    yoffset=0.00;
  double zoffset = GValue::Value("GRETINA_Z_OFFSET");
  if(std::isnan(zoffset))
    zoffset=0.00;

  TVector3 offset(xoffset,yoffset,zoffset);

  if(fNumberOfInteractions>0)
    return GetInteractionPosition(fNumberOfInteractions-1) + offset;
  return TDetectorHit::BeamUnitVec;

}
*/
/*
TVector3 TGretinaHit::GetSecondIntPosition() const {
  double xoffset = GValue::Value("GRETINA_X_OFFSET");
  if(std::isnan(xoffset))
    xoffset=0.00;
  double yoffset = GValue::Value("GRETINA_Y_OFFSET");
  if(std::isnan(xoffset))
    xoffset=0.00;
  double zoffset = GValue::Value("GRETINA_Z_OFFSET");
  if(std::isnan(xoffset))
    xoffset=0.00;
  TVector3 offset(xoffset,yoffset,zoffset);
  if(GetSecondIntPoint()>-1)
    return GetInteractionPosition(GetSecondIntPoint())+offset;
  return TDetectorHit::BeamUnitVec;
}
*/

void TGretinaHit::Print(Option_t *opt) const {

  std::cout << "TGretinaHit:" <<  std::endl;
  //std::cout << "\tAddress:        \t0x" << std::hex << fAddress << std::dec << std::endl;
  printf("\tAddress:        \t0x%08x\n",fAddress);
  std::cout << "\tHole:           \t" << GetHoleNumber()               << std::endl;
  std::cout << "\tCrystalNum      \t" << GetCrystalNumber()            << std::endl;
  std::cout << "\tCrystalId:      \t" << GetCrystalId()                << std::endl;

  std::cout << "\tLocal Timestamp:\t" << Timestamp()                   << std::endl;
  std::cout << "\tCorrected time: \t" << Timestamp() - fT0 << std::endl;
  std::cout << "\tDecomp Energy:  \t" << fCoreEnergy                   << std::endl;
  if(!strcmp(opt,"all")) {
    for(int i=0;i<4;i++)
      std::cout << "\tCharge[" << i << "]:       \t" << fCoreCharge[i] << std::endl;
  }
  std::cout << "\tErrorCode:       \t" << fPad                         << std::endl;
  std::cout << "\tInteractions:    \t" << fNumberOfInteractions        << std::endl;
  //std::cout << "\tFirst Int:       \t" << fFirstInteraction            << std::endl;
  //std::cout << "\tSecond Int:      \t" << fSecondInteraction           << std::endl;

  if(!strcmp(opt,"all")) {
    std::vector<TInteractionPoint>::const_iterator it;
    for(it=fInteractions.begin();it!=fInteractions.end();it++) {
      it->Print();
    
    //for(int i=0;i<fNumberOfInteractions;i++) {
      //printf("\t\tSeg[%02i]\tEng: % 4.2f / % 4.2f  \t(X,Y,Z) % 3.2f % 3.2f % 3.2f\n",
      //    fSegmentNumber[i],fInteractionEnergy[i],fInteractionFraction[i],
      //    fLocalInteractionPosition[i].X(),
      //    fLocalInteractionPosition[i].Y(),
      //    fLocalInteractionPosition[i].Z() ); fflush(stdout);
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

  fPad = 0;

  //fTimeStamp      = -1;
  fT0 = sqrt(-1);

  fAddress        = -1;
  fCrystalId      = -1;
  fCoreEnergy     = sqrt(-1);
  fCoreCharge[0]  = -1;
  fCoreCharge[1]  = -1;
  fCoreCharge[2]  = -1;
  fCoreCharge[3]  = -1;

  //fFirstInteraction  = -1;
  //fSecondInteraction = -1;

  fPad = 0;

  fNumberOfInteractions = 0;

  fInteractions.clear();

  //fSegmentNumber.clear();
  //fGlobalInteractionPosition.clear();
  //fLocalInteractionPosition.clear();
  //fInteractionEnergy.clear();
  //fInteractionFraction.clear();
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

Int_t TGretinaHit::Compare(const TObject *obj) const { 
  TGretinaHit *other = (TGretinaHit*)obj;
  if(this->GetCoreEnergy()>other->GetCoreEnergy())
    return -1;
  else if(this->GetCoreEnergy()<other->GetCoreEnergy())
    return 1;  //sort largest to smallest.
  return 0;
}

/*
Float_t TGretinaHit::GetInteractionPercentage(int i) const {
  //this function 'normalizes' the decomp output,
  //finds the percentage of the total_energy from the core assigne to the interaction
  //multiples this percentage byt the CC energy.
  float sum=0.0;
  for(int x=0;x<NumberOfInteractions();x++) {
    sum += GetInteractionFrac(x); // this is the amount decomp assigned to the value!
  }
  return (GetInteractionFrac(i)/sum)*fCoreEnergy;
}
*/


