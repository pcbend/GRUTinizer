
#include <TGraph.h>
#include <TFitResult.h>

#include "TCluster.h"

TClusterPoint::TClusterPoint() { }

TClusterPoint::~TClusterPoint() { }

TClusterPoint::TClusterPoint(TGretinaHit &hit, TInteractionPoint &ip) {
  this->Copy(ip);
  fXtalId      = hit.GetCrystalId();
  fXtalAddress = hit.Address(); 
  fTimestamp   = hit.Timestamp();
  fT0          = hit.GetT0();
  fPad         = hit.GetPad();
}

void TClusterPoint::Print(Option_t *opt) const { 
  opt=opt; //quiet the warnings.
  printf("xtal[%03i] %5.1f / %5.1f  seg[%02i]:[ %3.1f, %3.1f, %3.1f ] \n",
          GetXtal(), GetAssignE(), GetPreampE(), GetSegNum(),
          GetPosition().Mag(),GetPosition().Theta()*TMath::RadToDeg(),
          GetPosition().Phi()*TMath::RadToDeg());
}  

void TClusterPoint::Combine(const TClusterPoint &other) {
  if(other.GetEnergy()>GetEnergy()) {
     //fXtalId    = other.GetXtal();
     //SetSegNum(other.GetSegNum()); 
     fTimestamp = other.GetTimestamp();
     fT0        = other.fT0;
  }
  SetAssignE(GetAssignE()+other.GetAssignE());
  if(fPad<other.GetPad()) { fPad = other.GetPad(); }
}

void TClusterPoint::Clear(Option_t *opt) {
  TInteractionPoint::Clear(opt);
  
  fXtalId      = -1;
  fXtalAddress = -1;
  fTimestamp   = -1;
  fT0          = sqrt(-1);
  fPad         = -1;
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


TCluster::TCluster()  { Clear(); }

TCluster::~TCluster() { } 

void TCluster::Clear(Option_t *opt) { 
 opt=opt; //quiet the warnings.
 fEnergySum = 0.00;
 fCenterOfMass.SetXYZ(0,0,0);
 fClusterPoints.clear();
}

void TCluster::Print(Option_t *opt) const {
  printf("---- %6.1f-keV Cluster at ",GetEnergy()); fCenterOfMass.Print(opt);
  
  for(int x=0;x<Size();x++) {
    printf("\t");  fClusterPoints.at(x).Print(opt);
  } 
  printf("\n");
  double fom_best = 0.0;
  /*
  for(int x=0;x<Size();x++) {
    for(int y=0;y<Size();y++) {
      if(x==y) continue;
      TVector3 v1 = fClusterPoints.at(x).GetPosition();
      TVector3 v2 = fClusterPoints.at(y).GetPosition();
      double scattered_energy = GetEnergy() - fClusterPoints.at(x).GetEnergy();
      double total_energy     = GetEnergy();
      double scattered_angle  = v1.Angle(v2-v1) * TMath::RadToDeg(); // 1 -  (2-1)
  
      double calculated_angle  = GRootFunctions::ComptonAngle(&scattered_energy,&total_energy);  // i am in degrees!
      double calculated_energy  = GRootFunctions::ComptonEnergy(&scattered_angle,&total_energy);  // i am in degrees!
    
      double kn = GRootFunctions::KN_unpol_theta(&scattered_angle,&total_energy);
      TF1 f("f",GRootFunctions::KN_unpol_theta_norm,0,180,2);
      f.SetParameter(0,total_energy);  //initial gamma energy
      f.SetParameter(1,1.0);           //scaling factor.
      double integral = f.Integral(0,180);
      f.SetParameter(1,1/integral);
  
      double kn1 = f.Integral(0,scattered_angle);
      double kn2 = 1- kn1; //f.Integral(0,scattered_angle);
  
  
      //double fom = fabs(scattered_angle-calculated_angle)/
      double e_ratio = scattered_energy/calculated_energy;
      double a_ratio = scattered_angle/calculated_angle;
      double fom     = (e_ratio+a_ratio)/2.; 
  
      if(fabs(1-fom)  < fabs(1-fom_best)) { fom_best = fom; }
  
  
      printf("[%i][%i] angle %3.1f / %3.1f = %2.4f    \t  energy %4.1f / %4.1f = %4.4f   fom:  %2.4f   KN:  %f  KN1:  %f   KN2:  %f \n",x,y,
                                           scattered_angle,calculated_angle,scattered_angle/calculated_angle,
                                           scattered_energy,calculated_energy,scattered_energy/calculated_energy,fom,kn,kn1,kn2);
    }
  }
  */
  printf("best fom for the cluster is %2.4f\n",fom_best);
}      


    
//int TCluster::UniqueXtals() const {  
//  std::set<int> xtal;  
//  for(unsigned int x=0;x<fClusterPoints.size();x++) xtal.insert(fClusterPoints.at(x).GetXtal()); 
//  return xtal.size(); 
//}


void TCluster::CompressInteractions() {
  std::vector<TClusterPoint>::iterator it1;
  std::vector<TClusterPoint>::iterator it2;
  for(it1=fClusterPoints.begin();it1!=fClusterPoints.end();it1++) {
    for(it2=it1+1;it2!=fClusterPoints.end();) {
      if(*it1 == *it2) {  //checks the 2 ints are the same Xtal  && same Seg
        it1->Combine(*it2);
        it2 = fClusterPoints.erase(it2);
      } else {
        it2++;
      }
    }
  }
}


void TCluster::CompressInteractions(std::vector<TClusterPoint> *pts) {
  std::vector<TClusterPoint>::iterator it1;
  std::vector<TClusterPoint>::iterator it2;
  for(it1=pts->begin();it1!=pts->end();it1++) {
    for(it2=it1+1;it2!=pts->end();) {
      if(*it1 == *it2) {  //checks the 2 ints are the same Xtal  && same Seg
        it1->Combine(*it2);
        it2 = pts->erase(it2);
      } else {
        it2++;
      }
    }
  }
}


void TCluster::WedgeInteractions(std::vector<TClusterPoint> *pts) {
  std::vector<TClusterPoint>::iterator it1;
  std::vector<TClusterPoint>::iterator it2;
  for(it1=pts->begin();it1!=pts->end();it1++) {
    for(it2=it1+1;it2!=pts->end();) {
      //if(*it1 == *it2) {  //checks the 2 ints are the same Xtal  && same Seg
      if((it1->GetXtal() == it2->GetXtal() ) &&
         (it1->Wedge()   == it2->Wedge())) {
        it1->Combine(*it2);
        it2 = pts->erase(it2);
      } else {
        it2++;
      }
    }
  }
}

/*
void TCluster::XtalInteractions(std::vector<TClusterPoint> *pts) {
  std::vector<TClusterPoint>::iterator it1;
  std::vector<TClusterPoint>::iterator it2;
  for(it1=pts->begin();it1!=pts->end();it1++) {
    for(it2=it1+1;it2!=pts->end();) {
      //if(*it1 == *it2) {  //checks the 2 ints are the same Xtal  && same Seg
      if((it1->GetXtal() == it2->GetXtal() ) {
         //(it1->Wedge()   == it2->Wedge())) {
        it1->Combine(*it2);
        it2 = pts->erase(it2);
      } else {
        it2++;
      }
    }
  }
}
*/




void TCluster::Fit() {
  std::vector<double> xpts;
  std::vector<double> ypts;
  std::vector<TClusterPoint>::const_iterator it;
  for(it=fClusterPoints.begin();it!=fClusterPoints.end();it++) {
    xpts.push_back(it->GetThetaDeg());
    ypts.push_back(it->GetPhiDeg());
  }
  TGraph gr(xpts.size(),&(xpts[0]),&(ypts[0]));
  TFitResultPtr fr = gr.Fit("pol1","QNS");

  fFOM = fr->Chi2();
  fKN  = fr->Parameter(1);

  //printf("KN set to %.4f \n",GetKN());

}














