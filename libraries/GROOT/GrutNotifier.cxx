#include "GrutNotifier.h"

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
  //printf("%s I am notifing!\n",__PRETTY_FUNCTION__);
  //if(gChain) {
  //  printf("gChain:       0x%08x\n",gChain);
  //  printf("current file: %s\n",gChain->GetCurrentFile()->GetName());
  //}
  TFile *f = gChain->GetCurrentFile();
  f->Get("GValue");

  for(auto& val_file : TGRUTOptions::Get()->ValInputFiles()){
    GValue::ReadValFile(val_file.c_str());
  }

  for(auto& callback : callbacks){
    callback();
  }

  return true;
}
