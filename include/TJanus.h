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

//  virtual TJanusHit&    GetJanusHit(int i);
  virtual TDetectorHit& GetHit(int i);

  std::vector<TJanusHit>& GetAllChannels() { return janus_channels; }
  std::vector<TJanusHit>& GetAllHits() { return janus_hits; }

  int GetRingSize() { return ring_hits.size(); }
  int GetSectorSize() { return sector_hits.size(); }

  TJanusHit& GetJanusHit(int i);
  TJanusHit& GetRingHit(int i);
  TJanusHit& GetSectorHit(int i);

  static TVector3 GetPosition(int detnum, int ring_num, int sector_num);
  static TVector3 GetPosition(int ring, int sector, double zoffset, bool sectorsdownstream, bool smear = false);

  Int_t GetJanusSize();
  void BuildJanusHit();

  void SetMultiHit(bool set = false) { multhit = set; } //Multihits in S3
  void SetFrontBackTime(int time) { TDiff = time; }	//Time diff between ring/sector hits
  void SetFrontBackEnergy(double en) { EWin = en; }	//Energy ratio of ring/sector hits


  char StackTriggered() const { return stack_triggered; }
  int NumPackets() const { return num_packets; }
  int TotalBytes() const { return total_bytes; }

  // Allows for looping over all hits with for(auto& hit : janus) { }
  typedef std::vector<TJanusHit>::iterator iterator;
  iterator begin() { return janus_hits.begin(); }
  iterator end() { return janus_hits.end(); }

private:
  virtual int  BuildHits(std::vector<TRawEvent>& raw_data);

  void Build_VMUSB_Read(TSmartBuffer buf);

  std::vector<TJanusHit> janus_channels;
  std::vector<TJanusHit> janus_hits;

  std::vector<TJanusHit> ring_hits;   	//Vector to store ring hits
  std::vector<TJanusHit> sector_hits; 	//Vector to store sector hits

  char stack_triggered;
  int num_packets;
  int total_bytes;

  //For geometry
  static int NRing;
  static int NSector;

  static double PhiOffset;
  static double OuterDiameter;
  static double InnerDiameter;
  static double TargetDistance;

  //For matching ring/sectors
  static double TDiff;
  static double EWin;
  static double FrontBackOffset;
  //For enabling multihit events in S3
  static bool multhit;
  ClassDef(TJanus,3);
};

#endif /* _TJANUS_H_ */
