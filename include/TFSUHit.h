#ifndef __TFSUHIT_H__
#define __TFSUHIT_H__

#include <TDetectorHit.h>
#include <TVector3.h>

class TFSUHit : public TDetectorHit {
  public:
    TFSUHit();
    ~TFSUHit();
   
    void Clear(Option_t *opt="");
    void Print(Option_t *opt="") const;

    Int_t Crate()   const {return ((fAddress&0x00000f00)>>8); }
    Int_t Slot()    const {return ((fAddress&0x000000f0)>>4); }
    Int_t Channel() const {return ((fAddress&0x0000000f)); }

    Int_t Id()      const {return (Slot()*16+Channel()); }

    TVector3 GetPosition() const;
  
    double GetDoppler(const TVector3 *recoil_vec=0) const;

    double GetDoppler(double beta) const;

  ClassDef(TFSUHit,1)
};

#endif

