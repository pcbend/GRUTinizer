
#include "TDetector.h"

ClassImp(TDetector)

TDetector::TDetector() { }

TDetector::TDetector(const char* name,const char* title) {
  TNamed::SetNameTitle(name,title);

}

TDetector::~TDetector() { }

void TDetector::Clear(Option_t *opt) {
  TNamed::Clear(opt);
  fTimestamp = -1;
}

void TDetector::Print(Option_t *opt) const { }

void TDetector::Copy(TObject& obj) const {
  TNamed::Copy(obj);

  TDetector& det = (TDetector&)obj;
  det.fTimestamp = fTimestamp;
}

int TDetector::Compare(const TObject& obj) const {
  TDetector& det = (TDetector&)obj;
  if(fTimestamp < det.fTimestamp) {
    return -1;
  } else if (fTimestamp > det.fTimestamp) {
    return +1;
  } else {
    return 0;
  }
}
