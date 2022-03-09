#ifndef _TJANUSDDASHIT_H_
#define _TJANUSDDASHIT_H_

#include "TDetectorHit.h"
#include "TReaction.h"

class TJanusDDASHit : public TDetectorHit {
public:
  TJanusDDASHit() { }
  TJanusDDASHit(const TJanusDDASHit& hit);

  TJanusDDASHit& operator=(const TJanusDDASHit& hit);
  bool operator==(const TJanusDDASHit& rhs);

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

  double GetLabSolidAngle() const;
  double GetCmSolidAngle(TReaction& reac, int part = 2, bool before = false) const;

  TVector3 GetPosition(bool before = false, bool apply_array_offset = true) const;
  
  TVector3 GetReconPosition(TReaction& reac, int d_p, int r_p, bool s2,
			    bool before = false, bool apply_offset = true) const;
  //TVector3 GetReconPosition(int d_p, int r_p, bool s2, bool before = false, bool apply_offset = true) const;
  
  //double SimAngle(const char *beamname="72Se",const char *targetname="208Pb");

  bool operator<(const TJanusDDASHit& rhs) const { return Charge() > rhs.Charge(); }



private:
  TDetectorHit back_hit;
  
  //double Reconstruct(const char *beamname, const char *targetname, const char *srimfile, bool before = false,
  //                 bool apply_array_offset = true) const;



  ClassDef(TJanusDDASHit,1);
};

#endif /* _TJANUSDDASHIT_H_ */
