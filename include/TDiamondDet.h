#ifndef _TDIAMONDDET_H_
#define _TDIAMONDDET_H_

#include "TDetector.h"
#include "TDiamondDetHit.h"

class TDiamondDet : public TDetector {
public:
  TDiamondDet();
  virtual ~TDiamondDet();

  void Copy(TObject& obj) const;

  virtual void Clear(Option_t* opt = "");
  virtual TDiamondDetHit& GetDiamondDetHit(int i);

  virtual void Draw(Option_t *opt="");

  virtual void InsertHit(const TDetectorHit&);
  virtual TDetectorHit& GetHit(int i);

  void SortHitsByTimestamp();

  // Allows for looping over all hits with for(auto& hit : diamond) { }
  std::vector<TDiamondDetHit>::iterator begin() { return diamond_hits.begin(); }
  std::vector<TDiamondDetHit>::iterator end() { return diamond_hits.end(); }

private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  std::vector<TDiamondDetHit> diamond_hits;

  ClassDef(TDiamondDet,2);
};

#endif /* _TDIAMONDDET_H_ */
