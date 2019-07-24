#ifndef TGRETINA_H
#define TGRETINA_H

#ifndef __CINT__
#include <functional>
#endif

#include <set>

#include <TObject.h>
#include <TMath.h>
#include <TF1.h>

#include "TDetector.h"
#include "TGretinaHit.h"

#include "TCluster.h"


class TGretina : public TDetector {

public:
  TGretina();
  ~TGretina();

  virtual void Copy(TObject& obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void PrintInteractions(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");

  virtual size_t Size() const { return gretina_hits.size(); }
  //virtual UInt_t Size() const { return gretina_hits.size(); }
  //virtual Int_t AddbackSize(int EngRange=-1) { BuildAddback(EngRange); return addback_hits.size(); }
  //void ResetAddback() { addback_hits.clear();}

  virtual void InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(int i)            { return gretina_hits.at(i); }

  //const TGretinaHit& GetGretinaHit(int i) const { return gretina_hits.at(i); }
  TGretinaHit GetGretinaHit(int i) const        { return gretina_hits.at(i); }
  //const TGretinaHit& GetAddbackHit(int i) const { return addback_hits.at(i); }


  void PrintHit(int i) { gretina_hits.at(i).Print(); }

  static TVector3 CrystalToGlobal(int cryId,Float_t localX=0,Float_t localY=0,Float_t localZ=0);
  static TVector3 GetSegmentPosition(int cryid,int segment); //return the position of the segemnt in the lab system
  static TVector3 GetCrystalPosition(int cryid); //return the position of the crysal in the lab system

  //static double ComptonAngle(double eoriginal,double escatterer);
  //static double ComptonEnergy(double eoriginal,double theta);

  void CleanHits();
  
  const std::vector<TGretinaHit> &GetAllHits() const { return gretina_hits; }

  void  Sort() { }
  void  SortHits();

  //int  BuildAddback(int EngRange=-1) const;
  
  int  BuildClusters() const;
  void PrintClusters(Option_t *opt="") const;
  int       ClusterSize()     const { return clusters.size(); }
  TCluster &GetCluster(int i) const { return clusters.at(i);  } 


  double GetTotalEnergy() const;
  //void CompressClusters() { for(unsigned int x=0;x<clusters.size();x++) clusters[x].CompressInteractions(); }
  

private:
//#ifndef __CINT__ 
//  static std::function<bool(const TGretinaHit&,const TGretinaHit&)> fAddbackCondition;  
//#endif
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  std::vector<TGretinaHit> gretina_hits;
  //mutable std::vector<TGretinaHit> addback_hits; //!
  mutable std::vector<TCluster> clusters; //!

  static Float_t crmat[32][4][4][4];
  static Float_t m_segpos[2][36][3];
  static void SetCRMAT();
  static void SetSegmentCRMAT();
  static bool fCRMATSet;

  ClassDef(TGretina,4);
};



#endif
