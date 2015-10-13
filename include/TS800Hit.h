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


class TS800Hit : public TDetectorHit {
  public:
    TS800Hit();
    ~TS800Hit();
    virtual Int_t Charge() const { return 0; }

    virtual void Clear(Option_t *opt ="")       {  }
    virtual void Print(Option_t *opt ="") const {  }
    virtual void Copy(TObject &obj)       const { TDetectorHit::Copy(obj);  }

  private:

  ClassDef(TS800Hit,1);
};

#endif


