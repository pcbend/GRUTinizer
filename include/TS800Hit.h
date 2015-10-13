#ifndef TS800Hit_H
#define TS800HIT_H

#include <TObject.h>
#include <TClass.h>

class TS800Channel : public TObject {
  public:
    TS800Channel()                     { Init(); Clear();         }
    TS800Channel(short value)          { Init(); Set(value); } 
    TS800Channel(unsigned short value) { Init(); Set(value); }
    ~TS800Channel()                    {}

    void Set(short value)          { fValue = (unsigned short)value; }
    void Set(unsigned short value) { fValue = value; }

    short GetChannel() const { return (((fValue)&0xf000)>>12);  }
    short GetValue()   const { return fValue&0x0fff;  }

    virtual void Clear(Option_t *opt="")       { fValue = 0; }
    virtual void Print(Option_t *opt="") const { printf("[%i] = %i\n",GetChannel(),GetValue());} 
    virtual void Copy(TObject &obj)      const { ((TS800Channel&)obj).fValue = fValue; }

  private:
    void Init() { Class()->IgnoreTObjectStreamer(true); } 
    short fValue;

  ClassDef(TS800Channel,1);
};



class TS800TOF : public TS800Channel { 
  public:
    TS800TOF()  {  }
    TS800TOF(const TS800TOF &tof)  { tof.Copy(*this); }
    TS800TOF(short value) : TS800Channel(value)          {  }
    TS800TOF(unsigned short value) : TS800Channel(value) {  }
    //TS800TOF(char *data,int size);
    ~TS800TOF() {  }

    bool IsRF_tdc( ) {return GetChannel() == 0x000c;} 
    bool IsOBJ_tdc() {return GetChannel() == 0x000d;} 
    bool IsOBJ_tac() {return GetChannel() == 0x0005;} 
    bool IsXFP_tac() {return GetChannel() == 0x000e;} 
    bool IsXFP_tdc() {return GetChannel() == 0x0004;} 
    bool IsSI_tdc()  {return GetChannel() == 0x000f;}

    virtual void Clear(Option_t *opt="")       { TS800Channel::Clear(opt);              }
    virtual void Print(Option_t *opt="") const { printf("TOF");TS800Channel::Print(opt);} 
    virtual void Copy(TObject &obj)      const { TS800Channel::Copy(obj);             }

  ClassDef(TS800TOF,1);
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


