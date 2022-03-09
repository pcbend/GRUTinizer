#ifndef _TJANUSDDAS_H_
#define _TJANUSDDAS_H_

#include "TDetector.h"
#include "TJanusDDASHit.h"
#include "TReaction.h"
#include "TSRIM.h"
#include <limits>

class TJanusDDAS : public TDetector {
public:
  TJanusDDAS();
  virtual ~TJanusDDAS();

  void Copy(TObject& obj) const;

  virtual void Clear(Option_t* opt = "");
  virtual void Print(Option_t* opt = "hits") const;
  virtual void InsertHit(const TDetectorHit&);

  virtual TJanusDDASHit&    GetJanusHit(int i);
  virtual TJanusDDASHit&    GetJanusChannel(int i);
  virtual TDetectorHit& GetHit(int i);

  std::vector<TJanusDDASHit>& GetAllChannels() { return janus_channels; }
  std::vector<TJanusDDASHit>& GetBadChargeChannels() { return bad_charge_channels;}

  std::vector<TJanusDDASHit>& GetChannels() { return channels; }  //only considers usable channels
  std::vector<TJanusDDASHit> GetDetectorChannels(const int detNum); //only considers usable channels
  
  std::vector<TJanusDDASHit>& GetRings() { return rings; }  //only considers usable channels
  std::vector<TJanusDDASHit>& GetSectors() { return sectors; }  //only considers usable channels

  ///////////// Deprecated /////////////
  std::vector<TJanusDDASHit>& GetPrimaryHits() { return primary_hits; }
  std::vector<TJanusDDASHit>& GetSingleHits1() { return primary_single_hits; }
  std::vector<TJanusDDASHit>& GetDoubleHits1() { return primary_double_hits; }

  std::vector<TJanusDDASHit>& GetSecondaryHits() { return secondary_hits; }
  std::vector<TJanusDDASHit>& GetSingleHits2() { return secondary_singles; }
  std::vector<TJanusDDASHit>& GetDoubleHits2() { return secondary_doubles; }

  std::vector<TJanusDDASHit>& GetTertiaryHits() { return tertiary_hits; }
  std::vector<TJanusDDASHit>& GetSingleHits3() { return tertiary_singles; }
  std::vector<TJanusDDASHit>& GetDoubleHits3() { return tertiary_doubles; }

  std::vector<TJanusDDASHit>& GetUnusedChannels1() { return primary_unused_channels; }
  std::vector<TJanusDDASHit>& GetUnusedRings1() { return primary_unused_rings; }
  std::vector<TJanusDDASHit>& GetUnusedSectors1() { return primary_unused_sectors; }

  std::vector<TJanusDDASHit>& GetUnusedChannels2() { return secondary_unused_channels; }
  std::vector<TJanusDDASHit>& GetUnusedRings2() { return secondary_unused_rings; }
  std::vector<TJanusDDASHit>& GetUnusedSectors2() { return secondary_unused_sectors; }

  std::vector<TJanusDDASHit>& GetUnusedChannels3() { return tertiary_unused_channels; }
  std::vector<TJanusDDASHit>& GetUnusedRings3() { return tertiary_unused_rings; }
  std::vector<TJanusDDASHit>& GetUnusedSectors3() { return tertiary_unused_sectors; }
  //////////////////////////////////////

  //hit_level = 0 gives all hits, 1 gives primary hits, 2 secondary, 3 tertiary
  std::vector<TJanusDDASHit>& GetUnusedChannels(int hit_level = 0);
  std::vector<TJanusDDASHit>& GetUnusedRings(int hit_level = 0);
  std::vector<TJanusDDASHit>& GetUnusedSectors(int hit_level = 0);

  std::vector<TJanusDDASHit>& GetHits(int hit_level = 0);
  std::vector<TJanusDDASHit>& GetSingleHits(int hit_level = 0);
  std::vector<TJanusDDASHit>& GetDoubleHits(int hit_level = 0);
  std::vector<TJanusDDASHit>& GetAddbackHits(int hit_level = 0);
  
  std::vector<TJanusDDASHit> GetDetectorHits(const int detNum, const int hit_level = 0);

  //type = 0 gives single, double and addback hits, 1 give single hits, 2 double, 3 addback
  std::vector<TJanusDDASHit> GetSpecificHits(const int detNum, const int hit_level = 0, const int type = 0);

  static double LabSolidAngle(int detNum, int RingNum);
  static double CmSolidAngle(int detNum, int RingNum, TReaction& reaction, int part = 2,bool before = false,
			     bool max_ring = false, bool sol2 = false);
  //static double CmSolidAngle(int detNum, int RingNum, int part = 2,bool before = false);  
  
  static TVector3 GetPosition(int detnum, int ring_num, int sector_num, bool before_e17507B = false);
  
  static TVector3 GetReconPosition(double theta_det, double phi_det, TReaction& reac,
				   int det_part, int recon_part, bool sol2);
  //static TVector3 GetReconPosition(double theta_det, double phi_det, int det_part, int recon_part, bool sol2);

  static double GetBeta(double theta_rad, TReaction& reac, int part, TSRIM& srim_file, bool solution2 = false,
			bool dE_Out = true) ;
  
  // Allows for looping over all hits with for(auto& hit : janus) { }
  typedef std::vector<TJanusDDASHit>::iterator iterator;
  iterator begin() { return primary_hits.begin(); }
  iterator end() { return primary_hits.end(); }

  // static double GetBeta(double betamax, double kr_angle_rad, bool energy_loss=false, double collision_pos=0.5);
  // static double SimAngle();
  
  virtual void PrintChannels(Option_t* opt = "") const;
  virtual void PrintHits(Option_t* opt = "") const;

  size_t Size() const { return primary_hits.size(); }

  int GetMaxLevel() const { return levels_made; }

  void BuildCorrelatedHits();
  
private:
  virtual int  BuildHits(std::vector<TRawEvent>& raw_data);

  //return beam based on target (give 3, calculate 2).
  static double Reconstruct(double theta, TReaction& reaction, int det_pt, int recon_pt, bool s2);
  //static double Reconstruct(double theta, int det_pt, int recon_pt, bool s2);
  
  void UnpackChannels(std::vector<TRawEvent>& raw_data);
  void MakeHit(const TJanusDDASHit& chan_ring, const TJanusDDASHit& chan_sector,
	       const int hit_level, const bool double_hit = false, const bool addback_hit = false);

  void ClearCorrelatedHits();
  void ClearChannels();
  //void BuildRingSectors();

  void AssignLevel(const int max_level) {levels_made = max_level;}

  int levels_made;
  
  std::vector<TJanusDDASHit> janus_channels;
  std::vector<TJanusDDASHit> bad_charge_channels;
  
  std::vector<TJanusDDASHit> channels;
  std::vector<TJanusDDASHit> rings;
  std::vector<TJanusDDASHit> sectors;
  
  std::vector<TJanusDDASHit> primary_hits;
  std::vector<TJanusDDASHit> primary_single_hits;
  std::vector<TJanusDDASHit> primary_double_hits;
  std::vector<TJanusDDASHit> primary_addback_hits;

  std::vector<TJanusDDASHit> secondary_hits;
  std::vector<TJanusDDASHit> secondary_singles;
  std::vector<TJanusDDASHit> secondary_doubles;
  std::vector<TJanusDDASHit> secondary_addback_hits;
  
  std::vector<TJanusDDASHit> tertiary_hits;
  std::vector<TJanusDDASHit> tertiary_singles;
  std::vector<TJanusDDASHit> tertiary_doubles;
  std::vector<TJanusDDASHit> tertiary_addback_hits;

  std::vector<TJanusDDASHit> all_hits;
  std::vector<TJanusDDASHit> all_single_hits;
  std::vector<TJanusDDASHit> all_double_hits;
  std::vector<TJanusDDASHit> all_addback_hits;

  std::vector<TJanusDDASHit> primary_unused_channels;
  std::vector<TJanusDDASHit> primary_unused_rings;
  std::vector<TJanusDDASHit> primary_unused_sectors;

  std::vector<TJanusDDASHit> secondary_unused_channels;
  std::vector<TJanusDDASHit> secondary_unused_rings;
  std::vector<TJanusDDASHit> secondary_unused_sectors;

  std::vector<TJanusDDASHit> tertiary_unused_channels;
  std::vector<TJanusDDASHit> tertiary_unused_rings;
  std::vector<TJanusDDASHit> tertiary_unused_sectors;

  ClassDef(TJanusDDAS,1);
};

#endif /* _TJANUSDDAS_H_ */
