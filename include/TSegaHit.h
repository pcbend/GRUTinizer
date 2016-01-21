#ifndef _TSEGAHIT_H_
#define _TSEGAHIT_H_

#include "TDetectorHit.h"
#include "TSegaSegmentHit.h"

#include "TMath.h"

#define MAX_TRACE_LENGTH 100

class TSegaHit : public TDetectorHit {
public:
  TSegaHit();

  virtual void Copy(TObject&) const;
  virtual void Clear(Option_t *opt = "");
  virtual void Print(Option_t *opt = "") const;
  virtual void Draw(Option_t* opt = "");

  virtual Int_t Charge() const;

  int GetDetnum() const;
  int GetMainSegnum() const;

  bool HasCore() const;

  unsigned int GetNumSegments() const { return fSegments.size(); }
  TSegaSegmentHit& GetSegment(int i) { return fSegments.at(i); }
  unsigned long GetSegmentTimestamp() {
    if(fSegments.size()){
      return fSegments[0].Timestamp();
    } else {
      return -1;
    }
  }

  std::vector<unsigned short>* GetTrace(int segnum=0);

  void SetTrace(unsigned int trace_length, const unsigned short* trace);

  TSegaSegmentHit& MakeSegmentByAddress(unsigned int address);

  int GetSlot() const;
  int GetCrate() const;
  int GetChannel() const;

  void DrawTrace(int segnum);

  TVector3 GetPosition() const;
  double GetDCEnergy(double beta, TVector3 particle_dir = TVector3(0,0,1)) const;

  double GetDoppler(double beta,const TVector3 &vec=TVector3(0,0,1)) {
    if(GetNumSegments()<1)
      return 0.0;
    //if(vec==0) {
    //  vec = &BeamUnitVec;
    //}
    double tmp = 0.0;
    double gamma = 1/(sqrt(1-pow(beta,2)));
    tmp = GetEnergy()*gamma *(1 - beta*TMath::Cos(GetPosition().Angle(vec)));
    return tmp;
  }


private:
  std::vector<unsigned short> fTrace;
  std::vector<TSegaSegmentHit> fSegments;

  ClassDef(TSegaHit,4);
};

#endif /* _TSEGAHIT_H_ */
