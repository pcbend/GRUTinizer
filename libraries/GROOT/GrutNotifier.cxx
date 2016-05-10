#include "GrutNotifier.h"

#include <iostream>

#include "GValue.h"
#include "TFile.h"
#include "TGRUTOptions.h"

GrutNotifier *GrutNotifier::fGrutNotifier = 0;

GrutNotifier *GrutNotifier::Get() {
  if(!fGrutNotifier)
    fGrutNotifier = new GrutNotifier;
  return fGrutNotifier;
}

GrutNotifier::GrutNotifier() { }

GrutNotifier::~GrutNotifier() { }


bool GrutNotifier::Notify() {
  // Loads the GValues from the current file of the chain.
  // Does not overwrite parameters set by hand, or by a .val file.
  TFile *f = gChain->GetCurrentFile();
  f->Get("GValue");

  for(auto& callback : callbacks){
    callback();
  }

  return true;
}
