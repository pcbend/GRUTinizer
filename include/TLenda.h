#ifndef _TLENDA_H_
#define _TLENDA_H_

#include <TVector3.h>

#include <TDetector.h>
#include <TLendaHit.h>

class TLenda : public TDetector {
  public:
    TLenda() { }
    ~TLenda() { }

    void Copy(TObject &obj) const { } //TDetector::Copy(obj); }

    void Clear(Option_t *opt="") { TDetector::Clear(opt); }
    TLendaHit &GetLendaHit(int i) { return lenda_hits.at(i); }
    
    void InsertHit(const TDetectorHit&) {   }
    TDetectorHit &GetHit(int i) { return lenda_hits.at(i); } 

    static TVector3 GetPosition(int i) { return TVector3(0,0,1); }

  private:
    int BuildHits(std::vector<TRawEvent>& raw_data);
    //{ printf("lenda build hits called\n"); fflush(stdout);return 0;}
    std::vector<TLendaHit> lenda_hits;

  ClassDef(TLenda,1);
};

#endif
