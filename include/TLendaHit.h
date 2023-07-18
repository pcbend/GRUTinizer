#ifndef _TLENDAHIT_H_
#define _TLENDAHIT_H_

#include <TVector3.h>
#include <TDetectorHit.h>
#include <TGraph.h>
#include <TF1.h>
#include <TFitResult.h>

class TLendaHit : public TDetectorHit {
  public:
    TLendaHit() { }
    ~TLendaHit() { }

    virtual void Copy(TObject& obj) const        { TDetectorHit::Copy(obj); }
    virtual void Clear(Option_t *opt = "");//    { TDetectorHit::Clear(opt); }
    virtual void Print(Option_t *opt = "") const { TDetectorHit::Print(opt); }

    std::vector<unsigned short>* GetTrace()   	{ return &fTrace; }
//    std::vector<Double_t>* GetFF()  		{ return &fFastFilter; }

    void SetTrace(unsigned int trace_length, const unsigned short* trace);
    void SetDetectorNumber(int dnum) { fDetector = dnum; }

    int Size() { return fTrace.size(); }

    int GetSlot()    const;
    int GetCrate()   const;
    int GetChannel() const;
    int GetDetnum() const { return fDetector; }
    TVector3 GetPosition() { return TVector3(0,0,1); }

    Double_t numOfBadFits;

  private:
    std::vector<unsigned short> fTrace;
    int fDetector;
  ClassDef(TLendaHit,1)
};

#endif
