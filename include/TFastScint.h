#ifndef _TFASTSCINT_H_
#define _TFASTSCINT_H_

#include "TClonesArray.h"

#include "TDetector.h"
#include "TFastScintHit.h"
#include "TNSCLEvent.h"

class TFastScint : public TDetector {
public:
  TFastScint();
  virtual ~TFastScint();

  void Copy(TObject& obj) const;

  virtual void Clear(Option_t* opt = "");

  virtual void InsertHit(const TDetectorHit&);
  virtual TFastScintHit*    GetLaBrHit(int i);
  virtual TDetectorHit& GetHit(int i);
  virtual int Size();

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

private:
  virtual int  BuildHits();

  int Build_From(TNSCLEvent &event,bool Zero_Suppress = false);

  TClonesArray* fs_hits; //->
  

  int qdc_TS;
  int tdc_TS;

  static int errors; //!

  ClassDef(TFastScint,1);
};

#endif /* _TFASTSCINT_H_ */