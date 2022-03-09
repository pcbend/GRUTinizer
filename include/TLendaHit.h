#ifndef _TLENDAHIT_H_
#define _TLENDAHIT_H_

#include <TVector3.h>
#include <TDetectorHit.h>

class TLendaHit : public TDetectorHit {
  public:
    TLendaHit() { }
    ~TLendaHit() { }

    virtual void Copy(TObject& obj) const        { TDetectorHit::Copy(obj); }
    virtual void Clear(Option_t *opt = "");//    { TDetectorHit::Clear(opt); }
    virtual void Print(Option_t *opt = "") const { TDetectorHit::Print(opt); }
    //virtual void Draw(Option_t* opt = "")        { TDetectorHit::Draw(opt); }

    std::vector<unsigned short>* GetTrace()   { return &fTrace; } 
    void SetTrace(unsigned int trace_length, const unsigned short* trace);
    //void DrawTrace() { }
    int Size() { return fTrace.size(); }

    int GetSlot() const    { return 0; }
    int GetCrate() const   { return 0; }
    int GetChannel() const { return 0; }

    TVector3 GetPosition() { return TVector3(0,0,1); } 
   


  private:
    std::vector<unsigned short> fTrace;

  ClassDef(TLendaHit,1)
};

#endif
