#ifndef _TFASTSCINT_H_
#define _TFASTSCINT_H_

#include "TClonesArray.h"

#include "TDetector.h"
#include "TFastScintHit.h"

class TNSCLEvent;

class TFastScint : public TDetector {
public:
  TFastScint();
  virtual ~TFastScint();

  void Copy(TObject& obj) const;

  virtual void Print(Option_t* opt = "") const;
  virtual void Clear(Option_t* opt = "");

  virtual void InsertHit(const TDetectorHit&);
  virtual TFastScintHit GetLaBrHit(int i) const;
  virtual TDetectorHit& GetHit(int i);
          TFastScintHit* FindHit(int);
  virtual unsigned int Size() const;
  virtual int GoodSize() const;

  int GetDetNumberIn_fs_hits(Int_t det);
  
  //  void SetTDCTrigger(int trig) { tdc_trigger = trig; }
  //void SetTDC_Count(int count) { tdc_channels= count;}
  //void SetQDC_Count(int count) { qdc_channels= count;}
  void SetTDC_TimeStamp(int ts) { tdc_TS = ts; }
  void SetQDC_TimeStamp(int ts) { qdc_TS = ts; }

  int GetTDC_TimeStamp() {return tdc_TS; }
  int GetQDC_TimeStamp() {return qdc_TS; }
  //int GetTDCTrigger() {return tdc_trigger; }
  //int GetTDC_Count()  {return tdc_channels; }
  //int GetQDC_Count()  {return qdc_channels; }
  int GetErrors() {return errors; };

  void SetTrigTime(UShort_t time) { trig_time = time; }
  int GetTrigTime()	      { return trig_time; }

  static TVector3& GetPosition(int); 

private:
  virtual int  BuildHits(std::vector<TRawEvent>& raw_data);

  int Build_From(TNSCLEvent &event,bool Zero_Suppress = false);

  //TClonesArray* fs_hits; //->
  std::vector<TFastScintHit> fs_hits;

  int qdc_TS;
  int tdc_TS;

  int trig_time;

  static int errors; //!

  ClassDef(TFastScint,1);
};

#endif /* _TFASTSCINT_H_ */
