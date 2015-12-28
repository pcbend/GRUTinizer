#ifndef _TSEGA_H_
#define _TSEGA_H_

#include "TDetector.h"
#include "TSegaHit.h"

class TSega : public TDetector {
public:
  TSega();
  virtual ~TSega();

  void Copy(TObject& obj) const;

  virtual void Clear(Option_t* opt = "");
  virtual TSegaHit& GetSegaHit(int i);

  virtual void InsertHit(const TDetectorHit&);
  virtual TDetectorHit& GetHit(int i);

  static TVector3 GetSegmentPosition(int detnum, int segnum);
  static TVector3 CrystalToGlobal(int detnum, TVector3 crystal_pos);
  static void LoadDetectorPositions();

private:
  virtual int BuildHits();

  std::vector<TSegaHit> sega_hits;

  struct Transformation {
    TVector3 origin;
    TVector3 x;
    TVector3 y;
    TVector3 z;
  };
  static std::map<int,Transformation> detector_positions;

  ClassDef(TSega,2);
};

#endif /* _TSEGA_H_ */
