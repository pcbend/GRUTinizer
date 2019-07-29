#ifndef TCLUSTER_H
#define TCLUSTER_H

#include "TGretinaHit.h"

#include "GRootFunctions.h"

#define CLUSTER_ANGLE 0.383972
//22 degrees in rad. 
#define CLUSTER_BUILD_TIME 50
//500 ns - coarse timing between it actions points to build the cluster.... guesstamatied by Jing.  pcb.



//  GRETINA is currently a collection of xtals - each crystals contains:
//     -  a "good" core chare / energy
//     -  a LED trigger based on the core charge
//     -  t0 time, best event start time as determined by Decomp
//     -  a number of Segment preamps containing:
//         -  Calibrated segment charge
//         -  Energy assigned to the interaction by Decomp 
//         -  X,Y,Z locally assigned by Decomp
//         -  a error code supplied by Decomp
//
//
//  If we want to treat GRETINA as an individual array, there are a few things 
//  we need to do.  We are going to treat the list of individual detectors 
//  as long list of interactions points. To get to this state, there are a few 
//  data mapping we need to do as well as decisions that need to be made.  First 
//  an interaction will be defined as:
//     - a xtal preamp-id
//     - a seg  preamp-id
//     - energy  ( decomp assigned energy/decomp summed energy for the xtal) * Core -Energy
//     - time   core-led time - t0.
//     - X Y Z in laboratory coordinates 
//
//  some decisions need to be made to get the data in this format. The two biggest 
//  i can think of at the moment are:
//     - what Decomp error codes do we accept (some are worse than others) 
//     - what do we do when Decomp decides a signal segment has 2+ interaction points.
//        - currently, i plan to collapse the interactions.  The energy will be the 
//          summed assigned value.  The XYZ will be the weighted average (weights will 
//          be assigned of the recorded Decomp assignemt). 
//
//
//  This list of interactions then needs to be organized.  First step of organization tends 
//  to be clustering.  Here, we essentially group interactions that fall together within a 
//  22 deg cone.  This truncated list of interaction points are called a TCluster here.  To 
//  first order, this clusters replace a traditional addback hit - with the sum of all energies
//  of each interaction within the cluster the total assigned energy of an individual gamma-ray.
//  Hence a number of gamma-rays interacting in the array would coorispond to a number of clusters 
//  in the array, 
//
//  Once points are grouped - the next logical step is to track, ie. order the interaction points. 
//  We will comeback to this point in the future. 
//
//
//


/*
class TInteractionPoint {
  public:
    TInteractionPoint() { } 
    TInteractionPoint(const TInteractionPoint &IP);
    TInteractionPoint(int seg,float eng,float frac,TVector3 lpos) :
                      fSegNum(seg),fEng(eng),fDecompEng(frac),fLPosition(lpos) { }
    virtual ~TInteractionPoint() { }
    virtual void Copy(const TInteractionPoint&);
    void Add(TInteractionPoint&);
    virtual int   GetSegNum()              const { return fSegNum;    }
    virtual float GetPreampE()             const { return fEng;       }
    virtual float GetDecompE()             const { return fDecompEng; }
    virtual float GetAssignE()             const { return fAssignedEng; }
    virtual int   GetOrder()               const { return fOrder; } 
    virtual TVector3 GetPosition(int xtal) const; // { return TGretina::CrystalToGlobal(xtal,
    TVector3 GetLocalPosition()            const { return fLPosition; }
    void SetOrder(int o)     { fOrder=o; }
    void SetAssignE(float e) { fAssignedEng = e; }
    void Print(Option_t *opt="") const;
    void SetSegNum(int seg) { fSegNum = seg; }
    int Wedge() const { return ((GetSegNum()-1)%6); }
  private:
    int   fSegNum;
    float fEng;          // energy as recorded by preamp.  energy in mode2 format
    float fDecompEng;    // energy as assigned by decomp.  fraction in mode2 format
    float fAssignedEng;  // percent eng assigned by decomp scaled to the core. not in mode2 format
    int   fOrder;        // interaction order
    //TVector3 fPosition;  // in global coordinates
    TVector3 fLPosition; // in local coordinates.
  ClassDef(TInteractionPoint,1)
};
*/

class TClusterPoint : public TInteractionPoint {
  public:
    TClusterPoint();  
    TClusterPoint(TGretinaHit &hit, TInteractionPoint &ip);
    virtual ~TClusterPoint();  

    TVector3 GetPosition() const { return TInteractionPoint::GetPosition(fXtalId); }

    double GetEnergy()    const { return TInteractionPoint::GetAssignE(); }

    long   GetTimestamp() const { return fTimestamp;                  }
    double GetTime()      const { return ((double)fTimestamp) - fT0;  }
    double GetT0()        const { return fT0;                         }
    int    GetPad()       const { return fPad;                        }
    int    GetXtal()      const { return fXtalId;                     }

    int    GetWedge()     const { return GetSegNum()%6;               }  //returns 0-5  
    int    GetLayer()     const { return GetSegNum()/6;               }

    //TVector3 GetPosition() const { return GetPosition(GetXtal()); }
    double GetPhi() const {                                       
      double phi = GetPosition().Phi();                             
      if(phi<0) {                                                   
        return TMath::TwoPi()+phi;                                  
      } else {                                                      
        return phi;                                                 
      }                                                             
    }                                                               
    double GetTheta()    const { return GetPosition().Theta(); }             
    double GetPhiDeg()   const { return GetPhi()*TMath::RadToDeg(); }       
    double GetThetaDeg() const { return GetTheta()*TMath::RadToDeg(); }   
    
    void Print(Option_t *opt="") const;
    void Clear(Option_t *opt="");
    void Combine(const TClusterPoint &other);
    bool operator==(const TClusterPoint &rhs) const { 
      return ((GetXtal()==rhs.GetXtal()) && (GetSegNum()==rhs.GetSegNum())); }
    bool operator<(const TClusterPoint &rhs) const { 
      return (GetEnergy()>rhs.GetEnergy()); }

  private:
    int   fXtalId;
    int   fXtalAddress;
    long  fTimestamp;
    float fT0;
    int   fPad;
  ClassDef(TClusterPoint,1);
};


class TCluster { 
  public:
    TCluster(); 
    virtual ~TCluster();

    void Clear(Option_t *opt="");
    void Print(Option_t *opt="")  const;
    int  Size()                   const { return fClusterPoints.size(); }
    TClusterPoint Get(int i)      const { return fClusterPoints.at(i); }
    TClusterPoint GetPoint(int i) const { return fClusterPoints.at(i); }
    TVector3 GetCenterOfMass()    const { return fCenterOfMass; }
    TVector3 GetPosition()        const { return fClusterPoints.front().GetPosition(); }
    double GetEnergy()            const { return fEnergySum; }
    double GetTime()              const { return fClusterPoints.front().GetTime(); }
    int GetXtal(int i=0)          const { return fClusterPoints.at(i).GetXtal(); }
    int GetWedge(int i=0)         const { return fClusterPoints.at(i).GetWedge(); }
    //int UniqueXtals()             const; //determine the number of xtals in the cluster 
    int UniqueXtals()             const { return fXtals.size(); } 
                                     //determine the number of xtals in the cluster 
    
    void Add(TClusterPoint &cp)    {  // inserts an Cluster Point into the Cluster, book keeps 
      if(Size()==0) {
        fCenterOfMass = cp.GetPosition(); 
      } else {
        double esum = fEnergySum + cp.GetEnergy();
        double x = (fCenterOfMass.X()*fEnergySum  + cp.GetPosition().X()*cp.GetEnergy())/esum;
        double y = (fCenterOfMass.Y()*fEnergySum  + cp.GetPosition().Y()*cp.GetEnergy())/esum;
        double z = (fCenterOfMass.Z()*fEnergySum  + cp.GetPosition().Z()*cp.GetEnergy())/esum;
        fCenterOfMass.SetXYZ(x,y,z);
      }
      fXtals.insert(cp.GetXtal());
      fEnergySum+=cp.GetEnergy();
      fClusterPoints.push_back(TClusterPoint(cp)); 
      std::sort(fClusterPoints.begin(),fClusterPoints.end());
    }
    
    void CompressInteractions();  // combines int_pt with same xtal && same seg
    
    
    static void CompressInteractions(std::vector<TClusterPoint> *pts);
    static void WedgeInteractions(std::vector<TClusterPoint> *pts);
    //static void XtalInteractions(std::vector<TClusterPoint> *pts);
    //std::vector<TClusterPoints> GetClusters() const { return fClusterPoints; } 

    void Fit(); 

    double GetFOM() const { return fFOM; }
    void   SetFOM(double fom) { fFOM=fom; }

    double GetTheta() const { return fTheta; }
    void   SetTheta(double theta) { fTheta=theta; }

    double GetKN() const { return fKN; }
    void   SetKN(double kn) { fKN=kn; }
 
  private:
    double fEnergySum;
    double fFOM;
    double fKN;
    double fTheta; //for now, just to plot Klein-Nishina formula DH
  
    TVector3 fCenterOfMass; 
    std::vector<TClusterPoint> fClusterPoints; 
    std::set<int> fXtals; 
    //std::set<int,int> fXtals;  <xtal,number of segemnts>

  ClassDef(TCluster,3)  
};






#endif

