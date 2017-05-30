#ifndef TGRETINA_H
#define TGRETINA_H

#ifndef __CINT__
#include <functional>
#endif

#include <TObject.h>
#include <TMath.h>

//#include <TClonesArray.h>

#include "TDetector.h"
#include "TGretinaHit.h"

class TGretina : public TDetector {

public:
  TGretina();
  ~TGretina();

  virtual void Copy(TObject& obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");

  virtual size_t Size() const { return gretina_hits.size(); }
  virtual Int_t AddbackSize(int EngRange=-1) { BuildAddback(EngRange); return addback_hits.size(); }
  void ResetAddback() { addback_hits.clear();}

  virtual void InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(int i)            { return gretina_hits.at(i); }

  const TGretinaHit& GetGretinaHit(int i) const { return gretina_hits.at(i); }
        TGretinaHit& GetGretinaHit(int i)       { return gretina_hits.at(i); }
  const TGretinaHit& GetAddbackHit(int i) const { return addback_hits.at(i); }


  void PrintHit(int i){ gretina_hits.at(i).Print(); }

  static TVector3 CrystalToGlobal(int cryId,Float_t localX=0,Float_t localY=0,Float_t localZ=0);
  static TVector3 GetSegmentPosition(int cryid,int segment); //return the position of the segemnt in the lab system
  static TVector3 GetCrystalPosition(int cryid); //return the position of the crysal in the lab system

#ifndef __CINT__ 
  static void SetAddbackCondition(std::function<bool(const TGretinaHit&,const TGretinaHit&)> condition) {
    fAddbackCondition = condition;
  }
  static std::function<bool(const TGretinaHit&,const TGretinaHit&)> GetAddbackCondition() {
    return fAddbackCondition;
  }
#endif
  const std::vector<TGretinaHit> &GetAllHits() const { return gretina_hits; }

  void  SortHits();

  void  CleanHits(int i=-1) { 
    for(auto x=gretina_hits.begin();x!=gretina_hits.end();) { //x++) {
      if(x->GetPad()==0) {
        x->TrimSegments(1);
        if(i>=0) 
          x->SetCoreEnergy(x->GetCoreEnergy(i));
        x++;
      } else {
        x = gretina_hits.erase(x);
      }
    }
  }
 
  double SumHits(bool clean=true) { 
    double sum =0.0;
    for(auto x=gretina_hits.begin();x!=gretina_hits.end();x++) {
      if(clean && x->GetPad()==0) {
        sum += x->GetCoreEnergy();
      } else if(!clean) {
        sum += x->GetCoreEnergy();
      }
    }
    return sum;
  }

  float MSegPos(int type,int seg,int coord) const {
    return m_segpos[type][seg][coord];
  }

private:
  void BuildAddback(int EngRange=-1) const;
#ifndef __CINT__ 
  static std::function<bool(const TGretinaHit&,const TGretinaHit&)> fAddbackCondition;  
#endif
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  std::vector<TGretinaHit> gretina_hits;
  mutable std::vector<TGretinaHit> addback_hits; //!

  static Float_t crmat[32][4][4][4];
  static Float_t m_segpos[2][36][3];
  static void SetCRMAT();
  static void SetSegmentCRMAT();
  static bool fCRMATSet;


  ClassDef(TGretina,3);
};





#endif
