#ifndef _TJANUSDDASHIT_H_
#define _TJANUSDDASHIT_H_

#include "TDetectorHit.h"

class TJanusDDASHit : public TDetectorHit {
public:
  TJanusDDASHit() { }
  TJanusDDASHit(const TJanusDDASHit& hit);

  TJanusDDASHit& operator=(const TJanusDDASHit& hit);

  virtual float RawCharge() const;

  void Clear(Option_t* opt = "");
  void Print(Option_t* opt = "") const;
  void Copy(TObject& obj) const;

  TDetectorHit& GetBackHit() { return back_hit; }

  int GetFrontChannel() const;
  int GetBackChannel() const;

  int GetDetnum() const;
  int GetRing() const;
  int GetSector() const;

  bool IsRing() const;
  bool IsSector() const;

  TVector3 GetPosition(bool apply_array_offset = true) const;
  TVector3 GetReconPosition(const char *beamname="72Se",const char *targetname="208Pb",
                            bool apply_array_offset = true) const;
  //double SimAngle(const char *beamname="72Se",const char *targetname="208Pb");



private:
  TDetectorHit back_hit;
  
  double Reconstruct(const char *beamname,const char *targetname,const char *srimfile="se72_in_pb208.txt") const;



  ClassDef(TJanusDDASHit,1);
};

#endif /* _TJANUSDDASHIT_H_ */
