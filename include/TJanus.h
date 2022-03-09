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

  virtual void SetRunStart(unsigned int unix_time);

  char StackTriggered() const { return stack_triggered; }
  int NumPackets() const { return num_packets; }
  int TotalBytes() const { return total_bytes; }

  // Allows for looping over all hits with for(auto& hit : janus) { }
  typedef std::vector<TJanusHit>::iterator iterator;
  iterator begin() { return janus_hits.begin(); }
  iterator end() { return janus_hits.end(); }

  static double GetBeta(double betamax, double kr_angle_rad, bool energy_loss=false, double collision_pos=0.5);
  static double SimAngle();

private:
  virtual int  BuildHits(std::vector<TRawEvent>& raw_data);

  void Build_VMUSB_Read(TSmartBuffer buf);

  std::vector<TJanusHit> janus_channels;
  std::vector<TJanusHit> janus_hits;

  char stack_triggered;
  int num_packets;
  int total_bytes;

  ClassDef(TJanus,3);
};

#endif /* _TJANUS_H_ */
