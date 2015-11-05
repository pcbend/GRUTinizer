#include "TROOT_Shim.h"

#define protected public
#include "TROOT.h"
#undef protected

#include "TObject.h"
#include "TSeqCollection.h"

#include "THashList_ThreadSafe.h"

void ReplaceCleanups() {
  TSeqCollection* old = gROOT->fCleanups;
  THashList_ThreadSafe* replacement = new THashList_ThreadSafe;
  TIter next(old);
  TObject* obj;
  while((obj = next())) {
    replacement->Add(obj);
  }
  gROOT->fCleanups = replacement;
  delete old;
}
