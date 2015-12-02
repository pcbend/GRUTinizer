#ifndef _TSEGAHIT_H_
#define _TSEGAHIT_H_

#include "TDetectorHit.h"
#include "TSegaSegmentHit.h"

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

  unsigned int GetNumSegments() const { return fSegments.size(); }
  TSegaSegmentHit& GetSegment(int i) { return fSegments.at(i); }

  std::vector<unsigned short>* GetTrace(int segnum=0);

  void SetTrace(unsigned int trace_length, const unsigned short* trace);

  TSegaSegmentHit& MakeSegmentByAddress(unsigned int address);

  int GetSlot() const;
  int GetCrate() const;
  int GetChannel() const;

  void DrawTrace(int segnum);

  TVector3 GetPosition() const;
  double GetDCEnergy(double beta, TVector3 particle_dir = TVector3(0,0,1)) const;

private:
  std::vector<unsigned short> fTrace;
  std::vector<TSegaSegmentHit> fSegments;

  ClassDef(TSegaHit,4);
};

#endif /* _TSEGAHIT_H_ */
