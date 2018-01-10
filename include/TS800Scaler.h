#ifndef _TSEIGHTHUNDRED_SCALER_H_
#define _TSEIGHTHUNDRED_SCALER_H_

#include <vector>

#include <TDetector.h>

class TGEBEvent;

class TS800Scaler : public TDetector {
public:
  TS800Scaler();
  TS800Scaler(const TS800Scaler&);
  virtual ~TS800Scaler();

  virtual void Copy(TObject&)       const;
  virtual void Print(Option_t *opt) const;
  virtual void Clear(Option_t *opt);

  unsigned int Size()           const { return scalers.size(); }
  int GetScaler(int i) const { return scalers.at(i);  }

  // These functions are needed to satisfy TDetector's requirements.
  // They have no meaning here, and should never be called.
  virtual void InsertHit(const TDetectorHit&);
  virtual TDetectorHit& GetHit(int i);

private:
  std::vector<int> scalers;
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  int interval_start;
  int interval_end;
  int interval_div;
  int unix_time;
  int num_scalers;

  ClassDef(TS800Scaler,1)
};

#endif
