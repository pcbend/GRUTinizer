#ifndef _TJANUS_H_
#define _TJANUS_H_

#include "TClonesArray.h"

#include "TDetector.h"
#include "TJanusHit.h"

class TJanus : public TDetector {
public:
  TJanus();
  virtual ~TJanus();

  virtual void AddRawData(TSmartBuffer buf);
  virtual void BuildHits();

  virtual void InsertHit(const TDetectorHit&);
  virtual TDetectorHit& GetHit(int i);
  virtual int Size();

private:
  void Build_VMUSB_Read(TSmartBuffer buf);

  TClonesArray* janus_hits; //->
  std::vector<TSmartBuffer> raw_data; //!

  ClassDef(TJanus,1);
};

#endif /* _TJANUS_H_ */
