#ifndef TMODE3HIT_H
#define TMODE3HIT_H

#include "TDetector.h"
#include "TDetectorHit.h"
//#include "TGretinaHit.h"

#define MAXTRACE 1024

class TSmartBuffer;

class TMode3Hit : public TDetectorHit {
public:
  TMode3Hit();
  ~TMode3Hit();

  virtual void Copy(TObject& obj) const;
  //virtual void Compare(TObject &obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");

  //virtual void          InsertHit(const TDetectorHit& hit) { return;       }
  //virtual TDetectorHit& GetHit(const int &i=0)             { return hit; }
  //virtual int           Size()                             { return 1;     }
  double AverageWave(int samples=-1);

  void     BuildFrom(TSmartBuffer& buf);

  Int_t    GetChannel()   const { return (board_id & 0x000f);     }
  Int_t    GetVME()       const { return (board_id & 0x0030)>>4;    }
  Int_t    GetCrystal()   const { return (board_id & 0x00c0)>>6;    }
  Int_t    GetHole()      const { return (board_id & 0x1f00)>>8;        }
  Int_t    GetSegmentId() const { return GetVME()*10 + GetChannel(); }
  Int_t    GetAbsSegId()  const { return GetCrystal()*40 + GetSegmentId() ; }
  Int_t    GetCrystalId() const { return GetHole()*4 + GetCrystal(); }
  const std::vector<Short_t>& GetWave() const { return waveform; }

  Long_t   GetLed()       const { return led; }
  Long_t   GetCfd()       const { return cfd; }

  UShort_t GetDt1()       const { return dt1; }
  UShort_t GetDt2()       const { return dt2; }

  Int_t    GetCharge0()   const { return charge0; }
  Int_t    GetCharge1()   const { return charge1; }
  Int_t    GetCharge2()   const { return charge2; }

  static void SetExtractWaves(bool flag=true) { fExtractWaves = flag;  }
  static bool ExtractWaves()                  { return fExtractWaves;  }

  virtual Int_t  Charge() const   { return TDetectorHit::Charge()/128; }

  double GetEnergy0() const;
  double GetEnergy1() const;
  double GetEnergy2() const;

  void Draw(Option_t *opt="");


private:
  //virtual int BuildHits();

  static bool fExtractWaves; //!
  //mutable bool fOwnWave;             //!

  //TDetectorHit hit;

  Int_t  board_id;
  //Int_t  charge;
  Int_t  wavesize; // In 16-bit elements
  Long_t led;
  Long_t cfd;
  std::vector<Short_t> waveform;
  UShort_t dt1;    // time diff between this led and previous (last) led
  UShort_t dt2;    // time diff between previous and previous-previouss (yep.) led

  Int_t  charge0;  // charge using pick-off method.
  Int_t  charge1;  // charge of previous.
  Int_t  charge2;  // charge of previous-previous.

  ClassDef(TMode3Hit,3);
};





#endif
