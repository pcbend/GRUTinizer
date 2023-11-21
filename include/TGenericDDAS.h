#ifndef _TGENERICDDAS_H_
#define _TGENERICDDAS_H_

#include "TDetector.h"
#include "TGenericDDASHit.h"

class TGenericDDAS : public TDetector {
public:
  TGenericDDAS();
  virtual ~TGenericDDAS();

  void Copy(TObject& obj) const;

  virtual void Clear(Option_t* opt = "");
  virtual TGenericDDASHit& GetDDASHit(int i);
  virtual TDetectorHit& GetHit(int i);
  virtual void InsertHit(const TDetectorHit&);

  // Allows for looping over all hits with for(auto& hit : ddas) { }
  std::vector<TGenericDDASHit>::iterator begin() { return ddas_hits.begin(); }
  std::vector<TGenericDDASHit>::iterator end() { return ddas_hits.end(); }

private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  std::vector<TGenericDDASHit> ddas_hits;
  static bool fFileDetermined;
  static bool fGEB;
  ClassDef(TGenericDDAS,2);
};

#endif /* _TGENERICDDAS_H_ */
