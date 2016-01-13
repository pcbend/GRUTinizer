#ifndef _TCAESAR_H_
#define _TCAESAR_H_

#include "TDetector.h"
#include "TCaesarHit.h"

class TCaesar : public TDetector {
public:
  TCaesar();
  virtual ~TCaesar();

  void Copy(TObject& obj) const;
  virtual void InsertHit(const TDetectorHit&);

  virtual void Print(Option_t* opt="") const; 
  virtual void Clear(Option_t* opt = "");

  virtual TDetectorHit& GetHit(int i);
  //Is there any reason the below are virtual? There is no GetCaesarHit in other
  //classes.
  virtual TCaesarHit&   GetCaesarHit(int i);
  virtual const TCaesarHit& GetCaesarHit(int i) const;
  virtual std::vector<TCaesarHit> GetCaesarHits() const  { return caesar_hits; };

  int GetULM() const { return fUlm; }
  void SetULM(int ulm) { fUlm = ulm; }

  //static TVector3 GetPosition(int detnum, int ring_num, int sector_num);

  int Size() const { return caesar_hits.size(); }

private:
  void SetCharge(int vsn, int channel, int data);
  void SetTime(int vsn, int channel, int data);
  TCaesarHit& GetHit_VSNChannel(int vsn, int channel);

  virtual int  BuildHits();

  void Build_Single_Read(TSmartBuffer buf);

  int fUlm;
  std::vector<TCaesarHit> caesar_hits;

  ClassDef(TCaesar,1);
};

#endif /* _TJANUS_H_ */
