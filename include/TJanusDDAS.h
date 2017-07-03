#ifndef _TJANUSDDAS_H_
#define _TJANUSDDAS_H_

#include "TDetector.h"
#include "TJanusDDASHit.h"

class TJanusDDAS : public TDetector {
public:
  TJanusDDAS();
  virtual ~TJanusDDAS();

  void Copy(TObject& obj) const;

  virtual void Clear(Option_t* opt = "");
  virtual void Print(Option_t* opt = "") const;
  virtual void InsertHit(const TDetectorHit&);

  virtual TJanusDDASHit&    GetJanusHit(int i);
  virtual TDetectorHit& GetHit(int i);

  std::vector<TJanusDDASHit>& GetAllChannels() { return janus_channels; }
  std::vector<TJanusDDASHit>& GetAllHits() { return janus_hits; }

  static TVector3 GetPosition(int detnum, int ring_num, int sector_num);

  // Allows for looping over all hits with for(auto& hit : janus) { }
  typedef std::vector<TJanusDDASHit>::iterator iterator;
  iterator begin() { return janus_hits.begin(); }
  iterator end() { return janus_hits.end(); }

  // static double GetBeta(double betamax, double kr_angle_rad, bool energy_loss=false, double collision_pos=0.5);
  // static double SimAngle();

private:
  virtual int  BuildHits(std::vector<TRawEvent>& raw_data);

  void BuildCorrelatedHits();
  void UnpackChannels(std::vector<TRawEvent>& raw_data);
  void MakeHit(const TJanusDDASHit& chan_ring,
               const TJanusDDASHit& chan_sector);

  std::vector<TJanusDDASHit> janus_channels;
  std::vector<TJanusDDASHit> janus_hits;

  ClassDef(TJanusDDAS,1);
};

#endif /* _TJANUSDDAS_H_ */
