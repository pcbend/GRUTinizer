
#include "TGrandRaidenHit.h"
#include "TGRUTOptions.h"
#include "TSmartBuffer.h"
#include "TMath.h"

ClassImp(TGrandRaidenHit)

TGrandRaidenHit::TGrandRaidenHit() {
  madc1=0; madc2=0; tpos1=0; tpos2=0;
}
TGrandRaidenHit::TGrandRaidenHit(const TGrandRaidenHit& gr) {
  labr_hits = gr.labr_hits;
  madc1 = gr.madc1;
  madc2 = gr.madc2;
  tpos1 = gr.tpos1;
  tpos2 = gr.tpos2;
  Timestamp = gr.Timestamp;
  rcnp = gr.rcnp;
}
TGrandRaidenHit::TGrandRaidenHit(RCNPEvent& rcnpevent) :
  rcnp(rcnpevent) {
  madc1=0; madc2=0; tpos1=0; tpos2=0;
}
TGrandRaidenHit::~TGrandRaidenHit() {
}

void TGrandRaidenHit::BuildFrom(){
#ifdef RCNP
  static bool once = true;
  if (once) {
    RCNPEvent::HistDefCheckSum();
    once = false;
  }
  Clear();

  Timestamp = rcnp.GetTimestamp();

  auto adc = rcnp.GR_ADC();
  auto tdc = rcnp.GR_TDC();

  auto qtc_le_tdc =  rcnp.QTC_LEADING_TDC();
  auto qtc_le_chan =  rcnp.QTC_LEADING_CH();
  auto qtc_tr_tdc =  rcnp.QTC_TRAILING_TDC();
  auto qtc_tr_chan =  rcnp.QTC_TRAILING_CH();

  if (qtc_le_tdc && qtc_tr_tdc) {

    for (auto i=0u; i<qtc_le_chan->size(); i++) {
      for (auto j=0u; j<qtc_tr_chan->size(); j++) {
        if ((*qtc_le_chan)[i]==(*qtc_tr_chan)[j]) {

          LaBrHit temphit;
          temphit.channel = (*qtc_le_chan)[i];
          temphit.width = (*qtc_tr_tdc)[j] - (*qtc_le_tdc)[i];
          temphit.qtc_le =(*qtc_le_tdc)[i];
          temphit.qtc_tr = (*qtc_tr_tdc)[j];

          labr_hits.push_back(temphit);

        }
      }
    }

  }

  if (adc) {
    madc1 = TMath::Sqrt((*adc)[0]*(*adc)[1]);
    madc2 = TMath::Sqrt((*adc)[2]*(*adc)[3]);
  }
  if (tdc) {
    tpos1 = TMath::Sqrt((*tdc)[0]*(*tdc)[1]);
    tpos2 = TMath::Sqrt((*tdc)[2]*(*tdc)[3]);
  }

#endif
}





void TGrandRaidenHit::Copy(TObject& obj) const {
  TDetectorHit::Copy(obj);



}


void TGrandRaidenHit::Print(Option_t *opt) const { }

void TGrandRaidenHit::Clear(Option_t *opt) {
  TDetectorHit::Clear(opt);
}













