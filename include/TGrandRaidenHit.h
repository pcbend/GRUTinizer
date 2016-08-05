#ifndef TGRANDRAIDENHIT_H
#define TGRANDRAIDENHIT_H

#include "TDetector.h"
#include "TDetectorHit.h"

#ifdef RCNP
#include "RCNPEvent.h"
#else
#include "TTreeSource.h"
#endif



#include <vector>

struct LaBrHit;

class TGrandRaidenHit : public TDetectorHit {
  public:
    TGrandRaidenHit(); // TODO: move to private
    TGrandRaidenHit(RCNPEvent& rcnpevent);
    TGrandRaidenHit(const TGrandRaidenHit& gr);
    ~TGrandRaidenHit();

    virtual void Copy(TObject& obj) const;
    //virtual void Compare(TObject &obj) const;
    virtual void Print(Option_t *opt = "") const;
    virtual void Clear(Option_t *opt = "");

    void     BuildFrom();



    const std::vector<LaBrHit>& GetLaBr() { return labr_hits; }
    const Double_t& GetMeanPlastE1() { return madc1; }
    const Double_t& GetMeanPlastE2() { return madc2; }
    const Double_t& GetMeanPlastPos1() { return tpos1; }
    const Double_t& GetMeanPlastPos2() { return tpos2; }
    const Long_t& GetTimestamp() { return Timestamp; }
    RCNPEvent& GR() { return rcnp; }
private:
    // Double_t ADC[4];
    // Double_t RF;
    // Double_t QTCLead;

    std::vector<LaBrHit> labr_hits;
    Double_t madc1;
    Double_t madc2;
    Double_t tpos1;
    Double_t tpos2;
    Long_t Timestamp;
    RCNPEvent rcnp;



    ClassDef(TGrandRaidenHit,1);
};

struct LaBrHit {
    Int_t channel;
    Double_t qtc_le;
    Double_t qtc_tr;
    Double_t width;
    //Double_t width() { return qtc_tr - qtc_le; }
};

#endif
