#ifndef TMODE3_H
#define TMODE3_H

#include "TDetector.h"
#include "TDetectorHit.h"
//#include "TGretinaHit.h"

#define MAXTRACE 1024

class TMode3 : public TDetector {
  public:
    TMode3();
    ~TMode3();

    virtual void Copy(TObject& obj) const;
    //virtual void Compare(TObject &obj) const;
    virtual void Print(Option_t *opt = "") const;
    virtual void Clear(Option_t *opt = "");
    virtual void ClearWave(Option_t *opt = "");

    virtual void          InsertHit(const TDetectorHit& hit) { return;       } 
    virtual TDetectorHit& GetHit(const int &i=0)             { return hit; }
    virtual int           Size()                             { return 1;     }

    void     BuildFrom(TSmartBuffer& buf, bool read_waveform);

    Int_t    GetChannel()   { return (board_id & 0x000f);     }
    Int_t    GetVME()       { return (board_id & 0x0030)>>4;    }
    Int_t    GetCrystal()   { return (board_id & 0x00c0)>>6;    }
    Int_t    GetHole()      { return (board_id & 0x1f00)>>8;        }
    Int_t    GetSegmentId() { return GetVME()*10 + GetChannel(); }
    Int_t    GetCrystalId() { return GetHole()*4 + GetCrystal(); }
    Int_t    GetWaveSize()  { return wavesize; }
    Short_t* GetWave()      { return wave;     } 

    Long_t   GetLed()       { return led; }
    Long_t   GetCfd()       { return cfd; }


    static void SetExtractWaves(bool flag=true) { fExtractWaves = flag;  }
    static bool ExtractWaves()                  { return fExtractWaves;  }

    Int_t    Charge()    { return hit.Charge(); }
    Int_t    Address()    { return hit.Address(); }

  private:
    virtual int BuildHits();

    static bool fExtractWaves; //!
    //mutable bool fOwnWave;             //!

    TDetectorHit hit;

    Int_t  board_id;  
    //Int_t  energy;
    Int_t  wavesize; // In 16-bit elements
    Long_t led;
    Long_t cfd;     
    Short_t wavebuffer[MAXTRACE];  //!
    Short_t *wave;                 //[wavesize]

  ClassDef(TMode3,1);
};





#endif
