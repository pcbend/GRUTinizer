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
  virtual void Print(Option_t* opt = "") const;
  virtual void InsertHit(const TDetectorHit&);

  virtual TJanusHit&    GetJanusHit(int i);
  virtual TDetectorHit& GetHit(int i);

  std::vector<TJanusHit>& GetAllChannels() { return janus_channels; }
  std::vector<TJanusHit>& GetAllHits() { return janus_hits; }

  static TVector3 GetPosition(int detnum, int ring_num, int sector_num);

private:
  virtual int  BuildHits(std::vector<TRawEvent>& raw_data);

  void Build_VMUSB_Read(TSmartBuffer buf);

  std::vector<TJanusHit> janus_channels;
  std::vector<TJanusHit> janus_hits;

  char stack_triggered;
  int num_packets;

  ClassDef(TJanus,3);
};

#endif /* _TJANUS_H_ */
