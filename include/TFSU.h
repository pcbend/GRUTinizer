
#ifndef __TFSU_H__
#define __TFSU_H__


#include <TDetector.h>
#include <TFSUHit.h>

#include <TCutG.h>

class TFSU: public TDetector {
  public:
    TFSU();  
    TFSU(const TFSUHit& other);
    ~TFSU(); 


    void Clear(Option_t *opt="");
    void Print(Option_t *opt="") const;
    void Copy(TObject& obj) const;

    TFSUHit &GetFSUHit(int i) { return fFSUHits.at(i); }
    size_t Size() const       { return fFSUHits.size(); }
    
    void    ClearAddback() { fAddbackHits.clear(); }
    TFSUHit &GetAddbackHit(int i) { return fAddbackHits.at(i); }
    size_t AddbackSize() const       { return fAddbackHits.size(); }
 
    virtual void InsertHit(const TDetectorHit &hit) { fFSUHits.emplace_back((TFSUHit&)hit); fSize++; }
    virtual TDetectorHit& GetHit(int i)             { return fFSUHits.at(i);   }

    TFSUHit GetDeltaE() const { return fDeltaE; }
    TFSUHit GetE()      const { return fEnergy; }

    bool GoodParticle() const { if(fDeltaE.Charge()<1 || fEnergy.Charge()<1) return false; return true; }
    
    //int CleanHits(TCutG *timing,TCutG *pp_timing=0);
    int CleanHits(double low =25.,double high=9500.,double timediff=50);
    int MakeAddbackHits();
    
    int WriteToEv2(const char *filename) const;  

    void OrderHits() { std::sort(fFSUHits.rbegin(),fFSUHits.rend()); }
  private:
    int  BuildHits(std::vector<TRawEvent>& raw_data);
    std::vector<TFSUHit> fFSUHits;
    std::vector<TFSUHit> fAddbackHits; //!

    TFSUHit fDeltaE;
    TFSUHit fEnergy;


  ClassDef(TFSU,2)
};


#endif


