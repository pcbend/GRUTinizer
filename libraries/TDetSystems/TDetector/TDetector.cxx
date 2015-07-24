
#include "TDetector.h"

ClassImp(TDetector)

TDetector::TDetector() { }

TDetector::TDetector(const char* name,const char* title) {
  TNamed::SetNameTitle(name,title);

}

TDetector::~TDetector() { }

void TDetector::Copy(const TDetector& rhs) {
  TNamed::Copy((TObject&)rhs);

}

void TDetector::Clear(Option_t *opt) {
  TNamed::Clear(opt);

}

void TDetector::Print(Option_t *opt) { }
