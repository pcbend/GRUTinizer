
#ifndef __TFSU_H__
#define __TFSU_H__


#include <TDetector.h>
#include <TFSUHit.h>

class TFSU: public TDetector {
  public:
    TFSU();  
    ~TFSU(); 


    void Clear(Option_t *opt="");
    void Print(Option_t *opt="") const;
    void Copy(TObject& obj) const;

    TFSUHit &GetFSUHit(int i) { return fFSUHits.at(i); }
    size_t Size() const       { return fFSUHits.size(); }
 
    virtual void InsertHit(const TDetectorHit &hit) { fFSUHits.emplace_back((TFSUHit&)hit); fSize++; }
    virtual TDetectorHit& GetHit(int i)             { return fFSUHits.at(i);   }

    TFSUHit GetDeltaE() const { return fDeltaE; }
    TFSUHit GetE()      const { return fEnergy; }

    bool GoodParticle() const { if(fDeltaE.Charge()<1 || fEnergy.Charge()<1) return false; return true; }

  private:
    int  BuildHits(std::vector<TRawEvent>& raw_data);
    std::vector<TFSUHit> fFSUHits;

    TFSUHit fDeltaE;
    TFSUHit fEnergy;


  ClassDef(TFSU,2)
};


#endif


