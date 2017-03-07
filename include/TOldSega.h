#ifndef __TOLDSEGA_H__
#define __TOLDSEGA_H__


#include <vector>


#include <TDetector.h>
#include <TOldSegaHit.h>

class TOldSega : public TDetector {
  public:
    TOldSega() { } 
    TOldSega(const TOldSega &rhs) { rhs.Copy(*this); }
    
    void Copy(TObject &rhs)      const; // { TDetector::Copy(rhs); }
    void Print(Option_t *opt="") const; // { TDetector::Print(opt); }
    void Clear(Option_t *opt=""); 

    void InsertHit(const TDetectorHit &hit) { sega_hits.push_back((TOldSegaHit&)hit); }
    TDetectorHit& GetHit(int i)             { return sega_hits.at(i); }
    TOldSegaHit&  GetSegaHit(int i)         { return sega_hits.at(i); }
 
    unsigned short GetMasterLive() const { return masterlive; }
    unsigned short GetXfpScint()   const { return xfpscint; }
    unsigned short GetRf()         const { return rf; }

    void SetMasterLive(unsigned short tmp) { masterlive = tmp; }
    void SetXfpScint(unsigned short tmp)   { xfpscint   = tmp; }
    void SetRf(unsigned short tmp)         { rf         = tmp; }

    TOldSega& operator=(const TOldSega& rhs) { rhs.Copy(*this); return *this;}


  private:
    int  BuildHits(std::vector<TRawEvent>& raw_data)  { return sega_hits.size(); }

    std::vector<TOldSegaHit> sega_hits;

    unsigned short masterlive;
    unsigned short xfpscint;
    unsigned short rf;

  ClassDef(TOldSega,1)
};


#endif

