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
  virtual void InsertHit(const TDetectorHit&);
  virtual TSegaHit& GetSegaHit(int i);
  virtual TDetectorHit& GetHit(int i);

private:
  virtual int BuildHits();

  std::vector<TSegaHit> sega_hits;

  ClassDef(TSega,2);
};

#endif /* _TSEGA_H_ */
