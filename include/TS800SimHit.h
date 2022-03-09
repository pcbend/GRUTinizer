#ifndef TS800SIMHIT_H
#define TS800SIMHIT_H

#include <TObject.h>
#include <Rtypes.h>
#include <TVector3.h>
#include <TMath.h>

#include <cmath>

#include "TGEBEvent.h"
#include "TDetectorHit.h"

#define MAXHPGESEGMENTS 36

class TS800SimHit : public TDetectorHit {

public:
  TS800SimHit();
  ~TS800SimHit();

  void Copy(TObject& obj) const;
  virtual Int_t Charge()        const { return 0;  }

  const char *GetName() const;

  void  Print(Option_t *opt="") const;
  void  Clear(Option_t *opt="");

  float    GetATA() const   { return fATA; }
  float    GetBTA() const   { return fBTA; }
  float    GetDTA() const   { return fDTA; }
  float    GetYTA() const   { return fYTA; }

  double   fATA;
  double   fBTA;
  double   fDTA;
  double   fYTA;


private:
  ClassDef(TS800SimHit,1)
};


#endif
