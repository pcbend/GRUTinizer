#ifndef TS800Hit_H
#define TS800HIT_H

#include <TObject.h>
#include <TClass.h>

#define MAXCRDC 512


class TS800Channel : public TDetectorHit {
  public:
    TS800Channel()                     { Clear();         }
    TS800Channel(short value)          { Set(value); }
    TS800Channel(unsigned short value) { Set(value); }
    ~TS800Channel()                    {}

    void Set(short value)          { fValue = (unsigned short)value; }
    void Set(unsigned short value) { fValue = value; }

    short GetId()      const { return (((fValue)&0xf000)>>12);  }
    short GetValue()   const { return fValue&0x0fff;  }

    virtual void Clear(Option_t *opt="")       { TDetectorHit::Clear(opt); fValue = 0; }
    virtual void Print(Option_t *opt="") const { printf("[%i] = %i\n",GetId(),GetValue());}
    virtual void Copy(TObject &obj)      const { TDetectorHit::Copy(obj); ((TS800Channel&)obj).fValue = fValue; }

    virtual int  Charge() const { return GetValue(); }

  private:
    //void Init() { Class()->IgnoreTObjectStreamer(true); }
    short fValue;

  ClassDef(TS800Channel,1);
};



class TTOFHit :  public TS800Channel {
  public:
    TTOFHit()  {  }
    TTOFHit(const TTOFHit &tof)  { tof.Copy(*this); }
    TTOFHit(short value) : TS800Channel(value)          {  }
    TTOFHit(unsigned short value) : TS800Channel(value) {  }
    //TTOFHit(char *data,int size);
    ~TTOFHit() {  }

    bool IsRF_tdc( ) {return GetId() == 0x000c;}
    bool IsOBJ_tdc() {return GetId() == 0x000d;}
    bool IsOBJ_tac() {return GetId() == 0x0005;}
    bool IsXFP_tac() {return GetId() == 0x000e;}
    bool IsXFP_tdc() {return GetId() == 0x0004;}
    bool IsSI_tdc()  {return GetId() == 0x000f;}

    virtual void Clear(Option_t *opt="")       { TS800Channel::Clear(opt);              }
    virtual void Print(Option_t *opt="") const { printf("TOF");TS800Channel::Print(opt);}
    virtual void Copy(TObject &obj)      const { TS800Channel::Copy(obj);             }

  ClassDef(TTOFHit,1);
};

class TFPScint :  public TS800Channel {
  public:
    TFPScint()  {  }
    TFPScint(const TFPScint &fpscint)  { fpscint.Copy(*this); }
    TFPScint(short charge,short time):TS800Channel(charge),fTime((unsigned short)time) {  }
    TFPScint(unsigned short charge,unsigned short time):TS800Channel(charge),fTime(time) {  }
    //TFPScint(char *data,int size);
    ~TFPScint() {  }

    void SetCharge(short chg) { Set(chg);     }
    void SetTime(short time)  { fTime = time; }



    bool IsE1up()   {return GetId() == 0x0000;}
    bool IsE1down() {return GetId() == 0x0001;}
    bool IsE2up()   {return GetId() == 0x0002;}
    bool IsE2down() {return GetId() == 0x0003;}
    bool IsE3up()   {return GetId() == 0x0004;}
    bool IsE3down() {return GetId() == 0x0005;}

    short GetTime() const { return fTime&0x0fff; }

    virtual void Clear(Option_t *opt="")       { TS800Channel::Clear(opt); fTime = 0;   }
    virtual void Print(Option_t *opt="") const { printf("FPScint");TS800Channel::Print(opt);}
    virtual void Copy(TObject &obj)      const { TS800Channel::Copy(obj);  }
  private:

   unsigned short fTime;
  ClassDef(TFPScint,1);
};

class TIonChamber : public TS800Channel {
  public:
    TIonChamber() { }
    TIonChamber(const TIonChamber &ion) { ion.Copy(*this); }
    TIonChamber(short value):TS800Channel(value) { }
   
    virtual void Clear(Option_t *opt="")       { TS800Channel::Clear(opt);    }
    virtual void Print(Option_t *opt="") const { printf("Ion Chamber");TS800Channel::Print(opt);}
    virtual void Copy(TObject &obj)      const { TS800Channel::Copy(obj);  }
  private:
    ClassDef(TIonChamber,1)

};

class TCrdcPad : public TDetectorHit {
  public:
    TCrdcPad() { }
    ~TCrdcPad() { }
    

    virtual Int_t Charge() const { return 0; } //TODO


    short fChannel;

    Int_t   wavesize;
    Short_t wavebuffer[MAXCRDC];  //!
    Short_t *wave;                 //[wavesize]

  ClassDef(TCrdcPad,1)
};


class TS800Hit : public TDetectorHit {
  public:
    TS800Hit()   {  }
    ~TS800Hit()  {  }
    virtual Int_t Charge() const { return 0; }

    virtual void Clear(Option_t *opt ="")       {  }
    virtual void Print(Option_t *opt ="") const {  }
    virtual void Copy(TObject &obj)       const { TDetectorHit::Copy(obj);  }

  private:

  ClassDef(TS800Hit,1);
};

#endif
