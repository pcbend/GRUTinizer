#ifndef __TOLDSEGAHIT_H__
#define __TOLDSEGAHIT_H__


#include <TDetectorHit.h>


class TOldSegaHit : public TDetectorHit {

  public: 
    TOldSegaHit() { }
    TOldSegaHit(const TOldSegaHit &rhs) { rhs.Copy(*this); }
  
    void Copy(TObject &rhs) const;      //  { TDetectorHit::Copy(rhs); }
    void Print(Option_t *opt="") const; //  { TDetectorHit::Print(opt); }
    void Clear(Option_t *opt=""); 

    TDetectorHit &GetSegment(int i)  { return segments.at(i); }
    int Size() const                { return segments.size(); }

    void AddSegment(int id, float charge);

    int   GetSegId(int i)  const { return (segments.at(i).Address())&0x000000ff; }
    int   GetSegChg(int i) const { return (segments.at(i).Charge()); }
    float GetSegEng(int i) const { return (segments.at(i).GetEnergy()); }


    int GetDetId()       const { return (Address()&0x0000ff00)>>8; }
 

  private:
    std::vector<TDetectorHit> segments;

  ClassDef(TOldSegaHit,1)
};



#endif

