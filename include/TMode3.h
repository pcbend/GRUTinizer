#ifndef TGRETINA_H
#define TGRETINA_H

#include <TObject.h>
#include <TMath.h>

#include <TClonesArray.h>

#include "TDetector.h"
#include "TGretinaHit.h"

#define MAXTRACE 1024

class TMode3 : public TDetector {
  public:
    TMode3();
    ~TMode3();

    virtual void Copy(TObject& obj) const;
    virtual void Compare(TObject &obj) const;
    virtual void Print(Option_t *opt = "") const;
    virtual void Clear(Option_t *opt = "");

    virtual void          InsertHit(const TDetectorHit& hit) { return; } 
    virtual TDetectorHit& GetHit(const int &i=0)    { return *this; }
    virtual int           Size()                    { return 1; }

    Int_t    GetChannel()   { return (board_id & 0x0f00)>>8;  }
    Int_t    GetVME()       { return (board_id & 0x3000)>>12; }
    Int_t    GetCrystal()   { return (board_id & 0xc000)>>14; }
    Int_t    GetHole()      { return (board_id & 0x001f);     }
    Int_t    GetSegmentId() { return GetVME()*10 + GetChannel(); }
    Int_t    GetCrystalId() { return GetHole()*4 + GetCrystal(); }
    Int_t    GetWaveSize()  { return wavesize; }
    Short_t* GetWave()      { return wave;     } 

    void SetExtractWaves(bool flag=true} { fExtractWaves = flag; }
    bool ExtractWaves()                  {return fExtractWaves;  }
  private:
    virtual int BuildHits();

    static bool fExtractWaves; //!
    bool fOwnWave;             //!

    Int_t  board_id;  
    Int_t  energy;
    Int_t  wavesize 
    Long_t led;
    Long_t cfd;     
    Short_t *wave;  //[wavesize]

  ClassDef(TMode3,1);
};





#endif
