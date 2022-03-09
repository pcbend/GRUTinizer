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

  virtual void Draw(Option_t *opt="");

  virtual void InsertHit(const TDetectorHit&);
  virtual TDetectorHit& GetHit(int i);

  void SortHitsByTimestamp();

  virtual void SetRunStart(unsigned int unix_time);

  static TVector3 GetSegmentPosition(int detnum, int segnum);

  //Geometric (Theta-Phi) Segment Maps
  static int MappedSegnum(int detnum, int segnum); //Inidvidual segments, 1 to 512
  static int MappedPairnum(int detnum, int segnum); //Pairs of Segments, 1 to 256 
  static int MappedSlicenum(int detnum, int segnum); //Slices, 1 to 128
  
  static TVector3 CrystalToGlobal(int detnum, TVector3 crystal_pos);

  // Allows for looping over all hits with for(auto& hit : sega) { }
  std::vector<TSegaHit>::iterator begin() { return sega_hits.begin(); }
  std::vector<TSegaHit>::iterator end() { return sega_hits.end(); }

private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);
  static void LoadDetectorPositions();
  static void LoadSegmentMaps();

  std::vector<TSegaHit> sega_hits;

  struct Transformation {
    TVector3 origin;
    TVector3 x;
    TVector3 y;
    TVector3 z;
  };
  static std::map<int,Transformation> detector_positions;
  static std::map<std::array<int,2>,int> seg_map;
  static std::map<std::array<int,2>,int> pair_map;
  static std::map<std::array<int,2>,int> slice_map;

  ClassDef(TSega,2);
};

#endif /* _TSEGA_H_ */
