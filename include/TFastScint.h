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

  void SetTDCTrigger(int trig) { tdc_trigger = trig; }
  void SetTDC_Count(int count) { tdc_channels= count;}
  void SetQDC_Count(int count) { qdc_channels= count;}

  int GetTDCTrigger() {return tdc_trigger; }
  int GetTDC_Count()  {return tdc_channels; }
  int GetQDC_Count()  {return qdc_channels; }

private:
  virtual int  BuildHits();

  void Build_From(TNSCLEvent &event);

  TClonesArray* fs_hits; //->
  
  int tdc_trigger;
  int qdc_channels;
  int tdc_channels;

  ClassDef(TFastScint,1);
};

#endif /* _TFASTSCINT_H_ */
