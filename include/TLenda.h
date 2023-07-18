#ifndef _TLENDA_H_
#define _TLENDA_H_

#include <TVector3.h>

#include <TDetector.h>
#include <TLendaHit.h>

class TLenda : public TDetector {
  public:
    TLenda() { }
    ~TLenda() { }

    void Copy(TObject& obj) const;
    virtual void Clear(Option_t* opt = "");
//    virtual void Print(Option_t* opt = "") const;

    void InsertHit(const TDetectorHit&) {   }


    TDetectorHit &GetHit(int i) { return bar_hits.at(i); }

    int GetTopSize() {return top_hits.size(); }
    int GetBottomSize() {return bottom_hits.size(); }

    int GetBarSize();		//Size of Bars
    void BuildLendaBars();	//Loops over Top and Bottome and creates BarHits


    TLendaHit * GetBarHit(const int&);
    TLendaHit * GetTopHit(const int&);
    TLendaHit * GetBottomHit(const int&);

    static TVector3 GetPosition(int i) { return TVector3(0,0,1); }

  private:
    int BuildHits(std::vector<TRawEvent>& raw_data);
    //{ printf("lenda build hits called\n"); fflush(stdout);return 0;}

    std::vector<TLendaHit> bar_hits;

    std::vector<TLendaHit> top_hits;    //Top Hits
    std::vector<TLendaHit> bottom_hits;    //Bottom Hits

    static int TDiff;
  ClassDef(TLenda,1);
};

#endif
