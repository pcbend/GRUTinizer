#include "TDetector.h"

#include <TRawEvent.h>
#include <iostream>

#include <TClass.h>

ClassImp(TDetector)

TDetector::TDetector() {
  Class()->CanIgnoreTObjectStreamer();
  Clear();
}

TDetector::TDetector(const char* name,const char* title) {
  Clear();
  TNamed::SetNameTitle(name,title);
  Class()->CanIgnoreTObjectStreamer();
}

TDetector::~TDetector() { }

void TDetector::Clear(Option_t *opt) {
  TNamed::Clear(opt);
  fTimestamp = -1;
  fSize = 0;
  fRunStart = 0;
}

void TDetector::Print(Option_t *opt) const { }

void TDetector::Copy(TObject& obj) const {
  TNamed::Copy(obj);

  TDetector& det = (TDetector&)obj;
  det.fTimestamp = fTimestamp;
  det.fSize = fSize;
  det.fRunStart = fRunStart;
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

int TDetector::Build(std::vector<TRawEvent>& raw_data){
  int output = BuildHits(raw_data);
  if(output>0){
    SetBit(kBuilt,1);
  }
  return output;
}
