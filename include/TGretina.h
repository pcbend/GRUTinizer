#ifndef TGRETINA_H
#define TGRETINA_H

#include <TObject.h>
#include <TMath.h>

#include <TClonesArray.h>

#include "TDetector.h"
#include "TGretinaHit.h"

class TGretina : public TDetector {

public:
  TGretina();
  ~TGretina();

  virtual void Copy(TObject& obj) const;
  virtual void Print(Option_t *opt = "") const;
  virtual void Clear(Option_t *opt = "");

  virtual bool          AddRawData(TGEBEvent &event) { return AddRawData(event.GetPayloadBuffer()); }
  virtual bool          AddRawData(TSmartBuffer buf);
  virtual int           BuildHits();
  virtual void          InsertHit(const TDetectorHit& hit);
  virtual TDetectorHit& GetHit(const int &i)            { return *(TGretinaHit*)gretina_hits->At(i); }
  virtual int           Size()                          { return gretina_hits->GetEntries(); }

  const TGretinaHit& GetGretinaHit(int i) { return *(TGretinaHit*)gretina_hits->At(i); }
  TGretinaHit& GetAddbackHit(int i) { return *(TGretinaHit*)addback_hits->At(i); }
  void PrintHit(int i){ gretina_hits->At(i)->Print(); }

  static TVector3 CrystalToGlobal(int cryId,
                                  Float_t localX=0,Float_t localY=0,Float_t localZ=0);

private:
  void BuildAddbackHits();

  TClonesArray* gretina_hits;//("TGretinaHit");
  TClonesArray* addback_hits;//("TGretinaHit");

  static Float_t crmat[32][4][4][4];
  static void SetCRMAT();
  static bool fCRMATSet;

  std::vector<TSmartBuffer> raw_data; //!

  ClassDef(TGretina,3);
};





#endif
