#ifndef _TNSCL_SCALERS_H_
#define _TNSCL_SCALERS_H_

#include <vector>

#include "TDetector.h"
#include "TRawEvent.h"

class TNSCLScalers : public TDetector {
public:
  TNSCLScalers();
  TNSCLScalers(const TNSCLScalers&);
  virtual ~TNSCLScalers();

  virtual void Copy(TObject&)          const;
  virtual void Print(Option_t *opt="") const;
  virtual void Clear(Option_t *opt="");

  unsigned int Size() const { return scalers.size(); }
  int GetScaler(int i) const { return scalers.at(i);  }
  int GetSourceID() const { return source_id; }

  int GetIntervalStart() const { return interval_start; }
  int GetIntervalEnd() const { return interval_end; }
  int GetIntervalDiv() const { return interval_div; }
  int GetUnixTime() const { return unix_time; }

  // These functions are needed to satisfy TDetector's requirements.
  // They have no meaning here, and should never be called.
  virtual void InsertHit(const TDetectorHit&);
  virtual TDetectorHit& GetHit(int i);

private:
  virtual int BuildHits(std::vector<TRawEvent>& raw_data);

  std::vector<int> scalers;

  int interval_start;
  int interval_end;
  int interval_div;
  int unix_time;
  int num_scalers;

  int source_id;

  ClassDef(TNSCLScalers,1);
};

#endif
