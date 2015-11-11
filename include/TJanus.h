#ifndef _TJANUS_H_
#define _TJANUS_H_

#include "TDetector.h"
#include "TJanusHit.h"

class TJanus : public TDetector {
public:
  TJanus();
  virtual ~TJanus();

  void Copy(TObject& obj) const;

  virtual void Clear(Option_t* opt = "");

  virtual void InsertHit(const TDetectorHit&);
  virtual TJanusHit&    GetJanusHit(int i);
  virtual TDetectorHit& GetHit(int i);

private:
  virtual int  BuildHits();

  void Build_VMUSB_Read(TSmartBuffer buf);

  std::vector<TJanusHit> janus_hits;

  ClassDef(TJanus,2);
};

#endif /* _TJANUS_H_ */
